var durationTime = 750;
var minRectWidth = 35;
var detailData = [];
var totalArticle = 0;
var piechart_diameter;
var oldTitle = [[],[],[],[],[]];

function initilizeMainStage(width, height, left, top, right) {
	if (d3.selectAll("#mainStage")[0].length == 0){
		var div = null;

		div = d3.select("#ClusterVisualizationSection")
		.append("div")
		.attr("id", "mainStage")
		.attr("class", "mainStage")[0][0];
		div.__svg = d3.select(div)
		.append("svg")
		.attr("width", width/1.5)
		.attr("height", height)
		.style("float", "left")

		div.__g = div.__svg
		.append("g")
		.attr("transform", "translate("+left+","+top+")")

		div.__top = top;
		div.__left = left;
		div.__width = width - left;
		div.__height = height - top;

		div.__x = d3.time.scale().range([0, width/1.12-left-right]).clamp(true);

		div.__xAxis = d3.svg.axis()
		.scale(div.__x)
		.orient("top")
		.ticks(8)
						//.tickSubdivide(true)
						//.ticks(d3.time.hour, 1);

						div.__g.append("g")
						.attr("class", "timeline")
						.call(div.__xAxis)

						initBarChart(div.__g, width,height,left,top,right);
		//initWordCloud(div.__g, width,height,left,top,right);
		initKeywordList(div.__g, width,height,left,top,right);
		//div.__y = d3.scale.linear().range([height-2*top-tempy, innerCicleRadius*tempy]).domain([0, 0.7]).clamp(true);
	}
}

function updateMainStage(newData) {
	updateScale(newData);

	updateClusterVisualization(newData);
}

