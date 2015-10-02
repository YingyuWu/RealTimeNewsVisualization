
function createMessage(type, content){
	this.type = type;
	this.content = content;
	this.toString = function(){
		var result = {"messageType":this.type, "messageContent":this.content};
		return JSON.stringify(result);
	}
}
function playOnClick(){
	var status = d3.select("#play").select("img")
				.attr("name")
	var newMessage = new createMessage("VIS_Pause_Continue", status);
	websocket.send(newMessage.toString());

	if (status == "pausing"){
		d3.select("#play").select("img")
		.attr("src", "images/pause.jpg");
	}
	else if (status == "playing"){
		d3.select("#play").select("img")
		.attr("src", "images/play.jpg");
	}
}

function pauseOrContinue(newstatus){
	if (newstatus == "playing"){
		d3.select("#play").select("img")
		.attr("name", newstatus)
		.attr("src", "images/pause.jpg");
	}
	else if (newstatus == "pausing"){
		d3.select("#play").select("img")
		.attr("name", newstatus)
		.attr("src", "images/play.jpg");
	}
}

function goBackOneTimestamp(){
	console.log("goBack");
	var newMessage = new createMessage("VIS_Play_Control", {"d":-1, "c":count});
	websocket.send(newMessage.toString());
}

function goAheadOneTimestamp(){
	console.log("goAhead");
	var newMessage = new createMessage("VIS_Play_Control", {"d":1, "c":count});
	websocket.send(newMessage.toString());
}