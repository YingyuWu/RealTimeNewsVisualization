var duration   = 500,
    transition = 200;

/*
drawDonutChart(
  '#donut',
  $('#donut').data('donut'),
  290,
  290,
  ".35em"
);*/

function drawDonutChart(element, displayString, percent, x_off, y_off, width, height, text_y) {
  width = typeof width !== 'undefined' ? width : 290;
  height = typeof height !== 'undefined' ? height : 290;
  text_y = typeof text_y !== 'undefined' ? text_y : "-.10em";

  var dataset = {
        lower: calcPercent(0),
        upper: calcPercent(percent)
      },
      radius = Math.min(width, height) / 2,
      pie = d3.layout.pie().sort(null),
      format = d3.format(".0%");

  var arc = d3.svg.arc()
        .innerRadius(radius - 20)
        .outerRadius(radius);

  var svg = d3.select(element).append("svg")
        .attr("width", width)
        .attr("height", height)
        .attr("x",x_off)
        .attr("y",y_off)
        .append("g")
        .attr("transform", "translate(" + width / 2 + "," + height / 2 + ")");

  var path = svg.selectAll("path")
        .data(pie(dataset.lower))
        .enter().append("path")
        .attr("class", function(d, i) { return "color" + i })
        .attr("d", arc)
        .each(function(d) { this._current = d; }) // store the initial values
        .attr("stroke","white");

  var text = svg.append("text")
        .attr("text-anchor", "middle")
        .attr("dy", text_y)
        .attr("class", "pieChartText")

  
    text.text(displayString);


    var progress = 0;
    var timeout = setTimeout(function () {
      clearTimeout(timeout);
      path = path.data(pie(dataset.upper)); // update the data
      path.transition().duration(duration).attrTween("d", function (a) {
        // Store the displayed angles in _current.
        // Then, interpolate from _current to the new angles.
        // During the transition, _current is updated in-place by d3.interpolate.
        var i  = d3.interpolate(this._current, a);
        var i2 = d3.interpolate(progress, percent)
        this._current = i(0);
        return function(t) {
          //text.text( format(i2(t) / 100) );
          return arc(i(t));
        };
      }); // redraw the arcs
    }, 200);
  
};

function calcPercent(percent) {
  return [percent, 100-percent];
};