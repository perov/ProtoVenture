[assume n-topics 1]
[assume get-subjects (lambda (topic) (if (= topic 0) (list 0 1 2) (list -10)))]
[assume get-rels (lambda (topic) (if (= topic 0) (list 0 1) (list -10)))]
[assume get-subject-aliases (lambda (zs) (if (= zs 0) (list 0) (if (= zs 1) (list 0) (if (= zs 2) (list 0) (if (= zs 3) (list 4 8) (if (= zs 4) (list 3 8 6 9) (if (= zs 5) (list 7) (if (= zs 6) (list 5) (list -10)))))))))]
[assume get-rel-aliases (lambda (zr) (if (= zr 0) (list 2) (if (= zr 1) (list 10 1) (list -10))))]
[assume get-objects (lambda (subj pred) (if (and (= subj 0) (= pred 1)) (list (list 0 6)) (if (and (= subj 2) (= pred 0)) (list (list 0 4) (list 0 5)) (if (and (= subj 0) (= pred 0)) (list (list 0 3) (list 0 4) (list 0 5) (list 0 6)) (if (and (= subj 1) (= pred 0)) (list (list 0 3) (list 0 4) (list 0 5)) (if (and (= subj 1) (= pred 1)) (list (list 0 4)) (if (and (= subj 2) (= pred 1)) (list (list 0 4)) (list -10))))))))]
[assume get-token-value (mem (lambda (x) (normal 0 100)))]

[assume k 0]
[assume get-topic (mem (lambda (k) 
    (uniform-discrete 0 (int- n-topics 1))))]
[assume n-tokens 20]

[assume p-s-in-kg .9]
[assume p-r-in-kg .9]
[assume p-o-in-kg .8]
[assume p-wrong-alias .5]
[assume p-o-is-entity .5]

[assume sample (lambda (set)
    (let* ((N (length set))
           (n (uniform-discrete 1 N)))
      (nth set n)))]

[assume rand-token (lambda () 
    (uniform-discrete 0 (int- n-tokens 1)))]

[assume s-in-kg? (mem (lambda (row-idx)
    (flip p-s-in-kg)))]
[assume r-in-kg? (mem (lambda (col-idx)
    (flip p-r-in-kg)))]
[assume fact-in-kg? (mem (lambda (row-idx col-idx)
    (flip p-o-in-kg)))]

[assume ENT_T 0]
[assume ATOM_T 1]
[assume NUM_T 2]

[assume get-zo-value-num (lambda (row-idx col-idx)
    (if (= (get-zo row-idx col-idx 0) NUM_T)
        (get-zo row-idx col-idx 1)
        0.0))]

[assume zo-make (lambda (type value field)
    (if (= field 0)
        type
        value))]
[assume xo-make (lambda (type value) value)]

[assume S-UNK-SYM -1]
[assume R-UNK-SYM -1]
[assume S-UNK (lambda ()
    S-UNK-SYM)]
[assume R-UNK (lambda ()
    R-UNK-SYM)]
[assume known-value? (lambda (x) 
    (>= x 0))]

[assume get-zs (mem (lambda (row-idx) 
    (if (s-in-kg? row-idx) 
        (sample (get-subjects (get-topic k)))
        (S-UNK))))]

[assume get-zr (mem (lambda (col-idx) 
    (if (r-in-kg? col-idx) 
        (sample (get-rels (get-topic k)))
        (R-UNK))))]

[assume get-zo (mem (lambda (row-idx col-idx field)
    (let ((zs (get-zs row-idx))
          (zr (get-zr col-idx)))
      (if (and (known-value? zs) (known-value? zr))
        (if (fact-in-kg? row-idx col-idx)
            (nth (sample (get-objects zs zr)) (int+ 1 field))
            (zo-make ATOM_T (rand-token) field))
        (zo-make ATOM_T (rand-token) field)))))]

[assume alias-sample (lambda (alias_func idx)
    (if (= idx -1) 
        (rand-token) 
        (if (flip p-wrong-alias) 
            (rand-token) 
            (sample (alias_func idx)))))]

[assume get-xs (lambda (zs)
    (alias-sample get-subject-aliases zs))]

[assume get-xr (lambda (zr)
    (alias-sample get-rel-aliases zr))]

[assume get-xo-by-idx (mem (lambda (table-idx row-idx col-idx)
    (if (= (get-zo row-idx col-idx 0) NUM_T)
        (xo-make NUM_T (normal (get-zo-value-num zo) 100))
        (if (= (get-zo row-idx col-idx 0) ATOM_T)
            (xo-make ATOM_T (get-zo row-idx col-idx 1))
            (xo-make ATOM_T (get-xs (get-zo row-idx col-idx 1)))))))]

[assume get-xs-by-idx (mem (lambda (table-idx row-idx)
    (get-xs (get-zs row-idx))))]
[assume get-xr-by-idx (mem (lambda (table-idx col-idx)
    (get-xr (get-zr col-idx))))]



[observe (get-xs-by-idx 0 0) 0]
[observe (get-xs-by-idx 0 1) 0]
[observe (get-xs-by-idx 0 2) 0]
[observe (get-xr-by-idx 0 0) 1]
[observe (get-xr-by-idx 0 1) 2]
[observe (get-xo-by-idx 0 0 0) 3]
[observe (get-xo-by-idx 0 0 1) 4]
[observe (get-xo-by-idx 0 1 0) 5]
[observe (get-xo-by-idx 0 1 1) 4]
[observe (get-xo-by-idx 0 2 0) 6]
[observe (get-xo-by-idx 0 2 1) 7]