var fill_opacity = 0.6;
var z_index = 3;
var visualizationPadding = 0.11;
var pie = d3.layout.pie()
                   .sort(null);
var innerCicleRadius = 1.1

var pieChartColor = d3.scale.category10();

function initilizeStoryTellingVisualizationSVG(charID, width, height, left, top)
{
	if (d3.selectAll("#StoryTellingVisualization-"+charID.toString())[0].length == 0){
		var div = null;

		div = d3.select("#ClusterVisualizationSection")
			.append("div")
			.attr("id", "StoryTellingVisualization-"+charID.toString())
			.attr("class", "StoryTellingVisualization")[0][0];
		div.__svg = d3.select(div)
		    .append("svg")
		    .attr("width", width)
		    .attr("height", height)

		div.__svg.append("g")
			.attr("id", "forceNodeGroup");

		div.__g = div.__svg
				    .append("g")
				    .attr("transform", "translate("+left+","+top+")")

		div.__width = div.__g.attr("width");
		div.__height = height;
		div.__id = charID;

		var tempx = width * visualizationPadding;
		var tempy = height * visualizationPadding;

		div.__radius = d3.scale.linear().range([tempx/2, tempx]).domain([10, 60]).clamp(true);
		div.__x = d3.scale.linear().range([tempx, width-2*left-width*0.2]).domain([0, 0.7]).clamp(true);
		div.__y = d3.scale.linear().range([height-2*top-tempy, innerCicleRadius*tempy]).domain([0, 0.7]).clamp(true);
	}
}

