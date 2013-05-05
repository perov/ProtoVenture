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

If you want to compile and install the C++ extentions (necessary
if you want to run a local server), then run:

    python setup.py install --with-local-engine

The compiler may complain about missing C++ libraries, in which case
you should install them separately before compiling. TODO: compile
a list of C++ dependencies for various operating systems


Installation to global environment
==================================

Exactly the same as above, except you don't need to install "virtualenv".

    # Make sure you are not in an active virtualenv
    deactivate

    sudo pip install -r requirements.txt
    sudo python setup.py install [--with-local-engine]