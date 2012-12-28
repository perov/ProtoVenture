//Any new visualization module will have to extend this class and implement all the rendering/data-io logic
//Example (template) is provided in vis-template.js

var visualizer = Base.extend({
	constructor: function() {
		this.ripl = new ripl(); //look at ripl.js for public interface
	},
	
	//dynamically load any JS module which extens this and implements the abstract methods in this class. This can be called from a UI element 
	//ex. loadJS('file1.js'); //dynamically load and add this .js file
	loadJS: function (file) {
		var oHead = document.getElementsByTagName('HEAD').item(0);
		var oScript= document.createElement("script");
		oScript.type = "text/javascript";
		oScript.src=file;
		oHead.appendChild( oScript);
	},
	

	//check the current state of the probabilistic engine. If it is running an old code, get the directives IDs being sampled and return them (list on int's).
	//If not, then clearTrace.
	predicate: function(directive_name) {

		var directives = this.ripl.report_directives();
		if (directives == 'ERR') {
			console.log('\nClearing Trace ... \n');
			this.ripl.clearTrace();
			return 'ERR';
		}

		var currentDirectives = [];
		for (var i=0;i<directives.length;i++) {
			if (directives[i]["directive-type"] == "DIRECTIVE-ASSUME") {
				if (directives[i]["name"] == directive_name) {
					currentDirectives.push(directives[i]["value"]); //user program should report_value on this
				}
			}
		}
		
		if (currentDirectives.length > 0 ) {
			console.log('currentDirectives found:' + currentDirectives);
			return currentDirectives;
		}	
	},


	loadProgram: function() {
		console.log('Extended Class must implement this. This is the probabilistic program written in accordance with RIPL interface\n');
	},

	render: function(directives) {
		console.log('Extended Class must implement this. This should basically does report_directives and implement custom graphics functions in RaphaelJS/D3JS/webGL or any other browser graphing library \n');
	},

	run: function(directives) {
		console.log('Extended Class must implement this.\n');
	}
});