function updateClusterVisualization(newData) {

	var div = d3.selectAll("#mainStage")[0][0];
	var totalSize = 0;
	var sizeSum = 0;

	var existList = [];

	newData.forEach(function(d, i) {
		totalSize = totalSize + Math.sqrt(d.size);
		existList.push(d.id);
		sizeSum = sizeSum + d.size;//Calculate sizeSum
	})

	var y_offset = div.__top;
	var y_padding = 0.90;

	// remove not top 5 clusters
	for (var tempi=0; tempi<5; ++tempi) {
		if (existList.indexOf(tempi) == -1) {
			d3.selectAll("#cluster-"+tempi.toString())
			.selectAll("rect")
			.transition()
			.duration(durationTime)
			.attr("width", 0)
			.attr("height", 0)

			d3.selectAll("#cluster-"+tempi.toString())
			.selectAll("line")
			.remove();

			d3.selectAll("#piechart-"+tempi.toString())
			.remove();

			d3.selectAll("#clusterText-"+tempi.toString())
			.remove();

			d3.selectAll("#cluster-"+tempi.toString())
			.transition()
			.delay(durationTime)
			.remove();

			oldTitle[tempi] = [];
		}
	}

	var tempData = jQuery.extend(true, [], newData);

	/*
	tempData.sort(function (a, b) {
		return a.id > b.id;
	})*/

	// Wendy added
	barChartAnimation(tempData,sizeSum,div.__g);
	//wordCloudAnimation(tempData,div.__g);
	keywordListAnimation(tempData,div.__g);

	// Cheng added
	totalArticle = 0;
	tempData.forEach(function(d,i){
		totalArticle += d.size;
	})

	// _______________

	tempData.forEach(function(d, i) {
		piechart_diameter = 1.0 / 5 * div.__height * y_padding * 0.7
		var lineDY = [19,49];
		if (d3.selectAll("#cluster-"+d.id.toString())[0].length == 0) {
			// draw cluster color ground
			div.__g.append("g")
			.attr("id", "cluster-"+d.id.toString())
			.append("rect")
			.transition()
			.delay(durationTime)
			.attr("x", function() {
				return getDateScaleValue(div, d.mostEarlyTime);
			})
			.attr("y", function() {
				return y_offset;
			})
			.attr("width", function() {
				var temp = Math.max(getDateScaleValue(div, d.clusterCreateTime), getDateScaleValue(div, d.lastUpdateTime));
				return Math.max(minRectWidth, temp - getDateScaleValue(div, d.mostEarlyTime));
			})
			.attr("height", function() {
				return 1.0 / 5 * div.__height * y_padding;
			})
			.style("fill", function() {
				return color(d.id)
			})
			.style("opacity", 0.6)
			.attr("stroke",function(){
				if (d.IsNew) return "black";
				return color(d.id);
			})
			.attr("stroke-width", 2)

			// draw create line
			creatLine(div, d, y_offset + lineDY[0], 0, 0);
			creatLine(div, d, y_offset + lineDY[1], piechart_diameter+20, 1);


			// draw text
			var temptext = div.__g.selectAll("#cluster-"+d.id.toString())
			.append("text")
			.attr("x", function() {
				return getDateScaleValue(div, d.mostEarlyTime);
			})
			.attr("y", function() {
				return y_offset
			})
			.attr("width", function() {
				var temp = Math.max(getDateScaleValue(div, d.clusterCreateTime), getDateScaleValue(div, d.lastUpdateTime));
				return Math.max(minRectWidth, temp - getDateScaleValue(div, d.mostEarlyTime));
			})
			.attr("height", function() {
				//return Math.sqrt(d.size) / totalSize * div.__height * y_padding;
				return 1.0 / tempData.length * div.__height * y_padding;
			})
			.attr("id", "clusterText-"+d.id.toString())	


			drawClusterText(temptext, div, d, y_offset);	

			// add piechart
			drawPieChart(div,d, y_offset, piechart_diameter);
		}
		else {
			d3.selectAll("#cluster-"+d.id.toString())
			.selectAll("rect")
			.transition()
			//.delay(durationTime)
			.duration(durationTime)
			.attr("x", function() {
				return getDateScaleValue(div, d.mostEarlyTime);
			})
			.attr("y", function() {
				return y_offset
			})
			.attr("width", function() {
				var temp = Math.max(getDateScaleValue(div, d.clusterCreateTime), getDateScaleValue(div, d.lastUpdateTime));
				return Math.max(minRectWidth, temp - getDateScaleValue(div, d.mostEarlyTime));
			})
			.attr("height", function() {
				//return Math.sqrt(d.size) / totalSize * div.__height * y_padding;
				//return 1.0 / tempData.length * div.__height * y_padding;
				return 1.0 / 5 * div.__height * y_padding;
			})
			.style("fill", function() {
				return color(d.id)
			})
			.attr("stroke",function(){
				if (d.IsNew) return "black";
				return color(d.id);
			})
			.attr("stroke-width", 3)

			// move line
			updateLine(div, d, y_offset + lineDY[0], 0, 0);
			updateLine(div, d, y_offset + lineDY[1], piechart_diameter+20, 1);

			// move text
			var temptext = div.__g.selectAll("#cluster-"+d.id.toString())
			.selectAll("text")
			.attr("x", function() {
				return getDateScaleValue(div, d.mostEarlyTime);
			})
			.attr("y", function() {
				return y_offset
			})

			temptext.selectAll("tspan").remove();

			drawClusterText(temptext, div,d, y_offset);

			d3.selectAll("#piechart-"+d.id.toString())
			.remove();
			// add piechart
			drawPieChart(div,d, y_offset,piechart_diameter);
		}

		y_offset += (0.5 + y_padding/2) * div.__height / tempData.length;
	})
}

// Cheng added
function creatLine(div,d, liney, linex, num){
	div.__g.selectAll("#cluster-"+d.id.toString())
	.append("line")
	.transition()
	.delay(durationTime)
	.attr("x1", function() {
		return getDateScaleValue(div, d.mostEarlyTime) + linex;
	})
	.attr("y1", function() {
		return liney;
	})
	.attr("x2", function() {
		return Math.max(getDateScaleValue(div, d.clusterCreateTime), getDateScaleValue(div, d.lastUpdateTime));
	})
	.attr("y2", function() {
		return liney; 
	})
	.attr("stroke-width", 2)
	.attr("stroke", "#D2D2D2")
	.attr("id", "line-" + num)
}

