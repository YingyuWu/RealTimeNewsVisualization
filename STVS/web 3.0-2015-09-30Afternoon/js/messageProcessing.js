var color = d3.scale.category10();
var IsPlayBacking = false;
var count = 0;
var currentClusterData; 
var currentFocusData;
var currentTime;

function messageProcessing(message){
	//initilizeStoryTellingVisualizationSVG(0, 210, 140, 0, 0);
	count++;
	initilizeMainStage (1200, 540, 20, 20, 300);
	
	if (message.messageType == "VIS_Cluster_Data"){
		currentTime = message.messageContent.dateTime;
		currentClusterData = message.messageContent.clusterData;
		console.log(currentClusterData);
		updateDateTime(currentTime);
		updateMainStage(currentClusterData);
		//updateTableShowAndHide(cluster_selected);

		
		
		if (!IsPlayBacking){
			//generateAndUpdateStoryTellingVisualization(currentClusterData, 0, 210, 140, 0, 0, 2500);
			//if (typeof message.messageContent.changeParameter == "undefined"){
			generateAndUpdatePlayBar(message, 1200*0.96, 800*0.3, 150, 0.900, 1000, 10);
			//}
		}
		else{
			//generateAndUpdateStoryTellingVisualization(currentClusterData, 0, 210, 140, 0, 0, 2500);
			//if (typeof message.messageContent.changeParameter == "undefined"){
				updatePlayBarTextColor();
			//}
		}
	}

	if (message.messageType == "VIS_Article_Nodes"){
		//updateArticleClusterIDForTable(message.messageContent);
		//generateAndUpdateForceNodes(message.messageContent, 0, 840, 560, 0, 0);
	}

	if (message.messageType == "VIS_Article_Detail"){
		//updateArticleDetailTable(message.messageContent);
		//console.log(message.messageContent);
		updateNewArticle(message.messageContent);
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
		//focusClusterClick(currentFocusData);
	}

	if (message.messageType == ""){
		
	}
}

function reDraw(){
	generateAndUpdateStoryTellingVisualization(currentClusterData, 0, 840, 560, 0, 0, 0);
	focusClusterClick(currentFocusData);
}

function clusterParameterChange()
{
	var clusterparameter = d3.select("#cluster_parameter")[0][0].value;


	var result = {"messageType":"VIS_CLUSTER_COEFFICIENT", "messageContent":parseFloat(clusterparameter)}
	websocket.send(JSON.stringify(result));
}

function clusterSizeParameterChange(){
	var clustersizeparameter = d3.select("#cluster_size_parameter")[0][0].value;
	console.log(clustersizeparameter)
	var result = {"messageType":"VIS_CLUSTER_SIZE", "messageContent":parseFloat(clustersizeparameter)}
	websocket.send(JSON.stringify(result));
}