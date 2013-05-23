#!/usr/bin/env python
# -*- coding: utf-8 -*-

from pyparsing import Literal,CaselessLiteral,Regex,Word,Combine,Group,Optional,\
    ZeroOrMore,OneOrMore,Forward,nums,alphas,FollowedBy,Empty,ParseException,\
    Keyword, CaselessKeyword, MatchFirst, ParseResults, White, And
import re
import json
import copy

from venture.exception import VentureException

# NOTE: when a ParseException is raised -- it fails soft, and the parser
# will continue to try to match the string with the next pattern in its list
# when a VentureException is raised, it fails hard, and the parser will stop
# parsing and will return an error

def lw(thingy):
    # location_wrapper (or lw for short)
    # shortcut for chomping the leading whitespace
    # and augmenting the return value with a set
    # of text locations. "Thingy" is supposed to
    # be a ParserElement that returns a single
    # string at toks[0]
    x = MatchFirst([thingy]).leaveWhitespace()
    def process_x_token(s, loc, toks):
        if isinstance(toks[0], basestring):
            return [{'loc':[loc, loc+len(toks[0])-1], "value":toks[0]}]
        raise VentureException('fatal', 'The lw wrapper only accepts string-valued'
                "tokens. Got: " + toks[0])
    x.setParseAction(process_x_token)
    return Optional(White()).suppress() + x

def combine_locs(l):
    # combines the text-index locations of the given parsed tokens
    # into a single set
    mins = (a['loc'][0] for a in l)
    maxes = (a['loc'][1] for a in l)
    return [min(mins), max(maxes)]

# <symbol>
#
# evaluates to itself as a string
def symbol_token(blacklist_symbols=[], whitelist_symbols=[], symbol_map={}):
    regex = Regex(r'[a-zA-Z_][a-zA-Z_0-9]*')
    whitelist_toks = [Keyword(x) for x in whitelist_symbols]
    symbol = lw(MatchFirst([regex] + whitelist_toks))
    def process_symbol(s, loc, toks):
        tok = toks[0]['value']
        if tok in blacklist_symbols:
            #raise ParseException(s,loc,"Reserved word cannot be symbol: " + tok)
            raise VentureException("text_parse","Reserved word cannot be symbol: " + tok,text_index=toks[0]['loc'])
        if tok in symbol_map:
            tok = symbol_map[tok]
        return [{"loc":toks[0]['loc'], "value":tok}]
    symbol.setParseAction(process_symbol)
    return symbol

# <number>
#
# evaluates to a python float
def number_token():
    number = lw(Regex(r'(-?\d+\.?\d*)|(-?\d*\.?\d+)'))
    def process_number(s, loc, toks):
        return [{"loc":toks[0]['loc'], "value":float(toks[0]['value'])}]
    number.setParseAction(process_number)
    return number

# <integer>
#
# evaluates to a python integer
def integer_token():
    integer = lw(Regex(r'\d+'))
    def process_integer(s, loc, toks):
        return [{"loc":toks[0]['loc'], "value":int(toks[0]['value'])}]
    integer.setParseAction(process_integer)
    return integer

# <string>
#
# evaluates to a python string
def string_token():
    # string = lw(Regex(r'"(?:[^"\\]|\\"|\\\\|\\/|\\r|\\b|\\n|\\t|\\f|\\[0-7]{3})*"'))
    # match more strings to produce helpful error message
    string = lw(Regex(r'"(?:[^"\\]|\\.)*"'))
    def process_string(s, loc, toks):
        s = toks[0]['value']
        s = s[1:-1]
        s = s.replace(r'\n','\n')
        s = s.replace(r'\t','\t')
        s = s.replace(r'\f','\f')
        s = s.replace(r'\b','\b')
        s = s.replace(r'\r','\r')
        s = s.replace(r"\/", '/')
        s = s.replace(r'\"','"')
        octals = list(re.finditer(r'\\[0-7]{3}', s))
        octals.reverse()
        for match in octals:
            st = match.group()
            i = int(st[1])*64+int(st[2])*8+int(st[3])
            if i > 127:
                raise VentureException("text_parse",
                        "Octal too large for ASCII character: " + st, text_index=toks[0]['loc'])
            c = chr(i)
            start = match.start()
            end = match.end()
            s = s[:start] + c + s[end:]
        tmp = []
        i = 0
        while i < len(s):
            if s[i] == '\\':
                if i == len(s) or s[i+1] != '\\':
                    raise VentureException("text_parse",
                            "Invalid escape sequence within string.", text_index=toks[0]['loc'])
                tmp.append('\\')
                i += 2
            else:
                tmp.append(s[i])
                i += 1
        s = ''.join(tmp)
        return [{"loc":toks[0]['loc'], "value":s}]
    string.setParseAction(process_string)
    return string

