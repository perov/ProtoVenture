Installation to local environment
=================================

Download and install python "virtualenv" onto your computer.
https://pypi.python.org/pypi/virtualenv

Create a new virtual environment to install the requirements:

    virtualenv env.d
    source env.d/bin/activate
    pip install -r requirements.txt

Installation into your virtual environment:

    python setup.py install

By default, the setup script install a local C++ engine. If you
do not want to install the local module, then run:

    python setup.py install --without-local-engine

The compiler may complain about missing C++ libraries, in which case
you should install them on your machine before running the setup script.


Installation to global environment
==================================

Exactly the same as above, except you don't need to install "virtualenv".

    # Make sure you are not in an active virtualenv
    deactivate

    sudo pip install -r requirements.txt
    sudo python setup.py install [--with-local-engine]


Rapid Python Development
==================================

If you are developing the python libraries, you will
likely be running the installation script hundreds of
times. This is very slow if you don't have a c++ compiler
cache installed. Here is a quick shell command (aliased in
my bashrc file) which automatically resets the virtual
environment and reinstalls the module, using the compiler
cache. Make sure that the additional python dependencies
are installed in the global python environment.

    deactivate && rm -rf env.d build && virtualenv --system-site-packages env.d && \
      . env.d/bin/activate && CC=\"ccache gcc\" python setup.py install