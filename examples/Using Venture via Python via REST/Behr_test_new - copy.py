import client
from lisp_parser import parse

Port = 8082

MyRIPL = client.RemoteRIPL("http://127.0.0.1:" + str(Port))

MyRIPL.clear()

### Begin library functions
# I can't find cons, car, or cdr, so make them
MyRIPL.assume("not", parse("(lambda (x) (if x False True))"))
MyRIPL.assume("ghetto-nil", parse("False"))
MyRIPL.assume("ghetto-is-empty?", parse("not"))
MyRIPL.assume("cons", parse("(lambda (car cdr) (lambda (x) (if x car cdr))) "))
MyRIPL.assume("car", parse("(lambda (pair) (pair True)) "))
MyRIPL.assume("cdr", parse("(lambda (pair) (pair False)) "))

MyRIPL.assume("filter", parse("(lambda (f xs) (if (ghetto-is-empty? xs) ghetto-nil (if (f (car xs)) (cons (car xs) (filter f (cdr xs))) (filter f (cdr xs)))))"))

# this doesn't do error checking
MyRIPL.assume("list-ref", parse("(lambda (xs i) (if (ghetto-is-empty? xs) ((lambda () error)) (if (int= i c[0]) (car xs) (list-ref (cdr xs) (dec i))))) "))

MyRIPL.assume("length", parse("(lambda (xs) (if (ghetto-is-empty? xs) c[0] (inc (length (cdr xs)))))"))

# "range" returns the range [log, high) (inclusive and exclusive accordingly.
MyRIPL.assume("range", parse("(lambda (low high) (if (int>= low high) ghetto-nil (cons low (range (inc low) high))))"))

### Begin actual program

MyRIPL.assume("numbers", parse("(range c[1] c[101])"))

MyRIPL.assume("make-interval-concept", parse("(lambda (min max)  (lambda (x) (and (int<= min x) (int>= max x)))) "))

# TODO make priors better
MyRIPL.assume("random-interval-concept", parse("(lambda () ((lambda (interval-length) ((lambda (lower-bound) (make-interval-concept lower-bound (int+ lower-bound interval-length))) (uniform-discrete c[1] (int- c[10] interval-length)))) (uniform-discrete c[3] c[5])))"))

MyRIPL.assume("multiples-concept-helper", parse("(lambda (base x i) (if (int= x (int* base i)) True (if (int< x (int* base i)) False (multiples-concept-helper base x (inc i))))) "))
MyRIPL.assume("make-multiples-concept", parse("(lambda (base) (lambda (x) (multiples-concept-helper base x c[0]))) "))
MyRIPL.assume("random-multiples-concept", parse("(lambda () (make-multiples-concept (uniform-discrete c[2] c[10]))) "))

# MyRIPL.assume("gen-concept", parse("(lambda () (if (bernoulli 0.5) (random-interval-concept) (random-multiples-concept)))"))
MyRIPL.assume("gen-concept", parse("(lambda () (random-interval-concept))"))
# MyRIPL.assume("gen-concept", parse("(lambda () (make-interval-concept 1 (uniform-discrete 1 2)))"))

MyRIPL.assume("gen-number", parse("(lambda (concept) ((lambda (in-concept) (list-ref in-concept (uniform-discrete c[0] (dec (length in-concept))))) (filter concept numbers)))"))

MyRIPL.assume("exception-prob", parse("(beta r[1] r[1])"))

MyRIPL.assume("is-exception", parse("(mem (lambda (data-id) (bernoulli exception-prob)))"))

MyRIPL.assume("noisy-equals-probability", parse("(lambda (x y p) (if (int= x y) p (real- 1 p)))"))

MyRIPL.assume("make-observation", parse("(lambda (concept data-id) (if (is-exception data-id) (uniform-discrete c[1] c[10]) (gen-number concept)))"))

# MyRIPL.assume("multiples-base", parse("(uniform-discrete 2 10)"))
# MyRIPL.observe(parse("(bernoulli (noisy-equals-probability (make-observation (make-multiples-concept multiples-base) 0) 10 0.001))"), True)
# (multiples_base, _) = MyRIPL.predict("multiples-base")
# print MyRIPL.report_value(multiples_base)

MyRIPL.assume("concept", parse("(gen-concept)"))
MyRIPL.observe(parse("(bernoulli (noisy-equals-probability (make-observation concept c[0]) c[10] r[0.1]))"), True)
(generated_number, _) = MyRIPL.predict(parse("(gen-number concept)"))

#burnin = 1000
nsamples = 2000
intermediate_iterations = 1

for sample_number in range(nsamples):
  MyRIPL.infer(intermediate_iterations, 5)
  sample = MyRIPL.report_value(generated_number)
  print "Sample #" + str(sample_number) + " = " + str(sample)
  