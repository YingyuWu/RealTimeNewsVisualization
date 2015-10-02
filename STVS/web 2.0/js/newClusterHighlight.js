 var innerPaddings = 45;
 var arcAngularSize = 75;//105
 var arcsAngularGap = 15;
 var arcCount = 4;///
 var areaSpeed = 0.025;
 var innerpiePaddings = 50;
 var outterpiePaddings = 90;
 var innerarcPaddings = 10;
 var outterarcPaddings = 20;
 var degsToRads = d3.scale.linear().domain([0, 360]).range([0, 2 * Math.PI]);///

function hightlight(selection,current_data,div,delay){

//append hightlight
	if(current_data.IsNew == true){
		var id = current_data.id;
		var radius = current_data.size;
		selection.append("defs")
			.append("filter")
            .attr("id", "inner-glow")
            .append("feGaussianBlur")
            .attr("in", "SourceGraphic")
            .attr("stdDeviation", 7 + " " + 7);

			 selection.append("use")
			 .attr("id","use-glow")
			 .attr("xlink:href", "#"+id)
			 .attr("filter", "url(#inner-glow)");

		var innerArea = selection.append("g")
			.attr("id","inner-area");
			//draw gaosi inner circle
			innerArea.append("path")
            .attr("id", "inner-glowing-arc")
			.attr("transform","translate(" + 0 +"," + 0 + ")")
            .attr("d", d3.svg.arc()
            .innerRadius(div.__radius(radius) + innerarcPaddings)
			.outerRadius(div.__radius(radius) + outterarcPaddings)
            .startAngle(0)
            .endAngle(2 * Math.PI))
            .style("fill", function(d, i){
				return color(id);})
            .style("opacity","0")
            .attr("filter", "url(#inner-glow)");
         		
	        innerArea.selectAll("path")
	        .data(d3.range(arcCount + 1))			 
	        .enter()
	        .append("path")
	        .attr("id","glowing-arc")
	        .attr("transform", "translate(" + 0 + "," + 0 + ")" +
	 		 "rotate(" + (180 - arcsAngularGap / 2) + ")")
	         .attr("d", function(d, i){
	               var _innerRadius = div.__radius(radius) + innerarcPaddings,
	                    startAngle = degsToRads(arcAngularSize * i + arcsAngularGap * (i + 1)),
	                    endAngle = degsToRads(arcAngularSize) + startAngle;	                
	                 return d3.svg.arc()
	                    .innerRadius(_innerRadius)
	                    .outerRadius(div.__radius(radius) + outterarcPaddings)
	                    .startAngle(startAngle)//startAngle
	                    
	                    .endAngle(endAngle)();
	            })
	    
	          .style("fill", function(d, i){
				return color(id);})
	          .style("opacity", "0");

	          //set up delay appear
	         innerArea.selectAll("path").transition().duration(delay).style("opacity", 0.5);
	       
	          //make inner circle rotate
	        var t0 = Date.now();
	          d3.timer(function(){
	        	  var delta = (Date.now() - t0);
	              innerArea.attr("transform", function() {
	                   return "rotate(" + delta * areaSpeed + "," + 0 + "," + 0 + ")";
	               });					              
	            });			
		}
}


function hightlightreview(selection,current_data,div,delay){
	if(current_data.IsNew == false){
		//clean not new cluster's inner-area
		if(selection.selectAll("#inner-area")[0].length != 0){
			var innerArea = selection.selectAll("#inner-area");
			innerArea.transition().duration(delay).style("opacity",0).remove();
			selection.select("#inner-glow").remove();
			selection.select("#use-glow").remove();
		}
	}

	else if(selection.selectAll("#inner-area")[0].length != 0){		
		var radius = current_data.size;
		var id = current_data.id;
		var innerglowingarc = selection.selectAll("#inner-glowing-arc");
		innerglowingarc
		
		.attr("d", d3.svg.arc()
		.innerRadius(div.__radius(radius) + innerarcPaddings)
		.outerRadius(div.__radius(radius) + outterarcPaddings)
        .startAngle(0)
        .endAngle(2 * Math.PI)
		)
		.style("fill", function(d, i){
			return color(id);})
		.style("opacity", "0");
	
         var glowingarc = selection.selectAll("#glowing-arc");
         glowingarc
         .data(d3.range(arcCount + 1))        
         .attr("d", function(g, h){
           var _innerRadius = div.__radius(radius) + innerarcPaddings,
                startAngle = degsToRads(arcAngularSize * h + arcsAngularGap * (h + 1)),
                endAngle = degsToRads(arcAngularSize) + startAngle;
					
             return d3.svg.arc()
                .innerRadius(_innerRadius)
                .outerRadius(div.__radius(radius) + outterarcPaddings)
                .startAngle(startAngle)				    
                .endAngle(endAngle)();
          })
           .style("fill", function(d, i){
			return color(id);})
            .style("opacity", "0");

          selection.selectAll("#inner-area").selectAll("path").transition().duration(delay)
          .style("opacity", 0.5);

		}
}