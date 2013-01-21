import client
from lisp_parser import parse

# Port = 8082
# MyRIPL = client.RemoteRIPL("http://127.0.0.1:" + str(Port))

Port = 5098
MyRIPL = client.RemoteRIPL("http://ec2-54-235-201-199.compute-1.amazonaws.com:" + str(Port))

MyRIPL.clear()

### Begin library functions
# I can't find cons, car, or cdr, so make them
MyRIPL.assume("not", parse("(lambda (x) (if x False True))"))
MyRIPL.assume("ghetto-nil", parse("False"))
MyRIPL.assume("ghetto-is-empty?", parse("not"))
MyRIPL.assume("cons", parse("(lambda (car cdr) (lambda (x) (if (= x 0) car cdr))) "))
MyRIPL.assume("car", parse("(lambda (pair) (pair 0)) "))
MyRIPL.assume("cdr", parse("(lambda (pair) (pair 1)) "))

MyRIPL.assume("filter", parse("(lambda (f xs) (if (ghetto-is-empty? xs) ghetto-nil (if (f (car xs)) (cons (car xs) (filter f (cdr xs))) (filter f (cdr xs))))) "))

# this doesn't do error checking
MyRIPL.assume("list-ref", parse("(lambda (xs i) (if (<= i 0) (car xs) (list-ref (cdr xs) (- i 1)))) "))

MyRIPL.assume("length", parse("(lambda (xs) (if (ghetto-is-empty? xs) 0 (+ 1 (length (cdr xs)))))"))

MyRIPL.assume("range", parse("(lambda (low high) (if (>= low high) ghetto-nil (cons low (range (+ low 1) high))))"))

### Begin actual program

MyRIPL.assume("numbers", parse("(range 1 101)"))

MyRIPL.assume("make-interval-concept", parse("(lambda (min max)  (lambda (x) (and (<= min x) (>= max x)))) "))

# TODO make priors better
MyRIPL.assume("random-interval-concept", parse("(lambda () ((lambda (interval-length) ((lambda (lower-bound) (make-interval-concept lower-bound (+ lower-bound interval-length))) (uniform-continuous 1 (- 100 interval-length)))) (uniform-discrete 3 15)))"))

MyRIPL.assume("multiples-concept-helper", parse("(lambda (base x i) (if (= x (* base i)) True (if (< x (* base i)) False (multiples-concept-helper base x (+ i 1))))) "))
MyRIPL.assume("make-multiples-concept", parse("(lambda (base) (lambda (x) (multiples-concept-helper base x 0))) "))
MyRIPL.assume("random-multiples-concept", parse("(lambda () (make-multiples-concept (uniform-discrete 2 10))) "))

MyRIPL.assume("gen-concept", parse("(lambda () (if (bernoulli 0.5) (random-interval-concept) (random-multiples-concept)))"))

MyRIPL.assume("gen-number", parse("(lambda (concept) ((lambda (in-concept) (list-ref in-concept (uniform-continuous 0 (- (length in-concept) 1)))) (filter concept numbers)))"))

MyRIPL.assume("exception-prob", parse("(beta 100 1)"))

MyRIPL.assume("is-exception", parse("(mem (lambda (data-id) (bernoulli exception-prob)))"))

MyRIPL.assume("noisy-equals-probability", parse("(lambda (x y p) (if (= x y) p (- 1 p)))"))

MyRIPL.assume("make-observation", parse("(lambda (concept data-id) (if (is-exception data-id) (uniform-discrete 1 100) (gen-number concept)))"))

# MyRIPL.assume("multiples-base", parse("(uniform-discrete 2 10)"))
# MyRIPL.observe(parse("(bernoulli (noisy-equals-probability (make-observation (make-multiples-concept multiples-base) 0) 10 0.001))"), True)
# (multiples_base, _) = MyRIPL.predict("multiples-base")
# print MyRIPL.report_value(multiples_base)

MyRIPL.assume("concept", parse("(gen-concept)"))
MyRIPL.observe(parse("(bernoulli (noisy-equals-probability (make-observation concept 0) 10 0.001))"), True)
(generated_number, _) = MyRIPL.predict(parse("(gen-number concept)"))

#burnin = 1000
nsamples = 2000
intermediate_iterations = 1

for sample_number in range(nsamples):
  MyRIPL.infer(intermediate_iterations, 5)
  sample = MyRIPL.report_value(generated_number)
  print "Sample #" + str(sample_number) + " = " + str(sample)
  