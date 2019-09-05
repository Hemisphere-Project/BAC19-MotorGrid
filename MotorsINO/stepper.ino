#include <AccelStepper.h>

AccelStepper stepper(1, 16, 32);      // Type (TB6600: 1) , Pin PUL+ , Pin DIR+
SemaphoreHandle_t stepper_mutex;


void stepper_setup() {
    stepper_mutex = xSemaphoreCreateMutex();

    // STEPPER INIT
    stepper.setCurrentPosition(0);
    stepper.moveTo(0);
    stepper.setMaxSpeed(30000);
    stepper.setAcceleration(40000);
    stepper.moveTo(stepper_minposition);
}


void stepper_loop() {
    stepper_lock();

    // STEPPER bounce
    if (stepper_bouncing) {
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
    //Serial.println(stepper.currentPosition());

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

long stepper_minposition = 10000;
long stepper_maxposition = 50000;
long stepper_destination = stepper_minposition;
bool stepper_bouncing = true;

void stepper_startBounce() {
    stepper_lock();
    Serial.println("Start Bounce ");
    stepper_bouncing = true;
    stepper.moveTo(stepper_destination);
    stepper_unlock();
}

void stepper_stopBounce() {
    stepper_lock();
    Serial.println("Stop Bounce ");
    stepper_bouncing = false;
    stepper.stop();
    stepper.runToPosition();
    stepper_unlock();
}

void stepper_toggleBounce() {
    if (stepper_bouncing) stepper_stopBounce();
    else stepper_startBounce();
}




void trigOrigin() {
  stepper_lock();
  stepper.stop();
  stepper.runToPosition();
  stepper.setCurrentPosition(0);
  stepper_unlock();
}