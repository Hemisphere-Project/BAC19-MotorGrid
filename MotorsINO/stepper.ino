#include <AccelStepper.h>
#define TOP_PIN     34

AccelStepper stepper(1, 16, 32);      // Type (TB6600: 1) , Pin PUL+ , Pin DIR+
SemaphoreHandle_t stepper_mutex;
bool stepper_origin = false;
bool stepper_resetting = false;
long stepper_resetTime = 0;
long stepper_waitValid = 3000;

long stepper_offsetposition = 2000;
long stepper_offsetspeed = 1000;

long stepper_bouncespeed = 10000;
long stepper_minposition = 10000;
long stepper_maxposition = 50000;
long stepper_destination = stepper_minposition;
bool stepper_animate = true;

void stepper_setup() {
    stepper_mutex = xSemaphoreCreateMutex();
    digitalWrite(TOP_PIN, HIGH);
    pinMode(TOP_PIN, INPUT_PULLUP);
    

    // STEPPER INIT
    stepper.setCurrentPosition(0);
    stepper.moveTo(0);
    stepper.setMaxSpeed(10);
    stepper.setAcceleration(40000);
    stepper.moveTo(stepper_minposition);
}


void stepper_loop() {
    
    stepper_lock();

    // TOP detect
    bool topState = digitalRead(TOP_PIN);

    // Top triggered
    if ((topState == LOW)) {
        stepper.stop();
        stepper.runToPosition();
        
        // Enter resetting tempo
        if (!stepper_resetting) {
            stepper_resetTime = millis();
            stepper_resetting = true;    
            stepper_origin = false;
            Serial.println("Resetting... ");
        }

        // validate
        if ((millis()-stepper_resetTime) > stepper_waitValid ) {
            stepper.setCurrentPosition(0);
            stepper.moveTo(stepper_offsetposition);
            stepper.setMaxSpeed(stepper_offsetspeed);
            stepper.runToPosition();
            stepper.setMaxSpeed(stepper_bouncespeed);
            stepper_resetting = false;
            stepper_origin = true;
            Serial.println("Reset Validated");
        }
    }


    // STEPPER origin
    if (!stepper_origin) {

        // Going UP
        if(topState == HIGH) {
            if (!stepper_resetting) stepper.move(-1);
            else Serial.println("Going up to reset... ");
            stepper_resetting = false;
        }
    }

    // STEPPER animate
    else if (stepper_animate) {
      if (stepper.distanceToGo() == 0) {
        if (stepper_destination == stepper_minposition) stepper_destination = stepper_maxposition;
        else stepper_destination = stepper_minposition;
        stepper.moveTo(stepper_destination);
        Serial.print("bouncing... to ");
        Serial.println(stepper_destination);
      }
    }

    // STEPPER run
    stepper.run();

    stepper_unlock();
}

void stepper_lock() {
    xSemaphoreTake(stepper_mutex, portMAX_DELAY);
}

void stepper_unlock() {
    xSemaphoreGive(stepper_mutex);
}

/*
    ORIGIN
*/






/*
    BOUNCE
*/

void stepper_startBounce() {
    if (!stepper_origin) return;
    stepper_lock();
    Serial.println("Start Bounce ");
    stepper_animate = true;
    stepper.moveTo(stepper_destination);
    stepper_unlock();
}

void stepper_stopBounce() {
    if (!stepper_origin) return;
    stepper_lock();
    Serial.println("Stop Bounce ");
    stepper_animate = false;
    stepper.stop();
    stepper.runToPosition();
    stepper_unlock();
}

void stepper_toggleBounce() {
    if (stepper_animate) stepper_stopBounce();
    else stepper_startBounce();
}
