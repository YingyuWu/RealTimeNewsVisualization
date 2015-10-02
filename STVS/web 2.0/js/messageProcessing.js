var color = d3.scale.category10();
var IsPlayBacking = false;
var count = 0;
var currentClusterData; 
var currentFocusData;
var currentTime;

function messageProcessing(message){
	initilizeStoryTellingVisualizationSVG(0, 840, 560, 0, 0);
	count++;
	
	if (message.messageType == "VIS_Cluster_Data"){
		currentTime = message.messageContent.dateTime;
		updateDateTime(currentTime);

		currentClusterData = message.messageContent.clusterData;
		
		if (!IsPlayBacking){
			generateAndUpdateStoryTellingVisualization(currentClusterData, 0, 840, 560, 0, 0, 1000);
			if (!message.messageContent.changeParameter){
				generateAndUpdatePlayBar(message, 1200*0.96, 800*0.3, 150, 0.900, 1000, 10);
			}
		}
		else{
			generateAndUpdateStoryTellingVisualization(currentClusterData, 0, 840, 560, 0, 0, 1000);
			if (!message.messageContent.changeParameter){
				updatePlayBarTextColor();
			}
		}
	}

	if (message.messageType == "VIS_Article_Nodes"){
		updateArticleClusterIDForTable(message.messageContent);
		generateAndUpdateForceNodes(message.messageContent, 0, 840, 560, 0, 40);
	}

	if (message.messageType == "VIS_Article_Detail"){
		updateArticleDetailTable(message.messageContent);
	}

	if (message.messageType == "VIS_Pause_Continue"){
		pauseOrContinue(message.messageContent);
	}

	if (message.messageType == "VIS_Play_Back"){
		IsPlayBacking = message.messageContent.isPlayBacking;
		currentHistoryIndex = message.messageContent.currentHistoryIndex + 1;
	}

	if (message.messageType == "VIS_Focus_Context"){
		currentFocusData = message.messageContent;
		updateFocusCLusterCircle(250)
	}

	if (message.messageType == ""){
		
	}
}

function reDraw() {
	generateAndUpdateStoryTellingVisualization(currentClusterData, 0, 840, 560, 0, 0, 0, false);
	updateFocusCLusterCircle(250)
}

function clusterParameterChange()
{
	var clusterparameter = d3.select("#cluster_parameter")[0][0].value;

	console.log(clusterparameter)

	var result = {"messageType":"VIS_CLUSTER_COEFFICIENT", "messageContent":parseFloat(clusterparameter)}
	websocket.send(JSON.stringify(result));
}

function clusterSizeParameterChange(){
	var clustersizeparameter = d3.select("#cluster_size_parameter")[0][0].value;
	console.log(clustersizeparameter)
	var result = {"messageType":"VIS_CLUSTER_SIZE", "messageContent":parseFloat(clustersizeparameter)}
	websocket.send(JSON.stringify(result));
}