import client
from lisp_parser import parse

Port = 8082

MyRIPL = client.RemoteRIPL("http://127.0.0.1:" + str(Port))

MyRIPL.clear()

MyRIPL.assume("not", parse("(lambda (x) (if x False True))"))
MyRIPL.assume("ghetto-nil", parse("False"))
MyRIPL.assume("ghetto-is-empty?", parse("not"))
MyRIPL.assume("cons", parse("(lambda (car cdr) (lambda (x) (if x car cdr))) "))
MyRIPL.assume("car", parse("(lambda (pair) (pair True)) "))
MyRIPL.assume("cdr", parse("(lambda (pair) (pair False)) "))

MyRIPL.assume("filter", parse("(lambda (f xs) (if (ghetto-is-empty? xs) ghetto-nil (if (f (car xs)) (cons (car xs) (filter f (cdr xs))) (filter f (cdr xs)))))"))

# this doesn't do error checking
MyRIPL.assume("list-ref", parse("(lambda (xs i) (if (ghetto-is-empty? xs) ((lambda () error)) (if (int= i c[0]) (car xs) (list-ref (cdr xs) (dec i))))) "))

MyRIPL.assume("list-ref2", parse("(lambda (xs i xs-length index) (if (ghetto-is-empty? xs) ((lambda () error)) (if (int= i c[0]) (car xs) (list-ref2 (cdr xs) (dec i) xs-length index)))) "))

MyRIPL.assume("list-ref-helper", parse("(lambda (concept index) (list-ref2 concept index (length concept) index))"))

MyRIPL.assume("length", parse("(lambda (xs) (if (ghetto-is-empty? xs) c[0] (inc (length (cdr xs)))))"))

MyRIPL.assume("range", parse("(lambda (low high) (if (int>= low high) ghetto-nil (cons low (range (inc low) high))))"))

MyRIPL.assume("numbers", parse("(range 1 11)"))

MyRIPL.assume("make-interval-concept", parse("(lambda (min max)  (lambda (x) (and (int<= min x) (int>= max x)))) "))

MyRIPL.assume("random-interval-concept", parse("(lambda () ((lambda (interval-length) ((lambda (lower-bound) (make-interval-concept lower-bound (int+ lower-bound interval-length))) (uniform-discrete c[1] (int- c[10] interval-length)))) (uniform-discrete c[1] c[5])))"))

MyRIPL.assume("concept", parse("(filter (random-interval-concept) numbers)"))

(directive_id, _) = MyRIPL.predict(parse("(list-ref-helper concept (uniform-discrete c[0] (dec (length concept))))"))
# (directive_id, _) = MyRIPL.predict(parse("(length concept)"))

for i in range(1000):
  MyRIPL.infer(1)
  print MyRIPL.report_value(directive_id)
