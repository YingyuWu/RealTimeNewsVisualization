var durationTime = 750;
var minRectWidth = 35;

function initilizeMainStage(width, height, left, top, right) {
	if (d3.selectAll("#mainStage")[0].length == 0){
		var div = null;

		div = d3.select("#ClusterVisualizationSection")
			.append("div")
			.attr("id", "mainStage")
			.attr("class", "mainStage")[0][0];
		div.__svg = d3.select(div)
		    .append("svg")
		    .attr("width", width)
		    .attr("height", height)

		div.__g = div.__svg
				    .append("g")
				    .attr("transform", "translate("+left+","+top+")")

		div.__top = top;
		div.__left = left;
		div.__width = width - left;
		div.__height = height - top;

		div.__x = d3.time.scale().range([0, width-left-right]).clamp(true);

		div.__xAxis = d3.svg.axis()
						.scale(div.__x)
						.orient("top")
						.ticks(10)
						//.tickSubdivide(true)
						//.ticks(d3.time.hour, 1);

		div.__g.append("g")
			.attr("class", "timeline")
			.call(div.__xAxis)

		//div.__y = d3.scale.linear().range([height-2*top-tempy, innerCicleRadius*tempy]).domain([0, 0.7]).clamp(true);
	}
}

function updateMainStage(newData) {
	updateScale(newData);

	updateClusterVisualization(newData);
}

