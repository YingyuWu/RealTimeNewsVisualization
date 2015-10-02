  var stagewidth;
  var stageheight;
  var keywordlist;
  var cloudwidth;
  var cloudheight;
  var wordsizes = [];
  var oldData = [];
  var oldtable;
  var newtable;
  var oldrows;
  var newrows;
  var oldcells;
  var newcells;
  var oldtbody;
  var newtbody;
  var column;
  function initKeywordList(divg,width, height, left, top, right){
    if(d3.selectAll("#keywordlist")[0].length == 0){
      cloudwidth = 0.2 * width;
      cloudheight = height / 3;
      stagewidth = width;
      stageheight = height;
      var div = d3.select("#mainStage").append("div").attr("id","keywordlist");
      div.append("text")
      .text("Top Keywords")
      .style("font-size","17px")
      .style("font-family", "Arial Black")
      .style("margin-left", (cloudwidth / 2.5 ) + "px")

      oldtable = div
      .append("table")
      .attr("id","oldlist")
      .style("float","left")
      .style("margin-left","20px")
      .style("margin-top", "10px")
      .style("font-size", "12px")
      .style("background-color","#FAFAFA")
      .attr("width",cloudwidth / 1.7)
      .attr("height", cloudheight)

      d3.select("#keywordlist")
      .style("width", cloudwidth + 100 + "px")
      .style("height", cloudheight + 100 + "px")
      .style("background-color","#FAFAFA")
      .style("float","left")
      //.attr("transform", "translate("+1000+","+top + ")");




      newtable = d3.select("#keywordlist").append("table")
      .attr("id","newlist")
      .style("float","right")
      .style("margin-top", "10px")
      .style("margin-right","20px")
      .style("font-size", "12px")
      .style("background-color","#FAFAFA")
      .attr("width",cloudwidth / 1.7)
      .attr("height", cloudheight)

      //.attr("transform", "translate("+(width - cloudwidth - 30)+","+ (cloudheight ) + ")");

      column = ["Previous"];
      var inittable = [1,2,3,4,5,6,7,8,9,10];

      // keywordlist.selectAll("rect")
      // .data(inittable)
      // .enter()
      // .append("rect")
      // .attr("id","initlist")
      // .attr("x",0)
      // .attr("y",0)
      // .attr("width", cloudwidth / 2.5)
      // .attr("height", cloudheight /3)
      // .attr("fill", "none")
      // .style("stroke","black")
      // .style("stroke-width", "1px");

      var oldthead = oldtable.append("thead");
      oldtbody = oldtable.append("tbody");

      

      var inittd = [1];

      oldthead.append("tr")
      .selectAll("th")
      .data(column)
      .enter()
      .append("th")
      .text(function(column){return column;})
      .style("font-family", "Arial Black")
      .style("font-size","13px")

      // oldrows = oldtbody.selectAll("tr")
      // .data(inittable)
      // .enter()
      // .append("tr");

      // oldcells = oldrows.selectAll("td")
      // .data(inittd)
      // .enter()
      // .append("td")
      // //.text("ABC")

       var newthead = newtable.append("thead");
       newtbody = newtable.append("tbody");

      


       newthead.append("tr")
       .selectAll("th")
       .data(column)
       .enter()
       .append("th")
       .text("New")
       .style("font-family", "Arial Black")
       .style("font-size","13px")

      // newrows = newtbody.selectAll("tr")
      // .data(inittable)
      // .enter()
      // .append("tr");

      // newcells = newrows.selectAll("td")
      // .data(inittd)
      // .enter()
      // .append("td")
      //.text(function(d){return d;})



    }

  }

  function keywordListAnimation(tempData, divg){
    if(d3.selectAll("#keywordlist")[0].length != 0){
      var keyworddata = jQuery.extend(true, [], tempData);
      var listwords = [];
      var wordlength = [];
      keyworddata.forEach(function(d,i){
          var tempkey = d.keywords;
          tempkey.forEach(function(g,i){
              g.id = d.id;
              wordlength.push(g.value.length);
          });
          listwords = listwords.concat(tempkey);
      });
      var maxLength = d3.max(wordlength);//get the max length of all keywords
      listwords.sort(function (a, b) {
          return b.occurrence - a.occurrence ;
      })
      listwords = listwords.filter(function(d,i){//filter out only ? top keywords
        return d.value != "";        
      })
      listwords = listwords.filter(function(d,i){//filter out only ? top keywords
        return i < 7;        
      })
      wordsizes = [];
      listwords.forEach(function(d,i){
          wordsizes.push(d.occurrence);
      });

      
      //console.log(newwords);

      var minSize = d3.min(wordsizes);
      var maxSize = d3.max(wordsizes);
      var firstScale = d3.scale.linear()
        .domain([minSize,maxSize])
        .range([0,1]);
      var sizeScale = d3.scale.pow().exponent(.8)
        .domain([firstScale(minSize),firstScale(maxSize)])
        .range([10,25]);

      if(oldData.length == 0){
        //No OLD DATA

          //.text("ABC")
          newtable.selectAll("tbody").remove();
          newtbody = newtable.append("tbody");

          newrows = newtbody.selectAll("tr")
          .data(listwords)
          .enter()
          .append("tr")
          .attr("width",cloudwidth / 2);

          newcells = newrows.selectAll("td")
          .data(function(row){
              return column.map(function(column){
                  return {column: column, value: row.value, id: row.id}
              })
          })
          .enter()
          .append("td")
          .text(function(d){
            // if(d.value.length > 17){
            // var left = d.value.substring(0, 17); 
            // var right = d.value.substring(17, d.value.length);
            // console.log(left + "-" + right);
            // return left + "-" + right;
            // }else{
              return d.value;
            //}
          
          })
          .style("color", function(d){return color(d.id)})
          .style("font-family", "Arial Black")
          .style("border", "1px solid")
          oldData = jQuery.extend(true, [], listwords);

      }else{
        var newwords = [];
        listwords.forEach(function(d){
          var check = true;
          oldData.forEach(function(g){
              if(g.id == d.id && g.value == d.value){
                check = false;
              }
          });
          if(check == true){
            newwords.push(d);
          }
        })

        newtable.selectAll("tbody").remove();
        d3.select("#oldlist").selectAll("tbody").remove();
          
          oldtbody = oldtable.append("tbody");
          oldrows = oldtbody.selectAll("tr")
          .data(oldData)
          .enter()
          .append("tr")
          .attr("width",cloudwidth / 2);

          oldcells = oldrows.selectAll("td")
          .data(function(row){
              return column.map(function(column){
                  return {value: row.value, id:row.id}
              })
          })
          .enter()
          .append("td" )
          .text(function(d){return d.value})
          .style("color", function(d){return color(d.id)})
          .style("font-family", "Arial Black")
          
          

          newtbody = newtable.append("tbody");

          newrows = newtbody.selectAll("tr")
          .data(listwords)
          .enter()
          .append("tr")
          .attr("width",cloudwidth / 2);

          newcells = newrows.selectAll("td")
          .data(function(row){
              return column.map(function(column){
                  return {column: column, value: row.value, id:row.id}
              })
          })
          .enter()
          .append("td")
          .text(function(d){return d.value})
          .style("color", function(d){return color(d.id)})
          .style("font-family", "Arial Black")
          .style("border", function(d){
             var check = false;
             newwords.forEach(function(g){
                if(d.id == g.id && d.value == g.value){
                  check = true;
                }
             })
             if(check == true){
                return "1px solid"
             }else{
                return "0px";
             }
          })
          oldData = []; 
          oldData = jQuery.extend(true, [], listwords);

      }


      // d3.layout.cloud()
      // .size([cloudwidth, cloudheight])
      // .words(keyworddata.map(function(d) {
      //   return {text: d.value, size: sizeScale(firstScale(d.occurrence)), id: d.id};
      // }))
      // .padding(1)
      // .rotate(function() { return ~~(Math.random() * 2) * 1; })
      // .font("Verdana")
      // .fontSize(function(d) { return d.size; })
      // .on("end", draw)
      // .start();
    }
    
  }
  // var fill = d3.scale.category20();

  

  function draw(words) {
    if(keywordlist.selectAll("#keywordlist")[0].length != 0){
      //Transition Hide
          keywordlist.selectAll("#cloudtext").remove();
      } 
    keywordlist.append("g")
        .attr("id","cloudtext")
        .attr("transform", "translate(" + (cloudwidth / 2) + "," + (cloudheight / 2) + ")")
        .selectAll("text")
        .data(words)
        .enter().append("text")
        .style("font-size", function(d) { return d.size + "px"; })
        .style("font-family", "Impact")
        .style("fill", function(d, i) { return color(d.id); })
        .attr("text-anchor", "middle")
        .attr("transform", function(d) {
          return "translate(" + [d.x, d.y] + ")rotate(" + d.rotate + ")";
        })
        .text(function(d) { return d.text; });
  }