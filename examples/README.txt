
USING VENTURE REMOTELY ON EC2 INSTANCE

0. SECTION "USERS LIST"
  
      EMAIL                 DEFAULT PORT
      ...                   ...
      
      (Sorry, this list is out-of-date. Please, see the last version of this list here:
       http://www.yuraperov.com/MIT.PCP/static/25December2012/private/README.html)
      
      Your user name is the part of your email before @ symbol. 
     
1. SECTION "CONNECTION TO 'VENTURE' AMAZON EC2 INSTANCE"

      $ ssh ec2-54-235-201-199.compute-1.amazonaws.com -l *YOUR-USER-NAME*

      User: *YOUR-USER-NAME* (the part of your email before @ symbol)
      Password: metropolis1953
      
      Copy examples directory to your home directory:
      
      $ cp -r /usr/venture/examples ~


2. SECTION "HOW TO USE THE VENTURE VIA PYTHON SCRIPT AS PYTHON MODULE"
   *** Correspondent example: "examples/Using Venture via Python as Python module/" ***
   *** You can work on this section ONLY from the 'Venture' EC2 instance            ***

      You can write some Python script, which will import the Venture engine,
      which was previously compiled as Python module. Example of Python code:
      
        import venture_engine
        venture_engine.clear()
        venture_engine.predict(parse("(uniform-continuous 0.1 0.9)"))
      
      There is built and installed Python module on the 'Venture' EC2 instance.
      
      You can see the real example in the examples folder
      "examples/Using Venture via Python as Python module/".
      
      Just run:
      
      $ python simple_example.py
      
      For your own code it is enough to use the necessary provided Python script
      "venture_engine_requirements.py" (which is utilities to use the Venture Python module),
      and write your own Python script, e.g. "my_Church_code.py"
      (using as basis the "simple_example.py").
      
3. SECTION "HOW TO USE THE VENTURE VIA PYTHON SCRIPT AS PYTHON MODULE ON PICLOUD"
   *** Correspondent example: "examples/Using Venture on PiCloud/"                  ***
   *** You can work on this section ONLY from the 'Venture' EC2 instance            ***

      Additionally to the previous section, you can run your Python script,
      which uses the Venture engine as Python module, not on that one 'Venture' EC2
      instance, but thanks to PiCloud, on the cloud. And it is not harder at all! :)
   
      You can see the real example in the examples folder
      "examples/Using Venture on PiCloud/".
      
      Just run:
      
      $ python simple_example.py
      
      For your own code it is enough to use the necessary provided Python script
      "venture_engine_requirements.py" (which is utilities to use the Venture Python module),
      and write your own Python script, e.g. "my_Church_code.py"
      (using as basis the "simple_example.py").
      
4. SECTION "HOW TO START/STOP VENTURE ENGINE AS REST SERVER"
   (It is necessary to start the Venture engine as REST server
    to be able to connect to it from client side, see for details
    the section #5 "HOW TO USE THE VENTURE VIA PYTHON SCRIPT VIA REST"
    and the section #6 "HOW TO USE THE VENTURE VIA JAVASCRIPT VIA REST")

4.1.  Starting the engine's REST server copy on EC2 instance

      $ /usr/venture/venture PORT
      where PORT is a desired port from the range [5000, 5100] (inclusive both 5000 and 5100)

      If you can the engine in "background" by adding the "&":

      $ /usr/venture/venture PORT &
      
      (Though it is better to start the server in one SSH window, and to try examples in another,
       because the engine itself outputs some logs to the console.)

4.2.  Stopping the engine's REST server copy on EC2 instance (which was previously started by you)

      You can stop then the engine by killing its process:

      $ ps aux | grep venture

      user1    16233  0.4  0.2 212744 14488 pts/5    S    05:14   0:00 /usr/venture/venture 5001

      $ kill 16233

4.3.  Use "screen", if necessary.

      If you want to start the engine's copy for prolonged continuous job
      it is better to run it in Unix "screen".
      
      $ screen
      $ /usr/venture/venture PORT &
      
      If you closed your ssh connection, you can resume the screen:
      $ screen -r
      
5. SECTION "HOW TO USE THE VENTURE VIA PYTHON SCRIPT VIA REST"
   *** Correspondent example: "examples/Using Venture via Python via REST/" ***
   *** You can work on this section BOTH from your local machine            ***
   *** and from 'Venture' EC2 instance                                      ***

      You can write some Python script, which will connect to Venture remote server
      via REST protocol. In our case this Venture remote server would be
      the EC2 instance described in the section #4.
      
      You need to have (on your computer) the Python's module "requests",
      which you can install with Python's "easy_install":
      
      $ sudo easy_install requests
      
      A simple example how to connect to remote Venture server is given in the folder
      "Using Venture via Python via REST". Please, see the file "simple_example.py".
      
      For your own code it is enough to use necessary provided Python scripts
      "client.py" and "lisp_parser.py" (which are utilities to use the Venture remote server),
      and write your own Python script, e.g. "my_Church_code.py"
      (using as basis the "simple_example.py").
      
      PLEASE, SPECIFY YOUR PORT ON THE LINE #4 IN THE FILE "simple_example.py".
      YOUR PORT IS THE PORT ON WHICH YOU JUST HAVE STARTED YOUR REST SERVER COPY
      ACCORDING TO THE SECTION #4.
      
6. SECTION "HOW TO USE THE VENTURE VIA JAVASCRIPT VIA REST"
   *** Correspondent example: "examples/Using Venture via JavaScript via REST/" ***
   *** You can work on this section from your local machine                     ***

      You can address to Venture with JavaScript too.
      The basic example is provided in the folder "Using Venture via JavaScript via REST".
      It is necessary to specify the port in the file "ripl.js" (line #9).
      You can use the basic example, the "Bayesian curve fitting demo",
      to get familiar how to address to remote Venture engine server via JavaScript.
      
      "Bayesian curve fitting demo":
      a) HTML file to run: "curvefitting.html".
      b) JavaScript file to see: "curvefitting.js".
      
IF YOU HAVE ANY QUESTIONS, PLEASE CONTACT US. WE WOULD BE GLAD TO HELP WITH ANY ISSUES.

      yura.perov@gmail.com
      vkm@mit.edu
      