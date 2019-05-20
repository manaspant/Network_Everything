//In this exampple, the NY Times articles and Flickr images are not directly related

var nyTimesArticles = [];
var flickrData = [];
var last_article = localStorage.getItem("someVarKey");
var new_article;
var articleList;

$('#instructions2').hide();
$('#title-article').hide();
$('#section-name').hide();
$('#url').hide();
$('#buttton').hide();


// Socket Stuff is here!


var socket = io();

// Receiving data from server

socket.on('event', (data) => {
	console.log('received data from webserver: ' + data);
});

// Show answer once you see that the user has pressed a button. The server informs scripts.js that the button has been pressed.
// These different hide and how methods show the divs that have the details about the latest story.

socket.on('buttonPressed', (data) => {
	console.log('received button pressed event from webserver: ' + data);
	$('#instructions2').hide();
	$('#instructions1').hide();
	$('#section-name').show();
	$('#title-article').show();
	$('#url').show();
	$('#buttton').show();

});

// Functions to send message to server about which category of article needs to be sent to the Arduino

function worldArticle() {
  socket.emit('worldArticle');
  console.log("Button clicked on browser")
}

function USArticle() {
  socket.emit('USArticle');
  console.log("Button clicked on browser")
}

function NYArticle() {
  socket.emit('NYArticle');
  console.log("Button clicked on browser")
}


function otherArticle() {
  socket.emit('otherArticle');
  console.log("Button clicked on browser")
}

// This function informs server that new game is about to start and the server in turn will send message to arduino to reset all variables

function newGame(){
	socket.emit('NEWGAME');
	console.log('STARTING NEW GAME!');
}

// Getting data from NY Times API using this function

function getNYTimesData(){
	console.log("Get NY Times Data");

	var nyTimesURL = 'https://api.nytimes.com/svc/news/v3/content/all/all.json?api-key=';
	var myNYKey = 'ZZ5qAmnLssHvigR7XLKMs5s4xKyZDQfo';
	var nyTimesReqURL = nyTimesURL + myNYKey;
	$.ajax({
		url: nyTimesReqURL,
		type: 'GET',
		dataType: 'json',
		error: function(err){
			console.log("Uh oh...");
			console.log(err);
		},
		success: function(data){
			new_article = data.results[0].slug_name;
			articleList = data.results; // saving the top 20 articles in this list
			console.log(new_article);
			console.log(data.results);
		}
	});
}

// Making sure document is ready before we get the data from NYT

$(document).ready(function(){
	//console.log("I'm ready!");
	getNYTimesData();

});

// This is the first function that runs as soon as we refresh the page to begin a new game. It hides all the divs that have the title of the news // article as well as the category and source. These are revealed once the user has chosen their answer on the box. The function buttonpressed
//mentioned above shows these divs. 

function buttonFunction(){
	var randomNumber = Math.floor(Math.random()*articleList.length)
	var randomArticle = articleList[randomNumber]
	console.log(randomArticle);
	console.log(randomArticle.section)
	$('#instructions1').hide();
	$('#instructions2').show();

	$('#section-name').html(randomArticle.section);
	$('#title-article').html(randomArticle.title);
	$('#url-article').html(randomArticle.url);

	// functions below check what category the news article falls under and then assigns the functions mentioned above based on that

	if (randomArticle.section == "World") {
		worldArticle()
	}

	if (randomArticle.section == "U.S." || randomArticle.section == "New York") {
		USArticle()
	}

	if (randomArticle.section == "Opinion") {
		NYArticle()
	}

	if (randomArticle.section != "U.S." && randomArticle.section != "World" && randomArticle.section != "New York" && randomArticle.section != "Opinion") {
		console.log("reached other in scripts");
		otherArticle()
	}
}


// this function refreshes the page once the game is over. It is run when the start new game button is pressed on the webpage after the end of the game

function startNew(){
	location.reload();
	newGame();
}

// Once the user presses the start new game button on the arduino, the arduino informs the server to inform the webpage about the button press
socket.on('GameStartKaroBC', (data) => {
	console.log('Command to start game received');
	buttonFunction();
});