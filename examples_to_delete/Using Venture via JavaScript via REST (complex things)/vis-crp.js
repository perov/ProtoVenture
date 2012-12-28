//Example template which extends vis-module

//To run: 
//ex=new vistemplate()
//ex.run()


var vistemplate = visualizer.extend({

	testname: "crp-mixture",

	constructor: function() {
		this.ripl = new ripl(); //look at ripl.js for public interface
		//graphibg.html has a <div> with class = vis-holder for any visualization to get plugged in. Always use this
		this.insertHTML('vis-holder');

	},

	loadProgram: function() {
		var ret, expr = NaN;
		
		this.ripl.clearTrace();

//MyRIPL.assume('alpha', lisp_parser.parse(""))
//MyRIPL.assume('cluster-crp', lisp_parser.parse("(CRP/make alpha)"))

//MyRIPL.assume('get-cluster-mean', lisp_parser.parse("(mem (lambda (cluster) (gaussian 0 10)))"))
//MyRIPL.assume('get-cluster-variance', lisp_parser.parse("(mem (lambda (cluster) (gamma 0.1 100)))"))
/*
data_size = len(data)
for i in range(data_size):
	#MyRIPL.assume("datapoint" + str(i), lisp_parser.parse("(get-datapoint " + str(data[i]) + ")")) 
	#pdb.set_trace()
	MyRIPL.observe(lisp_parser.parse("(gaussian (get-datapoint " + str(data[i]) + " ) 0.000000001)"), 0.34)

for t in range(data_size):
        samples = []
	sample = MyRIPL.predict(lisp_parser.parse("(get-cluster-mean (get-cluster " + str(data[t]) + "))"));
	print sample
	directives.append(sample[0])
*/
  
// MyRIPL.assume('alpha', lisp_parser.parse("(gamma 0.1 20)"))
// MyRIPL.assume('cluster-crp', lisp_parser.parse("(CRP/make alpha)"))

		expr='(uniform-continuous 0.0001 2.0)'; // (uniform-continuous 0.1 2.0)
		ret = this.ripl.assumeLog('alpha',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));
    
		expr='(CRP/make alpha)';
		ret = this.ripl.assumeLog('cluster-crp',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));
    
		// expr='(+ (* (beta 1 1) 0.9) 0.1)';
		// ret = this.ripl.assume('noise',expr);
		// console.log('ASSUME RET:' + JSON.stringify(ret));
    
// MyRIPL.assume('get-cluster-mean', lisp_parser.parse("(mem (lambda (cluster dim) (uniform -10.0 10.0)))"))
// MyRIPL.assume('get-cluster-variance', lisp_parser.parse("(mem (lambda (cluster dim) (gamma 1.0 1.0)))"))
// MyRIPL.assume('get-cluster', lisp_parser.parse("(mem (lambda (id) (cluster-crp)))"))
// MyRIPL.assume('get-cluster-model', lisp_parser.parse("(mem (lambda (cluster dim) (normal (get-cluster-mean cluster) (get-cluster-variance cluster))))"))
// MyRIPL.assume('get-datapoint', lisp_parser.parse("(mem (lambda (id dim) (get-cluster-model (get-cluster id) dim)))"))
  
		expr='(mem (lambda (cluster dim) (- (* (uniform-continuous 0.0 1.0) 20) 10)))';
		ret = this.ripl.assume('get-cluster-mean',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));
		expr='(mem (lambda (cluster dim) (+ (* (beta 1 10) 10) 0.1)))';
		ret = this.ripl.assumeLog('get-cluster-variance',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));
		expr='(mem (lambda (id) (cluster-crp)))';
		ret = this.ripl.assumeLog('get-cluster',expr);
		console.log('ASSUME RET:' + JSON.stringify(ret));
		// expr='(mem (lambda (cluster dim) (normal (get-cluster-mean cluster dim) (get-cluster-variance cluster dim))))';
		// ret = this.ripl.assume('get-cluster-model',expr);
		// console.log('ASSUME RET:' + JSON.stringify(ret));
		// expr='(mem (lambda (id dim) (get-cluster-model (get-cluster id) dim)))';
		// ret = this.ripl.assume('get-datapoint',expr);
		// console.log('ASSUME RET:' + JSON.stringify(ret));
    
		var currentDirectives = [];
		
		return currentDirectives;
	},


	render: function() {
        //this.ripl.infer();
        var reported_directives_before = this.ripl.report_directives();
        var reported_directives = new Array;
        //alert(parseInt("5"))
        
	      var church_code_str = "<b>Church code:</b><br>";

        for (i = 0; i < reported_directives_before.length; i++) {
          reported_directives[parseInt(reported_directives_before[i]["directive-id"])] = reported_directives_before[i];
          church_code_str += reported_directives_before[i]["directive-expression"] + '<br>';
        }
        
	      church_code_str = church_code_str.replace(/ /g, "&nbsp;");
	      church_code_str = church_code_str.replace(/\(if/g, "(<font color='#0000FF'>if</font>");
	      church_code_str = church_code_str.replace(/\(/g, "<font color='#0080D5'>(</font>");
	      church_code_str = church_code_str.replace(/\)/g, "<font color='#0080D5'>)</font>");
	      church_code_str = church_code_str.replace(/lambda/g, "<font color='#0000FF'>lambda</font>");
	      church_code_str = church_code_str.replace(/list/g, "<font color='#0000FF'>list</font>");

	      church_code_str = church_code_str.replace(/\>=/g, "<font color='#0000FF'>>=</font>");
	      church_code_str = church_code_str.replace(/\+/g, "<font color='#0000FF'>+</font>");
	      church_code_str = church_code_str.replace(/\*/g, "<font color='#0000FF'>*</font>");
	      church_code_str = "<font face='Courier New' size='2'>" + church_code_str + "</font>";
	      $("#church_code").html(church_code_str);
        
        console.log(reported_directives);
        //alert(typeof reported_directives)
        //console.log(reported_directives[0].value)
        //alert(reported_directives[0])
        
        var clusters = new Object;
        
        for (i in reported_directives) {
          if (reported_directives[i]["directive-expression"] != undefined) {
            if (reported_directives[i]["directive-expression"].substr(0, "(PREDICT (get-cluster ".length) == "(PREDICT (get-cluster ") {
              try {
                clusters[reported_directives[i].value] = new Object;
                clusters[reported_directives[i].value]["cluster_id"] = reported_directives[i].value;
                clusters[reported_directives[i].value]["cluster_mean_x"] = reported_directives[parseInt(i) + 1].value;
                clusters[reported_directives[i].value]["cluster_mean_y"] = reported_directives[parseInt(i) + 2].value;
                clusters[reported_directives[i].value]["cluster_sigma_x"] = reported_directives[parseInt(i) + 3].value;
                clusters[reported_directives[i].value]["cluster_sigma_y"] = reported_directives[parseInt(i) + 4].value;
              } catch(err) {  }
            }
          }
        }
        
        for (key in this.ellipses) {
          this.ellipses[key].remove();
        }
        this.ellipses = new Array;
        
        for (key in clusters) {
          plot_x = (420 / 2) + clusters[key].cluster_mean_x * 20;
          plot_y = (420 / 2) - clusters[key].cluster_mean_y * 20;

          console.log(clusters[key]);
          var new_ellipse = this.paper.ellipse(plot_x, plot_y, clusters[key].cluster_sigma_x * 20 * 3, clusters[key].cluster_sigma_y * 20 * 3);
          new_ellipse.attr("stroke", "red");
          //point.circle2.attr("fill", "white");
          //point.circle2.attr("opacity", "0.9");
          
          this.ellipses.push(new_ellipse);
        }
        
        return true;
				
				var chain_element = new Object();
				chain_element.iteration_number = 2;
				chain_element.polynomial = new Object();
				chain_element.polynomial.degree = chain.value[0]
		      		chain_element.polynomial.coefficients = new Array;
				for (var order = 0; order < 5; ++order) {
					chain_element.polynomial.coefficients[order] = chain.value[order+1];
				}
		    chain_element.noise = chain.value[6];
        chain_element.alpha = chain.value[7];
        chain_element.fouerier_count = chain.value[8];
		    chain_element.fouerier_a = new Array;
        chain_element.fouerier_a[0] = chain.value[9];
        chain_element.fouerier_a[1] = chain.value[10];
        chain_element.fouerier_a[2] = chain.value[11];
        chain_element.fouerier_a[3] = chain.value[12];
        chain_element.fouerier_omega = chain.value[13];
		    chain_element.fouerier_theta = new Array;
        chain_element.fouerier_theta[1] = chain.value[14];
        chain_element.fouerier_theta[2] = chain.value[15];
        chain_element.fouerier_theta[3] = chain.value[16];

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
            
			    	for (var i = this.previous_polynomials_count; i > 0; i--) {
			      		this.previous_polynomials[i] = this.previous_polynomials[i - 1];
			      		iMult = i * 40;
			      		this.previous_polynomials[i].attr("stroke", "#" + iMult.toString(16) + iMult.toString(16) + iMult.toString(16));
			    	}

			    	this.previous_polynomials[0] = this.DrawPolynomial(this.polynomials);
			    	if (this.previous_polynomials_count < 9) {
			      		this.previous_polynomials_count++;
			    	} else {
			      		this.previous_polynomials[9].remove();
			    	}
	},


	uploadPreviousObserves: function() {
			return false;
	},


	run: function(MODE) {
		this.active_directives = this.loadProgram();
    
    this.ellipses = new Array;
    
		this.renderInit();
		
		//if continous inference is not on, turning it on
		ret = this.ripl.cont_infer_status();
		if (ret["status"] == "off") {
			ret = this.ripl.start_cont_infer(1);
		}

		this.SendInformationAboutPoints();
    //this.handle = new Object;
    //for (i = 1; i < 50; ++i) {
      // window.setTimeout("this.handle[i] = window.setInterval('ex.render();', 100);", i * 2);
    //}
    this.handle = window.setInterval('ex.render();', 100);
		console.log('\n Running Render [Report_directives on]:' + this.active_directives); 	
	},

	CalculatePolynomial: function (x, poly) {
		// TODO: Via Horner's method it would be faster?
    var result_poly = poly.polynomial.coefficients[0];
    for (var i = 1; i < poly.polynomial.coefficients.length; ++i) {
      result_poly += poly.polynomial.coefficients[i] * Math.pow(x, i);
    }
    var result_f = poly.fouerier_a[0];
    for (var i = 1; i <= poly.fouerier_count; ++i) {
      result_f += poly.fouerier_a[i] * Math.cos(poly.fouerier_omega * i * x + poly.fouerier_theta[i]);
    }
    result = poly.alpha * result_poly + (1.0 - poly.alpha) * result_f;
    return result;
	},

	renderInit: function() {
	    	this.polynomials = new Array();

	    	this.previous_polynomials = new Array();
	    	this.previous_polynomials_count = 0;
        
        this.points_id = 0;

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
						forgetdirectiveIDs = ex.points[i].directive_ids;
						for (var j = (i + 1); j < ex.points_number; j++) {
				      			ex.points[j - 1] = ex.points[j];
						}
				    		ex.points_number--;
				    		this.remove();
              for (var forget_id_key = 0; forget_id_key < forgetdirectiveIDs.length; forget_id_key++) {
                ex.ripl.forget(forgetdirectiveIDs[forget_id_key]);
              }
				  	}
				}
				ex.SendInformationAboutPoints();
	      		});
			point.circle2.click(function(e) {
				for (var i = 0; i < ex.points_number; i++) {
					if (ex.points[i].unique_point_id == this.data("unique_point_id")) {
            ex.points[i].circle.remove();
						forgetdirectiveIDs = ex.points[i].directive_ids;
						for (var j = (i + 1); j < ex.points_number; j++) {
				      			ex.points[j - 1] = ex.points[j];
						}
				    		ex.points_number--;
				    		this.remove();
              for (var forget_id_key = 0; forget_id_key < forgetdirectiveIDs.length; forget_id_key++) {
                ex.ripl.forget(forgetdirectiveIDs[forget_id_key]);
              }
				  	}
				}
				ex.SendInformationAboutPoints();
	      		});
			
      var pointTimeCreation = ex.points_id;
      ex.points_id += 1;

