var history_list,
	vis,
	w,
	h,
	gapWidth,
	gapRatio,
	delay,
	padding,
	line,
    currentHistoryIndex,
	playBar_translate_x;

var myLeftTimeVar, 
    myRightTimeVar,
    myTranslateVar;

var keywordsSize = 10;

var color = d3.scale.category10();

function generateAndUpdatePlayBar(newMessage, new_w, new_h, new_gapWidth, new_gapRatio, new_delay, new_padding){  
    
    var cluster_size = newMessage.messageContent.clusterData.length
    new_padding	= new_h / 80;

	vis = d3.selectAll("#playBarSVG");

    if (vis[0].length == 0){
		w = new_w;
		h = new_h-40;
		gapWidth = new_gapWidth;
		gapRatio = new_gapRatio;
		delay = new_delay;
		padding = new_padding;
		line = d3.svg.line()
	        .interpolate('basis');

	   vis = d3.select("#PlayBarSection")
      .append("svg:svg")
        .attr("width", new_w)
        .attr("height", new_h)
        .append("svg:g")
      	.attr("id", "playBarSVG")
      	.attr("transform", "translate("+50+","+10+")")

      	playBar_translate_x = 50;

		history_list = new play_bar_data(gapWidth, h, padding, gapRatio);
        currentHistoryIndex = 0;

        $("#moveLeft").on("mouseover", mouseLeftOver());
        $("#moveLeft").on("mouseout", mouseLeftOut());
        $("#moveRight").on("mouseover", mouseRightOver());
        $("#moveRight").on("mouseout", mouseRightOut());
    }
    padding = new_padding;
   

	var current_Nodes = new BarNode_list(newMessage, history_list.history.length);
	history_list.addNextNodes(current_Nodes);
    currentHistoryIndex += 1;

	if ((history_list.history.length * new_gapWidth - new_gapWidth*new_gapRatio) > w){
		playBar_translate_x = w - (history_list.history.length-1) * gapWidth - 60 - (1-gapRatio)*gapWidth;
		vis.transition()
        .duration(delay)
        .attr("transform", "translate("+playBar_translate_x+","+10+")");

        myTranslateVar = playBar_translate_x;
	}

    update();
}

