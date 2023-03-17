$(document).ready(function() {
  var taregt_url = './list_files';
  $.ajax({
    url: taregt_url,
    type: "GET",
    dataType: 'json', 
    success: function(data) {
      console.log(data); // Log the response
      // get the JSON response and update the HTML content
      var files = data.files;
      var fileList = document.getElementById("file-list");
      for (var i = 0; i < files.length; i++) {
        var li = document.createElement("li");
        li.appendChild(document.createTextNode(files[i]));
        fileList.appendChild(li);
      }
    },
    error: function(xhr, status, error) {
      console.log("Error: " + error.message);
    }
  });
});


function playTrack() {
	$.ajax({
		url: "./sound/play",
		type: "GET",
		success: function(response) {
			console.log(response);
		},
		error: function(xhr) {
			console.log(xhr.responseText);
		}
	});
}


function pauseTrack() {
	$.ajax({
		url: "./sound/pause",
		type: "GET",
		success: function(response) {
			console.log(response);
		},
		error: function(xhr) {
			console.log(xhr.responseText);
		}
	});
}

function nextTrack() {
	$.ajax({
		url: "./sound/next",
		type: "GET",
		success: function(response) {
			console.log(response);
		},
		error: function(xhr) {
			console.log(xhr.responseText);
		}
	});
}


function backTrack() {
	$.ajax({
		url: "./sound/back",
		type: "GET",
		success: function(response) {
			console.log(response);
		},
		error: function(xhr) {
			console.log(xhr.responseText);
		}
	});
}