//Example template which extends vis-module

//To run: 
//ex=new vistemplate()
//ex.run()


var vistemplate = visualizer.extend({

	testname: "timeseries",

	constructor: function() {
		this.ripl = new ripl(); //look at ripl.js for public interface
		//graphibg.html has a <div> with class = vis-holder for any visualization to get plugged in. Always use this
		this.insertHTML('vis-holder');
	},

	loadProgram: function() {
		var ret, expr = NaN;

		this.ripl.clearTrace();

		// no need to clear as predicate function will clear if no directive found
		expr='(normal 100.0 5.0)'; 
		ret = this.ripl.assume('initial',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));
 
		expr='(flip)'; 
		ret = this.ripl.assume('initial-state',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));

		expr='(normal 0.0 3.0)'; 
		ret = this.ripl.assume('true-drift-mu',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));

		expr='(uniform-continuous 0.1 50.0)'; 
		ret = this.ripl.assume('true-drift-sigma',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));

		expr='(normal 0.0 3.0)'; 
		ret = this.ripl.assume('false-drift-mu',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));

		expr='(uniform-continuous 0.1 50.0)'; 
		ret = this.ripl.assume('false-drift-sigma',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));

		expr='(beta 1.0 0.1)'; 
		ret = this.ripl.assume('true-true-transition-prob',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));

		expr='(beta 0.1 1.0)'; 
		ret = this.ripl.assume('false-true-transition-prob',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));


		expr='(mem (lambda (t) (if (= t 0) initial-state (if (get-state (- t 1)) (bernoulli true-true-transition-prob) (bernoulli false-true-transition-prob)))))'; 
		ret = this.ripl.assume('get-state',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));


		expr='(lambda (t) (if (get-state t) true-drift-mu false-drift-mu ))'; 
		ret = this.ripl.assume('drift-mu',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));


		expr='(lambda (t) (if (get-state t) true-drift-sigma false-drift-sigma ))'; 
		ret = this.ripl.assume('drift-sigma',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));


		expr='(mem (lambda (t) (if (= t 0) initial (normal (+ (drift-mu t) (get-value (- t 1)) ) (drift-sigma t) ) ) ) )'; 
		ret = this.ripl.assume('get-value',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));


		expr='get-value'; 
		ret = this.ripl.assume('clean-timeseries',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));


		expr='(mem (lambda (t) (clean-timeseries t)) )'; 
		ret = this.ripl.assume('timeseries-result',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));
		
		var currentDirectives = [];
		for (var i=0;i<10;++i) {
			expr='(timeseries-result ' + String(i) + ')';
			console.log(expr);
			ret = this.ripl.predict(expr);
			console.log('PREDICT RET:' + JSON.stringify(ret));
			currentDirectives.push(String(i+15)); //adding it to list so that render function can use it
		}

		
		return currentDirectives;
	},


	render: function() {
		var directives = this.active_directives;
		var data=[],arr = [];
		for(var i=0;i<directives.length;i++){
				var directive = directives[i];
				var sample = this.ripl.report_value(directive);
				//console.log(sample);
  				arr.push({x: i, y: sample.val }); //($('svg').attr('width')/2) 
		}//end for

		//calculate mean of data and subtract for alignment
		var sumx=0,sumy=0;
		for (var i=0;i<arr.length;i++) {
			sumx = sumx + arr[i].x;
			sumy = sumx + arr[i].y;
		}
		var avgx = sumx / arr.length;
		var avgy = sumy / arr.length;

		var WD = $('svg').attr('width');
		var HT = $('svg').attr('height');

		this.YAVG = avgy;

		for (var i=0;i<arr.length;i++) {
			arr[i].x = 40*arr[i].x+10;
			if (avgy > HT/2) {
				var offset = avgy - (HT/2);
				arr[i].y = arr[i].y- offset; //+ ($('svg').attr('height')/2);
			}
			else {
				var offset = (HT/2) - avgy;
				arr[i].y = offset - arr[i].y; //+ ($('svg').attr('height')/2);
			}
		}		

		///// RENDER data
		data.push(arr)
        	var line = d3.svg.line()
            		.x(function(d){return d.x;})
            		.y(function(d){return d.y;});

        	var g = this.svg.selectAll('g')
            		.data(data)

        	g.enter()
            		.append('svg:g')
        	g.exit()
            		.remove();

        	// LINES
        	var paths = g.selectAll('path')
            		.data(function(d) { return [d]; })
                	.attr('d', line);
        	paths
            	.enter()
                	.append('svg:path')
                	.attr('d', line);

        	// CIRCLES
        	var circles = g.selectAll('circle')
            		.data(function(d) { return d; })
                	.attr('cx', function(d){return d.x;})
                	.attr('cy', function(d){return d.y;});
       		circles
            		.enter()
                	.append('svg:circle')
                	.attr('r', 2)
                	.attr('cx', function(d){return d.x;})
                	.attr('cy', function(d){return d.y;});



		/*var x = d3.scale.linear().domain([0, 8]).range([0, 400]);
		var y = d3.scale.linear().domain([0, 200]).range([290, 10]);
		
		var xAxis = d3.svg.axis().scale(x).tickSize(-300).tickSubdivide(true);
		this.svg.append("svg:g")
			      .attr("class", "x axis")
			      .attr("transform", "translate(0," + 300 + ")")
			      .call(xAxis);

		// create left yAxis
		var yAxisLeft = d3.svg.axis().scale(y).ticks(4).orient("left");
			// Add the y-axis to the left
		this.svg.append("svg:g")
			      .attr("class", "y axis")
			      .attr("transform", "translate(20,0)")
			      .call(yAxisLeft);*/


	},


	run: function(directives) {
		console.log('\n--------- For this example, must starting from scratch as directive changes -------\n');
		this.active_directives = this.loadProgram();

	
		//if continous inference is not on, turning it on
		ret = this.ripl.cont_infer_status();
		if (ret["status"] == "off") {
			ret = this.ripl.start_cont_infer(1);
		}

		// this can be controlled by GUI elements on the webpage (use Jquery to access UI elements). User defines this
		this.renderInit();
		this.handle = window.setInterval('ex.render();',15);
		//for (var i=0;i<1;i++) ex.render();
		console.log('\n Running Render [Report_directives on]:' + this.active_directives); 	
	},


	renderInit: function() {
    		this.svg = d3.select('#chart').append('svg')
				 .on("mousedown", this.update)
				 .attr("width", 400)
        			 .attr("height", 300);

		this.arrStorage = [];
		this.YAVG = 0;
		for(var i=0;i<this.active_directives;i++) {
			this.arrStorage.push(40*i + 10);
		}
	},


	getClosestValues: function (a, x) {
	    var lo, hi;
	    for (var i = a.length; i--;) {
		if (a[i] <= x && (lo === undefined || lo < a[i])) lo = a[i];
		if (a[i] >= x && (hi === undefined || hi > a[i])) hi = a[i];
	    };
	    return lo;
	},

	update: function () {
		vertices = d3.mouse(this);
		var xx = ex.getClosestValues(ex.arrStorage,vertices[0]);
		ex.svg.append("svg:circle")
			.style("stroke", "black")
			.style("fill", "red")
			.attr("r", 2)
			.attr("cx", vertices[0])
			.attr("cy", vertices[1])
	

		var x = parseInt(vertices[0]);
		x=(x-10)/40;
		x=ex.getClosestValues(ex.arrStorage);
		var y = parseFloat(vertices[1]);

		var WD = $('svg').attr('width');
		var HT = $('svg').attr('height');

		if (ex.YAVG > HT/2) {
			var offset = ex.YAVG - (HT/2);
			y = y- offset;
		}
		else {
			var offset = (HT/2) - ex.YAVG;
			y = offset - y; //+ ($('svg').attr('height')/2);
		}
			
		var expr='(normal (timeseries-result ' + String(x) + ') 10.0)';
		var litval = String(y);
		var ret = ex.ripl.observe(expr,litval);
		console.log('OBSERVE RET:' + JSON.stringify(ret));
	},


	insertHTML: function(divclass) {
		$('div.'+divclass).html('\
			<h3><a href="https://github.com/perov/simpleinterpreter/">Timeseries Demo</a></h3>\
			<div style="background-color:LavenderBlush;width:400px;" id="chart"></div>\
			<style>\
			path {\
				stroke: steelblue;\
				stroke-width: 1;\
				fill: none;\
				}\
				.axis {\
				  shape-rendering: crispEdges;\
				}\
				.x.axis line {\
				  stroke: lightgrey;\
				}\
				.x.axis .minor {\
				  stroke-opacity: .5;\
				}\
				.x.axis path {\
				  display: none;\
				}\
				.y.axis line, .y.axis path {\
				  fill: none;\
				  stroke: #000;\
				}\
			</style>'
		);

	}	

});



