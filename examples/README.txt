
1. SECTION "HOW TO START/STOP VENTURE ENGINE"

1.1.  Starting the engine's copy

      $ ssh ec2-184-72-149-80.compute-1.amazonaws.com -l probcomp

      User: probcomp
      Password: metropolis1953

      $ sudo /home/ec2-user/venture/venture PORT
      (sudo password is the same: metropolis1953)
      where PORT is a desired port from the range [5000, 5100] (inclusive both 5000 and 5100)

      If you can the engine in "background" by adding the "&":

      $ sudo /home/ec2-user/venture/venture PORT &

1.2.  Stopping the engine's copy (which was previously started by you)

      You can stop then the engine by killing its process:

      $ ps aux | grep venture

      root      7278  0.0  0.0 173056  2388 pts/1    S    13:52   0:00 sudo /home/ec2-user/venture/venture 5000
      root      7279  0.5  0.2 212716 14456 pts/1    S    13:52   0:00 /home/ec2-user/venture/venture 5000

      $ sudo kill 7279

1.3.  Use "screen", if necessary.

      If you want to start the engine's copy for prolonged continuous job
      it is better to run it in Unix "screen".
      
      $ screen
      $ sudo /home/ec2-user/venture/venture PORT &
      
      If you closed your ssh connection, you can resume the screen:
      $ screen -r
      
2. SECTION "HOW TO USE THE VENTURE VIA PYTHON SCRIPT VIA REST"

      You can write some Python script, which will connect to Venture remote server
      via REST protocol.
      
      You need to have (on your computer) the Python's module "requests",
      which you can install with Python's "easy_install":
      
      $ sudo easy_install requests
      
      A simple example how to connect to remote Venture server is given in the folder
      "Using Venture via Python via REST". Please, see the file "simple_example.py".
      
      For your own code it is enough to use necessary provided Python scripts
      "client.py" and "lisp_parser.py" (which are utilities to use the Venture remote server),
      and write your own Python script, e.g. "my_Church_code.py"
      (using as basis the "simple_example.py").
      
3. SECTION "HOW TO USE THE VENTURE VIA JAVASCRIPT VIA REST"

      You can address to Venture with JavaScript too.
      The basic example is provided in the folder "Using Venture via JavaScript via REST".
      It is necessary to specify the port in the file "ripl.js" (line #20).
      You can use the basic example, the "Bayesian curve fitting demo" and the "CRP/Mixture demo",
      to get familiar how to address to remote Venture engine server via JavaScript.
      
      "Bayesian curve fitting demo":
      a) HTML file to run: "loadedvis.html" (follow instructions on the page how to run the demo).
      b) JavaScript file to see: "vis-curvefitting.js"
      
      "CRP/Mixture":
      a) HTML file to run: "graphingCRP.html" (to run demo press the blue button "Load Module" on the page).
      b) JavaScript file to see: "vis-crp.js"
      
IF YOU HAVE ANY QUESTIONS, PLEASE CONTACT US. WE WOULD BE GLAD TO HELP WITH ANY ISSUES.

      yura.perov@gmail.com
      vkm@mit.edu
      