function updateLine(div, d, liney, linex, num){
	div.__g.selectAll("#cluster-"+d.id.toString())
	.selectAll("#line-"+num)
	.transition()
	//.delay(durationTime)
	.duration(durationTime)
	.attr("x1", function() {
		return getDateScaleValue(div, d.mostEarlyTime) + linex;
	})
	.attr("y1", function() {
		return liney;
	})
	.attr("x2", function() {
		return Math.max(getDateScaleValue(div, d.clusterCreateTime), getDateScaleValue(div, d.lastUpdateTime));
	})
	.attr("y2", function() {
		return liney; 
	})

}


function drawPieChart(div,d,y_offset,diameter){
	div.__g.append("g")
	.attr("id", "piechart-"+d.id)
	.attr("x", function() {
		return getDateScaleValue(div, d.mostEarlyTime);
	})
	.attr("y", function() {
		return y_offset;
	})
	drawDonutChart('#piechart-'+d.id, d.size, 100*d.size/totalArticle,
		getDateScaleValue(div, d.mostEarlyTime)+10, y_offset+23, diameter, diameter,".200em");
}

function drawClusterText(temptext, div, d, y_offset){
	var tspanDY = [15,17,13,17,13];
	var piechart_offset = piechart_diameter + 20;;
	var m_temp = Math.max(getDateScaleValue(div, d.clusterCreateTime), getDateScaleValue(div, d.lastUpdateTime));
	var m_width = Math.max(minRectWidth, m_temp - getDateScaleValue(div, d.mostEarlyTime));
	//m_width = getDateScaleValue(div, d.lastUpdateTime) - getDateScaleValue(div, d.mostEarlyTime);
	var x_left = getDateScaleValue(div, d.mostEarlyTime);

	var keywordsText = "";
	d.keywords.forEach(function(l,i){
		if (l.value!="")
			keywordsText += l.value + "; "
	})
	appendTspan(temptext, keywordsText, x_left, m_width, tspanDY[0], ';', "keywordText")


	if (m_width <120) return;
	var tmpTitle0 = appendTspan(temptext, "S:   " + d.f_title,  x_left+piechart_offset, m_width-piechart_offset, tspanDY[1], ' ', "titleText")
	if (oldTitle[d.id][0]!= undefined && oldTitle[d.id][0] != d.f_title)
		tmpTitle0.attr("fill", "#9E1919");
	else {
		tmpTitle0.attr("fill", "black");
	}
	oldTitle[d.id][0] = d.f_title;

	var tmpTitle1 = appendTspan(temptext, "P:   " + d.p_title,  x_left+piechart_offset, m_width-piechart_offset, tspanDY[2], ' ', "titleText")
	if (oldTitle[d.id][1]!= undefined && oldTitle[d.id][1] != d.f_title)
		tmpTitle1.attr("fill", "#9E1919");
	else {
		tmpTitle1.attr("fill", "black");
	}
	oldTitle[d.id][1] = d.f_title;

	var detailX1 =  piechart_offset;
	if (detailData[d.id] != undefined){
		var detailX2 = detailX1 + 50;

		var timeString = detailData[d.id][0][0][0] + '-' + detailData[d.id][0][0][1] + '-' + 
		detailData[d.id][0][0][2];
		appendTspan(temptext, "" + timeString +  " " + detailData[d.id][0][1]
			, x_left + detailX2, m_width-detailX2, tspanDY[3], ' ', "newInsertText")
		appendTspanTwoLine(temptext, "" + detailData[d.id][0][3],x_left + detailX2, m_width-detailX2, tspanDY[4], ' ', "newInsertText")
	}
	temptext.append("tspan").attr("x",x_left + detailX1).attr("y",tspanDY[0] + tspanDY[1] + tspanDY[2] + tspanDY[3] + tspanDY[4] +y_offset)
	.text("Fresh: ")
	.attr("class", "freshText")
	

}
function appendTspan(text, str, x_left, width, height, character, cssClass) {
	var words = str.split(character);
	var sentence = "";
	var tspan = text.append("tspan")
	.attr("x", x_left)
	.attr('dy',height)
	.attr("class", cssClass)
	for(var i=0; i<words.length; ++i){
		if (i==0) {
			sentence = words[0];
			tspan.text(sentence);
		}else{
			tspan.text(sentence + character + words[i])
			if (tspan.node().getComputedTextLength() > (width - 10) ){
				tspan.text(sentence +  '...');
				break;
			}else{
				sentence += character + words[i];
			}
		}
	}
	return tspan;
}