function update() {
    // update data
    var last = history_list.history.length;
    var translate_x = 0;
    
    if (last == 0){
    	return;
    }  
    else if (last == 1){
    	translate_x = 10;
    	var currentHistory = history_list.history[0];
    	var times = vis.append("svg:g")
    					.attr('class', 'time')
    					.attr("transform", function(d, i) { return "translate(" + translate_x + ",0)" });

    	var nodes = times.selectAll('g.node')
            .data(currentHistory.nodes)
          .enter().append('svg:g')
            .attr('class', 'node');

        nodes.append('svg:rect')
                .attr('y', function(n, i) {
                    return n.offsets + i * padding;
                })
                .attr('width', function(n) { return n.width })
                .attr('height', function(n) { return n.height })
                .attr('fill', function(n){return color(n.id);})
                .attr('stroke', function(n){
                    if (n.IsNew){
                        return '#000000';
                    }
                    else{
                        return color(n.id);
                    }
                })
              .append('svg:title')
                .text(function(d, i) {
                var result = "";
                for (var k=0; k<d.keywords.length; ++k){
                    result += d.keywords[k].value + ","
                } 
                return result; 
            });

        nodes.append('svg:text')
        .attr('x', gapWidth*(1-gapRatio))
        .attr('y', function(n, i){
            return n.offsets + i * padding// +n.height/2;
        })
        .attr("fill", function(d){
            return "#000000"//color(d.id);
        })
        .attr("font-weight", "bold")
        .each(function(d, i){
            var el = d3.select(this);
            var words = d.keywords[0].value.split(' ');
            el.text('');

            var tempLength = 0;
            var tempString = "";

            for (var i = 0; i < words.length; i++) {

                tempLength += words[i].length;
                tempString += words[i] + " ";

                if (tempLength > 18){
                    var tspan = el.append('tspan').text(tempString).attr("font-size", keywordsSize);
                    tspan
                    .attr('x', gapWidth*(1-gapRatio))
                    .attr('dy', keywordsSize);

                    tempString = "";
                    tempLength = 0;
                }          
            }

            var tspan = el.append('tspan').text(tempString).attr("font-size", keywordsSize);
            tspan.attr('x', gapWidth*(1-gapRatio)).attr('dy', keywordsSize);
        })

        times.append('svg:text')
        .datum(currentHistory)
        .attr('x', gapWidth*(1-gapRatio)-40)
        .attr('y', h+20)
        .text(function(d){
            var timeData = d.currentDate;//currentHistory.currentDate;
            //var datestring = timeData[0]+'/'+timeData[1]+'/'+timeData[2];//+' '+timeData[3]+":"+timeData[4]+":"+timeData[5];
            var datestring = timeData[1]+'/'+timeData[2] +' '+timeData[3]+":"+timeData[4]+":"+timeData[5];
            return datestring
        })
        .attr("fill", function(d){
            if (d.historyID < currentHistoryIndex){
                return "#ff0000"
            }
            else{
                return "#000000"
            }
        })
        .on("click", onClickPlayBarDate())
    }
    else{
    	translate_x = 10 + gapWidth*(1-gapRatio) + (last-2)*gapWidth;
    	var currentHistory = history_list.history[last-1];
    	var previousHistory = history_list.history[last-2];
    	 // time slots
    	var times = vis.append("svg:g")
    					.attr('class', 'time')
    					.attr("transform", function(d, i) { return "translate(" + translate_x + ",0)" });
            
        // node bars
    	var nodes = times.selectAll('g.node')
            .data(currentHistory.nodes)
          .enter().append('svg:g')
            .attr('class', 'node');
        
        setTimeout(function() {
            nodes.append('svg:rect')
                .attr('fill', function(n){return color(n.id);})
                .attr('y', function(n, i) {
                    return n.offsets + i * padding;
                })
                .attr('x', gapWidth*gapRatio)
                .attr('width', function(n) { return n.width })
                .attr('height', function(n) { return n.height })
                .attr('stroke', function(n){
                    if (n.IsNew){
                        return '#000000';
                    }
                    else{
                        return color(n.id);
                    }
                })
                .on("click", onClickPlayBarNodes())
              .append('svg:title')
                .text(function(d, i) {
                var result = "";
                for (var k=0; k<d.keywords.length; ++k){
                    result += d.keywords[k].value + ","
                } 
                return result; 
            });

            nodes.append('svg:text')
                .attr('x', gapWidth)
                .attr('y', function(n, i){
                    return n.offsets + i * padding //+n.height/2;
                })
                .attr("fill", function(d){
                    return "#000000"//color(d.id);
                })
                .attr("font-weight", "bold")
                /*.text(function(n){
                    if(n.IsNew)
                        return n.keywords[0].value;
                    else if ((currentHistoryIndex+n.id)%4 ==0)
                        return n.keywords[0].value;
                    else
                        return "";
                })*/
                .each(function(d, i){
                    if (d.IsNew || (currentHistoryIndex+d.id)%4 ==0){
                        var el = d3.select(this);
                        var words = d.keywords[0].value.split(' ');
                        el.text('');

                        var tempLength = 0;
                        var tempString = "";

                        for (var i = 0; i < words.length; i++) {

                            tempLength += words[i].length;
                            tempString += words[i] + " ";

                            if (tempLength > 18){
                                var tspan = el.append('tspan').text(tempString).attr("font-size", keywordsSize);
                                tspan
                                .attr('x', gapWidth)
                                .attr('dy', keywordsSize);

                                tempString = "";
                                tempLength = 0;
                            }          
                        }

                        var tspan = el.append('tspan').text(tempString).attr("font-size", keywordsSize);
                        tspan.attr('x', gapWidth).attr('dy', keywordsSize);
                    }
                    else{
                        return ;
                    }
                })

            times.append('svg:text')
                .datum(currentHistory)
                .attr("class", "timesTitle")
                .attr('x', gapWidth-40)
                .attr('y', h+20)
                .text(function(d){
                    var timeData = d.currentDate;
                    //var datestring = timeData[0]+'/'+timeData[1]+'/'+timeData[2];//+' '+timeData[3]+":"+timeData[4]+":"+timeData[5];
                    var datestring = timeData[1]+'/'+timeData[2] +' '+timeData[3]+":"+timeData[4]+":"+timeData[5];
                    return datestring
                })
                .attr("fill", function(d){
                    if (d.historyID < currentHistoryIndex){
                        return "#ff0000"
                    }
                    else{
                        return "#000000"
                    }
                })
                .on("click", onClickPlayBarDate())
        }, delay);

         var linkLine = function(start) {
                return function(l) {
                    var source = previousHistory.nodes[l.source],
                        target = currentHistory.nodes[l.target],
                        bandWidth = gapRatio * gapWidth,
                        startx = 0,
                        endx = bandWidth,
                        sourcey = source.offsets + 
                            source.order * padding +
                            l.outOffsets// + l.outHeight/2
                            ,
                        sourcey2 = sourcey+l.outHeight,
                        targety = target.offsets + 
                            target.order * padding + 
                            l.inOffsets// + l.inHeight/2
                            ,
                        targety2 = targety + l.inHeight;

                        //start = true;
                        top_points = start ? 
                            [
                                [ startx, sourcey ], [ startx, sourcey ], [ startx, sourcey ], [ startx, sourcey ] 
                            ] :
                            [
                                [ startx, sourcey ],
                                [ startx + bandWidth/4, sourcey ],
                                [ startx + 3*bandWidth/4, targety ],
                                [ endx, targety ]
                            ];
                        right_points = " v "+ l.inHeight;

                        bot_points = start ? 
                            [
                                [ startx, sourcey2 ], [ startx, sourcey2 ], [ startx, sourcey2 ], [ startx, sourcey2 ] 
                            ] :
                            [
                                [ endx, targety2 ],
                                [ startx + 3*bandWidth/4, targety2 ],
                                [ startx + bandWidth/4, sourcey2 ],
                                [ startx, sourcey2 ]
                            ];

                        left_point = " v -"+ l.outHeight;

                        result = line(top_points) + right_points + line(bot_points) + left_point;
                    return result;
                }
            }
            
        // links
        var links = nodes.selectAll('path.link')
            .data(function(n) { return n.incomingLink || [] })
          .enter().append('svg:path')
            .attr('class', 'link')
            //.style('stroke-width', function(l) { 
            //	return l.inHeight })
            .attr('d', linkLine(true))
            //.attr('stroke', function(l){
            //	return color(previousHistory.nodes[l.source].id);
            //})
            .attr('fill', function(l){
            	return color(previousHistory.nodes[l.source].id);
            })
            .on('mouseover', function() {
                d3.select(this).attr('class', 'link on')
            })
            .on('mouseout', function() {
                d3.select(this).attr('class', 'link')
            })
          .transition()
            .duration(delay)
            .attr('d', linkLine());
    }          
}

