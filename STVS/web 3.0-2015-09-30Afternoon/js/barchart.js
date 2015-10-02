var stagewidth;
var stageheight;
var barchart;
var baropacity = 0.6;
function initBarChart(divg ,width,height,left,top,right){
	if(d3.selectAll("#barchart")[0].length == 0){

		//bar chart is not exist
			var barwidth = 0.2 * width;
			var barheight = height / 3;
			stagewidth = width;
			stageheight = height;
			
			
			var xScale = d3.scale.linear()
							.domain([0,5])//five clusters
							.range([0, barwidth]);

			var yScale = d3.scale.linear()
							.domain([0,100])
							.range([barheight, 0]);
			
			//Create SVG element
			var bar = d3.select("#mainStage").append("svg").attr("id","barsvg")
			.attr("width", barwidth + 100)
			.attr("height", barheight + 80)
			.style("background-color","#FAFAFA")
			.style("padding-top", "30px")
			//.style("margin-left","50px")
			//.style("margin-top","20px")
			.attr("transform", "translate(100,"+top + ")");
			//.attr("transform", "translate(100,0)");


			barchart = bar.append("g")
			.attr("id","barchart")
			.attr("transform", "translate(50,"+top + ")");

			bar.append("text")
			.attr("id","barheader")
			.text("Topic Cluster Distribution")
      		.style("font-size","17px")
      		.style("font-family", "Arial Black")
      		.attr("transform", "translate(50,0)");
      		//.style("margin-left",(barwidth / 2) + "px")

			barchart.append("text")
			.attr("transform", "translate(-50,"+ (top - 30) + ")")
			.style("font-color","black")
			.style("font-size","10")
			//.style("font-family", "Arial Black")
			.text("Percentage");

			barchart.append("text")
			.attr("transform", "translate(-50,"+ (top + barheight) + ")")
			.style("font-color","black")
			.style("font-size","10")
			//.style("font-family", "Arial Black")
			.text("Cluster");

			var xAxis = d3.svg.axis()
			.scale(xScale)
			.orient("bottom")
			.ticks(5);

			var yAxis = d3.svg.axis()
			.scale(yScale)
			.orient("left")
			.ticks(10);
			
			barchart.append("g")
			.attr("class" , "baraxis")
			.attr("id","barx")
			.attr("transform", "translate(0," + barheight + ")")
			.call(xAxis);

			barchart.selectAll("#barx")
			.selectAll("text")
			.attr("transform","translate(" + barwidth / 10 + ", 0)");

			barchart.append("g")
			.attr("class", "baraxis")
			.call(yAxis);

			barchart.selectAll("#barx")
			.selectAll("text")[0][5].remove();

			
	}
}
function barChartAnimation(tempData, totalSize, divg){
	if(d3.selectAll("#barchart")[0].length != 0){
		var barwidth = 0.2 * stagewidth;
		var barheight = stageheight / 3;
		var xScale = d3.scale.linear()
						.domain([0,5])//five clusters
						.range([0, barwidth]);

		var yScale = d3.scale.linear()
						.domain([0,100])
						.range([0, barheight]);

		var bardata = jQuery.extend(true, [], tempData);
		bardata.forEach(function(d,i){
			d.percentage = (d.size/totalSize) * 100;
		})

		for(var i = bardata.length; i < 5; i++){
			var tempobj = {};
			tempobj.id = i;
			tempobj.size = 0;
			tempobj.percentage = 0;
			bardata.push(tempobj);
		}

		if(barchart.selectAll("rect")[0].length == 0){
			barchart.selectAll("rect")
			   .data(bardata)
			   .enter()
			   .append("rect")
			   .attr("id","barchart-rect")
			   .attr("x", function(d, i) {
			   		return xScale(d.id);
			   })
			   .attr("y", function(d) {
			   		return barheight - yScale(d.percentage);
			   })
			   .attr("width", function(d,i){
			   		return barwidth / 5;
			   })
			   .attr("height", function(d) {
			   		if(d.percentage == 0){
			   			return 0;
			   		}
			   		return yScale(d.percentage);
			   })
			   .attr("fill", function(d) {
					return color(d.id);
			   })
			   .style("stroke","#f1ffff")
			   .style("stroke-width","1px")
			   .style("opacity", baropacity);
			}else{

				barchart.selectAll("rect")
			   .data(bardata)
			   .transition()
			   .delay(0.5)
			   .attr("x", function(d, i) {
			   		return xScale(d.id);
			   })
			   .attr("y", function(d) {
			   		return barheight - yScale(d.percentage);
			   })
			   .attr("width", function(d,i){
			   		return barwidth / 5;
			   })
			   .attr("height", function(d) {
			   		if(d.percentage == 0){
			   			return 0;
			   		}
			   		return yScale(d.percentage);
			   })
			   .attr("fill", function(d) {
					return color(d.id);
			   })
			   .style("stroke","#f1ffff")
			   .style("stroke-width","1px")
			   .style("opacity", baropacity);
			}
		
	}
}