function appendTspanTwoLine(text, str, x_left, width, height, character, cssClass) {
	var words = str.split(character);
	var sentence = "";
	var tspan = text.append("tspan")
	.attr("x", x_left)
	.attr('dy',height)
	.attr("class", cssClass)
	for(var i=0; i<words.length; ++i){
		if (i==0) {
			sentence = words[0];
			tspan.text(sentence);
		}else{
			tspan.text(sentence + character + words[i])
			if (tspan.node().getComputedTextLength() > (width - 12 ) ){
				tspan.text(sentence);
				break;
			}else{
				sentence += character + words[i];
			}
		}
	}
	str = str.replace(sentence,'');
	if (str.length>0){
		words = str.split(character);
		sentence = "";
		var tspan2 = text.append("tspan")
		.attr("x", x_left)
		.attr('dy',height)
		.attr("class", cssClass)
		for(var i=0; i<words.length; ++i){
			if (i==0) {
				sentence = words[0];
				tspan2.text(sentence);
			}else{
				tspan2.text(sentence + character + words[i])
				if (tspan2.node().getComputedTextLength() > (width - 15 ) ){
					tspan2.text(sentence + "...");
					break;
				}else{
					sentence += character + words[i];
				}
			}
		}
	}
	return [tspan,tspan2];
}

function updateScale(newData) {
	var minDateIndex = 0;
	var maxTemp = Number.MAX_VALUE;
	newData.forEach(function (d, i) {
		var tempValue = (d.mostEarlyTime[0]-1995) * 365 * 24 * 3600
		+ (d.mostEarlyTime[1]-1) * 30 * 24 * 3600
		+ d.mostEarlyTime[2] * 24 * 3600
		+ d.mostEarlyTime[3] * 3600
		+ d.mostEarlyTime[4] * 60
		+ d.mostEarlyTime[5];
		
		if (tempValue < maxTemp) {
			minDateIndex = i;
			maxTemp = tempValue;
		}

	})

	var startTime = newData[minDateIndex].mostEarlyTime;

	var div = d3.selectAll("#mainStage")[0][0];

	div.__x.domain([new Date(startTime[0], startTime[1]-1, startTime[2], startTime[3], startTime[4], startTime[5]), new Date(currentTime[0], currentTime[1]-1, currentTime[2], currentTime[3], currentTime[4], currentTime[5])])
	var t = div.__g.transition().duration(durationTime);
	t.select(".timeline").call(div.__xAxis);
}

function getDateScaleValue (div, startTime) {
	return div.__x(new Date(startTime[0], startTime[1]-1, startTime[2], startTime[3], startTime[4], startTime[5]))
}

function updateNewArticle (newData) {
	//detailData = [];
	var filled = [false,false,false,false,false];
	if (newData != undefined){
		for(var i=newData.length-1; i>=0; --i){
			var clusterNum = newData[i][4];
			if (filled[clusterNum]) continue;
			else {
				detailData[clusterNum] = [newData[i]];
				filled[clusterNum] = true;
			}
		}
	}

}