function generateAndUpdateStoryTellingVisualization(newdata, charID, width, height, left, top, cluster_delay){
	var div = null;
	var timeColorScale = d3.scale.pow()
							.domain([10, 40])
							.range(["#FF0000", "#00FF00"])
							.clamp(true)

	div = d3.selectAll("#StoryTellingVisualization-"+charID.toString())[0][0];
	div.__data = newdata;

	var my_circle_list = d3.select(div).selectAll(".cluster_circle")[0]
	my_circle_list.forEach(function(c){
		c.__checkExist = false;
	})

	newdata.forEach(function(d, i){
		var cluster = div.__g.select("#cluster_circle-"+d.id);

		if (d.IsNew){
			if (cluster[0][0] != null){
				cluster.selectAll("text").remove();
				cluster.selectAll("line").remove();
				cluster.selectAll("rect").remove();

				cluster.selectAll("path")
				.transition()
				.duration(cluster_delay)
				.attr("d", generateArcByRadius(2))

				cluster.select("circle")
				.transition()
		            .duration(cluster_delay)
				.attr("r", 2)

				cluster
				.transition()
				.delay(cluster_delay)
				.remove();
			}

			cluster[0][0] = null;
		}

		if (cluster[0][0] == null){
			cluster = div.__g.append("g")
						.datum(d)
						.attr("id", "cluster_circle-"+d.id)
						.attr("class", "cluster_circle")
						.attr("transform", function(d) { return "translate(" + (div.__x(d.position[0])) + "," + (div.__y(d.position[1])) + ")"; });
		
			cluster.append("circle")
			.style("fill", function(d, i){
				return color(d.id);})
			//.style("fill-opacity", fill_opacity)
			.attr("r", function(d, i){
				return 0;
			})	
			.attr("cx", function(d) { return 0 })
			.attr("cy", function(d) { return 0 })
			.transition()
	            .duration(cluster_delay)
			.attr("r", function(d, i){
				return div.__radius(d.size)/innerCicleRadius;
			})	

			cluster.selectAll("path")
					.data(pie(generatePieDataByNewData(d)))
					.enter().append("path")
					.attr("fill", function(d, i){ 
						if (i == 1){
							return timeColorScale(d.data);
						}
						else if (i == 2){
							return "#aaaaaa"
						}
						else{
							return pieChartColor(i);
						}
					})
					.style("opacity", function(d, i){
						if (i ==0 || i==3){
							return 0.0;
						}
						else if (i == 1 || i == 2){
							return fill_opacity;
						}
						else{
							return 0.0;
						}
					})
					.attr("d", generateArcByRadius(2))
					.each(function(d) { this._current = d; })
					.attr("transform", "rotate(-90 0 0)")
					.transition()
	            		.duration(cluster_delay)
					.attr("d", generateArcByRadius(div.__radius(d.size)))

			cluster.append("text")
			.attr("class", "summerizeInformationOfCluster")
			.attr("text-anchor", "middle")
			//.attr("font-weight", "bold")
			.attr("background-color", "#000000")

			cluster.append("svg:title")

			appendLineForKeywords(cluster, div.__radius(d.size));

			cluster[0][0].__checkExist = true;
		}
		else{
			cluster
			.datum(d)
			.transition()
            	.duration(cluster_delay)
			.attr("transform", function(d) { return "translate(" + div.__x(d.position[0]) + "," + div.__y(d.position[1]) + ")"; });
				
			
			cluster.selectAll("path")
				.transition()
	            	.duration(cluster_delay)
				.attr("d", generateArcByRadius(div.__radius(d.size)))
				.each(function(t){ 
					this.__arc = generateArcByRadius(div.__radius(d.size))
					this.__d = d;
				})

			cluster.select("circle")
				.datum(d)
				.transition()
	            	.duration(cluster_delay)
				.attr("r", function(d, i){
					return div.__radius(d.size)/innerCicleRadius;
				})

			cluster.selectAll("path")
				.data(pie(generatePieDataByNewData(d)))
				.transition()
					.delay(cluster_delay)
	            	.duration(500)
				.attrTween("d", arcTween)
				.attr("fill", function(d, i){ 
					if (i == 1){
						return timeColorScale(d.data);
					}
					else if (i == 2){
						return "#aaaaaa"
					}
					else{
						return pieChartColor(i);
					}
				})

			cluster[0][0].__checkExist = true;
		}

		function clusterOnClick(d){
			onClickClusterNodes(d,div);
		}
		
		cluster.on("click", clusterOnClick)

		cluster.select(".summerizeInformationOfCluster")//.selectAll("path")
			.each(generateSummerizeInformationForCluster)

		cluster.select("title")
			.datum(d)
			.text(function(d, i) {
				var result = "";
				for (var k=0; k<d.keywords.length; ++k){
					result += d.keywords[k].value + "#"
				} 
				return result; 
			});

		updateLineForKeywords(cluster, div.__radius(d.size))
	})

	var my_circle_list = d3.select(div).selectAll(".cluster_circle")[0]

	my_circle_list.forEach(function(c){
		if (!c.__checkExist){
			d3.select(c).selectAll("text").remove();
			d3.select(c).selectAll("line").remove();
			d3.select(c).selectAll("rect").remove();

			d3.select(c).selectAll("path")
			.transition()
			.duration(cluster_delay)
			.attr("d", generateArcByRadius(2))

			d3.select(c).select("circle")
			.transition()
	            .duration(cluster_delay)
			.attr("r", 2)

			d3.select(c)
			.transition()
			.delay(cluster_delay)
			.remove();
		}

		d3.select(c)
		.style("fill-opacity", fill_opacity)
	})

	if (cluster_selected == true){
		d3.select("#cluster_circle-"+cluster_selected_id)
		.style("fill-opacity", 0)
	}
}

function generatePieDataByNewData(newData){
	var result;
	var keywordNumber = 7;

	var tempcurrentTime = currentTime.slice(0, 3); // [year, month, day, hour, minutes, second] 
	var tempcreateTime = newData.clusterCreateTime.split("T")[0].split("-").map(Number);
	var templastupDateTime = newData.lastUpdateTime.substring(1, newData.lastUpdateTime.length-1).split("T")[0].split("-").map(Number);

	var time = calculateTime(tempcurrentTime, tempcreateTime, templastupDateTime);

	result = [5,time,40-time,5]

	var result_keyword = [];

	var keywords = newData.keywords.slice(0, keywordNumber);

	var sum = 0;
	keywords.forEach(function(d, i){
		sum += d.occurrence;
	})

	keywords.forEach(function(d, i){
		result_keyword.push(d.occurrence*50.0/sum);
	})

	return result.concat(result_keyword.reverse());
}

