#include <WebServer.h>
#include "webinc.h"
#include <SPIFFS.h>

WebServer server(80);

// SERVER

void webserver_setup() {

    
    server.onNotFound([](){ 
        server.send(404, "text/plain", "Not found"); 
    });

    server.on("/play", [](){ 
        stepper_play(); 
        server.send(200, "text/plain", "");
    });

    server.on("/pause", [](){ 
        stepper_pause(); 
        server.send(200, "text/plain", "");
    });

    server.on("/stop", [](){ 
        stepper_stop(); 
        server.send(200, "text/plain", "");
    });
    
    server.on("/reset", [](){ 
        stepper_reset(); 
        server.send(200, "text/plain", "");
    });

    server.on("/load", [](){ 
        server.send(200, "text/plain", seq_export()); 
    });

    // Args: sequence
    server.on("/save", [](){
        if (server.hasArg("sequence")) {
            if (seq_import( server.arg("sequence") )) server.send(200, "text/plain", seq_export());
            else server.send(500, "text/plain", "parsing error");
        }
        server.send(500, "text/plain", "no sequence received");
    });
    
    // Args: position / speed / accel
    server.on("/goto", [](){

            int to = server.hasArg("position") ? server.arg("position").toInt() : 0;
            int speed = server.hasArg("speed") ? server.arg("speed").toInt() : 0;
            int accel = server.hasArg("accel") ? server.arg("accel").toInt() : 0;
            int param1 = server.hasArg("param1") ? server.arg("param1").toInt() : 0;
            int param2 = server.hasArg("param2") ? server.arg("param2").toInt() : 0;
            int param3 = server.hasArg("param3") ? server.arg("param3").toInt() : 0;

            if (to > 0) {
                step_t step = {SEQ_GOTO, to, speed, accel, param1, param2, param3};
                stepper_goto(step);
            }

            server.send(200, "text/plain", "manual goto\n position = "+String(to)+" speed = "+String(speed)+" accel = "+String(accel)+
                                            "param1 = "+String(param1)+" param2 = "+String(param2)+" param3 = "+String(param3)+"\n"  ); 
    });

    // STATIC

    server.on("/", [](){ 
        String index = "";
        int error = 0;

        if(SPIFFS.begin()){
            File file = SPIFFS.open("/index.html", "r");
            if (file && file.size()) {
                while (file.available()) index += char(file.read());
                file.close();
            }
            else error = 2;
        }   
        else error = 1;

        if (error) {
            index = "Error loading index.html from ESP32... error:"+String(error);
            LOG("WEBSERVER: Error loading index.html... error:"+String(error));
        }

        server.send(200, "text/html", index); 
    });


    server.on("/inner", [](){ 
        server.send(200, "text/html", indexhtml); 
    });
    server.on("/http.min.js", [](){ 
        server.send(200, "text/javascript", httpjs); 
    });
    server.on("/nanoajax.min.js", [](){ 
        server.send(200, "text/javascript", nanoajax); 
    });

    server.begin();
}

void webserver_loop() {
    server.handleClient();
}

