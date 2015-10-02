  var stagewidth;
  var stageheight;
  var wordcloud;
  var cloudwidth;
  var cloudheight;
  var wordsizes = [];
  function initWordCloud(divg,width, height, left, top, right){
    if(d3.selectAll("#wordcloud")[0].length == 0){
      cloudwidth = 0.2 * width;
      cloudheight = height / 2;
      stagewidth = width;
      stageheight = height;
      wordcloud = divg.append("g")
      .attr("id","wordcloud")
      .attr("transform", "translate("+(width - cloudwidth - 30)+","+ (cloudheight - top) + ")");

      wordcloud.append("rect")
      .attr("id","initcloud")
      .attr("x",0)
      .attr("y",0)
      .attr("width", cloudwidth)
      .attr("height", cloudheight)
      .attr("fill", "none")
      .style("stroke","black")
      .style("stroke-width", "1px");


    }

  }

  function wordCloudAnimation(tempData, divg){
    if(d3.selectAll("#wordcloud")[0].length != 0){
      var clouddata = jQuery.extend(true, [], tempData);
      var cloudwords = [];
      clouddata.forEach(function(d,i){
          var tempkey = d.keywords;
          tempkey.forEach(function(g,i){
              g.id = d.id;
          });
          cloudwords = cloudwords.concat(tempkey);
      });
      cloudwords.sort(function (a, b) {
          return b.occurrence - a.occurrence ;
      })
      cloudwords = cloudwords.filter(function(d,i){//filter out only 10 top keywords
          return i < 10;
      })
      wordsizes = [];
      cloudwords.forEach(function(d,i){
          wordsizes.push(d.occurrence);
      });
      var minSize = d3.min(wordsizes);
      var maxSize = d3.max(wordsizes);
      var firstScale = d3.scale.linear()
        .domain([minSize,maxSize])
        .range([0,1]);
      var sizeScale = d3.scale.pow().exponent(.8)
        .domain([firstScale(minSize),firstScale(maxSize)])
        .range([10,25]);



      d3.layout.cloud()
      .size([cloudwidth, cloudheight])
      .words(cloudwords.map(function(d) {
        return {text: d.value, size: sizeScale(firstScale(d.occurrence)), id: d.id};
      }))
      .padding(1)
      .rotate(function() { return ~~(Math.random() * 2) * 1; })
      .font("Verdana")
      .fontSize(function(d) { return d.size; })
      .on("end", draw)
      .start();
    }
    
  }
  // var fill = d3.scale.category20();

  

  function draw(words) {
    if(wordcloud.selectAll("#cloudtext")[0].length != 0){
      //Transition Hide
          wordcloud.selectAll("#cloudtext").remove();
      } 
    wordcloud.append("g")
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