var foci = [];
var forceNodes = [];
var force = undefined;
var node;
var width_svg = 0;
var height_svg = 0;
var forceDuration = 00;
var newArticleTitleFontSize = 12;

var shownewTitleNumber = 50;

function generateAndUpdateForceNodes(newdata, charID, width, height, left, top){
	if (d3.selectAll("#StoryTellingVisualization-"+charID.toString())[0].length == 0){
		return ;		
	}
	else{
		var svg = d3.select("#ClusterVisualizationSection").select("svg")
		width_svg = parseInt(svg.attr("width"));
		height_svg = parseInt(svg.attr("height"));
		var div = d3.selectAll("#StoryTellingVisualization-"+charID.toString())[0][0];

		foci = []

		newdata.foci.forEach(function(f){
			var foci_item = {x:0, y:0};
			foci_item.x = div.__x(f.x);
			foci_item.y = div.__y(f.y);

			foci.push(foci_item);
		})

	

		if (force == undefined){
			forceNodes = newdata.articleNodes;

			force = d3.layout.force()
					.nodes(forceNodes)
					.links([])
					.gravity(0.00)
					.charge(-40)
					.chargeDistance(30)
					.friction(0.4)
					.size([width, height])
					.on("tick", tick)


			node = svg.select("#forceNodeGroup").selectAll(".forceNode");

			node = node.data(forceNodes);

			node.enter().append("circle")
				.attr("class", "forceNode")
				.attr("r", 7)
		      	.attr("cx", width_svg)
      			.attr("cy", height_svg/3)
				.style("fill", function(d) { 
					if (d.cid == -1) return "#ffffff";
					return color(d.cid); })
				.style("stroke-width", 0)
				.style("fill-opacity", function(d) { 
					if (d.cid == -1) return 0;
					return d.age; })

			node.exit().remove();
		}
		else{
			forceNodes.forEach(function(n, i){
				n.id = newdata.articleNodes[i].id;
				n.cid = newdata.articleNodes[i].cid;
				n.age = newdata.articleNodes[i].age;
			})
		}

		/*
		  forceNodes.forEach(function(o, i) {
		  	if (o.id == -1) {
			    o.y = height_svg/3;
			    o.x = width_svg;
		  	}
		  	else if (i < ArticleInformationArray.length-newArticleIndex){
		    	o.y = foci[o.id].y;
				o.x = foci[o.id].x;
		  	}
		  });
		*/

		var startIndex = ArticleInformationArray.length-newArticleIndex;
		d3.selectAll(".newArticleTitle").remove();

		if (newArticleIndex < shownewTitleNumber){
			for (var i=0; i<newArticleIndex; ++i){
				var temptitle = svg.append("g")
					.attr("class", "newArticleTitle")
					.attr("id", "newArticleTitle-"+i)
					.attr("transform", "translate("+width_svg+","+height_svg/3+ ")")
					.attr("opacity", 0)

				var titleWidth = 0;

				temptitle
					.append("rect")
					.attr("opacity", 0.5)
					//.attr("fill", "#00ff00")
					.attr("fill", function() {
						return color(ArticleInformationArray[startIndex + i][0])
					})
					.attr("height", newArticleTitleFontSize+2)
					.attr("width", 10)

				temptitle
					.append("text")
					.attr("text-anchor", "end")
					.attr("font-weight", "bold")
					.attr("fill", "#000000")
					.attr("font-size", newArticleTitleFontSize)
					.text(function(){
						var tempstring = clampTitle(ArticleInformationArray[startIndex + i][2]);
						titleWidth = tempstring.length;
						return tempstring
					})
					.attr("opacity", 0.5)

				temptitle.select("rect")
					.attr("x", titleWidth*newArticleTitleFontSize/2*-1)
					.attr("y", -1 * newArticleTitleFontSize)
					.attr("width", titleWidth*newArticleTitleFontSize/2)
			}		
		}
		

		function clampTitle(title){
			var words = title.split(' ');
			var size = 5;
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


		force.start();
		//console.log(newArticleIndex);
	}
}

function tick(e) {
  //var k = .4 * Math.sqrt(e.alpha);
  //var k = 0.8 * e.alpha * e.alpha;
  //var k = .8 * (e.alpha>0?0.04:-0.04);
  // Push nodes toward their designated focus.
  var k = e.alpha;

  forceNodes.forEach(function(o, i) {
  	if (o.id == -1) {
	    o.y += (height_svg/3 - o.y) * k;
	    o.x += (width_svg - o.x) * k;
  		return;
  	}

    o.y += (foci[o.id].y - o.y) * k;
    o.x += (foci[o.id].x - o.x) * k;
  });

  node
	    .transition()
	    .duration(forceDuration)
	    //.attr("transform", function(d) { return "translate(" + d.x + "," + d.y + ")"; })
      	.attr("cx", function(d) { return d.x; })
      	.attr("cy", function(d) { return d.y; })
		.style("fill", function(d) { 
			if (d.cid == -1) return "#ffffff";
			return color(d.cid); })
		.each(function(d, i){
			if (d.cid == -1){
				d3.select(this)
				.style("fill-opacity", 0)
			}
			else if (i>=ArticleInformationArray.length-newArticleIndex && i <= ArticleInformationArray.length){
			//else if (i==ArticleInformationArray.length-newArticleIndex){

				if (i<ArticleInformationArray.lenth-shownewTitleNumber){ return ;}
				d3.select(this)
				.style("fill-opacity", function(d){ return d.age})

				var titleIndex = i-ArticleInformationArray.length+newArticleIndex;

				d3.select("#newArticleTitle-"+titleIndex)
					.attr("opacity", 0.8)
					.attr("transform", "translate("+d.x+","+d.y+ ")")
			}
			else {
				if (k < 0.13){
					d3.select(this)
					.style("fill-opacity", function(d){ return d.age})
				}
			}

		})
}