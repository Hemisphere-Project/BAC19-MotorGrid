#include <ArduinoOSC.h>

OscWiFi osc;
int oscPort = 3737;


void osc_setup() {

    // test
    osc.subscribe("/test", [](OscMessage& m)
    {
        // do something with osc message
        LOGINL("OSC /test ");
        LOGINL(m.arg<int>(0));    LOGINL(" ");
        LOGINL(m.arg<float>(1));  LOG(" ");
    });

    // animate
    osc.subscribe("/play", [](OscMessage& m) { stepper_play(); });
    osc.subscribe("/stop", [](OscMessage& m) { stepper_stop(); });
    osc.subscribe("/pause", [](OscMessage& m) { stepper_pause(); });
    osc.subscribe("/reset", [](OscMessage& m) { stepper_reset(); });
}

void osc_loop() {
    osc.parse();
}

void osc_begin() {
    osc.begin(oscPort);
}