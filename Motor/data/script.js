console.log("hello");

function play() {
    var request = new Http.Get("http://10.0.0.3/play", true);
    request.start().then(function(response) {
        console.log(response);
    }).fail(function(error, errorCode){
        console.log(error);
    });
}

function pause() {
    var request = new Http.Get("http://10.0.0.3/pause", true);
    request.start().then(function(response) {
        console.log(response);
    }).fail(function(error, errorCode){
        console.log(error);
    });
}

function stop() {
    var request = new Http.Get("http://10.0.0.3/stop", true);
    request.start().then(function(response) {
        console.log(response);
    }).fail(function(error, errorCode){
        console.log(error);
    });
}

function reset() {
    var request = new Http.Get("http://10.0.0.3/reset", true);
    request.start().then(function(response) {
        console.log(response);
    }).fail(function(error, errorCode){
        console.log(error);
    });
}

function reload() {
    var request = new Http.Get("http://10.0.0.3/load", true);
    request.start().then(function(response) {
        // console.log(response);
        var seq = document.getElementById("seq");
        seq.value = response; 
    }).fail(function(error, errorCode) {

        console.log(error);
    });
}


///
// TODO IMPLEMENT POST::

function save() {
    var data = "";
    var request = new Http.Post("http://10.0.0.3/save", data, true);
    request.start().then(function(response) {
        console.log(response);
    }).fail(function(error, errorCode){
        console.log(error);
    });
}

function go() {
    var data = "position=10&speed=20";
    var request = new Http.Get("http://10.0.0.3/goto?"+data,  true);
    request.start().then(function(response) {
        console.log(response);
    }).fail(function(error, errorCode){
        console.log(error);
    });
}


reload();