# <null>
#
# evaluates to a python None
def null_token():
    def process_null(s, loc, toks):
        return [{"loc":toks[0]['loc'], "value":None}]
    null = lw(Keyword('null'))
    null.setParseAction(process_null)
    return null

# <boolean>
#
# evaluates to a python boolean
def boolean_token():
    def process_boolean(s, loc, toks):
        n = toks[0]['value'] == 'true'
        return [{"loc":toks[0]['loc'], "value":n}]
    boolean = lw(Keyword('true') | Keyword('false'))
    boolean.setParseAction(process_boolean)
    return boolean

# <json_value>
#
# evaluates to a native json value
def json_value_token():
    json_value = Forward()

    string = string_token()
    number = number_token()
    null = null_token()
    boolean = boolean_token()

    #json_list
    json_list = lw(Literal('[')) + Optional(
                json_value + ZeroOrMore(
                    lw(Literal(',')) + json_value
                )
            ) + lw(Literal(']'))
    def process_json_list(s, loc, toks):
        v = [x['value'] for x in toks[1:-1:2]]
        l = combine_locs(toks)
        return [{"loc":l, "value":v}]
    json_list.setParseAction(process_json_list)

    #json_object
    json_object = lw(Literal('{')) + Optional(
                string + lw(Literal(':')) + json_value + ZeroOrMore(
                    lw(Literal(',')) + string +
                    lw(Literal(':')) + json_value
                )
            ) + lw(Literal('}'))
    def process_json_object(s, loc, toks):
        v = dict(zip(*(iter(x['value'] for x in toks[1:-1:2]),)*2))
        l = combine_locs(toks)
        return [{"loc":l, "value":v}]
    json_object.setParseAction(process_json_object)

    #json_value
    json_value << (string | number | boolean |
            null | json_list | json_object)
    def process_json_value(s, loc, toks):
        return [toks[0]]
    json_value.setParseAction(process_json_value)

    return json_value

# <value>
#
# evalues to something of the form {"type": "type_name", "value":<json_value>}
#
# forbid type_name in ('boolean', 'number') to preserve bijection
def value_token():
    value = symbol_token() + lw(Literal('<')) + json_value_token() + lw(Literal('>'))
    def process_value(s, loc, toks):
        if toks[0]['value'] in ('number', 'boolean'):
            raise VentureException("text_parse",
                "Not allowed to construct a " + toks[0]['value'] + " value. " +
                "use a built-in primitive instead", text_index=[loc,loc])
        v = {"type": toks[0]['value'], "value": toks[2]['value']}
        l = combine_locs(toks)
        return [{"loc":l, "value":v}]
    value.setParseAction(process_value)
    return value

# <number_literal>
#
# evalutes to a literal number value: {"type":"number", "value":s}
def number_literal_token():
    def process_number_literal(s, loc, toks):
        v = {'type':'number', 'value':float(toks[0]['value'])}
        l = toks[0]['loc']
        return [{"loc":l, "value":v}]
    number_literal = MatchFirst([number_token()])
    number_literal.setParseAction(process_number_literal)
    return number_literal

# <boolean_literal>
#
# evalutes to a literal boolean value: {"type":"boolean", "value":s}
def boolean_literal_token():
    def process_boolean_literal(s, loc, toks):
        v = {'type':'boolean', 'value':toks[0]['value']}
        l = toks[0]['loc']
        return [{"loc":l, "value":v}]
    boolean_literal = MatchFirst([boolean_token()])
    boolean_literal.setParseAction(process_boolean_literal)
    return boolean_literal

# <literal>
#
# evaluates a json data type
def literal_token():
    literal = boolean_literal_token() | number_literal_token() | value_token()
    def process_literal(s, loc, toks):
        return [toks[0]]
    literal.setParseAction(process_literal)
    return literal

# convert ParseException into VentureException
def apply_parser(element, string):
    if not isinstance(string, basestring):
        raise VentureException('fatal',
                'Parser only accepts strings')
    try:
        return list(element.parseString(string, parseAll=True))
    except ParseException as e:
        raise VentureException('text_parse',
                str(e), text_index=[e.loc,e.loc])

