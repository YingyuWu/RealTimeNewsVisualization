var focusID = -1;
var isFocusing = false;
var focusClusterData = undefined;

function onClickClusterCircle(tempData) {
	focusClusterData = tempData;

	if (isFocusing == false) {
		isFocusing = true;
		focusID = focusClusterData.id;
	}
	else {
		if (focusID == focusClusterData.id) {
			isFocusing = false;
		}
		else {
			focusID = focusClusterData.id;
		}
	}

	generateAndUpdateStoryTellingVisualization(currentClusterData, 0, 840, 560, 0, 0, 0, false);

	if (isFocusing) {
		d3.select("#cluster_circle-" + focusID)
		.attr("opacity", 0)

		createFocusClusterCircle(250);
	}
	else {
		d3.selectAll(".focusingClusterCircle").remove();
	}
}

function onClickFocusingCLusterCircle() {
	if (isFocusing == true) {
		isFocusing = false;
	}

	generateAndUpdateStoryTellingVisualization(currentClusterData, 0, 840, 560, 0, 0, 0, false);
	d3.selectAll(".focusingClusterCircle").remove();
}

function updateFocusCLusterCircle(radius) {
	var pack = d3.layout.pack()
			.size([radius,radius])
			.value(function(d){return d.size})

	if (isFocusing) {
		var focusData = currentFocusData[focusID];
		//var position = d3.select("#cluster_circle-" + focusID).attr("transform");

		var tempFocusingClusterCircle = d3.selectAll(".focusingClusterCircle")
		.datum(focusData)
		

		tempFocusingClusterCircle.selectAll(".subClusterNode").remove();
		tempFocusingClusterCircle.selectAll("title").remove();
		tempFocusingClusterCircle.selectAll("circle").remove();
		tempFocusingClusterCircle.selectAll("text").remove();

		var node = tempFocusingClusterCircle.selectAll(".subClusterNode")
		.data(pack.nodes)
		.enter().append("g")
		.attr("class", function(d) { return d.children ? "node" : "leaf node"; })
		.attr("transform", function(d) { return "translate(" + d.x + "," + d.y + ")"; })
		.style("fill", function(d,i){
			if (i == 0) {
				return color(focusID)
			} 
			else {
				return d3.rgb(color(focusID)).darker(i/3);
			}
		})
		.style("opacity", 0.8)

		var tempClusterDate = currentClusterData.filter(function(l, i) {
			return l.id == focusID;
		})

		node.append("title")
	      .text(function(d) { 
	      	var result = ""
	      	if (typeof d.size == 'undefined') {
	      		tempClusterDate[0].keywords.forEach(function(l, i){
	      			result += "#"+l.value+";"
	      		})
	      	}
	      	else {
	      		d.keywords.forEach(function(l, i){
	      			result += "#"+l+";"
	      		})
	      	}
	      	return result
	      });

		node.append("circle")
		  .attr("r", function(d) { return d.r; });

		node.append("text")
		.attr("text-anchor", "middle")
		.attr("fill", function(d){
		  return "#111111"
		 })
		.attr("font-weight", "bold")
		.each(generateHeadLineForSubCluster);

		tempFocusingClusterCircle
		.transition()
		.duration(500)
		.attr("transform", function() {
			var x = focusPosition[0] - radius/2;
			var y = focusPosition[1] - radius/2;

			return "translate("+x+","+y+")";
		})
	}
}

function createFocusClusterCircle(radius) {
	d3.selectAll(".focusingClusterCircle").remove();

	var focusData = currentFocusData[focusID];

	if (typeof focusData == 'undefined') {
		console.log("focusData is undefined. ")
		return;
	}

	var pack = d3.layout.pack()
				.size([radius,radius])
				.value(function(d){return d.size})

	var position = d3.select("#cluster_circle-" + focusID).attr("transform");

	var div = d3.select(".StoryTellingVisualization")[0][0];

	var tempFocusingClusterCircle = div.__g
	.append("g")
	.attr("class", "focusingClusterCircle")
	.datum(focusData)
	.attr("transform", function() {
		var x = position.split("(")[1].split(")")[0].split(",")[0] - radius/2;
		var y = position.split("(")[1].split(")")[0].split(",")[1] - radius/2;

		return "translate("+x+","+y+")";
	})
	.on("click", function () {
		return onClickFocusingCLusterCircle()
	})

	var node = tempFocusingClusterCircle.selectAll(".subClusterNode")
	.data(pack.nodes)
	.enter().append("g")
	.attr("class", function(d) { return d.children ? "node" : "leaf node"; })
	.attr("transform", function(d) { return "translate(" + d.x + "," + d.y + ")"; })
	.style("fill", function(d,i){
		if (i == 0) {
			return color(focusID)
		} 
		else {
			return d3.rgb(color(focusID)).darker(i/3);
		}
	})
	.style("opacity", 0.8)

	var tempClusterDate = currentClusterData.filter(function(l, i) {
		return l.id == focusID;
	})

	node.append("title")
      .text(function(d) { 
      	var result = ""
      	if (typeof d.size == 'undefined') {
      		tempClusterDate[0].keywords.forEach(function(l, i){
      			result += "#"+l.value+";"
      		})
      	}
      	else {
      		d.keywords.forEach(function(l, i){
      			result += "#"+l+";"
      		})
      	}
      	return result
      });

	node.append("circle")
	  .attr("r", function(d) { return d.r; });

	node.append("text")
	.attr("text-anchor", "middle")
	.attr("fill", function(d){
	  return "#111111"
	 })
	.attr("font-weight", "bold")
	.each(generateHeadLineForSubCluster);
}

