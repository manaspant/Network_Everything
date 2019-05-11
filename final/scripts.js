//In this exampple, the NY Times articles and Flickr images are not directly related

var nyTimesArticles = [];
var flickrData = [];
var last_article = localStorage.getItem("someVarKey");
var new_article;


function makeHTML(){
	var theHTML = '';
	for (var i = 0; i < nyTimesArticles.length; i++){
		theHTML += "<div class='flickrArticle'>";
		theHTML += "<h3>" + nyTimesArticles[i].headline.main + "</h3>";
		theHTML += "<img src='" + flickrData[i].url_o + "'/>";
		theHTML += "</div>";
	}
	$('main').html(theHTML);
}

function getNYTimesData(){
	console.log("Get NY Times Data");

	var nyTimesURL = 'https://api.nytimes.com/svc/news/v3/content/all/all.json?api-key=';
	var myNYKey = 'ZZ5qAmnLssHvigR7XLKMs5s4xKyZDQfo';
	var nyTimesReqURL = nyTimesURL + myNYKey;
	console.log(nyTimesReqURL);
	$.ajax({
		url: nyTimesReqURL,
		type: 'GET',
		dataType: 'json',
		error: function(err){
			console.log("Uh oh...");
			console.log(err);
		},
		success: function(data){
			//console.log(data);
			new_article = data.results[0].slug_name;
			console.log(new_article);
			console.log(data.results);
		}
	});
}

$(document).ready(function(){
	//console.log("I'm ready!");
	getNYTimesData();

});


function buttonFunction(){
	
	if (new_article != last_article) {
		console.log("There is a new article that you have not seen!!!!");
		last_article = new_article;
		localStorage.setItem("someVarKey", last_article);
	}

	else{
		console.log("You are up to date!");
	}

}