def value_to_string(v):
    if isinstance(v, dict):
        if (not 'type' in v) or (not 'value' in v):
            raise VentureException('fatal',
                    'Value should take the form {"type":<type>, "value":<value>}.')
        if v['type'] == "boolean":
            if v['value'] == True:
                return 'true'
            if v['value'] == False:
                return 'true'
            raise VentureException('fatal',
                    'Invalid boolean value')
        if v['type'] == "number":
            if not isinstance(v['value'], (int, float)):
                raise VentureException('fatal',
                        'Invalid number value')
            return "{}".format(v['value'])
        try:
            return "{}<{}>".format(
                    v['type'],
                    json.dumps(v['value']))
        except:
            raise VentureException('fatal',
                    'Invalid value -- could not convert to json string')
    if isinstance(v, bool):
        if v:
            return "true"
        return "false"
    if isinstance(v, (int, float)):
        return "{}".format(v)
    if isinstance(v, basestring):
        return v
    raise VentureException('fatal',
            'Value must be a dict, boolean, number, or pre-formatted string')

def _unpack(l):
    if isinstance(l['value'], (list, tuple, ParseResults)):
        return [_unpack(x) for x in l['value']]
    return l['value']

def simplify_expression_parse_tree(parse_tree):
    return _unpack(parse_tree)

def simplify_instruction_parse_tree(parse_tree):
    output = {}
    for key, value in parse_tree['value'].items():
        output[key] = _unpack(value)
    return output

def simplify_program_parse_tree(parse_tree):
    return [simplify_instruction_parse_tree(x) for x in parse_tree['value']]

def split_instruction_parse_tree(parse_tree):
    output = {}
    for key, value in parse_tree['value'].items():
        output[key] = value['loc']
    return output

def split_program_parse_tree(parse_tree):
    return [x['loc'] for x in parse_tree['value']]

def get_program_string_fragments(s,locs):
    output = []
    for loc in locs:
        output.append(s[loc[0]:loc[1]+1])
    return output

def get_instruction_string_fragments(s,locs):
    output = {}
    for key, loc in locs.items():
        output[key] = s[loc[0]:loc[1]+1]
    return output

def get_text_index(parse_tree, expression_index):
    """parse-tree-index to text index"""
    if len(expression_index) == 0:
        return parse_tree['loc']
    tmp = parse_tree
    try:
        for i in expression_index[:-1]:
            tmp = tmp['value'][i]
        x = tmp['value'][expression_index[-1]]['loc']
        if x == None:
            raise VentureException("no_text_index", "Expression index does"\
                " not have a corresponding text index" + str(e.message))
        return x
    except Exception as e:
        raise VentureException("no_text_index", "Expression index is invalid: " + str(e.message))

def get_expression_index(parse_tree, text_index):
    """text index to expression-index, the parse_tree
    is supposed to be the parse tree of an exception"""
    d = {}
    def unpack(l, prefix=[]):
        for loc in range(l['loc'][0], l['loc'][1]+1):
            if loc in d:
                d[loc].append(prefix)
            else:
                d[loc] = [prefix]
        if isinstance(l['value'], (list, tuple, ParseResults)):
            for index, elem in enumerate(l['value']):
                unpack(elem, prefix + [index])
    unpack(parse_tree)
    if not text_index in d:
        raise VentureException("no_expression_index", "Text index {} does not"
                "correspond to an expression index".format(text_index))
    #preference for the most specific (longest) expression index
    exps = d[text_index]
    output = None
    for e in exps:
        if output == None or len(e) > len(output):
            output = e
    return output

def inst(keyword, instruction):
    k = lw(CaselessKeyword(keyword))
    def process_k(s, loc, toks):
        return [{'loc':toks[0]['loc'], "value":instruction}]
    k.setParseAction(process_k)
    return k

