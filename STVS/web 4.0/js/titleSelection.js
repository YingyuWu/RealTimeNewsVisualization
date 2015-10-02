var title_selection = "f_title";

function changeTitleF(){
	$("#f-title").addClass("active");
	$("#p-title").removeClass("active");
	$("#g-title").removeClass("active");

	title_selection = "f_title";
	reDraw()
}

function changeTitleP(){
	$("#f-title").removeClass("active");
	$("#p-title").addClass("active");
	$("#g-title").removeClass("active");

	title_selection = "p_title";
	reDraw()
}

function changeTitleG(){
	$("#f-title").removeClass("active");
	$("#p-title").removeClass("active");
	$("#g-title").addClass("active");

	title_selection = "g_title";
	reDraw()
}