function generateHeadLineForSubCluster(d, i){
	var maxRadius = parseFloat(d3.select(".StoryTellingVisualization").select("svg").attr("width"))*2/5;

	if (i == 0){
		var el = d3.select(this);
		el.text('');

		return;

		var radius = d.r;
	    var titlefontsize = 15;
		var keywords = currentFocusData.currentCluster;
		var keywordfontsize = titlefontsize - 1;

		keywords.forEach(function(d, i){
			if (i == 0){
					var keywordString1 = el.append('tspan').text("#"+d.value+";").attr("text-anchor", "start").attr("class", "keywordsTitle");
	    			keywordString1.attr('x', radius*-1).attr('y', radius*-2/3).attr("font-size", keywordfontsize)
			}
			else{
				var keywordString1 = el.append('tspan').text("#"+d.value+";").attr("text-anchor", "start").attr("class", "keywordsTitle");
    			keywordString1.attr('x', radius*-1).attr('dy', titlefontsize).attr("font-size", keywordfontsize);
			}	
		})
	}
	else{ //don't need to create terxt for the root node
		var el = d3.select(this);
		el.text('');

		var fontscale = d3.scale.linear()
					.domain([30, maxRadius/2])
					.range([9, 18])
					.clamp(true)

		var radius = d.r;

		var tempTimeString = ""
		
		var title = d.f_title;
	    var words = title.split(' ');
	    
	    var titlelinenumber = 0;
	    var titlefontsize = fontscale(radius);


	    if (radius == 0){
			var tempTimeString = ""
			tempTimeString += "Last Update Time"//d.lastupdateTime; //"2013-09-04T00:00:00Z"
			//tempTimeString += " -> ";
			//tempTimeString += d.lastUpdateTime.substr(1, 10);  //""2013-09-08T00:00:00Z"";

			var createTimeString = el.append('tspan').text(tempTimeString).attr("class", "someInformation");
		    createTimeString.attr('x', 0).attr('y', radius*5/6*-1).attr("font-size", titlefontsize);

	    }

	    if (radius > 30){
		    function getTitle(title_selection){
		    	if (title_selection == "f_title"){
		    		return d.f_title.split(' ');
		    	}
		    	else if (title_selection == "p_title"){
		    		return d.p_title.split(' ');
		    	}
		    	else if (title_selection == "g_title"){
		    		return d.g_title.split(' ');
		    	}
		    }

		    var words = getTitle(title_selection);
		    

		    var tempLength = 0;
		    var tempString = "";

		    for (var i = 0; i < words.length; i++) {

		    	tempLength += words[i].length;
		    	tempString += words[i] + " ";

		    	if (tempLength > radius/5.0){
		    		if (titlelinenumber == 0){
			    		var tspan = el.append('tspan').text(tempString).attr("class", "headlineTitle");
		    			tspan.attr('x', 0).attr('y', radius/3*-2).attr("font-size", titlefontsize);
		    		}
		    		else{
			    		var tspan = el.append('tspan').text(tempString).attr("class", "headlineTitle");
		    			tspan.attr('x', 0).attr('dy', titlefontsize).attr("font-size", titlefontsize);
		    		}

		    		titlelinenumber += 1;
		    		tempString = "";
		    		tempLength = 0;
		    	}          
		    }

		    var tspan = el.append('tspan').text(tempString);
			tspan.attr('x', 0).attr('dy', titlefontsize).attr("font-size", titlefontsize);

			if (tempString != "") titlelinenumber += 1;
		}

		var keywordPosition = titlelinenumber*titlefontsize  + radius*1/3*-2 + radius*1/8;
		var keywordfontsize = titlefontsize - 1;

		d.keywords.forEach(function(d, i){
			if (i > 3) {
				//return;
			}
			if ( keywordPosition + titlefontsize < radius * 0.8){
				if (i == 0){
					var keywordString1 = el.append('tspan').text("#"+clampKeyword(d)).attr("text-anchor", "start").attr("class", "keywordsTitle");
	    			keywordString1.attr('x', radius*-0.7).attr('y', keywordPosition).attr("font-size", keywordfontsize)
				}
				else{
					var keywordString1 = el.append('tspan').text("#"+clampKeyword(d)).attr("text-anchor", "start").attr("class", "keywordsTitle");
	    			keywordString1.attr('x', radius*-0.7).attr('dy', titlefontsize).attr("font-size", keywordfontsize);
				}
				
	    		keywordPosition += titlefontsize;
			}
		})

		function clampKeyword(keyword){
			var words = keyword.split(' ');
			var size = 3;
			var result = "";
			var tempString = "...";

			if (size > words.length){
				size = words.length;
				tempString = "";
			}

			for (var i=0; i<size; ++i){
				result += words[i] + " ";
			}

			result += tempString

			return result;
		}
	}
}