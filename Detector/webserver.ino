#include <WebServer.h>

WebServer server(80);

// SERVER

void webserver_setup() {


    server.onNotFound([](){
        server.send(404, "text/plain", "Not found");
    });

    server.on("/play", [](){
        triggerAction();
        server.send(200, "text/plain", "Called Start !");
    });

    // STATIC

    server.on("/", [](){
        server.send(200, "text/plain", "Hello !");
    });

    server.begin();
}

void webserver_loop() {
    server.handleClient();
}
