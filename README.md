Installation:
See http://venture.csail.mit.edu/wiki/index.php5?title=Installation for prerequisites.
Run python/global_install.sh to install Venture as a python module. See python/README.md for details.

Usage:
Shortcuts are provided to allow easy creation of a RIPL object.

from venture import shortcuts
ripl = shortcuts.make_church_prime_ripl()
