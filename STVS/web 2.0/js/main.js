var wsUri = "ws://127.0.0.1:2014";
var websocket = null;
var message = null;
var pause = false;

function ab2str(buf) {
    var str = ''
    var bytes = new Uint8Array( buf )
    var len = bytes.byteLength;
    for (var i = 0; i < len; i++) {
        str += String.fromCharCode( bytes[ i ] )
    }
    return str;
}


function str2ab(str) {
  var buf = new ArrayBuffer(str.length); // 2 bytes for each char
  var bufView = new Uint8Array(buf);
  for (var i=0, strLen=str.length; i<strLen; i++) {
    bufView[i] = str.charCodeAt(i);
  }
  return buf;
}

function initWebsocket()
{
	try{
		if (typeof MozWebSocket == 'function'){
			WebSocket = MozWebSocket;
		}

		if (websocket && websocket.readyState == 1){
			websocket.close();
		}

		websocket = new WebSocket(wsUri);
		websocket.binaryType = 'arraybuffer';

		websocket.onopen = function(evt){
			console.log("CONNECTED");
			updateConnectionStatus();
		};
		websocket.onclose = function(evt){
			console.log("DISCONNECTED");
			updateConnectionStatus();
		};
		websocket.onmessage = function(evt){
			updateConnectionStatus();
			message = "";
			var tempData = ab2str(evt.data)

			if (tempData[0] != '{') {
				var binaryData = new Uint8Array(evt.data);
				return;
			}

			message = JSON.parse(tempData)	
			messageProcessing(message);
			
		};
		websocket.onerror = function(evt){
			updateConnectionStatus();
			console.log('ERROR: ' + evt.data);
		};
	}
	catch( exception ){
		updateConnectionStatus();
		console.log('ERROR: ' + exception);
	}
}

function stopWebsocket()
{
	if (websocket){
		websocket.close();
	}
}

function updateConnectionStatus()
{
	$("#websocket_status").text("Connection Status : "+ checkSocket());
}

function checkSocket()
{
	if (websocket != null){
		var stateStr;
		switch (websocket.readyState){
			case 0:
				stateStr = "CONNECTING";
				break;
			case 1:
				stateStr = "OPEN";
				break;
			case 2:
				stateStr = "CLOSING";
				break;
			case 3:
				stateStr = "CLOSED";
				break;
			default:
				stateStr = "UNKNOW";
				break;
		}
		return stateStr;
	}
	else{
		return "Websocket is null";
	}
}