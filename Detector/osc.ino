#include <ArduinoOSC.h>

OscWiFi osc;
int oscPort = 3737;


void osc_setup() {

    // test
    osc.subscribe("/test", [](OscMessage& m)
    {
        // do something with osc message
        Serial.print("OSC /test ");
        Serial.print(m.arg<int>(0));    Serial.print(" ");
        Serial.print(m.arg<float>(1));  Serial.println(" ");
    });

    // animate
    osc.subscribe("/animate", [](OscMessage& m)
    {
        Serial.println("/animate.. nothing to do yet ");
    });
}

void osc_loop() {
    osc.parse();
}

void osc_begin() {
    osc.begin(oscPort);
}