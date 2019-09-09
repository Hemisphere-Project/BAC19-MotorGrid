#include <AccelStepper.h>
#define TOP_PIN     34

AccelStepper stepper(1, 16, 32);      // Type (TB6600: 1) , Pin PUL+ , Pin DIR+
SemaphoreHandle_t stepper_mutex;

bool stepper_ready = false;
bool stepper_resetting = false;
long stepper_waitValid = 300;

bool stepper_animate = false;
bool stepper_manual = false;

// GLOBAL
float base_accel = 1.0;
float base_speed = 1.0;

// OFFSET
float offset_position = 1.0;
float offset_speed = 0.5;

// RESET
float reset_steps = 100;
float reset_speed = 1.0;
float reset_timeClick = 0;

// DEBUG
long debugPos_tick = 0;
int debugPos_interval = 1000;
long debugPos_last = 0;


/*
    PRIVATE
*/

void stepper_lock() {
    xSemaphoreTake(stepper_mutex, portMAX_DELAY);
}

void stepper_unlock() {
    xSemaphoreGive(stepper_mutex);
}

void stepper_goto(step_t step) {
    
    // empty step: default to offset position
    if (step.pos == 0) step = {offset_position, base_speed, base_accel};

    stepper_lock();
    stepper.moveTo(step.pos * sfactor);
    if (step.speed > 0) stepper.setMaxSpeed(step.speed * sfactor);
    if (step.accel > 0) stepper.setAcceleration(step.accel * sfactor);
    stepper_unlock();

    LOGINL("- goto position "); LOG( step.pos * 1.0 );
}


/*
    PUBLIC
*/


void stepper_setup() {
    stepper_mutex = xSemaphoreCreateMutex();
    pinMode(TOP_PIN, INPUT);

    // STEPPER INIT
    stepper_ready = false;
    stepper.setMaxSpeed(reset_speed * sfactor);
    stepper.setAcceleration(base_accel * sfactor);
}


void stepper_loop() {
    
    // STEPPER must find Zero
    if (stepper_resetProcedure()) return;

    // STEPPER animate (sequencer)
    if (stepper_animate) {
        
        // In position: load next step
        if (stepper.distanceToGo() == 0) {
            step_t step = seq_nextStep();
            
            // SEQ end
            if (step.pos == 0) {
                LOG("SEQ end");
                stepper_stop();
            }

            // SEQ next
            else {
                LOGINL("SEQ next step ");
                stepper_goto( step );
            }
        }
    }

    // STEPPER Run
    if (stepper_animate || stepper_manual) {
        stepper_lock();
        stepper.run();
        stepper_unlock();

        // debug position
        if ( millis() - debugPos_tick > debugPos_interval && stepper.currentPosition() != debugPos_last) {
            debugPos_last = stepper.currentPosition();
            debugPos_tick = millis();
            LOGINL("-- position "); LOG( debugPos_last / sfactor );
        }
    }
    
}



/*
    ORIGIN
*/
bool stepper_resetProcedure() {

    // TOP detect
    bool topState = digitalRead(TOP_PIN);

    // TOP triggered
    if ((topState == LOW)) {
        
        // Engage Trigger watch
        if (reset_timeClick == 0) {
            reset_timeClick = millis(); 
            LOG("Top switch triggered");
            return false;   // Pause
        }

        // It's reset point
        if (stepper_ready && (millis()-reset_timeClick) > stepper_waitValid ) {
            stepper_ready = false;
            LOG("Resetting...");
        }


        // Validate reset point
        if ((millis()-reset_timeClick) > stepper_waitValid * 6 ) {
            LOG("Reset Validated");
            stepper_lock();
            stepper.setCurrentPosition(0);
            stepper.setMaxSpeed(reset_speed * sfactor);
            stepper.moveTo(offset_position * sfactor);
            stepper.runToPosition();
            stepper_unlock();
            reset_timeClick = 0;
            stepper_ready = true;
            stepper_stop();
        }
    }

    // TOP released
    else {

        // Reset point not found: going UP
        if (!stepper_ready) {
            stepper_lock();
            stepper.move(-1*reset_steps);
            stepper.runToPosition();
            stepper_unlock();
            LOG("Going up to reset ^ ");
        }

        // no click
        reset_timeClick = 0;
    }

    return !stepper_ready;
}





/*
    Animate
*/

void stepper_play() {
    stepper_animate = true;
    stepper_manual = false;
    LOG("Play ");
}

void stepper_pause() {
    stepper_animate = false;
    stepper_manual = false;
    LOG("Pause ");
}

void stepper_stop() {
    stepper_animate = false;
    stepper_manual = false;
    if (!stepper_ready) return;
    LOG("Stop ");

    // Get Step 0 (or default to offset position)
    stepper_goto( seq_initStep() );
    stepper_manual = true;
}

void stepper_reset() {
    LOG("Reset (manual) ");
    stepper_manual = true;
    stepper_animate = false;
    stepper_ready = false;
}

void stepper_go(step_t step) {
    LOG("Go (manual) ");
    stepper_goto( step );
    stepper_animate = false;
    stepper_manual = true;
}