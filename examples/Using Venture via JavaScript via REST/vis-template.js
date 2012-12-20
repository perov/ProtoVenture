//Example template which extends vis-module

//To run: 
//ex=new vistemplate()
//ex.run()


var vistemplate = visualizer.extend({

	testname: "positive-mammogram-test",

	loadProgram: function() {
		var ret, expr = NaN;

		// no need to clear as predicate function will clear if no directive found
		expr='(flip 0.01)'; 
		ret = this.ripl.assume('breast-cancer',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));

		expr='(lambda () (if breast-cancer (flip 0.8) (flip 0.096)))';
		ret = this.ripl.assume('positive-mammogram',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));

		expr='(noise-negate (positive-mammogram) 0.00001)';
		var litval = 'true';
		ret = this.ripl.observe(expr,litval);
		console.log('OBSERVE RET:' + JSON.stringify(ret));

		ret = this.ripl.infer(1000,1);
		console.log('INFER RET:' + JSON.stringify(ret));

		ret = this.ripl.predict('(positive-mammogram)');
		console.log('PREDICT RET:' + JSON.stringify(ret));

		//Here we basically put a reference for all the directives we need to report after continous inference. This is useful because it serves as a flag for the 'predicate' function to check the current state of engine. If directives for the current test are present in the trace, we just use these and report! No need to clear and re-run the program. 
		//This is done by synthetically inserting an ASSUME value with <name> as the test-name and <expr> as directive-id value of the thing we want to report

		expr='4'; //4 is directive-ID for (PREDICT (positive-mammogram)) ... directive which we want to report! 
		ret = this.ripl.assume(this.testname,expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));
		var currentDirectives = [];
		currentDirectives.push(expr); //adding it to list so that render function can use it
		
		return currentDirectives;
	},


	render: function(directives) {
		//custom graphics logic
		//Now user can just loop through directives, and do this.report_value(directives[..]) at every time instance in animation to get data. That's it!
	},



	run: function(directives) {
		//check engine status to see if this test is already running or not
		var active_directives = this.predicate(this.testname);

		if (active_directives == 'ERR') {
			console.log('\n--------- Nothing running on the Engine. Starting from scratch -------\n');
			active_directives = this.loadProgram();
		}
		else {
			console.log('\n------This example is currently running on the engine. No need to loadProgram. Can just send it to render for graphics ----------\n');
		}
	
		//if continous inference is not on, turning it on
		ret = this.ripl.cont_infer_status();
		if (ret["status"] == "off") {
			ret = this.ripl.start_cont_infer(1);
		}

		// this can be controlled by GUI elements on the webpage (use Jquery to access UI elements). User defines this
		this.render(active_directives);	
		console.log('\n Running Render [Report_directives on]:' + active_directives); 	

	}


});