function play_bar_data(w, h, padding, gapRatio){
    this.history = [];
    this.w = w; // gap width
    this.h = h;	// svg height;
    this.gapRatio = gapRatio;
    this.padding = padding;

    this.addNextNodes = function (node_list) {
    	var last = this.history.length;
    	var maxn = node_list.nodes.length;
    	var maxv = node_list.amount;

        var y = d3.scale.linear()
		    .domain([0, maxv])
		    .range([0, this.h - this.padding * maxn]);

		var cumValue = 0;

		node_list.y = y;

		// calculate offset for node;
		node_list.nodes.forEach(function(n, i) {
            n.order = i;
            n.offsets = y(cumValue);
            n.height = y(n.value);
            n.width = this.w * (1-this.gapRatio)
            cumValue += n.value;	           
        }, this)


		// create links then attach lins, eventually calculate offset for links.
    	if (last == 0){ 
    		// First time, only nodes, no links need to do.
    	}
    	else{ // create Links to previous history
    		var previous_history = this.history[last-1];

    		// create and attach links;
    		node_list.nodes.forEach(function(n, i) {
    			n.parents.forEach(function(l, j){
    				if (l!=0){
    					var newLink = new link(j, i, l);
    					n.incomingLink.push(newLink);
    					previous_history.nodes[j].outgoingLink.push(newLink);
    				}
    			}, this);
    		}, this);

    		// calculate offset for links
			previous_history.nodes.forEach(function(n, i) {
	            var lCumValue;
	            var y = previous_history.y;
	            // outgoing
	            if (n.outgoingLink) {
	                lCumValue = 0;
	                n.outgoingLink.sort(function(a,b) {
	                    return d3.descending(b.target, a.target)
	                });
	                n.outgoingLink.forEach(function(l) {
	                    l.outOffsets = y(lCumValue);
	                    l.outHeight = y(l.value);
	                    lCumValue += l.value;
	                });
	            }
            }, this);

    		
    		node_list.nodes.forEach(function(n, i) {
	            var lCumValue;
	            var y = node_list.y;
	            // incoming
	            if (n.incomingLink) {
	                lCumValue = 0;
	                n.incomingLink.sort(function(a,b) {
	                    return d3.descending(b.source, a.source)
	                });
	                n.incomingLink.forEach(function(l) {
	                    l.inOffsets = y(lCumValue);
	                    l.inHeight = y(l.value);
	                    lCumValue += l.value;
	                });
                }
            }, this);
    	}



        this.history.push(node_list);
    };
}