function updateClusterVisualization(newData) {

	var tspanDY = 15;

	var div = d3.selectAll("#mainStage")[0][0];
	var totalSize = 0;

	var existList = [];

	newData.forEach(function(d, i) {
		totalSize = totalSize + Math.sqrt(d.size);
		existList.push(d.id);
	})

	var y_offset = div.__top;
	var y_padding = 0.95;

	for (var tempi=0; tempi<30; ++tempi) {
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

			d3.selectAll("#cluster-"+tempi.toString())
			.transition()
			.delay(durationTime)
			.remove();
		}
	}

	var tempData = jQuery.extend(true, [], newData);

	tempData.sort(function (a, b) {
		return a.id > b.id;
	})

	tempData.forEach(function(d, i) {
		if (d3.selectAll("#cluster-"+d.id.toString())[0].length == 0) {
			div.__g.append("g")
			.attr("id", "cluster-"+d.id.toString())
			.append("rect")
			.transition()
			.delay(durationTime)
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
			.style("fill", function() {
				return color(d.id)
			})
			.style("opacity", 0.6)

			div.__g.selectAll("#cluster-"+d.id.toString())
			.append("line")
			.transition()
			.delay(durationTime)
			.attr("x1", function() {
				return getDateScaleValue(div, d.clusterCreateTime)
			})
			.attr("y1", function() {
				return y_offset
			})
			.attr("x2", function() {
				return getDateScaleValue(div, d.clusterCreateTime)
			})
			.attr("y2", function() {
				//return y_offset + Math.sqrt(d.size) / totalSize * div.__height * y_padding; 
				return y_offset + 1.0 / tempData.length * div.__height * y_padding; 
			})
			.attr("stroke-width", 4)
			.attr("stroke", function() {
				return d3.rgb(color(d.id)).darker()
			})

			var temptext = div.__g.selectAll("#cluster-"+d.id.toString())
			.append("text")
			.attr("x", function() {
				return getDateScaleValue(div, d.mostEarlyTime);
			})
			.attr("y", function() {
				return y_offset
			})

			temptext.append("tspan")
			.attr("x", function() {
				return getDateScaleValue(div, d.mostEarlyTime);
			})
			.attr('dy', tspanDY)
			.text("# Title F : "+ d.f_title);

			temptext.append("tspan")
			.attr("x", function() {
				return getDateScaleValue(div, d.mostEarlyTime);
			})
			.attr('dy', tspanDY)
			.text("# Title G : "+ d.g_title);

			temptext.append("tspan")
			.attr("x", function() {
				return getDateScaleValue(div, d.mostEarlyTime);
			})
			.attr('dy', tspanDY)
			.text("# Title P : "+ d.p_title);

			temptext.append("tspan")
			.attr("x", function() {
				return getDateScaleValue(div, d.mostEarlyTime);
			})
			.attr('dy', tspanDY)
			.text(function() {
				var result = "";
				d.keywords.forEach(function(l, i) {
					result += l.value + "; "
				})
				return "# Keywords : "+ result
			});					
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
				return 1.0 / tempData.length * div.__height * y_padding;
			})
			.style("fill", function() {
				return color(d.id)
			})

			div.__g.selectAll("#cluster-"+d.id.toString())
			.selectAll("line")
			.transition()
			//.delay(durationTime)
			.duration(durationTime)
			.attr("x1", function() {
				return getDateScaleValue(div, d.clusterCreateTime)
			})
			.attr("y1", function() {
				return y_offset
			})
			.attr("x2", function() {
				return getDateScaleValue(div, d.clusterCreateTime)
			})
			.attr("y2", function() {
				//return y_offset + Math.sqrt(d.size) / totalSize * div.__height * y_padding;
				return y_offset + 1.0 / tempData.length * div.__height * y_padding;
			})
			.attr("stroke-width", 4)
			.attr("stroke", function() {
				return d3.rgb(color(d.id)).darker()
			})

			var temptext = div.__g.selectAll("#cluster-"+d.id.toString())
			.selectAll("text")
			.attr("x", function() {
				return getDateScaleValue(div, d.mostEarlyTime);
			})
			.attr("y", function() {
				return y_offset
			})

			temptext.selectAll("tspan").remove();

			temptext.append("tspan")
			.attr("x", function() {
				return getDateScaleValue(div, d.mostEarlyTime);
			})
			.attr('dy', tspanDY)
			.text("# Title F : "+ d.f_title);

			temptext.append("tspan")
			.attr("x", function() {
				return getDateScaleValue(div, d.mostEarlyTime);
			})
			.attr('dy', tspanDY)
			.text("# Title G : "+ d.g_title);

			temptext.append("tspan")
			.attr("x", function() {
				return getDateScaleValue(div, d.mostEarlyTime);
			})
			.attr('dy', tspanDY)
			.text("# Title P : "+ d.p_title);

			temptext.append("tspan")
			.attr("x", function() {
				return getDateScaleValue(div, d.mostEarlyTime);
			})
			.attr('dy', tspanDY)
			.text(function() {
				var result = "";
				d.keywords.forEach(function(l, i) {
					result += l.value + "; "
				})
				return "# Keywords : "+ result
			});	
		}

		//y_offset += Math.sqrt(d.size) / totalSize * div.__height * y_padding + div.__height * (1 - y_padding) / tempData.length;
		y_offset += 1 / tempData.length * div.__height * y_padding + div.__height * (1 - y_padding) / tempData.length;
	})
}

function updateScale(newData) {
	var minDateIndex = 0;
	var maxTemp = Number.MAX_VALUE;
	newData.forEach(function (d, i) {
		var tempValue = (d.mostEarlyTime[0]-2010) * 365 * 24 * 3600
						+ d.mostEarlyTime[1] * 30 * 24 * 3600
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

	div.__x.domain([new Date(startTime[0], startTime[1], startTime[2], startTime[3], startTime[4], startTime[5]), new Date(currentTime[0], currentTime[1], currentTime[2], currentTime[3], currentTime[4], currentTime[5])])
	var t = div.__g.transition().duration(durationTime);
	t.select(".timeline").call(div.__xAxis);
}

function getDateScaleValue (div, startTime) {
	return div.__x(new Date(startTime[0], startTime[1], startTime[2], startTime[3], startTime[4], startTime[5]))
}

function updateNewArticle (newData) {

}