function calculateTime(tempcurrentTime, tempcreateTime, templastupDateTime){
	var a = (templastupDateTime[0]-tempcreateTime[0])*360.0
	a += (templastupDateTime[1]-tempcreateTime[1])*30.0
	a += (templastupDateTime[2]-tempcreateTime[2])

	if (a < 0) a = 0;

	var b = (tempcurrentTime[0]-tempcreateTime[0])*360.0
	b += (tempcurrentTime[1]-tempcreateTime[1])*30.0
	b += (tempcurrentTime[2]-tempcreateTime[2])


	var c = (tempcurrentTime[0]-tempcreateTime[0])*360.0
	c += (tempcurrentTime[1]-tempcreateTime[1])*30.0
	c += (tempcurrentTime[2]-tempcreateTime[2])

	if (c == 0) c = 1;

	var tempscale = d3.scale.linear()
		.domain([0, 1])
		.range([0, 40])
		.clamp(true)

	if (b == 0){
		return tempscale(1)
	}
	else if (a == 0){
		return tempscale(1/c)	
	}
	else{
		return tempscale(a/b)
	}
}

function generateArcByRadius(r){
	result = d3.svg.arc()
				.innerRadius(r/innerCicleRadius)
				.outerRadius(r)

	return result;
}

function arcTween(d) {
  var i = d3.interpolate(this._current, d);
  this._current = i(0);
  var tempArc = this.__arc
  return function(t) {
    return tempArc(i(t));
  };
}

function generateAndUpdateStoryTellingVisualization_old(newdata, charID, width, height, left, top, cluster_delay)
{
	var div = null;

	div = d3.selectAll("#StoryTellingVisualization-"+charID.toString())[0][0];
	div.__data = newdata;

	var my_circle_list = d3.select(div).selectAll(".cluster_circle")[0]
	my_circle_list.forEach(function(c){
		c.__checkExist = false;
	})

	newdata.forEach(function(newcluster, i){
		var clusters = div.__g.select("#cluster_circle-"+newcluster.id);

		if (newcluster.IsNew){
			if (clusters.select("circle")[0][0] != null){
				clusters.select("circle")
				.transition()
		            .duration(cluster_delay)
				.attr("r", 0)

				clusters
				.transition()
				.delay(cluster_delay)
				.remove();
			}

			clusters[0][0] = null;
		}

		if (clusters[0][0] == null){
			clusters = div.__g.append("g")
			.datum(newcluster)
			.attr("id", "cluster_circle-"+newcluster.id)
			.attr("class", "cluster_circle")
			.attr("transform", function(d) { return "translate(" + (div.__x(d.position[0])) + "," + (div.__y(d.position[1])) + ")"; });
		
			if (cluster_selected == true){
				fill_opacity = 0.5;
			}
			else{
				fill_opacity = 0.5;
			}

			clusters
			.append("circle")
			.style("fill", function(d, i){
				return color(d.id);})
			.style("fill-opacity", fill_opacity)
			.attr("r", function(d, i){
				return 0;
			})	
			.attr("cx", function(d) { return 0 })
			.attr("cy", function(d) { return 0 })
			.transition()
	            .duration(cluster_delay)
			.attr("r", function(d, i){
				return div.__radius(d.size);
			})	

			clusters.append("text")
				.attr("class", "headline")
				.attr("dy", ".35em")
				.attr("text-anchor", "middle")
				.style("font-size", "12px")
				.attr("fill", function(d){
		            return "#000000"//color(d.cid);
		        })
				.each(generateHeadLineForCluster)

			clusters.append("svg:title")
			.text(function(d, i) {
				var result = "";
				for (var k=0; k<d.keywords.length; ++k){
					result += d.keywords[k] + "#"
				} 
				return result; 
			});

			clusters[0][0].__checkExist = true;

			//hightlight(clusters,newcluster,div,2000);

 			function clusterOnClick(d){
 				onClickClusterNodes(d,div);
     		 }

     		//clusters.select("circle").on("click", clusterOnClick)
     		clusters.on("click", clusterOnClick)

     		if(clusters.select("#pie-chart")[0].length != 0){
     			
				//clusters.select("#pie-chart").on("click", clusterOnClick)
			}
		}
		else{
			//if(clusters[0][0].__select_status != true){
				clusters
				.datum(newcluster)
				.transition()
	            	.duration(cluster_delay)
				.attr("transform", function(d) { return "translate(" + div.__x(d.position[0]) + "," + div.__y(d.position[1]) + ")"; });
				
				clusters.select("circle")
				.datum(newcluster)
				.transition()
	            	.duration(cluster_delay)
				.attr("r", function(d, i){
					return div.__radius(d.size);
				})

				clusters.select("text")
				.datum(newcluster)
				.attr("fill", function(d){
		            return "#000000"//color(d.id);
		        })
				.each(generateHeadLineForCluster)

			//}
			//else{
			//	clusters.datum(newcluster);
			//}
		}


		clusters.select("title")
		.datum(newcluster)
		.text(function(d, i) {
			var result = "";
			for (var k=0; k<d.keywords.length; ++k){
				result += d.keywords[k].value + "#"
			} 
			return result; 
		});

		clusters[0][0].__checkExist = true;

		//hightlightreview(clusters,newcluster,div,2000);

		function clusterOnClick(d){
			onClickClusterNodes(d,div);
		}
		
		//clusters.select("circle").on("click", clusterOnClick)
		clusters.on("click", clusterOnClick)

		if(clusters.select("#pie-chart")[0].length != 0){
			//clusters.select("#pie-chart").on("click", clusterOnClick)
		}
	})

	var my_circle_list = d3.select(div).selectAll(".cluster_circle")[0]
	my_circle_list.forEach(function(c){
		if (!c.__checkExist){
			d3.select(c).select("circle")
			.transition()
	            .duration(cluster_delay)
			.attr("r", 0)

			if(c.__select_status == true){
				if(d3.selectAll("#inner-area")[0].length != 0){
					d3.selectAll("#inner-area").remove();
				}
				if(d3.selectAll("#pie-chart")[0].length != 0){
					d3.selectAll("#pie-chart").remove();
				}

				if(d3.selectAll("#left-button")[0].length != 0){
					d3.selectAll("#left-button").remove();
				}
				if(d3.selectAll("#right-button")[0].length != 0){
					d3.selectAll("#right-button").remove();
				}
				cluster_selected = false;

				d3.select(div).selectAll(".cluster_circle").style("opacity", 0.8);
				d3.select(div).selectAll(".forceNode").style("opacity", 0.8);
				c.__select_status = false;
			}

			d3.select(c)
			.transition()
			.delay(cluster_delay)
			.remove();
		}
	})
}

