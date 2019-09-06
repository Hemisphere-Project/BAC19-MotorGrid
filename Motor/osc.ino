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
    osc.subscribe("/animate", [](OscMessage& m)
    {
        if (m.arg<int>(0) == 0) stepper_stopAnimate();
        else stepper_startAnimate();
    });
}

void osc_loop() {
    osc.parse();
}

void osc_begin() {
    osc.begin(oscPort);
}