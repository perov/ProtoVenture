
import venture.engine as ripl

ripl.assume('map_make', '(lambda (map) (mem map))')
 
ripl.assume('map_set', '(lambda (map key val)\
    (mem (lambda (k)\
        (if (= k key)\
            val\
            (map key)\
        )\
    )\
))')

# Though "key" is not the right thing (should be "k"), the code above produces an error.

ripl.assume('f', '(lambda (key) false)')
ripl.assume('map0', '(map_make f)')

N = 100
M = 1
 
for i in range(N/M):
    ripl.assume('map%d' % (i+1), '(map_set map%d (uniform-discrete 0 %d) true)' % (i, N-1))
 
for i in range(N):
    ripl.predict('(map%d %d)' % (N/M, i))
    
ripl.infer(10000)
