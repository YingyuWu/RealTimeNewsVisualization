var articleDetailTable;
var focusClusterTable;
var ArticleInformationArray = [];
var ArticleDetailArray = [];
var FocusClusterInformationArray = [];
var newArticleIndex;
var newArticleIndexForCluster;

function updateArticleDetailTable(newData){
	newArticleIndex = 0;
	newData.forEach(function(d){
		newArticleIndex += 1;
		var tempItem = [-1];
		tempItem = tempItem.concat(d)
		tempItem[1] = tempItem[1].substr(1,tempItem[1].length-2)
		ArticleInformationArray.push(tempItem);
	})
}

function updateTableShowAndHide(IsFocusing){
	if (IsFocusing){
		$("#focusClusterDataTableDiv").show();
		$("#articleDetailDataTableDiv").hide();
	}
	else{
		$("#articleDetailDataTableDiv").show();
		$("#focusClusterDataTableDiv").hide();
	}
}

function updateArticleClusterIDForTable(newData){
	FocusClusterInformationArray = [];
	ArticleDetailArray = [];
	newArticleIndexForCluster = 0;

	ArticleInformationArray.forEach(function(d, i){
		d[0] = newData.articleNodes[i].cid;

		if (d[0] != -1 && i>=ArticleInformationArray.length-newArticleIndex){
			ArticleDetailArray.push(d);
		}

		if (typeof cluster_selected_id != "undefined"){
			if (d[0] == cluster_selected_id){
				FocusClusterInformationArray.push(d)

				if (i >= ArticleInformationArray.length-newArticleIndex){
					newArticleIndexForCluster += 1;
				}
			}
		}
	})

	articleDetailTable.fnClearTable();
	articleDetailTable.fnAddData(ArticleDetailArray);

    $('#articleDetailDataTable tbody tr').each( function (i, d) {
      fill = color(parseInt(this.childNodes[0].childNodes[0].data));
      $(this).css('background-color',fill);
      $(this).css('opacity',0.8);

      if (i < newArticleIndex){
  	      //$(this.childNodes[0]).css("border", "2px solid black")
	      //$(this.childNodes[1]).css("border", "2px solid black")
	      //$(this.childNodes[2]).css("border", "2px solid black")
      }
      //this.childNodes[0].innerHTML = '<div style="width:20px;height:20px;float:left;background:'+fill+'"></div>';
     // this.childNodes[0].innerHTML = '<div style="width:10px;height:20px;float:left;" align="top" ></div>';
      this.childNodes[0].innerHTML = '<img src="images/details_open.png">'
    } );

    $('#articleDetailDataTable tbody td img').on('click', onClickForArticleDetailTable());

	var stop = 0;

	focusClusterTable.fnClearTable();
	focusClusterTable.fnAddData(FocusClusterInformationArray);

    $('#focusClusterDataTable tbody tr').each( function (i, d) {
      fill = color(parseInt(this.childNodes[0].childNodes[0].data));
      $(this).css('background-color',fill);
      $(this).css('opacity',0.8);

      if (i < newArticleIndexForCluster){
  	      $(this.childNodes[0]).css("border", "2px solid black")
	      $(this.childNodes[1]).css("border", "2px solid black")
	      $(this.childNodes[2]).css("border", "2px solid black")
      }
      //this.childNodes[0].innerHTML = '<div style="width:20px;height:20px;float:left;background:'+fill+'"></div>';
      //this.childNodes[0].innerHTML = '<div style="width:10px;height:20px;float:left;"></div>';
      this.childNodes[0].innerHTML = '<img src="images/details_open.png">'
    } );

    $('#focusClusterDataTable tbody td img').on('click', onClickForFocusClusterTable());
}

