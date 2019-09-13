#include <WebServer.h>

WebServer server(80);

// SERVER

void webserver_setup() {


    server.onNotFound([](){
        server.send(404, "text/plain", "Not found");
    });
    server.on("/play", [](){
        triggerAction();
        server.send(200, "text/plain", "Trigger Action");
    });
    server.on("/playgroup", [](){
        triggerMyGroup();
        server.send(200, "text/plain", "Trigger My Group");
    });
    server.on("/stopgroup", [](){
        stopMyGroup();
        server.send(200, "text/plain", "Stop Group");
    });
    server.on("/resetgroup", [](){
        resetMyGroup();
        server.send(200, "text/plain", "Reset Group");
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
