
USING VENTURE REMOTELY ON EC2 INSTANCE

0. SECTION "USERS LIST"
  
      EMAIL                  DEFAULT PORT
      maxhkw@gmail.com       5011
      tejask@mit.edu         5012
      jhuggins@mit.edu       5013
      ardavans@mit.edu       5014
      d.roy@eng.cam.ac.uk    5015
      dlovell@gmail.com      5016
      malmaud@mit.edu        5017
      vkm@mit.edu            5018
      atyurina@sipan.org     5019
      andresr@mit.edu        5020
      mkbehr@mit.edu         5021
      fwood@robots.ox.ac.uk  5022
      jbaxter@mit.edu        5023
      jrl44@cam.ac.uk        5024
      maxs@mit.edu           5025
      jharts@mit.edu         5026
      vladfi2@gmail.com      5027
      zdrach@mit.edu         5028
      saee.paliwal@gmail.com 5029
      
      Your user name is the part of your email before @ symbol. 
     
1. SECTION "CONNECTION TO 'VENTURE' AMAZON EC2 INSTANCE"

      $ ssh ec2-54-235-201-199.compute-1.amazonaws.com -l *YOUR-USER-NAME*

      User: *YOUR-USER-NAME* (the part of your email before @ symbol)
      Password: metropolis1953
      
      Copy examples directory to your home directory:
      
      $ cp -r /usr/venture/examples ~


2. SECTION "HOW TO USE THE VENTURE VIA PYTHON SCRIPT AS PYTHON MODULE"
   *** Correspondent example: "examples/simple_example.py"                          ***

      You can write some Python script, which will import the Venture engine,
      which was previously compiled as Python module. Example of Python code:
      
        import venture.engine as venture
        venture.clear()
        venture.predict("(uniform-continuous 0.1 0.9)")
      
      There is built and installed Python module on the 'Venture' EC2 instance.
      
      You can see the real example in the examples folder
      "examples/simple_example.py".
      
      Just run:
      
      $ python examples/simple_example.py
      
      For your own code it is enough to write your own Python script,
      e.g. "my_Church_code.py" (using as basis the "examples/simple_example.py").
      
3. SECTION "HOW TO USE THE VENTURE VIA PYTHON SCRIPT AS PYTHON MODULE ON PICLOUD"
   *** Correspondent example: "examples/simple_example_using_picloud.py"            ***

      Additionally to the previous section, you can run your Python script,
      which uses the Venture engine as Python module, not on that one 'Venture' EC2
      instance, but thanks to PiCloud, on the cloud. And it is not harder at all! :)
   
      You can see the real example in the examples folder
      "examples/simple_example_using_picloud.py".
      
      Just run:
      
      $ python examples/simple_example_using_picloud.py
      
4. SECTION "HOW TO START/STOP VENTURE ENGINE AS REST SERVER"
   (It is necessary to start the Venture engine as REST server
    to be able to connect to it from client side, see for details
    the section #5 "HOW TO USE THE VENTURE VIA PYTHON SCRIPT VIA REST"
    and the section #6 "HOW TO USE THE VENTURE VIA JAVASCRIPT VIA REST")

4.1.  Starting the engine's REST server copy on EC2 instance

      $ /usr/venture/start_venture PORT
      where PORT is a desired port from the range [5000, 5100] (inclusive both 5000 and 5100)

      If you can the engine in "background" by adding the "&":

      $ /usr/venture/start_venture PORT &
      
      (Though it is better to start the server in one SSH window, and to try examples in another,
       because the engine itself outputs some logs to the console.)

4.2.  Stopping the engine's REST server copy on EC2 instance (which was previously started by you)

      You can stop then the engine by killing its process:

      $ ps aux | grep start_venture

      user1    16233  0.4  0.2 212744 14488 pts/5    S    05:14   0:00 /usr/venture/start_venture 5001

      $ kill 16233

4.3.  Use "screen", if necessary.

      If you want to start the engine's copy for prolonged continuous job
      it is better to run it in Unix "screen".
      
      $ screen
      $ /usr/venture/start_venture PORT &
      
      If you closed your ssh connection, you can resume the screen:
      $ screen -r
      
5. SECTION "HOW TO USE THE VENTURE VIA PYTHON SCRIPT VIA REST"
   *** Correspondent example: "examples/simple_example_using_rest.py" ***

      You can write some Python script, which will connect to Venture remote server
      via REST protocol. In our case this Venture remote server would be
      the EC2 instance described in the section #4.
      
      You need to have (on your computer) the Python's module "requests",
      which you can install with Python's "easy_install":
      
      $ sudo easy_install requests
      
      A simple example how to connect to remote Venture server is given in the file
      "examples/simple_example_using_rest.py".
      
      PLEASE, SPECIFY YOUR PORT ON THE LINE #4 IN THE FILE "simple_example_using_rest.py".
      YOUR PORT IS THE PORT ON WHICH YOU JUST HAVE STARTED YOUR REST SERVER COPY
      ACCORDING TO THE SECTION #4.
      
6. SECTION "HOW TO USE THE VENTURE VIA JAVASCRIPT VIA REST"
   *** Correspondent example: "examples/java_script_examples/"                  ***
   *** You can work on this section from your local machine                     ***

      You can address to Venture with JavaScript too.
      The basic example is provided in the folder "examples/java_script_examples/".
      It is necessary to specify the port in the file "ripl.js" (line #9).
      You can look to two basic examples to get familiar how to address to remote
      Venture engine server via JavaScript.
      
      "Bayesian curve fitting demo":
      a) HTML file to run: "curvefitting.html".
      b) JavaScript file to see: "curvefitting.js".
      
      "CRP mixture demo":
      a) HTML file to run: "CRPMixture.html".
      b) JavaScript file to see: "CRPMixture.js".
      
IF YOU HAVE ANY QUESTIONS, PLEASE CONTACT US. WE WOULD BE GLAD TO HELP WITH ANY ISSUES.

      yura.perov@gmail.com
      vkm@mit.edu
      