// expr='(mem (lambda (cluster dim) (- (* (beta 1 1) 20) 10)))';
		// ret = this.ripl.assume('get-cluster-mean',expr);
		// console.log('ASSUME RET:' + JSON.stringify(ret));
		// expr='(mem (lambda (cluster dim) (uniform-continuous 0.1 10.0)))';
		// ret = this.ripl.assume('get-cluster-variance',expr);
		// console.log('ASSUME RET:' + JSON.stringify(ret));
		     
      cluster_id = ex.ripl.predict("(get-cluster " + pointTimeCreation + ")")
      mean_x = ex.ripl.predict("(get-cluster-mean (get-cluster " + pointTimeCreation + ") 1)")
      mean_y = ex.ripl.predict("(get-cluster-mean (get-cluster " + pointTimeCreation + ") 2)")
      variance_x = ex.ripl.predict("(get-cluster-variance (get-cluster " + pointTimeCreation + ") 1)")
      variance_y = ex.ripl.predict("(get-cluster-variance (get-cluster " + pointTimeCreation + ") 2)")
      observe_x = ex.ripl.observe("(normal (get-cluster-mean (get-cluster " + pointTimeCreation + ") 1) (get-cluster-variance (get-cluster " + pointTimeCreation + ") 1))", "r[" + point.x.toFixed(5) + "]")
      observe_y = ex.ripl.observe("(normal (get-cluster-mean (get-cluster " + pointTimeCreation + ") 2) (get-cluster-variance (get-cluster " + pointTimeCreation + ") 2))", "r[" + point.y.toFixed(5) + "]")
      
      // var outlierName = "is-outlier-" + String(Math.round(pointTimeCreation * 1000));
      // var expr = "(flip outlier-prob)";
      // ex.ripl.assume(outlierName, expr)
      
			// var expr='(normal (if ' + outlierName + ' 0 (clean-func (normal r['+ String(point.x.toFixed(5)) + '] noise))) (if ' + outlierName + ' outlier-sigma noise))';
      // var litval = "r[" + String(point.y.toFixed(5)) + "]"
			// var ret = ex.ripl.observe(expr,litval);
			// console.log('OBSERVE RET:' + JSON.stringify(ret));
      
      // ex.points_outliers[outlierName] = point;

	      		point.unique_point_id = ex.next_unique_point_id;
            ex.next_unique_point_id++;
	      		ex.points[ex.points_number] = point;
            ex.points[ex.points_number].directive_ids = new Array;
            ex.points[ex.points_number].directive_ids.push(cluster_id["d_id"])
            ex.points[ex.points_number].directive_ids.push(mean_x["d_id"])
            ex.points[ex.points_number].directive_ids.push(mean_y["d_id"])
            ex.points[ex.points_number].directive_ids.push(variance_x["d_id"])
            ex.points[ex.points_number].directive_ids.push(variance_y["d_id"])
            ex.points[ex.points_number].directive_ids.push(observe_x["d_id"])
            ex.points[ex.points_number].directive_ids.push(observe_y["d_id"])
            console.log(ex.points[ex.points_number].directive_ids)
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


	SendInformationAboutPoints:  function () {
        return;
	      ex.church_code_str = "<b>Church code:</b><br>";
        ex.church_code_str += this.ripl.assumes;

	      // ex.church_code_str += "(ASSUME c0 (if (>= order 0) (normal 0 10) 0))<br>";
	      // ex.church_code_str += "(ASSUME c1 (if (>= order 1) (normal 0 1) 0))<br>";
	      // ex.church_code_str += "(ASSUME c2 (if (>= order 2) (normal 0 0.1) 0))<br>";
	      // ex.church_code_str += "(ASSUME c3 (if (>= order 3) (normal 0 0.01) 0))<br>";
	      // ex.church_code_str += "(ASSUME c4 (if (>= order 4) (normal 0 0.001) 0))<br>";
        
	      // ex.church_code_str += "(ASSUME clean-func<br>";
	      // ex.church_code_str += "  (lambda (x)<br>";
	      // ex.church_code_str += "    (+ c0 (* c1 (power x 1)) (* c2 (power x 2))<br>";
	      // ex.church_code_str += "      (* c3 (power x 3)) (* c4 (power x 4)))))<br>";
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
	      //$("#church_code").html(ex.church_code_str);
	},

	

	insertHTML: function(divclass) {
		$('div.'+divclass).html('\
			<h3><a href="https://github.com/perov/simpleinterpreter/">CRP/Mixture demo</a></h3>\
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



