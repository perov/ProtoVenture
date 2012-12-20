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
		expr='(uniform-discrete 0 2)'; 
		ret = this.ripl.assumeLog('model-type',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));
    
		expr='(uniform-continuous 0.001 0.3)'; 
		ret = this.ripl.assumeLog('outlier-prob',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));

		expr='(uniform-continuous 0.001 100.0)'; 
		ret = this.ripl.assumeLog('outlier-sigma',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));

		expr='(uniform-continuous 0.1 1.0)'; 
		ret = this.ripl.assumeLog('noise',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));

		// no need to clear as predicate function will clear if no directive found
		expr='(if (= model-type 0) 1 (if (= model-type 1) 0 (beta 1 1)))'; 
		ret = this.ripl.assumeLog('alpha',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));
    
		// no need to clear as predicate function will clear if no directive found
		expr='(uniform-discrete 0 4)'; 
		ret = this.ripl.assumeLog('poly-order',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));

		expr='(normal 0.0 10.0)';
		ret = this.ripl.assumeLog('a0-c0',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));

		expr='((lambda (value) (if (>= poly-order 1) value 0.0)) (normal 0.0 1.0))';
		ret = this.ripl.assumeLog('c1',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));

		expr='((lambda (value) (if (>= poly-order 2) value 0.0)) (normal 0.0 0.1))';
		ret = this.ripl.assumeLog('c2',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));

		expr='((lambda (value) (if (>= poly-order 3) value 0.0)) (normal 0.0 0.01))';
		ret = this.ripl.assumeLog('c3',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));

		expr='((lambda (value) (if (>= poly-order 4) value 0.0)) (normal 0.0 0.001))';
		ret = this.ripl.assumeLog('c4',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));

		// expr='(uniform-discrete 0 1)'; 
		// ret = this.ripl.assume('fouerier-count',expr);
		// console.log('ASSUME RET:' + JSON.stringify(ret));

		// expr='((lambda (value) (if (>= fouerier-count 0) value 0.0)) (normal 0.0 5.0))';
		// ret = this.ripl.assume('fouerier-a0',expr);
		// console.log('ASSUME RET:' + JSON.stringify(ret));

		// expr='((lambda (value) (if (>= fouerier-count 1) value 0.0)) (normal 0.0 10.0))';
		expr='(normal 0.0 5.0)';
		ret = this.ripl.assumeLog('fouerier-a1',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));

		// expr='((lambda (value) (if (>= fouerier-count 2) value 0.0)) (normal 0.0 1.0))';
		// ret = this.ripl.assume('fouerier-a2',expr);
		// console.log('ASSUME RET:' + JSON.stringify(ret));

		// expr='((lambda (value) (if (>= fouerier-count 3) value 0.0)) (normal 0.0 0.1))';
		// ret = this.ripl.assume('fouerier-a3',expr);
		// console.log('ASSUME RET:' + JSON.stringify(ret));

		expr='(uniform-continuous (/ 3.14 50) (* 3.14 1))';
		ret = this.ripl.assumeLog('fouerier-omega',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));

		expr='(uniform-continuous -3.14 3.14)';
		ret = this.ripl.assumeLog('fouerier-theta1',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));

		// expr='((lambda (value) (if (>= fouerier-count 2) value 0.0)) (uniform-continuous -3.14 3.14))';
		// ret = this.ripl.assume('fouerier-theta2',expr);
		// console.log('ASSUME RET:' + JSON.stringify(ret));

		// expr='((lambda (value) (if (>= fouerier-count 3) value 0.0)) (uniform-continuous -3.14 3.14))';
		// ret = this.ripl.assume('fouerier-theta3',expr);
		// console.log('ASSUME RET:' + JSON.stringify(ret));

		expr='(lambda (x) (+ (* c1 (power x 1.0)) (* c2 (power x 2.0))  (* c3 (power x 3.0)) (* c4 (power x 4.0))))';
		ret = this.ripl.assumeLog('clean-func-poly',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));

		//expr='(lambda (x) (+ fouerier-a0 (* fouerier-a1 (cos (+ (* fouerier-omega x 1) fouerier-theta1))) (* fouerier-a2 (cos (+ (* fouerier-omega x 2) fouerier-theta2))) (* fouerier-a3 (cos (+ (* fouerier-omega x 3) fouerier-theta3)))))';
		expr='(lambda (x)  (* fouerier-a1 (sin (+ (* fouerier-omega x 1) fouerier-theta1))))';
		ret = this.ripl.assumeLog('clean-func-fourier',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));

		expr='(lambda (x) (+ a0-c0 (* alpha  (clean-func-poly x)) (* (- 1 alpha) (clean-func-fourier x))))';
		ret = this.ripl.assumeLog('clean-func',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));

		ret = this.ripl.predict('(list poly-order a0-c0 c1 c2 c3 c4 noise alpha  fouerier-a1 fouerier-omega fouerier-theta1)');
		console.log('PREDICT RET:' + JSON.stringify(ret));

    predict_directive_id = String(ret['d_id']);
    
		expr=predict_directive_id;
		ret = this.ripl.assume(this.testname,expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));
    
    this.SendInformationAboutPoints();

		var currentDirectives = [];
		currentDirectives.push(predict_directive_id); //adding it to list so that render function can use it
		
		return currentDirectives;
	},


	render: function() {
		var directives = this.active_directives;
		for(var directive_id_for=0;directive_id_for<directives.length;directive_id_for++){
				var directive = directives[directive_id_for];
        var reported_directives = this.ripl.report_directives();
        //alert(typeof reported_directives)
        //console.log(reported_directives[0].value)
				var chain = reported_directives[directive - 1];
        //alert(reported_directives[0])
        
        for (i = 0; i < reported_directives.length; i++) {
          if (reported_directives[i].name != undefined) {
            if (reported_directives[i].name.substr(0, "is-outlier-".length) == "is-outlier-") {
              // console.log(ex.points_outliers[reported_directives[i].name]);
              // console.log(reported_directives[i].value);
              
              try {
                if (document.getElementById('IfShowCurves').checked == true) {
                  if (reported_directives[i].value == false) {
                    ex.points_outliers[reported_directives[i].name].circle.attr("fill", "red");
                    ex.points_outliers[reported_directives[i].name].circle2.attr("stroke", "red");
                    ex.points_outliers[reported_directives[i].name].circle2.attr("r", this.polynomials.noise * 10.0);
                  } else {
                    ex.points_outliers[reported_directives[i].name].circle.attr("fill", "#888888");
                    ex.points_outliers[reported_directives[i].name].circle2.attr("stroke", "#888888");
                    ex.points_outliers[reported_directives[i].name].circle2.attr("r", 5.0);
                  }
                } else {
                  ex.points_outliers[reported_directives[i].name].circle.attr("fill", "red");
                  ex.points_outliers[reported_directives[i].name].circle2.attr("stroke", "red");
                  ex.points_outliers[reported_directives[i].name].circle2.attr("r", 5.0);
                }
              } catch(err) {  }
            }
          }
        }
				
				var chain_element = new Object();
				chain_element.iteration_number = 2;
				chain_element.polynomial = new Object();
				chain_element.polynomial.degree = chain.value[0]
        this.polyordersqueue.push(chain.value[0])
        PolynomialDegreesInfo = new Array;
        var polydegreesinfo_size = 0;
        for (key in this.polyordersqueue) {
          if (PolynomialDegreesInfo[this.polyordersqueue[key]] != undefined) {
            PolynomialDegreesInfo[this.polyordersqueue[key]]++;
            //alert(PolynomialDegreesInfo[parseInt(key)]);
          } else {
            PolynomialDegreesInfo[this.polyordersqueue[key]] = 1;
          }
          polydegreesinfo_size++;
        }
        //console.log(PolynomialDegreesInfo[0], PolynomialDegreesInfo[1], PolynomialDegreesInfo[2], PolynomialDegreesInfo[3], PolynomialDegreesInfo[4], polydegreesinfo_size)
        if (polydegreesinfo_size > 100) { PolynomialDegreesInfo[this.polyordersqueue.shift()]--; polydegreesinfo_size--; }
        for (i = 0; i <= 4; i++) {
          if (PolynomialDegreesInfo[i] == undefined) {
            PolynomialDegreesInfo[i] = 0;
          }
          document.getElementById('poly' + i).style.height = Math.round(100 * PolynomialDegreesInfo[i] / polydegreesinfo_size) + 'px'
          console.log('poly' + i, Math.round(100 * PolynomialDegreesInfo[i] / polydegreesinfo_size))
        }
        
		    chain_element.polynomial.coefficients = new Array;
				for (var order = 0; order < 5; ++order) {
					chain_element.polynomial.coefficients[order] = chain.value[order+1];
				}
		    chain_element.noise = chain.value[6];
        chain_element.alpha = chain.value[7];
		    chain_element.fouerier_a = new Array;
        chain_element.fouerier_a[1] = chain.value[8];
        chain_element.fouerier_omega = chain.value[9];
		    chain_element.fouerier_theta = new Array;
        chain_element.fouerier_theta[1] = chain.value[10];
        // chain_element.fouerier_count = chain.value[8];
		    // chain_element.fouerier_a = new Array;
        // chain_element.fouerier_a[0] = chain.value[9];
        // chain_element.fouerier_a[1] = chain.value[10];
        // chain_element.fouerier_a[2] = chain.value[11];
        // chain_element.fouerier_a[3] = chain.value[12];
        // chain_element.fouerier_omega = chain.value[13];
		    // chain_element.fouerier_theta = new Array;
        // chain_element.fouerier_theta[1] = chain.value[14];
        // chain_element.fouerier_theta[2] = chain.value[15];
        // chain_element.fouerier_theta[3] = chain.value[16];

				this.polynomials = chain_element;	

		    		var coefficients = this.polynomials.polynomial.coefficients; // by reference would be more efficient here?

		    		/*var formula = "<i>y(x)</i> = " + String(coefficients[0].toFixed(3)).replace("-", "–");
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
			    	//$("#noise_value").html(noise_information);*/
            
            
            if (document.getElementById('IfShowCurves').checked == false) {
              for (var i = this.previous_polynomials_count; i > 0; i--) {
                  this.previous_polynomials[i] = this.previous_polynomials[i - 1];
                  iMult = i * 40;
                  this.previous_polynomials[i].attr("stroke", "#" + iMult.toString(16) + iMult.toString(16) + iMult.toString(16));
                  this.previous_polynomials[i].attr("stroke-opacity", "0.0");
              }
            } else {
              for (var i = this.previous_polynomials_count; i > 0; i--) {
                  this.previous_polynomials[i] = this.previous_polynomials[i - 1];
                  iMult = i * 40;
                  this.previous_polynomials[i].attr("stroke", "#" + iMult.toString(16) + iMult.toString(16) + iMult.toString(16));
              }
            }

			    	this.previous_polynomials[0] = this.DrawPolynomial(this.polynomials);
            if (document.getElementById('IfShowCurves').checked == false) {
              this.previous_polynomials[0].attr("stroke-opacity", "0.0");
            }
			    	if (this.previous_polynomials_count < 9) {
			      		this.previous_polynomials_count++;
			    	} else {
			      		this.previous_polynomials[9].remove();
			    	}


		}//end for
	},


	uploadPreviousObserves: function() {
			directives = this.ripl.report_directives();
	
			for(var i=0;i<directives.length;i++) {
				if(directives[i]['directive-type'] == 'DIRECTIVE-OBSERVE'){		
					var pp=parse(directives[i]['directive-expression']);
          console.log(pp)
					YY = pp[2];					
					XX = pp[1][1][3][1][1];
            		var point = new Object();
			    ex.points_outliers[ pp[1][1][1] ] = point;
			      		//point.x = XX - $('#div_for_plots').offset().left;
					//point.y = YY - $('#div_for_plots').offset().top;
					point.x = (20*XX) + (420/2);
					point.y = (420/2) - (20*YY);

					//alert(point.x + ' ' +point.y);
		      			point.circle2 = this.paper.circle(point.x, point.y, 5);
					point.circle2.attr("stroke", "red");
					point.circle2.attr("fill", "white");
					point.circle2.attr("opacity", "0.9");
					point.circle = this.paper.circle(point.x, point.y, 2);
					//point.circle2.attr("fill", "red");
					//point.circle2.attr("opacity", "0.5");
					//point.x = (point.x - (420 / 2)) / 20;
					//point.y = ((point.y - (420 / 2)) * -1) / 20;
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
								ex.ripl.forget(forgetdirectiveID-1);
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
								ex.ripl.forget(forgetdirectiveID-1);
						  	}
						}
						ex.SendInformationAboutPoints();
			      		});
			
					/*var expr='(normal (clean-func (normal '+ String(point.x) + ' noise)) noise)';
					var litval = String(point.y);
					var ret = ex.ripl.observe(expr,litval);
					console.log('OBSERVE RET:' + JSON.stringify(ret));*/

			      		point.unique_point_id = ex.next_unique_point_id;
			     	 	ex.next_unique_point_id++;
			      		ex.points[ex.points_number] = point;
					ex.points[ex.points_number].directive_id = directives[i]['directive-id']; 
			      		ex.points_number++;
			      		ex.SendInformationAboutPoints();
				}
			}
      this.SendInformationAboutPoints();
	},


	run: function(MODE) {
		this.active_directives = this.predicate(this.testname);

		// this can be controlled by GUI elements on the webpage (use Jquery to access UI elements). User defines this
		this.renderInit();

		if (1 == 1 || this.active_directives == 'ERR' || this.active_directives == undefined) {
			if (MODE) {
				//alert('Please load demo from dropdown menu. Engine has to run before trying to visualize data');
				return;
			}
			else {
				console.log('\n--------- Nothing running on the Engine. Starting from scratch -------\n');
				this.active_directives = this.loadProgram();
			}
		}
		else {
			console.log('\n------This example is currently running on the engine. No need to loadProgram. Can just send it to render for graphics ----------\n');
			this.uploadPreviousObserves();
		}
		
		//if continous inference is not on, turning it on
		ret = this.ripl.cont_infer_status();
		if (ret["status"] == "off") {
			ret = this.ripl.start_cont_infer(1);
		}

		this.SendInformationAboutPoints();
    //this.handle = new Object;
    //for (i = 1; i < 50; ++i) {
      // setTimeout("this.handle[i] = setInterval('ex.render();', 100);", i * 2);
    //}
    this.handle = setInterval('ex.render();', 1);
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


	CalculatePolynomial: function (x, poly) {
		// TODO: Via Horner's method it would be faster?
    var result_poly = 0;
    for (var i = 1; i < poly.polynomial.coefficients.length; ++i) {
      result_poly += poly.polynomial.coefficients[i] * Math.pow(x, i);
    }
    var result_f = 0;
    for (var i = 1; i <= 1; ++i) {
      result_f += poly.fouerier_a[i] * Math.sin(poly.fouerier_omega * i * x + poly.fouerier_theta[i]);
    }
    result = poly.polynomial.coefficients[0] + poly.alpha * result_poly + (1.0 - poly.alpha) * result_f;
    return result;
	},

	renderInit: function() {
		this.setupcustomhandlers(); //remove later
    
        this.polyordersqueue = [];

	    	this.polynomials = new Array();

	    	this.previous_polynomials = new Array();
	    	this.previous_polynomials_count = 0;

	    	this.points = new Array();
        this.points_outliers = new Object();
	    	this.points_number = 0;
	    	this.next_unique_point_id = 0;

	    	this.paper = Raphael(div_for_plots, 420, 420);
	    	this.paper_rect =  this.paper.rect(0, 0, 420, 420).attr({fill: "white"});
	    	this.paper_rect.click(function(e) {
	      		var point = new Object();
	      		point.x = e.pageX - $('#div_for_plots').offset().left;
			point.y = e.pageY - $('#div_for_plots').offset().top;
			//alert(point.x + ' ' +point.y);
      			point.circle2 = this.paper.circle(point.x, point.y, 5);
			point.circle2.attr("stroke", "red");
			point.circle2.attr("fill", "white");
			point.circle2.attr("opacity", "0.9");
			point.circle = this.paper.circle(point.x, point.y, 2);
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
						ex.ripl.forget(forgetdirectiveID-1);
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
						ex.ripl.forget(forgetdirectiveID-1);
				  	}
				}
				ex.SendInformationAboutPoints();
	      		});
			
      var pointTimeCreation = (new Date()).getTime() / 1000;
      
      var outlierName = "is-outlier-" + String(Math.round(pointTimeCreation * 1000));
      var expr = "(flip outlier-prob)";
      ex.ripl.assume(outlierName, expr)
      
			var expr='(normal (if ' + outlierName + ' 0 (clean-func (normal r['+ String(point.x.toFixed(5)) + '] noise))) (if ' + outlierName + ' outlier-sigma noise))';
      var litval = "r[" + String(point.y.toFixed(5)) + "]"
			var ret = ex.ripl.observe(expr,litval);
			console.log('OBSERVE RET:' + JSON.stringify(ret));
      
      ex.points_outliers[outlierName] = point;

	      		point.unique_point_id = ex.next_unique_point_id;
	     	 	ex.next_unique_point_id++;
	      		ex.points[ex.points_number] = point;
            ex.points[ex.points_number].observe_expr = "(ASSUME is-outlier-" + String(Math.round(pointTimeCreation * 1000)) + " (flip outlier-prob))<br>" + "(OBSERVE (normal (if " + outlierName + " 0 (clean-func (normal "+ String(point.x.toFixed(5)) + " noise)))   (if " + outlierName + " outlier-sigma noise)) " + String(point.y.toFixed(5)) + ")";
			
			ex.points[ex.points_number].directive_id = ret['d_id'];
      
	      		ex.points_number++;
			
	      		ex.SendInformationAboutPoints();
	    	});

	    	for (var x = 0; x <= 400; x = x + (400) / 10) {
		      	currentObject = this.paper.path("M" + (x + 10) + " 0L" + (x + 10) + " 420");
		      	currentObject.attr("stroke-dasharray", "-");
		      	currentObject.attr("stroke-width", "0.2");
		      	currentObject = this.paper.path("M0 " + (x + 10) + "L420 " + (x + 10) + "");
		      	currentObject.attr("stroke-dasharray", "-");
		      	currentObject.attr("stroke-width", "0.2");

		      	currentObject = this.paper.text(x + 10, 15, "" + (((x / 40) - 5) * 2) + "");
		      	currentObject.attr("fill", "#aaaaaa");

		      	if (x != 0) {
				currentObject = this.paper.text(10, x + 10, "" + (((x / 40) - 5) * -2) + "");
				currentObject.attr("fill", "#aaaaaa");
		      	}
	    	}

	    	currentObject = this.paper.circle(420 / 2, 420 / 2, 2);
	    	currentObject = this.paper.text(420 / 2 + 20, 420 / 2, "(0; 0)");
	    	currentObject.attr("fill", "#aaaaaa");
	},




	DrawPolynomial: function (poly) {
	 	var step = 1;
	      	var plot_y = (420 / 2) - this.CalculatePolynomial(-10, poly) * 20;
	      	var line_description = "M-10 " + plot_y;
	      	var plot_x1 = 0;
	      	var plot_y1 = 0;
	      	var plot_x = 0;
	      	var plot_y = 0;
	      	for (var x = -12 + step; x <= 12; x = x + step) {
			plot_x = (420 / 2) + x * 20;
			plot_y = (420 / 2) - this.CalculatePolynomial(x, poly) * 20;
			plot_x1 = (420 / 2) + (x + step / 2) * 20;
			plot_y1 = (420 / 2) - this.CalculatePolynomial(x + step / 2, poly) * 20;
			line_description += "Q" + plot_x + " " + plot_y + " " + plot_x1 + " " + plot_y1;
	      	}
	      	var path_object = this.paper.path(line_description);
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
        ex.church_code_str += this.ripl.assumes;
        
	      for (var i = 0; i < this.points_number; i++) {
          ex.church_code_str += this.points[i].observe_expr + "<br>";
	      }
        
        ex.church_code_str += this.ripl.predicts;
	      ex.church_code_str = ex.church_code_str.replace(/  /g, "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");

	      ex.church_code_str = ex.church_code_str.replace(/ /g, "&nbsp;");
	      ex.church_code_str = ex.church_code_str.replace(/\(if/g, "(<font color='#0000FF'>if</font>");
	      ex.church_code_str = ex.church_code_str.replace(/\(/g, "<font color='#0080D5'>(</font>");
	      ex.church_code_str = ex.church_code_str.replace(/\)/g, "<font color='#0080D5'>)</font>");
	      ex.church_code_str = ex.church_code_str.replace(/lambda/g, "<font color='#0000FF'>lambda</font>");
	      ex.church_code_str = ex.church_code_str.replace(/list/g, "<font color='#0000FF'>list</font>");

	      //ex.church_code_str = ex.church_code_str.replace(/\>=/g, "<font color='#0000FF'>>=</font>");
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
          <label for="IfShowCurves"><input type="checkbox" id="IfShowCurves" name="IfShowCurves" checked> Show curves</label>\
			    <div>\
				<div class="btn-group">\
					<button class="btn btn-danger">Engine Status</button>\
					<button class="btn btn-danger dropdown-toggle" data-toggle="dropdown"><span class="caret"></span></button>\
					<ul class="dropdown-menu">\
						<li><a href="#" onclick="ex.handle = setInterval(\'ex.render();\',1)">Turn Engine ON</a></li>\
					 	<li><a href="#" onclick="window.clearInterval(ex.handle)">Turn Engine OFF</a></li>\
					</ul>\
				</div>\
			    </div>\
			    <br>\
			    Based on <a href="http://projects.csail.mit.edu/church/wiki/Church">the Church probabilistic programming language</a>\
			  </td><td>&nbsp;&nbsp;&nbsp;</td>\
			  <td style="vertical-align: top;">\
          <table width="100%" height="120px">\
            <tr>\
              <td width="30px" valign="bottom"><b>Degree:</b></td>\
              <td width="30px" align="center" valign="bottom"><div id="poly0" style="width: 20px; height: 50px; background-color: #ee1111;"></div><br>0</td>\
              <td width="30px" align="center" valign="bottom"><div id="poly1" style="width: 20px; height: 50px; background-color: #ee1111;"></div><br>1</td>\
              <td width="30px" align="center" valign="bottom"><div id="poly2" style="width: 20px; height: 50px; background-color: #ee1111;"></div><br>2</td>\
              <td width="30px" align="center" valign="bottom"><div id="poly3" style="width: 20px; height: 50px; background-color: #ee1111;"></div><br>3</td>\
              <td width="30px" align="center" valign="bottom"><div id="poly4" style="width: 20px; height: 50px; background-color: #ee1111;"></div><br>4</td>\
              <td width="20px"></td>\
              <td valign="bottom" align="right">\
                <div style="text-align: left;">\
                To start/restart the demo, please:<br>\
                1) Click "Clear".<br>\
                2) Click "Load Demo" > "Bayesian Curve Fitting".\
                </div>\
              </td>\
            </tr>\
          </table>\
          <br>\
			    <div id="church_code"></div>\
          <div style="display: none;"><div style="display: none;"><input type="text" id="iteration_info" style="border:0; background-color: white;" value="" disabled></div>\
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