function updateDateTime(timeData){
	if (timeData != null){
		var datestring = ""+timeData[0]+'-'+timeData[1]+'-'+timeData[2]; //+' '+timeData[3]+":"+timeData[4]+":"+timeData[5];
		d3.select("#date_time").text(datestring);
	}
}

function generateHeadLineForCluster(d, i){
		var el = d3.select(this);
		el.text('');

		var tempTimeString = ""
		tempTimeString += d.clusterCreateTime.substr(0, 10); //"2013-09-04T00:00:00Z"
		tempTimeString += " -> ";
		tempTimeString += d.lastUpdateTime.substr(1, 10);  //""2013-09-08T00:00:00Z"";

		var createTimeString = el.append('tspan').text(tempTimeString);
	    createTimeString.attr('x', 0).attr('y', '-45');

	    var keywordString1 = el.append('tspan').text("#"+d.keywords[0].value+";");
	    keywordString1.attr('x', 0).attr('dy', '20');

	    var keywordString2 = el.append('tspan').text("#"+d.keywords[1].value+";");
	    keywordString2.attr('x', 0).attr('dy', '15');

	    var gapspan = el.append('tspan').text("-----------------");
	    gapspan.attr('x', 0).attr('dy', '15');

	    //var words = d.headline.split(' ');

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

	    	if (tempLength > 15){
	    		var tspan = el.append('tspan').text(tempString);
	    		tspan.attr('x', 0).attr('dy', '15');

	    		tempString = "";
	    		tempLength = 0;
	    	}          
	    }

	    var tspan = el.append('tspan').text(tempString);
		tspan.attr('x', 0).attr('dy', '15');
}

