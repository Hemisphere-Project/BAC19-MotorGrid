#include <AccelStepper.h>
#define TOP_PIN     34

AccelStepper stepper(1, 16, 32);      // Type (TB6600: 1) , Pin PUL+ , Pin DIR+
SemaphoreHandle_t stepper_mutex;

bool stepper_ready = false;
bool stepper_resetting = false;
long stepper_waitValid = 2000;
bool stepper_animate = false;

// GLOBAL
int base_accel = 10 * SMULTI * SRESO;

// STOP
int stop_speed = 10 * SMULTI * SRESO;
int stop_position = 10 * SMULTI * SRESO;

// OFFSET
int offset_position = SMULTI * SRESO;
int offset_speed = offset_position/2;

// RESET
int reset_steps = 300;
int reset_speed = 1 * SMULTI * SRESO;
long reset_timeClick = 0;



void stepper_setup() {
    stepper_mutex = xSemaphoreCreateMutex();
    digitalWrite(TOP_PIN, HIGH);
    pinMode(TOP_PIN, INPUT_PULLUP);
    

    // STEPPER INIT
    stepper_ready = false;
    stepper.setMaxSpeed(reset_speed);
    stepper.setAcceleration(base_accel);
}


void stepper_loop() {
    
    if (stepper_resetProcedure()) return;

    stepper_lock();

    // STEPPER animate
    if (stepper_animate) {
        
        
        // In position: load next step
        if (stepper.distanceToGo() == 0) {
            step_t step = seq_nextStep();
            
            // SEQ end
            if (step.pos == 0) {
                LOG("SEQ end");

                stepper_unlock();
                stepper_stopAnimate();
                stepper_lock();
            }

            // SEQ next
            else {
                LOG("SEQ next step");
                stepper_goto( step );
            }
        }

        //   if (stepper.distanceToGo() == 0) {
        //     if (stepper_destination == stepper_minposition) stepper_destination = stepper_maxposition;
        //     else stepper_destination = stepper_minposition;
        //     stepper.moveTo(stepper_destination);
        //     LOGINL("bouncing... to ");
        //     LOG(stepper_destination);
        //   }

    }

    stepper.run();
    stepper_unlock();
    
    // STEPPER wait
    // else delay(10);

}

void stepper_lock() {
    xSemaphoreTake(stepper_mutex, portMAX_DELAY);
}

void stepper_unlock() {
    xSemaphoreGive(stepper_mutex);
}

void stepper_goto(step_t step) {
    stepper.moveTo(step.pos * SMULTI * SRESO);
    if (step.speed > 0) stepper.setMaxSpeed(step.speed * SMULTI * SRESO);
    if (step.accel > 0) stepper.setAcceleration(step.accel * SMULTI * SRESO);
}

/*
    ORIGIN
*/
bool stepper_resetProcedure() {

    // TOP detect
    bool topState = digitalRead(TOP_PIN);

    // Top triggered
    if ((topState == LOW)) {
        stepper_lock();
        stepper.stop();
        stepper.runToPosition();
        stepper_unlock();
        
        // Enter resetting tempo
        if (reset_timeClick == 0) {
            reset_timeClick = millis(); 
            stepper_ready = false;
            LOG("Resetting... ");
        }

        // validate
        if ((millis()-reset_timeClick) > stepper_waitValid ) {
            stepper_lock();
            stepper.setCurrentPosition(0);
            stepper.setMaxSpeed(reset_speed);
            stepper.moveTo(offset_position);
            stepper.runToPosition();
            stepper_unlock();
            reset_timeClick = 0;
            stepper_ready = true;
            LOG("Reset Validated");
            stepper_stopAnimate();
            
        }
    }

    // looking for origin : going UP
    if (!stepper_ready) {
        if(topState == HIGH) {
            if (reset_timeClick == 0) {
                stepper_lock();
                stepper.move(-1*reset_steps);
                stepper.runToPosition();
                stepper_unlock();
            }
            else LOG("Going up to reset... ");
            reset_timeClick = 0;
        }
    }   

    return !stepper_ready;
}





/*
    Animate
*/

void stepper_startAnimate() {
    LOG("Start Animate ");
    stepper_animate = true;
}

void stepper_pauseAnimate() {
    LOG("Pause Animate ");
    stepper_animate = false;
}

void stepper_stopAnimate() {
    stepper_animate = false;
    if (!stepper_ready) return;
    LOG("Stop Animate ");

    stepper_lock();
    step_t step = seq_initStep();
    if (step.pos == 0) step = {stop_position, stop_speed, base_accel};
    stepper_goto( step );
    while (stepper_ready && stepper.distanceToGo() != 0) stepper.run();
    stepper_unlock();

}

void stepper_resetAnimate() {
    LOG("Manual reset ");
    stepper_animate = false;
    stepper_ready = false;
}

void stepper_manualAnimate(step_t step) {
    LOG("Manual goto ");
    stepper_lock();
    stepper_goto( step );
    // while (stepper_ready && stepper.distanceToGo() != 0) stepper.run();
    stepper_unlock();
}