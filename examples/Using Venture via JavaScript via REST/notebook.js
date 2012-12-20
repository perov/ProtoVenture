
function notebook() {
	this.ripl = new ripl();
	this.ShowDirectives();
	this.datastorage=new Array();
	this.refresh = 0;
	

	/* I am sure we will come back to this later 
	//ADD NOTES
	this.noteDB = new Array();
	this.notes = "";
	this.addNote(0,"");
	*/
};

notebook.prototype.addNote = function(notenum, FLAG) {
	var tmp =  '\
			<h3> Note '+String(notenum)+' </h3>\
			<textarea style="height:130px;width:400px;float:none;" id="commentBox'+String(notenum)+'"></textarea>\
			<br><button class="btn btn-medium btn-inverse" type="button" onclick="nb.LoadProgram('+String(notenum)+');">Run</button>\
			<button class="btn btn-medium btn-info" type="button" onclick="nb.addNote('+String(notenum+1)+');">Add Note</button>\
			<button class="btn btn-medium btn-danger" type="button" onclick="nb.deleteNote('+String(notenum)+');">Delete Note</button><hr>';
				


	if (!FLAG) {	
		this.notes += tmp;
		$('#snippet').html(this.notes);
		this.noteDB.push({'divID':this.noteDB.length, 'all':this.notes});
	}
	return tmp;
}


notebook.prototype.deleteNote = function(notenum) {
	var code = "";
	if (notenum > 0 ){
		this.noteDB[notenum] = NaN;
		for (var i=0;i<this.noteDB.length;i++) {
			if (this.noteDB[i] !== NaN) {
				code += this.addNote(i,1);
			}
		}
	}
	console.log(code);
	$('#snippet').html(code);
	this.noteDB.push({'divID':this.noteDB.length, 'all':this.notes});
}

notebook.prototype.conInf = function (state) {
	if(state == 1) {
		this.ripl.start_cont_infer(1);
		//this.handle = setInterval('nb.ShowDirectives()',70);
	}
	else if (state == 0) {
		this.ripl.stop_cont_infer();
		//clearInterval('nb.handle');
	}
	else {
		var stat = this.ripl.cont_infer_status()
		alert('Continous Inferece is: ' + stat.status);
	}
}

notebook.prototype.loadEX = function(num) {
	if (num == 0){
		$('#commentBox0').html('\
CLEAR\n\
ASSUME isTricky (flip 0.1)\n\
ASSUME myCoinWeight (if isTricky (beta 1.0 1.0) 0.5)\n\
ASSUME myCoin (lambda () (flip myCoinWeight))\n\
OBSERVE (noise-negate (myCoin) 0.00001) true (repeat 9 times)\n\
INFER 1000\n\
PREDICT isTricky (5, False)');
	}

	if (num == 1){
		$('#commentBox0').html('\
CLEAR\n\
ASSUME out (lambda () (normal (if (flip 0.7) 1 (+ (out) (out))) 2.0))\n\
PREDICT (out)');
	}
}


notebook.prototype.forgetDirective = function (id) {
	this.ripl.forget(id,1);
	this.ShowDirectives();
}

notebook.prototype.cloneDirective = function (id, directives) {
	var directives = this.ripl.report_directives();
	for (var i=0;i<directives.length;i++) {
		if (directives != 'ERR') {	
			if(directives[i]['directive-id'] == id) {
				if(directives[i]['directive-type'] == 'DIRECTIVE-OBSERVE') {
					var parseList = parse(directives[i]['directive-expression']);
					this.ripl.observe(parseList[1],parseList[2],1);
				}
				else if(directives[i]['directive-type'] == 'DIRECTIVE-PREDICT') {
					var parseList = parse(directives[i]['directive-expression']);
					this.ripl.predict(parseList[1],1);
				}
			}
		}	
	}
	this.ShowDirectives();
}

