Venture
=======

Compilation options:
* ```$ make -j 5```
* ```$ withzmq=true enablegdb=true make -j 5```

(For your information: "-j" runs object compilation in parallel.
 You can vary this parameter (e.g., depending on number of cores on your computer).)

If you previously compiled without "flags" "withzmq=true enablegdb=true", you should remake:
```bash
$  withzmq=true enablegdb=true make -B -j 5 # (unconditionally make all targets)
```
or just
```
$ make clean
$ withzmq=true enablegdb=true make -j 5
```

Venture license information: http://venture.csail.mit.edu/license_info.html"
