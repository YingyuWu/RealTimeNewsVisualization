var durationTime = 750;
var minRectWidth = 35;

var horizontalPosition = [0, 0.3, 0.05, 0.3];
var tspanDY = 17;

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

		//div.__xAxis = d3.svg.axis()
		//				.scale(div.__x)
		//				.orient("top")
		//				.ticks(10)
						//.tickSubdivide(true)
						//.ticks(d3.time.hour, 1);

		//div.__g.append("g")
		//	.attr("class", "timeline")
		//	.call(div.__xAxis)

		//div.__y = d3.scale.linear().range([height-2*top-tempy, innerCicleRadius*tempy]).domain([0, 0.7]).clamp(true);
	}
}

function updateMainStage(newData) {
	//updateScale(newData);

	updateClusterVisualization(newData);
}

function updateClusterVisualization(newData) {
	
	

	var div = d3.selectAll("#mainStage")[0][0];
	var totalSize = 0;

	var existList = [];

	newData.forEach(function(d, i) {
		totalSize = totalSize + d.size;
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
			var newG = div.__g.append("g")
			.attr("id", "cluster-"+d.id.toString())

			newG.append("rect")
			.classed("groupbackground", true)
			.attr("y", function() {
				return y_offset
			})
			.attr("x", function() {
				return horizontalPosition[0] * div.__width;
			})
			.style("fill", function() {
				return color(d.id)
			})
			.style("opacity", 1)
			.attr("width", function() {
				//var temp = Math.max(getDateScaleValue(div, d.clusterCreateTime), getDateScaleValue(div, d.lastUpdateTime));
				return horizontalPosition[1] * div.__width;
			})
			.transition()
			.delay(500)
			.duration(500)	
			.attr("height", function() {
				//return Math.sqrt(d.size) / totalSize * div.__height * y_padding;
				return 1.0 / tempData.length * div.__height * y_padding;
			})

			var numberAndPercentageText = newG.append("text")
			.attr("x", function() {
				return horizontalPosition[0]*div.__width;
			})
			.attr("y", function() {
				return y_offset;
			})
			.style("opacity", 0)

			numberAndPercentageText.append("tspan")
			.attr("x", 0)
			.attr("dy", 40)
			.attr("font-size", 30)
			.text(function() {
				return d.size;
			})

			numberAndPercentageText.append("tspan")
			.attr("x", 0)
			.attr("dy", 40)
			.attr("font-size", 30)
			.text(function() {
				return (d.size*100/totalSize).toFixed(0) + "%";
			})

			var temptext = newG//div.__g.selectAll("#cluster-"+d.id.toString())
			.append("text")
			.attr("x", function() {
				return 0
			})
			.attr("y", function() {
				return y_offset
			})
			.style("opactiy", 0)

			var tempTitle = "# Title P : "+ d.p_title;
			var tempwidth = 40;
			if (tempTitle.length >= tempwidth*3) {
				var temptext1 = tempTitle.substring(0, tempwidth);
				var temptext2 = tempTitle.substring(tempwidth, 2*tempwidth);
				var temptext3 = tempTitle.substring(2*tempwidth);
				var temp = [temptext1, temptext2, temptext3]

				temp.forEach(function(t, ti) {
					temptext.append("tspan")
					.attr("x", function() {
						if (ti == 0) return 60;
						else return 90;
					})
					.attr('dy', tspanDY)
					.text(t);
				})
			}
			else if (tempTitle.length >= tempwidth) {
				var temptext1 = tempTitle.substring(0, tempwidth);
				var temptext2 = tempTitle.substring(tempwidth);
				var temp = [temptext1, temptext2]

				temp.forEach(function(t, ti) {
					temptext.append("tspan")
					.attr("x", function() {
						if (ti == 0) return 60;
						else return 90;
					})
					.attr('dy', tspanDY)
					.text(t);
				})
			}
			else {
				temptext.append("tspan")
				.attr("x", function() {
					return 60
				})
				.attr('dy', tspanDY)
				.text(tempTitle);
			}

			var tempKeywords = "# Keywords : ";
			d.keywords.forEach(function(l, li) {
				tempKeywords += l.value + "; "
			})

			var tempN = Math.round(tempKeywords.length/tempwidth+0.5);
			for (var ti=0; ti<tempN; ++ti) {
				var tempText = tempKeywords.substring(ti*tempwidth, (ti+1)*tempwidth);
				if (ti == 2 && tempN > 3) {
					tempText += " ...";
					temptext.append("tspan")
					.attr("x", function() {
						return 60
					})
					.attr('dy', tspanDY)
					.text(tempText);

					break;
				}
				else {
					temptext.append("tspan")
					.attr("x", function() {
						return 60
					})
					.attr('dy', tspanDY)
					.text(tempText);
				}
			}

			newG.selectAll("text")
			.transition()
			.delay(1000)
			.duration(500)
			.style("opacity", 1)
		}
		else {
			var newG = d3.selectAll("#cluster-"+d.id.toString())

			newG.selectAll(".groupbackground")
			.transition()
			.duration(500)
			.attr("height", function() {
				//return Math.sqrt(d.size) / totalSize * div.__height * y_padding;
				return 1.0 / tempData.length * div.__height * y_padding;
			})
			.transition()
			.delay(500)
			.duration(500)
			.attr("x", function() {
				return horizontalPosition[0] * div.__width;
			})
			.attr("y", function() {
				return y_offset
			})
			.attr("width", function() {
				//var temp = Math.max(getDateScaleValue(div, d.clusterCreateTime), getDateScaleValue(div, d.lastUpdateTime));
				return horizontalPosition[1] * div.__width;
			})
			.style("fill", function() {
				return color(d.id)
			})
			
			

			var tempDeleteText = newG.selectAll("text")

			var numberAndPercentageText = newG
			.append("text")
			.attr("x", function() {
				return horizontalPosition[0]*div.__width;
			})
			.attr("y", function() {
				return y_offset;
			})
			.style("opacity", 0)

			numberAndPercentageText.append("tspan")
			.attr("x", 0)
			.attr("dy", 40)
			.attr("font-size", 30)
			.text(function() {
				return d.size;
			})

			numberAndPercentageText.append("tspan")
			.attr("x", 0)
			.attr("dy", 40)
			.attr("font-size", 30)
			.text(function() {
				return (d.size*100/totalSize).toFixed(0) + "%";
			})

			var temptext = newG//div.__g.selectAll("#cluster-"+d.id.toString())
			.append("text")
			.attr("x", function() {
				return 0
			})
			.attr("y", function() {
				return y_offset
			})
			.style("opacity", 0)

			var tempTitle = "# Title P : "+ d.p_title;
			var tempwidth = 40;
			if (tempTitle.length >= tempwidth*3) {
				var temptext1 = tempTitle.substring(0, tempwidth);
				var temptext2 = tempTitle.substring(tempwidth, 2*tempwidth);
				var temptext3 = tempTitle.substring(2*tempwidth);
				var temp = [temptext1, temptext2, temptext3]

				temp.forEach(function(t, ti) {
					temptext.append("tspan")
					.attr("x", function() {
						if (ti == 0) return 60;
						else return 90;
					})
					.attr('dy', tspanDY)
					.text(t);
				})
			}
			else if (tempTitle.length >= tempwidth) {
				var temptext1 = tempTitle.substring(0, tempwidth);
				var temptext2 = tempTitle.substring(tempwidth);
				var temp = [temptext1, temptext2]

				temp.forEach(function(t, ti) {
					temptext.append("tspan")
					.attr("x", function() {
						if (ti == 0) return 60;
						else return 90;
					})
					.attr('dy', tspanDY)
					.text(t);
				})
			}
			else {
				temptext.append("tspan")
				.attr("x", function() {
					return 60
				})
				.attr('dy', tspanDY)
				.text(tempTitle);
			}

			var tempKeywords = "# Keywords : ";
			d.keywords.forEach(function(l, li) {
				tempKeywords += l.value + "; "
			})

			var tempN = Math.round(tempKeywords.length/tempwidth+0.5);
			for (var ti=0; ti<tempN; ++ti) {
				var tempText = tempKeywords.substring(ti*tempwidth, (ti+1)*tempwidth);
				if (ti == 2 && tempN > 3) {
					tempText += " ...";
					temptext.append("tspan")
					.attr("x", function() {
						return 60
					})
					.attr('dy', tspanDY)
					.text(tempText);

					break;
				}
				else {
					temptext.append("tspan")
					.attr("x", function() {
						return 60
					})
					.attr('dy', tspanDY)
					.text(tempText);
				}
			}

			tempDeleteText
			.transition()
			.duration(500)
			.style("opacity", 0)
			.remove();

			temptext
			.transition()
			.delay(400)
			.duration(500)
			.style("opacity", 1)

			numberAndPercentageText
			.transition()
			.delay(300)
			.duration(500)
			.style("opacity", 1)
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
	if (currentClusterData == undefined) return;

	var div = d3.selectAll("#mainStage")[0][0];

	var newArticleClusters = currentClusterData.map(function(d) {
		return newData.filter(function(l) {
			if (l[4] == d.id) return true;
			else return false;
		})
	})

	console.log(newArticleClusters)

	newArticleClusters.forEach(function(d) {
		if (d.length == 0) return;
		else {
			var newRect = d3.selectAll("#cluster-"+d[0][4].toString()).selectAll(".groupbackground")

			var tempN = d.length;
			var x = newRect.attr("x");
			var y = newRect.attr("y");

			var newG = div.__g.append("g")
			.classed("newArticleBackground", true)
			.attr("transform", function() {
				return "translate("+div.__width+","+y+")";
			})

			newG.append("rect")	
			.attr("y", function() {
				return 0
			})
			.attr("x", function() {
				return 0;
			})
			.style("fill", function() {
				return color(d[0][4])
			})
			.style("opacity", 1)
			.attr("width", function() {
				//var temp = Math.max(getDateScaleValue(div, d.clusterCreateTime), getDateScaleValue(div, d.lastUpdateTime));
				return horizontalPosition[3] * div.__width;
			})
			.attr("height", function() {
				//return Math.sqrt(d.size) / totalSize * div.__height * y_padding;
				return newRect.attr("height");
			})

			var numberAndPercentageText = newG.append("text")
			.style("opacity", 0)

			numberAndPercentageText.append("tspan")
			.attr("x", 10)
			.attr("dy", 40)
			.attr("font-size", 40)
			.text(function() {
				return d.length;
			})

			var tempTextElement = newG//div.__g.selectAll("#cluster-"+d.id.toString())
			.append("text")
			.attr("x", function() {
				return 50
			})
			.attr("y", function() {
				return 0
			})
			.style("opactiy", 0)

			var tempwidth = 40;
			var tempLineNumber = 0;
			d.forEach(function(l, li) {
				if (tempLineNumber >= 4) {
					return;
				}

				var tempTitle = "Title "+li+":"+l[1];
				var tempN = Math.round(tempTitle.length/tempwidth+0.5);

				for (var ti=0; ti<tempN; ++ti, ++tempLineNumber) {
					var tempText = tempTitle.substring(ti*tempwidth, (ti+1)*tempwidth);

					if (tempLineNumber >= 4) {
						tempText += " ...";
						tempTextElement.append("tspan")
						.attr("x", function() {
							return 60
						})
						.attr('dy', tspanDY)
						.text(tempText);

						break;
					}
					if (ti == 0) {
						tempTextElement.append("tspan")
						.attr("x", function() {
							return 40
						})
						.attr('dy', tspanDY)
						.text(tempText);
					}
					else if (ti == 1 && tempN > 2) {
						tempText += " ...";
						tempTextElement.append("tspan")
						.attr("x", function() {
							return 60
						})
						.attr('dy', tspanDY)
						.text(tempText);

						break;
					}
					else {
						tempTextElement.append("tspan")
						.attr("x", function() {
							return 60
						})
						.attr('dy', tspanDY)
						.text(tempText);
					}
				}
			})

			tempTextElement.append("tspan")
			.attr("x", function() {
				return 40
			})
			.attr('dy', tspanDY)
			.text(function(){
				return "Keywords : "+d[0][2][0] + ";" + d[0][2][1] + ";" + d[0][2][2] + ";"			 
			});

			newG.selectAll("text")
			.transition()
			.delay(500)
			.duration(500)
			.style("opacity", 1)

			newG
			.transition()
			.delay(500)
			.duration(1000)
			.attr("transform", function() {
				return "translate("+((horizontalPosition[0]+horizontalPosition[1]+horizontalPosition[2]) * div.__width)+","+y+")";
			})

			newG
			.transition()
			.delay(5500)
			.duration(2000)
			.attr("transform", function() {
				return "translate("+((horizontalPosition[0]) * div.__width)+","+y+")";
			})
			.style("opacity", 0)
			.remove();
		}
		
	})
}