def make_instruction(patterns, instruction, syntax, defaults={}):
    toks = syntax.split()
    pattern = []
    mapping = []
    optional = []
    for tok in toks:
        m = re.match(r"<(\w*):(\w*)>", tok)
        if m:
            w = m.groups()
            mapping.append(w[0])
            pattern.append(patterns[w[1]])
            optional.append(False)
            continue
        m = re.match(r"<!(\w*)>", tok)
        if m:
            w = m.groups()[0]
            mapping.append('instruction')
            pattern.append(inst(w,instruction))
            optional.append(False)
            continue
        m = re.match(r"<\?(\w*):(\w*)>", tok)
        if m:
            w = m.groups()
            mapping.append(w[0])
            pattern.append(Optional(patterns[w[1]],default=None))
            optional.append(True)
            if w[0] not in defaults:
                raise VentureException('fatal','Missing default argument value: ' + w[0])
            continue
        mapping.append(None)
        pattern.append(lw(Literal(tok)))
        optional.append(False)
    pattern = And(pattern)
    def process_pattern(s, loc, toks):
        v = {}
        l = combine_locs([x for x in toks if x != None])
        for tok, m, o in zip(toks, mapping, optional):
            if o==True:
                if tok != None:
                    v[m] = tok
                else:
                    v[m] = {'loc':l, 'value':defaults[m]}
            elif m != None:
                v[m] = tok
        return [{'loc':l, "value":v}]
    pattern.setParseAction(process_pattern)
    return pattern

def make_instruction_parser(instruction_list, patterns):
    instructions = []
    for x in instruction_list:
        instructions.append(make_instruction(patterns,*x))
    return MatchFirst(instructions)

def make_instruction_strings(instruction_list,antipatterns,escape='%'):
    d = {}
    for l in instruction_list:
        key,value = l[:2]
        toks = value.split()
        newlist = []
        for tok in toks:
            m = re.match(r"<\??(\w*):(\w*)>", tok)
            if m:
                label,pattern = m.groups()
                newlist.append(escape+"("+label+")"+antipatterns[pattern])
                continue
            m = re.match(r"<!(\w*)>", tok)
            if m:
                newlist.append(m.groups()[0].replace(escape,escape*2))
                continue
            toks = tok.replace(escape,escape*2)
            newlist.append(tok)
        d[key] = ' '.join(newlist)
    return d

def make_program_parser(instruction):
    program = OneOrMore(instruction)
    def process_program(s, loc, toks):
        v = list(toks)
        l = combine_locs(toks)
        return [{'loc':l, "value":v}]
    program.setParseAction(process_program)
    program.parseWithTabs()
    return program

def make_standard_substitution_patterns():
    def s(x):
        return str(x)
    def v(x):
        return value_to_string(x)
    def j(x):
        return json.dumps(x)
    return {
            "s":s,
            "v":v,
            "j":j,
            }

standard_substitution_patterns = make_standard_substitution_patterns()

def substitute_params(s, params, patterns=standard_substitution_patterns, escape='%'):
    out = []
    if isinstance(params, (tuple, list)):
        i = 0
        j = 0
        while i < len(s):
            if s[i] == escape:
                if i + 1 >= len(s) or s[i+1] not in [escape] + patterns.keys():
                    raise VentureException('fatal', 'Param substitution failure. '
                        'Dangling "' + escape + '" at index ' + str(i), params=params, string=s)
                i += 1
                if s[i] == escape:
                    out.append(escape)
                if s[i] in patterns:
                    if j >= len(params):
                        raise VentureException('fatal', 'Param substitution failure. '
                                'Not enough params provided.', params=params, string=s)
                    out.append(patterns[s[i]](params[j]))
                    j += 1
            else:
                out.append(s[i])
            i += 1
        if j < len(params):
            raise VentureException('fatal', 'Param substitution failure. '
                    'Too many params provided', params=params, string=s)
    elif isinstance(params, dict):
        i = 0
        while i<len(s):
            if s[i] == escape:
                if i + 1 >= len(s) or s[i+1] not in [escape,'(']:
                    raise VentureException('fatal', 'Param substitution failure. '
                        'Dangling "'+escape+'" at index ' + str(i), params=params, string=s)
                i += 1
                if s[i] == escape:
                    out.append(escape)
                if s[i] == '(':
                    j = i
                    p = 1
                    while p > 0:
                        j += 1
                        if j >= len(s):
                            raise VentureException('fatal', 'Param substitution failure. '
                                'Incomplete "'+escape+'" expression starting at index ' + str(i-1), params=params, string=s)
                        if s[j] == ')':
                            p -= 1
                    if j+1 >= len(s) or s[j+1] not in patterns:
                        raise VentureException('fatal', 'Param substitution failure. '
                            'Expected ' + ', '.join(patterns.keys()) + ' after index ' + str(j), params=params, string=s)
                    key = s[i+1:j]
                    if not key in params:
                        raise VentureException('fatal', 'Param substitution failure. '
                                'Key not present in params dict: ' + key, params=params, string=s)
                    c = s[j+1]
                    out.append(patterns[c](params[key]))
                    i = j+1
            else:
                out.append(s[i])
            i += 1
    return ''.join(out)
