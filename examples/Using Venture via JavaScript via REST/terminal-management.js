

function term_ripl() {
	this.ripl = new ripl();
	this.term_ripl_Python_server = 8080;	    
	this.terminal_prompt = 'mit.pcp> ';
}

term_ripl.prototype.setCommand = function (event)
{
	  var originalElement = event.srcElement || event.originalTarget;
	  if (term.paused() == false)
	  {
	    command = originalElement.innerHTML;
	    term.set_command(command);
	  } else {
	    alert("Sorry, you can't send a command now.");
	  }
};   

term_ripl.prototype.sendCommandToRIPLServer = function (command, term)
{
	  term.pause();
/*	  $.post("http://ec2-50-16-19-74.compute-1.amazonaws.com:" + this.term_ripl_Python_server + "/", { directive: command },
	    function(data) {
	      //clearTimeout(updateWaitingTimer);

	      //$("#chat_inner").html(data.session_log);
	      //term.echo(data.my_response);
	      term.echo(data);
	      //current_state = "waiting_command";
	      //$('#status_div').html('To send command, please, press <i>Enter</i>');
	      term.resume();
	    }, 'text')
	  .error(function() {
	    alert('Oops, unfortunately, it seems some error has happened... Sorry...');
	    term.resume();
	  });
*/

	term.pause();
	var command = '('+command + ')';
	var parseList = parse(command);

	var ret;

	if (parseList[0] == 'ASSUME') {
		ret = this.ripl.assume(parseList[1] , parseList[2] ,1);
	}
	if (parseList[0] == 'OBSERVE') {
		ret = this.ripl.observe(parseList[1] , parseList[2] ,1);
	}
	if (parseList[0] == 'PREDICT') {
		ret = this.ripl.predict(parseList[1] ,1);
	}
	if (parseList[0] == 'CLEAR') {
		ret = this.ripl.clearTrace();
		ret = 'Trace has been cleared';
	}
	if (parseList[0] == 'INFER') {
		ret = this.ripl.infer(parseList[1],1);
	}
	if (parseList[0] == 'START_CONT_INFER') {
		ret = this.ripl.start_cont_infer(1);
	}
	if (parseList[0] == 'STOP_CONT_INFER') {
		ret = this.ripl.stop_cont_infer();
	}
	if (parseList[0] == 'FORGET') {
		ret = this.ripl.forget(parseList[1],1);
	}
	if (parseList[0] == 'CONT_INFER_STATUS') {
		ret = this.ripl.cont_infer_status();
	}
	if (parseList[0] == 'REPORT_DIRECTIVES') {
		ret = this.ripl.report_directives();
	}
	if (parseList[0] == 'REPORT_VALUE') {
		ret = this.ripl.report_value(parseList[1],1);
	}
	
	/*if (ret == undefined) {
		ret = 'Trace has been cleared';
	}*/
	
	term.echo(JSON.stringify(ret));
	
	
	term.resume(); 
};


term_ripl.prototype.sendCommand = function (event)
{
	  var originalElement = event.srcElement || event.originalTarget;
	  if (term.paused() == false)
	  {
	    command = originalElement.innerHTML;
	    term.echo(this.terminal_prompt + command);
	    this.sendCommandToRIPLServer(command, term);
	    //term.set_command(command);
	  } else {
	    alert("Sorry, you can't send a command now.");
	  }
};  