function generateSummerizeInformationForCluster(d, i){
		var el = d3.select(this);
		el.text('');
	    var div = d3.selectAll("#StoryTellingVisualization-0")[0][0];

	    var radius = div.__radius(d.size)

	    var titlelinenumber = 0;
	    var titlefontsize = getFontSizeByRadius(radius);

	    if (radius > 25){
			var tempTimeString = ""
			tempTimeString += d.clusterCreateTime.substr(0, 10); //"2013-09-04T00:00:00Z"
			//tempTimeString += " -> ";
			//tempTimeString += d.lastUpdateTime.substr(1, 10);  //""2013-09-08T00:00:00Z"";

			var createTimeString = el.append('tspan').text(tempTimeString).attr("class", "timeTitle");
		    createTimeString.attr('x', 0).attr('y', radius/innerCicleRadius*-1).attr("font-size", 10);

	    }

	    if (radius > 20){
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
		    var line1 = "";
		    var line2 = "";

		    for (var i = 0; i < words.length; i++) {

		    	tempLength += words[i].length;
	    		tempString += words[i] + " ";

		    	if (tempLength > radius/6.0){
		    		if (titlelinenumber == 0){
		    			line1 = tempString;
		    			titlelinenumber += 1;
		    			tempLength = 0;
		    			tempString = ""
		    		}
		    		else if (titlelinenumber == 1){
		    			line2 = tempString;
		    			tempLength = 0;
		    			titlelinenumber += 1;
		    			tempString = ""
		    		}
		    		else{
		    			break;
		    		}
		    		/*
		    		if (titlelinenumber == 0){
			    		var tspan = el.append('tspan').text(tempString).attr("class", "headlineTitle");
		    			tspan.attr('x', 0).attr('y', radius*-1 - 2*titlefontsize).attr("font-size", titlefontsize);
		    		}
		    		else{
			    		var tspan = el.append('tspan').text(tempString).attr("class", "headlineTitle");
		    			tspan.attr('x', 0).attr('dy', titlefontsize).attr("font-size", titlefontsize);
		    		}
		    		*/
		    	}     
		    }

		    if (titlelinenumber == 0 && tempString != ""){
		    	line1 = tempString;
		    	titlelinenumber += 1;
		    }

		    if (titlelinenumber == 1 && tempString != ""){
		    	line2 = tempString;
		    	titlelinenumber += 1;
		    }

		    if (titlelinenumber == 2 && tempString != ""){
		    	line2 += " ...\"";
		    }

		    if (titlelinenumber == 1){
		    	var tspan = el.append('tspan').text(line1);
				tspan.attr('x', 0).attr('y', radius/innerCicleRadius*-1 - titlelinenumber*titlefontsize).attr("font-size", titlefontsize).attr("font-weight", "bold");
		    }
		    else if (titlelinenumber == 2){
		    	var tspan = el.append('tspan').text(line1);
				tspan.attr('x', 0).attr('y', radius/innerCicleRadius*-1 - titlelinenumber*titlefontsize).attr("font-size", titlefontsize).attr("font-weight", "bold");
		    	
		    	tspan = el.append('tspan').text(line2).attr("class", "headlineTitle");
    			tspan.attr('x', 0).attr('dy', titlefontsize).attr("font-size", titlefontsize).attr("font-weight", "bold");
		    }

		    

			//if (tempString != "") titlelinenumber += 1;
		}
		
		var keywordPosition = 0;
		var keywordfontsize = titlefontsize - 1;
		var keywordMaxSize = 0;

		d.keywords.forEach(function(d, i){
			if (i > 3){
				return;
			}

			if (keywordPosition + titlefontsize < radius){
				if (i == 0){
					var tempstring = clampKeyword(d.value);
					var keywordString1 = el.append('tspan').text("#"+tempstring).attr("text-anchor", "start").attr("class", "keywordsTitle");
	    			keywordString1.attr('x', radius*0.4).attr('y', keywordPosition).attr("font-size", keywordfontsize).attr("fill", "#ffffff")

	    			if (keywordMaxSize < tempstring.length){
	    				keywordMaxSize = tempstring.length
	    			}
	    			
				}
				else{
					var tempstring = clampKeyword(d.value);
					var keywordString1 = el.append('tspan').text("#"+tempstring).attr("text-anchor", "start").attr("class", "keywordsTitle");
	    			keywordString1.attr('x', radius*0.4).attr('dy', titlefontsize).attr("font-size", keywordfontsize).attr("fill", "#ffffff");

	    			if (keywordMaxSize < tempstring.length){
	    				keywordMaxSize = tempstring.length
	    			}
				}
				
	    		keywordPosition += keywordfontsize;
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

		d3.select(this.parentNode).select("rect").remove();

		d3.select(this.parentNode).insert("rect", "text")
		.attr("width", keywordMaxSize * keywordfontsize/2)
		.attr("height", titlefontsize * 4 + 1)
		.attr("x", radius * 0.4)
		.attr("y", 0-keywordfontsize)
		//.attr("fill", "#ffffff")
		.attr("opacity", fill_opacity)
		.attr("fill", function(d){
			var stop = 0;
			return color(d.id)
		})
		

		/*
		var tempTimeString = ""
		tempTimeString += d.clusterCreateTime.substr(0, 10); //"2013-09-04T00:00:00Z"
		tempTimeString += " -> ";
		tempTimeString += d.lastUpdateTime.substr(1, 10);  //""2013-09-08T00:00:00Z"";

		var createTimeString = el.append('tspan').text(tempTimeString);
	    createTimeString.attr('x', 0).attr('y', '-45');

	    var keywordString1 = el.append('tspan').text("#"+d.keywords[0].value+";");
	    keywordString1.attr('x', 0).attr('dy', '20');

	    var keywordString2 = el.append('tspan').text("#"+d.keywords[1].value+";");
	    keywordString2.attr('x', 0).attr('dy', '15');

	    var keywordString3 = el.append('tspan').text("#"+d.keywords[2].value+";");
	    keywordString3.attr('x', 0).attr('dy', '15');

	    var gapspan = el.append('tspan').text("-----------------");
	    gapspan.attr('x', 0).attr('dy', '15');
	    */
}

function getFontSizeByRadius(radius){
	var div = d3.selectAll("#StoryTellingVisualization-0")[0][0];
	var fontscale = d3.scale.linear()
					.domain(div.__radius.domain())
					.range([6, 12])
	return fontscale(radius);
}

function appendLineForKeywords(cluster, radius){

	var fontSize = getFontSizeByRadius(radius);
	var linecolor = "#000000"
	var linewidth = "1px"

	var x1_position = 0//radius*1/3*-1;
	var y1_position = 0//-2*radius/3;
	var x2_position = radius * 0.4 - 1;
	var y_position = 0-fontSize/3;

	cluster.append("line")
		.attr("id", "line1")
		.attr("x1", x1_position)
		.attr("y1", y1_position)
		.attr("x2", x2_position)
		.attr("y2", y_position)
		.style("stroke", linecolor)
 		.style("stroke-width",linewidth)

	cluster.append("line")
		.attr("id", "line2")
		.attr("x1", x1_position)
		.attr("y1", y1_position)
		.attr("x2", x2_position)
		.attr("y2", y_position+fontSize)
		.style("stroke", linecolor)
 		.style("stroke-width",linewidth)

 	cluster.append("line")
 		.attr("id", "line3")
		.attr("x1", x1_position)
		.attr("y1", y1_position)
		.attr("x2", x2_position)
		.attr("y2", y_position+fontSize*2)
		.style("stroke", linecolor)
 		.style("stroke-width",linewidth)

 	cluster.append("line")
 		.attr("id", "line4")
		.attr("x1", x1_position)
		.attr("y1", y1_position)
		.attr("x2", x2_position)
		.attr("y2", y_position+fontSize*3)
		.style("stroke", linecolor)
 		.style("stroke-width",linewidth)
}

function updateLineForKeywords(cluster, radius){

	var fontSize = getFontSizeByRadius(radius);
	var linecolor = "#ffffff"
	var linewidth = "2px"

	var x1_position = 0//radius*1/3*-1;
	var y1_position = 0//-2*radius/3;
	var x2_position = radius * 0.4 - 1;
	var y_position = 0-fontSize/3;

	cluster.select("#line1")
		//.attr("id", "line1")
		.attr("x1", x1_position)
		.attr("y1", y1_position)
		.attr("x2", x2_position)
		.attr("y2", y_position)
		.style("stroke", linecolor)
 		.style("stroke-width",linewidth)

	cluster.select("#line2")
		//.attr("id", "line2")
		.attr("x1", x1_position)
		.attr("y1", y1_position)
		.attr("x2", x2_position)
		.attr("y2", y_position+fontSize)
		.style("stroke", linecolor)
 		.style("stroke-width",linewidth)

 	cluster.select("#line3")
 		//.attr("id", "line3")
		.attr("x1", x1_position)
		.attr("y1", y1_position)
		.attr("x2", x2_position)
		.attr("y2", y_position+fontSize*2)
		.style("stroke", linecolor)
 		.style("stroke-width",linewidth)

 	cluster.select("#line4")
 		//.attr("id", "line4")
		.attr("x1", x1_position)
		.attr("y1", y1_position)
		.attr("x2", x2_position)
		.attr("y2", y_position+fontSize*3)
		.style("stroke", linecolor)
 		.style("stroke-width",linewidth)
}