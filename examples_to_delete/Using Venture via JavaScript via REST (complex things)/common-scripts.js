function refreshInfButton() {
	var r = new ripl();
	var ret = r.cont_infer_status()
	if (ret.status == 'off') {
		$('#infstat').html('INFERENCE IS OFF');
	}
	else {
		$('#infstat').html('INFERENCE IS ON.');
	}
}



var STR_PAD_LEFT = 1;
var STR_PAD_RIGHT = 2;
var STR_PAD_BOTH = 3;
 
function pad(str, len, pad, dir) {
 
	if (typeof(len) == "undefined") { var len = 0; }
	if (typeof(pad) == "undefined") { var pad = ' '; }
	if (typeof(dir) == "undefined") { var dir = STR_PAD_RIGHT; }
 
	if (len + 1 >= str.length) {
 
		switch (dir){
 
			case STR_PAD_LEFT:
				str = Array(len + 1 - str.length).join(pad) + str;
			break;
 
			case STR_PAD_BOTH:
				var right = Math.ceil((padlen = len - str.length) / 2);
				var left = padlen - right;
				str = Array(left+1).join(pad) + str + Array(right+1).join(pad);
			break;
 
			default:
				str = str + Array(len + 1 - str.length).join(pad);
			break;
 
		} // switch
 
	}
 
	return str;
 
}



function loadJS(file) {
		var oHead = document.getElementsByTagName('HEAD').item(0);
		var oScript= document.createElement("script");
		oScript.type = "text/javascript";
		oScript.src=file;
		oHead.appendChild( oScript);
}

function fetchUserScript(file) {
		if (file == "NONE") {
			loadJS($('#getLibLocation').val()); //isnt working for some reason
		}
		else {
			loadJS(file);	
		}	
		setTimeout("ex = new vistemplate();ex.run()",300);
}


function JustLoadProgramFromUserScript(file) {
		if (file == "NONE") {
			loadJS($('#getLibLocation').val()); //isnt working for some reason
		}
		else {
			loadJS(file);	
		}
		if (typeof LOADVISPAGE === 'undefined') {
			setTimeout("ex = new vistemplate();ex.loadProgram()",300);
		}
		else {
			setTimeout("ex = new vistemplate();ex.run()",300);
		}
}

function conInf(state) {
	var r=new ripl();
	if(state == 'INFON') {
		r.start_cont_infer(1);
		//this.handle = setInterval('nb.ShowDirectives()',70);
		if (typeof txtview != 'undefined') {
			TIMER = window.setInterval('txtview.ShowDirectives()',70);
			REFRESH = 1;
		}
		else if (typeof nb != 'undefined') {
			TIMER = window.setInterval('nb.ShowDirectives()',70);
			REFRESH = 1;
		}
	}
	else if (state == 'INFOFF') {
		r.stop_cont_infer();
		//clearInterval('nb.handle');
		if (typeof txtview != 'undefined') {
			window.clearInterval(TIMER);
			REFRESH = 0;
			txtview.ShowDirectives();
		}
		else if (typeof nb != 'undefined') {
			window.clearInterval(TIMER);
			REFRESH = 0;
			nb.ShowDirectives();
		}
	}
	else if (state == 'INFSTAT') {
		var stat = r.cont_infer_status()
		alert('Continous Inferece is: ' + stat.status);
	}
	else if (state == 'CLEAR') {
		r.clearTrace();
	}
	else if (state == 'INFER') {
		r.infer(100,1);
	}
	else if (state == 'REFRESH') {
		if (typeof txtview != 'undefined') {
			txtview.ShowDirectives();
		}
		else if (typeof nb != 'undefined') {
			nb.ShowDirectives();
		}
	}
}
