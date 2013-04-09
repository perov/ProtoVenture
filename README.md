Venture
=======

Compilation options:
1) make -j 5
2) make withzmq=true enablegdb=true -j 5

"-j" runs object compilation in parallel.

If you previously compiled without flags "withzmq=true enablegdb=true", you should remake:
"make -B withzmq=true enablegdb=true -j 5" (unconditionally make all targets)
(or just "make clean" and "make withzmq=true enablegdb=true -j 5")
