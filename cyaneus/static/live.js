var Url = {

	// public method for url encoding
	encode : function (string) {
		return escape(this._utf8_encode(string));
	},

	// public method for url decoding
	decode : function (string) {
		return this._utf8_decode(unescape(string));
	},

	// private method for UTF-8 encoding
	_utf8_encode : function (string) {
		string = string.replace(/\r\n/g,"\n");
		var utftext = "";

		for (var n = 0; n < string.length; n++) {

			var c = string.charCodeAt(n);

			if (c < 128) {
				utftext += String.fromCharCode(c);
			}
			else if((c > 127) && (c < 2048)) {
				utftext += String.fromCharCode((c >> 6) | 192);
				utftext += String.fromCharCode((c & 63) | 128);
			}
			else {
				utftext += String.fromCharCode((c >> 12) | 224);
				utftext += String.fromCharCode(((c >> 6) & 63) | 128);
				utftext += String.fromCharCode((c & 63) | 128);
			}
		}
		return utftext;
	},

	// private method for UTF-8 decoding
	_utf8_decode : function (utftext) {
		var string = "";
		var i = 0;
		var c = c1 = c2 = 0;

		while ( i < utftext.length ) {

			c = utftext.charCodeAt(i);

			if (c < 128) {
				string += String.fromCharCode(c);
				i++;
			}
			else if((c > 191) && (c < 224)) {
				c2 = utftext.charCodeAt(i+1);
				string += String.fromCharCode(((c & 31) << 6) | (c2 & 63));
				i += 2;
			}
			else {
				c2 = utftext.charCodeAt(i+1);
				c3 = utftext.charCodeAt(i+2);
				string += String.fromCharCode(((c & 15) << 12) | ((c2 & 63) << 6) | (c3 & 63));
				i += 3;
			}
		}
		return string;
	}
}


var preChannel = '';
var preGame = 'game_tab_0';
function fetchMethods(channel){
	new Ajax.Request('/channel/' + Url.encode(channel) + '/',
	  {
		method:'get',
		onSuccess: function(transport){
		  var response = transport.responseText || "no response text";
		  $('methods').update(response);
		},
		onFailure: function(){
			$('methods').update("<p>Update failed.</p>");
		}
	  });
	$('methods').style.display = 'block';
}

function switchChannelTab(current){
	if(preChannel != '')
		$(preChannel).className = '';
	$(current).className = 'active';
	preChannel = current;
}

function switchGameTab(current){
	if(preGame != '')
		$(preGame).className = '';
	$(current).className = 'active';
	preGame = current;
}

function loadGame(game){
	new Ajax.Request('/game/' + Url.encode(game) + '/',
	  {
		method:'get',
		onSuccess: function(transport){
		  var response = transport.responseText || "no response text";
		  $('matches').update(response);
		},
		onFailure: function(){
			$('matches').update("<p>Update failed.</p>");
		}
	  });
}

function loadChannel(channel, tool){
	new Ajax.Request('/go/' + Url.encode(channel) + '/' + tool + '/',
	  {
		method:'get',
		onSuccess: function(transport){
		  var response = transport.responseText || "no response text";
		  $('media').update(response);
		},
		onFailure: function(){
			$('media').update("<p>Update failed.</p>");
		}
	  });
}