function link (source, target, size){
	this.source = source;
	this.target = target;
	this.value = size;

	this.inOffsets = 0;
	this.inHeight = 0;
	this.outOffsets = 0;
	this.outHeight = 0;
}


function BarNode_list (newMessage, id){
	this.nodes = [];
	this.amount = 0;
    this.currentDate = newMessage.messageContent.dateTime;
    this.historyID = id;

	newMessage.messageContent.clusterData.forEach(function(node, i) {
		var newNode = new BarNode(node, i, this.historyID);
		this.nodes.push(newNode);
		this.amount += newNode.value;
	}, this);
}

function BarNode (tempdata, i, historyID){
	this.id = tempdata.id;
	this.incomingLink = [];
	this.outgoingLink = [];
	this.IsNew = tempdata.IsNew;
	this.order = i;
	this.value = tempdata.size;
	this.offsets = 0;
	this.height = 0;
	this.width = 0;
	this.nodeName = tempdata.headline;
	this.parents = tempdata.parents;
    this.historyID = historyID;
    this.keywords = tempdata.keywords
}

function onClickPlayBarDate(){
    return function(d){
         var newMessage = {"messageType":"VIS_Play_Back", "messageContent":d.historyID};
         websocket.send(JSON.stringify(newMessage));
    }
}

function onClickPlayBarNodes(){
    return function(d){
        //var newMessage = {"messageType":"VIS_Play_Back", "messageContent":d.historyID};
        //websocket.send(JSON.stringify(newMessage));

        var newMessage = {"messageType":"VIS_Focus_Context"
        , "messageContent":{"isFocus":true, "focusID":d.id}};
        websocket.send(JSON.stringify(newMessage));
    }
}

function updatePlayBarTextColor(){
    vis = d3.selectAll("#playBarSVG");
    vis.selectAll(".timesTitle")
    .each(function(d){
        d3.select(this)
        .attr("fill", function(d){
           if (d.historyID < currentHistoryIndex){
                return "#ff0000"
            }
            else{
                return "#000000"
            }
        })
    })

    currentHistoryIndex += 1;
}

function mouseLeftOver(){
    return function f(){
        myTranslateVar += 200;
        if (myTranslateVar<50){
            vis.transition()
            .duration(500)
            .attr("transform", "translate("+myTranslateVar+","+10+")");
        }else{
            myTranslateVar = 50;
            vis
            .transition()
            .duration(500)
            .attr("transform", "translate("+myTranslateVar+","+10+")");
        }

        myLeftTimeVar = setTimeout(f,1000);
    };
}

function mouseLeftOut(){
    return function(){
        clearTimeout(myLeftTimeVar);
    };
}

function mouseRightOver(){
    return function f(){
        myTranslateVar -= 200;
        if (myTranslateVar>playBar_translate_x){
            vis.transition()
            .duration(500)
            .attr("transform", "translate("+myTranslateVar+","+10+")");
        }else{
            myTranslateVar = playBar_translate_x
            vis
            .transition()
            .duration(500)
            .attr("transform", "translate("+playBar_translate_x+","+10+")");
        }

        myRightTimeVar = setTimeout(f,1000);
    };
}

function mouseRightOut(){
    return function(){
        clearTimeout(myRightTimeVar);
    };
}