function initDataTable(){
		articleDetailTable = $("#articleDetailDataTable").dataTable({
	        "sScrollY": "480px",
	        //"sScrollX": "100%",
	        "sScrollXInner": "110%",
	        "bPaginate": false,
	        "bScrollCollapse": true,
	        "aaSorting": [[ 1, "desc" ]],
	        "aoColumnDefs": [
	            { "bSortable": false, "aTargets": [ 0 ] }
	        ],
	        /*"aoColumns": [
	            null,
	            { "sType": "numeric" },
	            null,
	            null,
	            null,
	            null
	        ],
	        */
	        /*"fnDrawCallback": function() {
	        	var dataTableWrapper = $("#articleDetailDataTable").closest(".dataTables_wrapper");
	        	var panelHeight = dataTableWrapper.parent().height();
	        	var toolbarHeights = 0;

	        	dataTableWrapper.find(".fg-toolbar").each(function(i, obj) {
                                  toolbarHeights = toolbarHeights + $(obj).height();
                              });
	        	
                var height = panelHeight - toolbarHeights - dataTableWrapper.find(".dataTables_scrollHead").height();
                dataTableWrapper.find(".dataTables_scrollBody").height(height - 60);
	        }*/
	        "fnDrawCallback": function() {
                              $("#articleDetailDataTable_wrapper .dataTables_scrollBody").height(480);
          },
	    } );

        articleDetailTable.fnSetColumnVis( 3, false);
   		articleDetailTable.fnSetColumnVis( 4, false);
   		//articleDetailTable.fnSetColumnVis( 5, false);

		focusClusterTable = $("#focusClusterDataTable").dataTable({
	        "sScrollY": "480px",
	        "sScrollX": "100%",
	        "sScrollXInner": "110%",
	        "bPaginate": false,
	        "bScrollCollapse": true,
	        "aaSorting": [[ 1, "desc" ]],
	        "aoColumnDefs": [
	            { "bSortable": false, "aTargets": [ 0 ] }
	        ],
	        "fnDrawCallback": function() {
                              $("#focusClusterDataTable_wrapper .dataTables_scrollBody").height(480);
          },
	        /*"aoColumns": [
	            null,
	            { "sType": "numeric" },
	            null,
	            null,
	            null,
	            null
	        ]
	        */
	    } );

        focusClusterTable.fnSetColumnVis( 3, false);
   		focusClusterTable.fnSetColumnVis( 4, false);
   		//focusClusterTable.fnSetColumnVis( 5, false);

	    $("#focusClusterDataTableDiv").hide();
	    $("#articleDetailDataTableDiv").show();
}

function fnFormatDetails ( myTable, nTr )
{
  var aData = myTable.fnGetData( nTr );
  var sOut = '<table cellpadding="5" cellspacing="0" border="0" style="padding-left:50px; background:'+color(parseInt(aData[0]))+'; width:100%">';
  sOut += '<tr><td>Keywords:</td><td>'+aData[3]+'</td></tr>';
  sOut += '<tr><td>PubDate:</td><td>' + aData[1] + '</td></tr>';
  sOut += '<tr><td>First Paragraph:</td><td>'+ aData[4] +'</td></tr>';
  sOut += '</table>';
   
  return sOut;
}

function onClickForFocusClusterTable(){
	return function () {
        var nTr = $(this).parents('tr')[0];
        if ( focusClusterTable.fnIsOpen(nTr) )
        {
            /* This row is already open - close it */
            this.src = "images/details_open.png";
            focusClusterTable.fnClose( nTr );
        }
        else
        {
            /* Open this row */
            this.src = "images/details_close.png";
            focusClusterTable.fnOpen( nTr, fnFormatDetails(focusClusterTable, nTr), 'details' );
        }
    }
}

function onClickForArticleDetailTable(){
	return function () {
        var nTr = $(this).parents('tr')[0];
        if ( articleDetailTable.fnIsOpen(nTr) )
        {
            /* This row is already open - close it */
            this.src = "images/details_open.png";
            articleDetailTable.fnClose( nTr );
        }
        else
        {
            /* Open this row */
            this.src = "images/details_close.png";
            articleDetailTable.fnOpen( nTr, fnFormatDetails(articleDetailTable, nTr), 'details' );
        }
    }
}