notebook.prototype.showChurchCode = function (directives) {

	var church_code_str = "";
	for (var i=directives.length-1; i>=0;i--) {
		if (directives != 'ERR') {
			church_code_str += String(directives[i]['directive-expression']) + '<br><br>';
		}
	}

	//church_code_str = church_code_str.replace(/ /g, "&nbsp;");
	church_code_str = church_code_str.replace(/\(if/g, "(<font color='#0000FF'>if</font>");
	church_code_str = church_code_str.replace(/\(/g, "<font color='#0080D5'>(</font>");
	church_code_str = church_code_str.replace(/\)/g, "<font color='#0080D5'>)</font>");
	church_code_str = church_code_str.replace(/lambda/g, "<font color='#0000FF'>lambda</font>");
	church_code_str = church_code_str.replace(/list/g, "<font color='#0000FF'>list</font>");

	church_code_str = church_code_str.replace(/\>=/g, "<font color='#0000FF'>>=</font>");
	church_code_str = church_code_str.replace(/\+/g, "<font color='#0000FF'>+</font>");
	church_code_str = church_code_str.replace(/\*/g, "<font color='#0000FF'>*</font>");
	church_code_str = "<font face='Courier New' size='2'>" + church_code_str + "</font>";	
	$("#churchcode").html(church_code_str);
}


notebook.prototype.ShowDirectives = function ()
{
	var htmlcode="";
	var directives = this.ripl.report_directives();
	//var engine_status = this.ripl.cont_infer_status();

	this.currentDirectives = directives;
	for (var i=0;i<directives.length;i++) {
		if (directives != 'ERR') {

			htmlcode += '<a href="#" style="font-weight:bold;color:black;" class="btn btn-medium btn-default disabled">'+String(directives[i]['directive-id']) + '</a>&nbsp;&nbsp;&nbsp;';

			if(directives[i]['directive-type'] == 'DIRECTIVE-ASSUME') {
				var color = 'blue';
			}
			else if(directives[i]['directive-type'] == 'DIRECTIVE-OBSERVE') {
				var color = 'red';
			}
			else if(directives[i]['directive-type'] == 'DIRECTIVE-PREDICT') {
				var color = 'orange';
			}
			else {
				var color = 'grey';
			}

			var expr =  String(directives[i]['directive-expression']);

			/*if (directives[i]['directive-type'] == 'DIRECTIVE-OBSERVE') {
				if (expr.length > 80) {
					expr = expr.substring(0,80);
				}
			}
			else {
				if (expr.length > 60) {
					expr = expr.substring(0,60);
				}
			}			

			expr = pad(expr, 80);
	     		expr = expr.replace(/ /g, "&nbsp;");*/

			if (directives[i]['directive-type'] == 'DIRECTIVE-PREDICT') {
				htmlcode += '<a href="#" style="width:200px;font-size:11px;font-weight:bold;color:black; background-color:' + color + ';" class="btn btn-medium btn-info disabled">'+expr+ '</a>&nbsp;&nbsp;&nbsp;';
			}
			else {
				htmlcode += '<a href="#" style="width:200px;font-size:11px;font-weight:bold;color:white; background-color:' + color + ';" class="btn btn-medium btn-info disabled">'+expr+ '</a>&nbsp;&nbsp;&nbsp;';
			}
			
			if (directives[i]['directive-type'] == 'DIRECTIVE-PREDICT' || directives[i]['directive-type'] == 'DIRECTIVE-ASSUME') {
				var value =  JSON.stringify(directives[i].value)
				/*if (value.length >= 15) {
					value = value.substring(0,15);
				}*/

				htmlcode += '<a href="#" style="width:200px;font-size:11px;font-weight:bold;color:white;" class="btn btn-medium btn-inverse disabled">'+ value + '</a>';
			}

			var dtype = directives[i]['directive-type'];
			if(dtype == 'DIRECTIVE-PREDICT' ||dtype == 'DIRECTIVE-OBSERVE') {
				console.log(REFRESH);
				if (REFRESH == 0) { 
					htmlcode += '&nbsp; <a onclick="nb.cloneDirective('+ directives[i]['directive-id'] +')" href="#"><img style="height:20px;width:auto;" src="assets/img/add.png"></img> </a>';
					htmlcode += '&nbsp;&nbsp;&nbsp;<a onclick="nb.forgetDirective('+ directives[i]['directive-id'] +')" href="#"><img style="height:20px;width:auto;" src="assets/img/delete.png"></img> </a>';
				}
			}


		}
		htmlcode += '</br></br>';
	}
	$('#directiveTable').html(htmlcode);
	this.showChurchCode(directives);	
		
/* We will probably come back to this :)
	var table="";
	table += '<table class="gridtable">';
        table += '<thead>';
	table += '<tr>';
	table += '<th>ID</th>';
	table += '<th>Expression</th>';
	table += '<th>Value</th>';
        table += '</tr>';
        table += '</thead>';
	table += '<tbody>';

	var directives = this.ripl.report_directives();
	
	for (var i=0;i<directives.length;i++) {
        	table += '<tr>';
        	table +=    '<td>' + String(directives[i]['directive-id']) +'</td>';
       	 	table +=    '<td>' + String(directives[i]['directive-expression']) + '</td>';
        	table +=    '<td>' + String(directives[i]['value'])  + '</td>';
        	table += '</tr>';	
	}
        table +='</tbody>';
        table +='</table>';
	$('#directiveTable').html(table);
*/
};

notebook.prototype.LoadProgram = function (divID)
{
	divID = '#commentBox' + divID;
	var program = $(divID).val();
	console.log(program);
	program = program.split('\n');
	for (var i=0;i<program.length;i++) {
		var command = program[i];
		this.SendProgramLine(command);
	}
	this.ShowDirectives();
};

notebook.prototype.SendProgramLine = function (command, DIRECTIVE)
{
	console.log(command);
	var ret;
	if (!DIRECTIVE)  {
		var command = '('+command + ')';
		var parseList = parse(command);

		if (parseList[0] == 'ASSUME') {
			ret = this.ripl.assume(parseList[1] , parseList[2] ,1);
		}
		if (parseList[0] == 'OBSERVE') {
			ret = this.ripl.observe(parseList[1] , parseList[2] ,1);
		}
		if (parseList[0] == 'PREDICT') {
			ret = this.ripl.predict(parseList[1] ,1);
		}
		if (parseList[0] == 'CLEAR') {
			ret = this.ripl.clearTrace();
		}
		if (parseList[0] == 'INFER') {
			ret = this.ripl.infer(parseList[1],1);
		}
		if (parseList[0] == 'START_CONT_INFER') {
			ret = this.ripl.start_cont_infer(1);
		}
		if (parseList[0] == 'STOP_CONT_INFER') {
			ret = this.ripl.stop_cont_infer();
		}
		if (parseList[0] == 'FORGET') {
			ret = this.ripl.forget(parseList[1],1);
		}
		if (parseList[0] == 'CONT_INFER_STATUS') {
			ret = this.ripl.cont_infer_status();
		}
		if (parseList[0] == 'REPORT_DIRECTIVES') {
			ret = this.ripl.report_directives();
		}
		if (parseList[0] == 'REPORT_VALUE') {
			ret = this.ripl.report_value(parseList[1],1);
		}

		if (parseList[0] !== 'CLEAR')
			this.datastorage = {'id':ret.d_id, 'str':command};
	}
	else {
		if (DIRECTIVE == 'ASSUME') {
			var command = '( '+ 'ASSUME' + ' ' +  $('#ASSUMEname').val() + ' ' + $('#ASSUMEexpr').val() + ' )';
			var parseList = parse(command);
			ret = this.ripl.assume(parseList[1],parseList[2] ,1);
		}
		if (DIRECTIVE == 'OBSERVE') {
			var command = '( '+ 'OBSERVE' + ' ' +  $('#OBSERVEexpr').val() + ' ' +  $('#OBSERVEvalue').val() + ' )';
			var parseList = parse(command);
			ret = this.ripl.observe(parseList[1],parseList[2],1);
		}
		if (DIRECTIVE == 'PREDICT') {
			var command = '( '+ 'PREDICT' + ' ' +  $('#PREDICTexpr').val() + ' )';
			var parseList = parse(command);
			ret = this.ripl.predict(parseList[1],1);
		}
		if (DIRECTIVE == 'INFER') {
			var command = '( '+ 'INFER' + ' ' +  $('#infer').val() + ' )';
			var parseList = parse(command);
			ret = this.ripl.infer(parseList[1],1);
		}	
	
	}
	this.ShowDirectives();
};
