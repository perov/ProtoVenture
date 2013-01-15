
function InitializeDemo() {
  ripl = new ripl(); // Create a RIPL client object to communicate with the engine.
  ripl.clearTrace(); // Clear the engine state.
  
  ellipses = new Array;
  
  // Define the generic model:
  ripl.assume('alpha', '(uniform-continuous 0.0001 2.0)')
  ripl.assume('cluster-crp', '(CRP/make alpha)')
  ripl.assume('get-cluster-mean', '(mem (lambda (cluster dim) (- (* (uniform-continuous 0.0 1.0) 20) 10)))') // "dim" means "dimension".
  ripl.assume('get-cluster-variance', '(mem (lambda (cluster dim) (+ (* (beta 1 10) 10) 0.1)))')
  ripl.assume('get-cluster', '(mem (lambda (id) (cluster-crp)))')
  
  ripl.start_cont_infer(1); // Start the continuous ("infinite") inference.

  // Prepare the canvas in your browser.
  paper = Raphael($('#graphics_div')[0], 420, 420);
  background = paper.rect(0, 0, 420, 420).attr({fill: "white"});
  LabelPlot();
  
  all_points = new Object(); // Init a JavaScript dictionary to save current points.
  next_point_unique_id = 0;
  
  background.click(function(background_event) {
    // If user clicks on the plot, create a new point.
    var point = new Object();
    
    // Coordinates in the browser window:
    point.html_x = background_event.pageX - $('#graphics_div').offset().left;
    point.html_y = background_event.pageY - $('#graphics_div').offset().top;
    
    // "Mathematical" coordinates on the canvas (relative to the plot centre):
    point.plot_math_x = (point.html_x - (420 / 2)) / 20;
    point.plot_math_y = ((point.html_y - (420 / 2)) * -1) / 20;
    
    // Create and pretty a circle on the plot.
    point.circle = paper.circle(point.html_x, point.html_y, 5);
    point.circle.attr("fill", "red");
    point.circle.attr("stroke", "white");
    point.circle.attr("opacity", "0.5");
              
    point.unique_id = next_point_unique_id;
    next_point_unique_id++;

    // Send information about this data point to the engine.
    point.directive_ids = new Array;
    point.directive_ids.push(ripl.predict("(get-cluster " + point.unique_id + ")")["d_id"])
    point.directive_ids.push(ripl.predict("(get-cluster-mean (get-cluster " + point.unique_id + ") 1)")["d_id"])
    point.directive_ids.push(ripl.predict("(get-cluster-mean (get-cluster " + point.unique_id + ") 2)")["d_id"])
    point.directive_ids.push(ripl.predict("(get-cluster-variance (get-cluster " + point.unique_id + ") 1)")["d_id"])
    point.directive_ids.push(ripl.predict("(get-cluster-variance (get-cluster " + point.unique_id + ") 2)")["d_id"])
    point.directive_ids.push(ripl.observe("(normal (get-cluster-mean (get-cluster " + point.unique_id + ") 1) (get-cluster-variance (get-cluster " + point.unique_id + ") 1))", "r[" + point.plot_math_x.toFixed(5) + "]")["d_id"]) // e.g., r[5] means real[5.0]
    point.directive_ids.push(ripl.observe("(normal (get-cluster-mean (get-cluster " + point.unique_id + ") 2) (get-cluster-variance (get-cluster " + point.unique_id + ") 2))", "r[" + point.plot_math_y.toFixed(5) + "]")["d_id"])
    
    // Save the point to the dictionary of all current points.
    all_points[point.unique_id] = point;
    point.circle.data("point_reference", point);
    
    point.circle.click(function(circle_event) {
      // If user clicks on the point, delete it.
      
      // That is, remove it from the engine trace:
      forgetdirectiveIDs = this.data("point_reference").directive_ids;
      for (var forget_id_key = 0; forget_id_key < forgetdirectiveIDs.length; forget_id_key++) {
        ripl.forget(forgetdirectiveIDs[forget_id_key]);
      }
      
      delete all_points[this.data("point_reference").unique_id]; // also remove it from the dictionary of all current points,
      this.remove(); // and finally remove the point from the paper.
    });
  });
  
  // Run first time the function GetAndDrawCurrentSample().
  // Next setTimeout(...) are being called in this function itself.
  // (FYI, setTimeout(..., msec) runs the function only once after X msec instead
  //  of setInterval(..., msec), which continuously runs the function each X msec.)
  setTimeout("GetAndDrawCurrentSample();", 1);
}

function GetAndDrawCurrentSample() {
  var reported_directives_before = ripl.report_directives();
  var reported_directives = new Array;
  
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
  
  var clusters = new Object;
  
  // This part of the code is implemented in not the best way.
  // We enumerate here all active directives, and if the directive
  // is "PREDICT (get-cluster POINT-ID)", we collect information
  // from this directive and 4 next directives about the cluster of this point.
  // Because generally there are several points per cluster,
  // we collect the same information about the same cluster several times.
  for (i in reported_directives) {
    if (reported_directives[i]["directive-expression"] != undefined) {
      if (reported_directives[i]["directive-expression"].substr(0, ("(PREDICT (get-cluster ").length) == "(PREDICT (get-cluster ") {
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
  
  for (key in ellipses) {
    ellipses[key].remove();
  }
  ellipses = new Array;
  
  for (key in clusters) {
    plot_x = (420 / 2) + clusters[key].cluster_mean_x * 20;
    plot_y = (420 / 2) - clusters[key].cluster_mean_y * 20;

    var new_ellipse = paper.ellipse(plot_x, plot_y, clusters[key].cluster_sigma_x * 20 * 3, clusters[key].cluster_sigma_y * 20 * 3);
    new_ellipse.attr("stroke", "red");
    
    ellipses.push(new_ellipse);
  }

  setTimeout("GetAndDrawCurrentSample();", 50); // Run this function again after 1 msec.
}

// Marking up the canvas (just for user's convenience).
function LabelPlot() {
  var currentObject;
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
}
