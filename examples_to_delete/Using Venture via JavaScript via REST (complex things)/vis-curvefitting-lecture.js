//Example template which extends vis-module

//To run: 
//ex=new vistemplate()
//ex.run()


var vistemplate = visualizer.extend({

	testname: "bayesian-curve-fitting",

	constructor: function() {
		this.ripl = new ripl(); //look at ripl.js for public interface
		//graphibg.html has a <div> with class = vis-holder for any visualization to get plugged in. Always use this
		this.insertHTML('vis-holder');
	},

	loadProgram: function() {
		var ret, expr = NaN;

		this.ripl.clearTrace();

		// no need to clear as predicate function will clear if no directive found
		expr='(uniform-discrete 0 4)'; 
		ret = this.ripl.assume('order',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));

		expr='0';
		ret = this.ripl.assume('center-of',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));

		expr='0';
		ret = this.ripl.assume('a',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));


		expr='(if (>= order 0) (normal center-of 10) 0)';
		ret = this.ripl.assume('c0',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));

		expr='(if (>= order 1) (normal center-of 1) 0)';
		ret = this.ripl.assume('c1',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));

		expr='(if (>= order 2) (normal center-of 0.1) 0)';
		ret = this.ripl.assume('c2',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));

		expr='(if (>= order 3) (normal center-of 0.01) 0)';
		ret = this.ripl.assume('c3',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));

		expr='(if (>= order 4) (normal center-of 0.001) 0)';
		ret = this.ripl.assume('c4',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));

		expr='(uniform-continuous 0.1 1)';
		ret = this.ripl.assume('noise',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));

		expr='(lambda (x) (+ c0 (* c1 (power (- x a) 1)) (* c2 (power (- x a) 2)) (* c3 (power (- x a) 3)) (* c4 (power (- x a) 4)))))';
		ret = this.ripl.assume('clean-func',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));

		ret = this.ripl.predict('(list order c0 c1 c2 c3 c4 noise)');
		console.log('PREDICT RET:' + JSON.stringify(ret));

		var currentDirectives = [];
		currentDirectives.push('11'); //adding it to list so that render function can use it
		
		return currentDirectives;
	},


	render: function() {
		var directives = this.active_directives;
		for(var i=0;i<directives.length;i++){
				var directive = directives[i];
				var chain = this.ripl.report_value(directive);
				
				var chain_element = new Object();
				chain_element.iteration_number = 2;
				chain_element.polynomial = new Object();
				chain_element.polynomial.degree = chain.val[0]
		      		chain_element.polynomial.coefficients = new Array;
				for (var order = 0; order < 5; ++order) {
					chain_element.polynomial.coefficients[order] = chain.val[order+1];
				}
		      		chain_element.noise = chain.val[6];

				this.polynomials = chain_element;	

		    		var coefficients = this.polynomials.polynomial.coefficients; // by reference would be more efficient here?
		    		if (this.previous_drawn_polynomial) {
		      			this.previous_drawn_polynomial.remove();
		    		}

		    		var formula = "<i>y(x)</i> = " + String(coefficients[0].toFixed(3)).replace("-", "–");
		    		for (var i = 1; i <= this.polynomials.polynomial.degree; ++i) {
		      			if (coefficients[i] < 0) {
		        			formula += " – " + Math.abs(coefficients[i].toFixed(3));
		      			} else {
		        			formula += " + " + coefficients[i].toFixed(3);
		      			}
		      			formula += "<i>x</i>";
		      			if (i >= 2) {
		        			formula += "<sup>" + i + "</sup>";
		      			}
		    		}

		    		formula += "<sup>&nbsp;</sup>";
			    	$("#function_formula").html(formula);
			    	var noise_level = this.polynomials.noise;
			    	var noise_information = "<table><tr><td>Noise value: " + noise_level.toFixed(5);
			    	var noise_table_width = 300;
			    	noise_information += "</td><td>&nbsp;</td><td>";
			    	noise_information += "<table style='width: " + noise_table_width + "px; height: 10px; background-color: green;'><tr>";
			    	noise_information += "<td style='width: " + Math.round(noise_table_width * noise_level) + "px; background-color: red;'></td>";
			    	noise_information += "<td style='width: " + (300 - Math.round(noise_table_width * noise_level)) + "px; background-color: green;'></td>";
			    	noise_information += "</tr></table>";
			    	noise_information += "</td></tr></table>";
			    	//$("#noise_value").html(noise_information);
            
            for (var i = 0; i < ex.points_number; i++) {
              ex.points[i].circle2.attr("r", noise_level * 10.0);
            }
				
			    	for (var i = this.previous_polynomials_count; i > 0; i--) {
			      		this.previous_polynomials[i] = this.previous_polynomials[i - 1];
			      		iMult = i * 40;
			      		this.previous_polynomials[i].attr("stroke", "#" + iMult.toString(16) + iMult.toString(16) + iMult.toString(16));
			    	}
            console.log(noise_information);
			    	this.previous_polynomials[0] = this.DrawPolynomial(this.polynomials.polynomial.coefficients);
			    	if (this.previous_polynomials_count < 9) {
			      		this.previous_polynomials_count++;
			    	} else {
			      		this.previous_polynomials[9].remove();
			    	}


		}//end for
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
		this.SendInformationAboutPoints();
		this.handle = window.setInterval('ex.render();',15);
		console.log('\n Running Render [Report_directives on]:' + this.active_directives); 	
	},

	
	setupcustomhandlers: function() {
		$('#do_inference_button').attr('disabled', 'disabled'); // Not very wisely, because it is necessary to make it disabled as soon as possible ; And not through jQuery?
		$("#number_of_MH_burnin_iterations").change(function() {
			this.SendInformationAboutPoints(1);
		});
		$("#polynomial_degree").change(function() {
		    	this.SendInformationAboutPoints(1);
		});
		$("#church_precode").change(function() {
		    	this.SendInformationAboutPoints(1);
		});
		$("#where_start_the_burnin__from_prior").change(function() {
			this.SendInformationAboutPoints(1);
		});
		$("#where_start_the_burnin__from_last_state").change(function() {
			this.SendInformationAboutPoints(1);
		});
  		$("#noise_type__uni_cont").change(function() {
    			this.ChangeCoeffChurchCode();
  		});
  		$("#noise_type__beta1").change(function() {
    			this.ChangeCoeffChurchCode();
  		});
  		$("#noise_type__beta2").change(function() {
    			this.ChangeCoeffChurchCode();
  		});
  		$("#coeff_type__decr").change(function() {
    			this.ChangeCoeffChurchCode();
  		});
  		$("#coeff_type__ident_sigma10").change(function() {
    			this.ChangeCoeffChurchCode();
  		});
  		$("#coeff_type__ident_sigma1").change(function() {
    			this.ChangeCoeffChurchCode();
  		});
  		$("#coeff_type__incr").change(function() {
    			this.ChangeCoeffChurchCode();
  		});
  		$("#coeff_type__ident_uniform").change(function() {
    			this.ChangeCoeffChurchCode();
  		});
  		$("#center_is_random").change(function() {
    			this.ChangeCoeffChurchCode();
  		});
  		$("#a_is_random").change(function() {
    			this.ChangeCoeffChurchCode();
  		});
  		$('#do_inference_button').click(function() {
    			$('#do_inference_button').attr('disabled', 'disabled');
  		});
	},


	CalculatePolynomial: function (x, coefficients) {
		// TODO: Via Horner's method it would be faster?
	     	var result = coefficients[0];
	      	for (var i = 1; i < coefficients.length; ++i) {
			result += coefficients[i] * Math.pow(x, i);
	      	}
	      	return result;
	},

	renderInit: function() {
		this.setupcustomhandlers(); //remove later

	    	this.polynomials = new Array();
	    	this.previous_drawn_polynomial = null;

	    	this.previous_polynomials = new Array();
	    	this.previous_polynomials_count = 0;

	    	this.points = new Array();
	    	this.points_number = 0;
	    	this.next_unique_point_id = 0;

	    	paper = Raphael(div_for_plots, 420, 420);
	    	paper_rect = paper.rect(0, 0, 420, 420).attr({fill: "white"});
	    	paper_rect.click(function(e) {
	      		var point = new Object();
	      		point.x = e.pageX - $('#div_for_plots').offset().left;
			point.y = e.pageY - $('#div_for_plots').offset().top;
      point.circle2 = paper.circle(point.x, point.y, 5);
			point.circle2.attr("stroke", "red");
			point.circle2.attr("fill", "white");
			point.circle2.attr("opacity", "0.9");
			point.circle = paper.circle(point.x, point.y, 2);
			//point.circle2.attr("fill", "red");
			//point.circle2.attr("opacity", "0.5");
			point.x = (point.x - (420 / 2)) / 20;
			point.y = ((point.y - (420 / 2)) * -1) / 20;
			point.circle.attr("fill", "red");
			point.circle.attr("stroke", "white");
			point.circle.attr("opacity", "0.5");
			point.circle.data("unique_point_id", ex.next_unique_point_id);
			point.circle2.data("unique_point_id", point.circle.data("unique_point_id"));
			point.circle.click(function(e) {
				for (var i = 0; i < ex.points_number; i++) {
					if (ex.points[i].unique_point_id == this.data("unique_point_id")) {
            ex.points[i].circle2.remove();
						forgetdirectiveID = ex.points[i].directive_id;
						for (var j = (i + 1); j < ex.points_number; j++) {
				      			ex.points[j - 1] = ex.points[j];
						}
				    		ex.points_number--;
				    		this.remove();
						ex.ripl.forget(forgetdirectiveID);
				  	}
				}
				ex.SendInformationAboutPoints();
	      		});
			point.circle2.click(function(e) {
				for (var i = 0; i < ex.points_number; i++) {
					if (ex.points[i].unique_point_id == this.data("unique_point_id")) {
            ex.points[i].circle.remove();
						forgetdirectiveID = ex.points[i].directive_id;
						for (var j = (i + 1); j < ex.points_number; j++) {
				      			ex.points[j - 1] = ex.points[j];
						}
				    		ex.points_number--;
				    		this.remove();
						ex.ripl.forget(forgetdirectiveID);
				  	}
				}
				ex.SendInformationAboutPoints();
	      		});
			
			var expr='(normal (clean-func (normal '+ String(point.x) + ' noise)) noise)';
			var litval = String(point.y);
			var ret = ex.ripl.observe(expr,litval);
			console.log('OBSERVE RET:' + JSON.stringify(ret));

	      		point.unique_point_id = ex.next_unique_point_id;
	     	 	ex.next_unique_point_id++;
	      		ex.points[ex.points_number] = point;
			ex.points[ex.points_number].directive_id = ret['d_id'];
	      		ex.points_number++;
			
	      		ex.SendInformationAboutPoints();
	    	});

	    	for (var x = 0; x <= 400; x = x + (400) / 10) {
		      	currentObject = paper.path("M" + (x + 10) + " 0L" + (x + 10) + " 420");
		      	currentObject.attr("stroke-dasharray", "-");
		      	currentObject.attr("stroke-width", "0.2");
		      	currentObject = paper.path("M0 " + (x + 10) + "L420 " + (x + 10) + "");
		      	currentObject.attr("stroke-dasharray", "-");
		      	currentObject.attr("stroke-width", "0.2");

		      	currentObject = paper.text(x + 10, 15, "" + (((x / 40) - 5) * 2) + "");
		      	currentObject.attr("fill", "#aaaaaa");

		      	if (x != 0) {
				currentObject = paper.text(10, x + 10, "" + (((x / 40) - 5) * -2) + "");
				currentObject.attr("fill", "#aaaaaa");
		      	}
	    	}

	    	currentObject = paper.circle(420 / 2, 420 / 2, 2);
	    	currentObject = paper.text(420 / 2 + 20, 420 / 2, "(0; 0)");
	    	currentObject.attr("fill", "#aaaaaa");
	},




	DrawPolynomial: function (coefficients) {
	 	var step = 1;
	      	var plot_y = (420 / 2) - this.CalculatePolynomial(-10, coefficients) * 20;
	      	var line_description = "M-10 " + plot_y;
	      	var plot_x1 = 0;
	      	var plot_y1 = 0;
	      	var plot_x = 0;
	      	var plot_y = 0;
	      	for (var x = -12 + step; x <= 12; x = x + step) {
			plot_x = (420 / 2) + x * 20;
			plot_y = (420 / 2) - this.CalculatePolynomial(x, coefficients) * 20;
			plot_x1 = (420 / 2) + (x + step / 2) * 20;
			plot_y1 = (420 / 2) - this.CalculatePolynomial(x + step / 2, coefficients) * 20;
			line_description += "Q" + plot_x + " " + plot_y + " " + plot_x1 + " " + plot_y1;
	      	}
	      	var path_object = paper.path(line_description);
	      	return path_object;
	},



	ChangeCoeffChurchCode:  function () {
	    church_precode_to_rewrite = "";
	    if ($("#center_is_random").prop("checked") == true) {
	      church_precode_to_rewrite += "(ASSUME center-of (uniform-continuous -10 10))\n";
	    } else {
	      church_precode_to_rewrite += "(ASSUME center-of 0)\n";
	    }
	    if ($("#a_is_random").prop("checked") == true) {
	      church_precode_to_rewrite += "(ASSUME a (uniform-continuous -10 10))\n";
	    } else {
	      church_precode_to_rewrite += "(ASSUME a 0)\n";
	    }
	    if ($("#coeff_type__decr").prop("checked") == true) {
	      church_precode_to_rewrite += "(ASSUME c0 (if (>= order 0) (normal center-of 10) 0))\n";
	      church_precode_to_rewrite += "(ASSUME c1 (if (>= order 1) (normal center-of 1) 0))\n";
	      church_precode_to_rewrite += "(ASSUME c2 (if (>= order 2) (normal center-of 0.1) 0))\n";
	      church_precode_to_rewrite += "(ASSUME c3 (if (>= order 3) (normal center-of 0.01) 0))\n";
	      church_precode_to_rewrite += "(ASSUME c4 (if (>= order 4) (normal center-of 0.001) 0))\n"; }
	    if ($("#coeff_type__ident_sigma10").prop("checked") == true) {
	      church_precode_to_rewrite += "(ASSUME c0 (if (>= order 0) (normal center-of 10) 0))\n";
	      church_precode_to_rewrite += "(ASSUME c1 (if (>= order 1) (normal center-of 10) 0))\n";
	      church_precode_to_rewrite += "(ASSUME c2 (if (>= order 2) (normal center-of 10) 0))\n";
	      church_precode_to_rewrite += "(ASSUME c3 (if (>= order 3) (normal center-of 10) 0))\n";
	      church_precode_to_rewrite += "(ASSUME c4 (if (>= order 4) (normal center-of 10) 0))\n"; }
	    if ($("#coeff_type__ident_sigma1").prop("checked") == true) {
	      church_precode_to_rewrite += "(ASSUME c0 (if (>= order 0) (normal center-of 1) 0))\n";
	      church_precode_to_rewrite += "(ASSUME c1 (if (>= order 1) (normal center-of 1) 0))\n";
	      church_precode_to_rewrite += "(ASSUME c2 (if (>= order 2) (normal center-of 1) 0))\n";
	      church_precode_to_rewrite += "(ASSUME c3 (if (>= order 3) (normal center-of 1) 0))\n";
	      church_precode_to_rewrite += "(ASSUME c4 (if (>= order 4) (normal center-of 1) 0))\n"; }
	    if ($("#coeff_type__incr").prop("checked") == true) {
	      church_precode_to_rewrite += "(ASSUME c0 (if (>= order 0) (normal center-of 0.001) 0))\n";
	      church_precode_to_rewrite += "(ASSUME c1 (if (>= order 1) (normal center-of 0.01) 0))\n";
	      church_precode_to_rewrite += "(ASSUME c2 (if (>= order 2) (normal center-of 0.1) 0))\n";
	      church_precode_to_rewrite += "(ASSUME c3 (if (>= order 3) (normal center-of 1) 0))\n";
	      church_precode_to_rewrite += "(ASSUME c4 (if (>= order 4) (normal center-of 10) 0))\n"; }
	    if ($("#coeff_type__ident_uniform").prop("checked") == true) {
	      church_precode_to_rewrite += "(ASSUME f (lambda () (+ (* (- (beta 3 3) 0.5) 20) center-of)))\n";
	      church_precode_to_rewrite += "(ASSUME c0 (if (>= order 0) (f) 0))\n";
	      church_precode_to_rewrite += "(ASSUME c1 (if (>= order 1) (f) 0))\n";
	      church_precode_to_rewrite += "(ASSUME c2 (if (>= order 2) (f) 0))\n";
	      church_precode_to_rewrite += "(ASSUME c3 (if (>= order 3) (f) 0))\n";
	      church_precode_to_rewrite += "(ASSUME c4 (if (>= order 4) (f) 0))\n"; }
	    if ($("#noise_type__uni_cont").prop("checked") == true) {
	      church_precode_to_rewrite += "(ASSUME noise (uniform-continuous 0.1 1))";
	    }
	    if ($("#noise_type__beta1").prop("checked") == true) {
	      church_precode_to_rewrite += "(ASSUME noise (beta 1 3))";
	    }
	    if ($("#noise_type__beta2").prop("checked") == true) {
	      church_precode_to_rewrite += "(ASSUME noise (+ (* (beta 1 3) 0.9) 0.1))";
	    }
	    $("#church_precode").val(church_precode_to_rewrite);
	    $("#church_precode").change();
	},




	SendInformationAboutPoints:  function () {

	      ex.church_code_str = "<b>Church code:</b><br>";
	      ex.church_code_str += "(ASSUME order (uniform-discrete 0 " + $("#polynomial_degree").val() + "))<br>";
	      ex.church_code_str += "(ASSUME center-of 0)<br>";
	      ex.church_code_str += "(ASSUME a 0)<br>";

	      ex.church_code_str += "(ASSUME c0 (if (>= order 0) (normal center-of 10) 0))<br>";
	      ex.church_code_str += "(ASSUME c1 (if (>= order 1) (normal center-of 1) 0))<br>";
	      ex.church_code_str += "(ASSUME c2 (if (>= order 2) (normal center-of 0.1) 0))<br>";
	      ex.church_code_str += "(ASSUME c3 (if (>= order 3) (normal center-of 0.01) 0))<br>";
	      ex.church_code_str += "(ASSUME c4 (if (>= order 4) (normal center-of 0.001) 0))<br>";
        
	      ex.church_code_str += "(ASSUME clean-func<br>";
	      ex.church_code_str += "  (lambda (x)<br>";
	      ex.church_code_str += "    (+ c0 (* c1 (power (- x a) 1)) (* c2 (power (- x a) 2))<br>";
	      ex.church_code_str += "      (* c3 (power (- x a) 3)) (* c4 (power (- x a) 4)))))<br>";
	      for (var i = 0; i < this.points_number; i++) {
		ex.church_code_str += "(OBSERVE (normal (clean-func (normal " + this.points[i].x + " noise)) noise) " + this.points[i].y + ")<br>";
	      }

	      ex.church_code_str += "(PREDICT (list order c0 c1 c2 c3 c4 noise))<br>";
	      ex.church_code_str = ex.church_code_str.replace(/ /g, "&nbsp;");
	      ex.church_code_str = ex.church_code_str.replace(/\(if/g, "(<font color='#0000FF'>if</font>");
	      ex.church_code_str = ex.church_code_str.replace(/\(/g, "<font color='#0080D5'>(</font>");
	      ex.church_code_str = ex.church_code_str.replace(/\)/g, "<font color='#0080D5'>)</font>");
	      ex.church_code_str = ex.church_code_str.replace(/lambda/g, "<font color='#0000FF'>lambda</font>");
	      ex.church_code_str = ex.church_code_str.replace(/list/g, "<font color='#0000FF'>list</font>");

	      ex.church_code_str = ex.church_code_str.replace(/\>=/g, "<font color='#0000FF'>>=</font>");
	      ex.church_code_str = ex.church_code_str.replace(/\+/g, "<font color='#0000FF'>+</font>");
	      ex.church_code_str = ex.church_code_str.replace(/\*/g, "<font color='#0000FF'>*</font>");
	      ex.church_code_str = "<font face='Courier New' size='2'>" + ex.church_code_str + "</font>";
	      $("#church_code").html(ex.church_code_str);
	},

	

	insertHTML: function(divclass) {
		$('div.'+divclass).html('\
			<h3><a href="https://github.com/perov/simpleinterpreter/">Bayesian Curve Fitting Demo</a></h3>\
			<div id="working_space" style="display: ;">\
			  <table><tr><td style="vertical-align: top;">\
			    <div id="div_for_plots" style="background-color: white; width: 420px; height: 420px;"></div>\
			    <br>\
			    <div>\
				<div class="btn-group">\
					<button class="btn btn-danger">Engine Status</button>\
					<button class="btn btn-danger dropdown-toggle" data-toggle="dropdown"><span class="caret"></span></button>\
					<ul class="dropdown-menu">\
						<li><a href="#" onclick="ex.handle = window.setInterval(\'ex.render();\',15)">Turn Engine ON</a></li>\
					 	<li><a href="#" onclick="window.clearInterval(ex.handle)">Turn Engine OFF</a></li>\
					</ul>\
				</div>\
			    </div>\
			    <br>\
			    Based on <a href="http://projects.csail.mit.edu/church/wiki/Church">the Church probabilistic programming language</a>\
			  </td><td>&nbsp;&nbsp;&nbsp;</td>\
			  <td style="vertical-align: top;">\
			    <div id="terminalWindow"></div>\
          <div style="display: none;"><div id="church_code"></div><div style="display: none;"><input type="text" id="iteration_info" style="border:0; background-color: white;" value="" disabled></div>\
			    <div id="slider" style="width: 500px; font-size: 10; display: none;"></div>\
			    <div id="function_formula"></div>\
			    <div id="noise_value"></div>\
			    Number of MH burn-in iterations: <input type="text" id="number_of_MH_burnin_iterations" value="100" style="width: 50px;">\
			    <input type="hidden" id="number_of_steps_to_show" value="1">\
			    start from\
			    <input type="radio" id="where_start_the_burnin__from_prior" name="where_start_the_burnin" value="from_prior"> the prior\
			    <input type="radio" id="where_start_the_burnin__from_last_state" name="where_start_the_burnin" value="from_last_state" checked> the last state\
			    <br>Polynomial degree (from 0 to 4 inclusive): <input type="text" id="polynomial_degree" value="4" style="width: 20px;">\
			    <br>\
			    <table><tr><td valign="top" style="width: 450px;">\
			    <br><br></div>\
			    </td><td>&nbsp;</td>\
			    <td valign="top">\
			      <small>\
			      Noise type:\
				<input type="radio" name="noise_type" id="noise_type__uni_cont" checked> U[0.1; 1.0]\
				<input type="radio" name="noise_type" id="noise_type__beta1"> &beta;(1, 3)\
				<input type="radio" name="noise_type" id="noise_type__beta2"> &beta;(1, 3) * 0.9 + 0.1\
			      </small>\
			      <br>\
			      <input type="radio" name="coeff_type" id="coeff_type__decr" checked> Decreasing coefficients (normal)<br>\
			      <input type="radio" name="coeff_type" id="coeff_type__ident_sigma10"> Identical coefficients (normal, sigma = 10)<br>\
			      <input type="radio" name="coeff_type" id="coeff_type__ident_sigma1"> Identical coefficients (normal, sigma = 1)<br>\
			      <input type="radio" name="coeff_type" id="coeff_type__incr"> Increasing coefficients (normal)<br>\
			      <input type="radio" name="coeff_type" id="coeff_type__ident_uniform"> Via &beta;<br>\
			      <input type="checkbox" id="center_is_random"> If the center of distribution on coefficients is random<br>\
			      <input type="checkbox" id="a_is_random" disabled> If the center of polynomial is random\
			    </td></tr>\
			    </table>\
			    <br><div id="polynomialListbox" style="display: none;"></div>\
			    <br><input type="button" value="Do inference" id="do_inference_button" style="display: none;">\
			  </td></tr></table>\
			</div>\
			<div id="forced_disconnection" style="display: none;">\
			  Oh, no, you has been disconnected from the SimpleInterpreter cloud.<br>\
			  It is not very good. I am sorry about it.<br>\
			  Please, contact Yura Perov (<a href="mailto:yura.perov@gmail.com">yura.perov@gmail.com</a>)\
			  about this issue.\
			</div>\
		');
	}	

});



