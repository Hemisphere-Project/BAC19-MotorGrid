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
int base_accel = 100;
int base_speed = 10;
long pauseUntil = 0;
int repeatStep = 0;

// OFFSET
int offset_position = 2;

// RESET
int reset_steps = 1;
int reset_speed = 1;
int reset_timeClick = 0;

// DEBUG
long debugPos_tick = 0;
int debugPos_interval = 500;
long debugPos_last = 0;
long startMove = 0;
bool debugGoingup = false;


/*
    PRIVATE
*/

void stepper_lock() {
    xSemaphoreTake(stepper_mutex, portMAX_DELAY);
}

void stepper_unlock() {
    xSemaphoreGive(stepper_mutex);
}

void stepper_execute(step_t step) {
    
    // Stop current movement
    stepper_lock();
    stepper.stop();
    stepper.runToPosition();
    pauseUntil = 0;

    // Stop
    if (step.type == SEQ_STOP) {
        stepper_unlock();
        stepper_stop();
        return;
    }

    // Wait
    else if (step.type == SEQ_WAIT) {
        pauseUntil = millis() + step.param1;
        LOGINL(" - delay "); LOG( step.param1 );
    }

    // Goto
    else if (step.type == SEQ_GOTO) {
        if (step.pos > 0) {
            startMove = millis();

            // values
            long pos = step.pos;
            long speed = step.speed;
            long accel = step.accel;

            // random
            if (step.param1 > 0) pos = random( (pos<step.param1)?pos:step.param1, (pos>step.param1)?pos+1:step.param1+1 ); 
            if (step.param2 > 0) speed = random( (speed<step.param2)?speed:step.param2, (speed>step.param2)?speed+1:step.param2+1 ); 
            if (step.param3 > 0) accel = random( (accel<step.param3)?accel:step.param3, (accel>step.param3)?accel+1:step.param3+1 ); 

            stepper.moveTo( pos * posFactor );
            if (step.speed > 0) stepper.setMaxSpeed( speed * speedFactor );
            if (step.accel > 0) stepper.setAcceleration( accel * accelFactor);

            LOGINL(" - goto position "); LOGINL( pos * posFactor   / MICROSTEP ); LOGINL(" / "); LOG( pos );
            LOGINL(" - speed "); LOGINL( speed * speedFactor       / MICROSTEP ); LOGINL(" / "); LOG( speed );
            LOGINL(" - accel "); LOGINL( accel * accelFactor       / MICROSTEP ); LOGINL(" / "); LOG( accel );
        }
    }

    // REPEAT
    else if (step.type == SEQ_REPEAT && repeatStep == 0) {
        LOGINL(" - repeat "); LOG( step.param1 );
        repeatStep = step.param1;
    }

    stepper_unlock();
}


/*
    PUBLIC
*/


void stepper_setup() {
    stepper_mutex = xSemaphoreCreateMutex();
    pinMode(TOP_PIN, INPUT);

    // STEPPER INIT
    stepper_ready = false;
    stepper.setMaxSpeed(reset_speed * speedFactor);
    stepper.setAcceleration(base_accel * accelFactor);
}


void stepper_loop() {
    
    // STEPPER must find Zero
    if (stepper_resetProcedure()) return;

    // STEPPER delay in progress
    if ((pauseUntil > 0) && (millis() < pauseUntil)) return;

    // STEPPER animate (sequencer)
    if (stepper_animate) {

        // In position: load next step
        if (stepper.distanceToGo() == 0) {

            if (repeatStep > 0) {
                LOG("SEQ repeat step ");
                stepper_execute( seq_beforeStep() );
                repeatStep -= 1;
            }
            else {
                LOG("SEQ next step ");
                stepper_execute( seq_nextStep() );
            }
        }
    }

    // STEPPER Run
    if (stepper_animate || stepper_manual) {
        stepper_lock();
        stepper.run();
        stepper_unlock();
    }

    // DEBUG
    if (stepper.distanceToGo() == 0 && startMove > 0) {
        LOGINL("Moved in "); LOG( (millis()-startMove) /1000.0);
        startMove = 0;
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

        // debug info
        debugGoingup = false;
        
        // Engage Trigger watch
        if (reset_timeClick == 0) {
            reset_timeClick = millis(); 
            LOG("Top switch triggered");
            return true;   // Pause
        }

        // It's reset point
        if (stepper_ready && ((millis()-reset_timeClick) > stepper_waitValid) ) {
            stepper_ready = false;
            LOG("Resetting...");
            return true;   // Pause
        }


        // Validate reset point
        if ((millis()-reset_timeClick) > (stepper_waitValid * 6) ) {
            LOG("Reset Validated");
            stepper_lock();
            stepper.setCurrentPosition(0);
            stepper.setMaxSpeed(reset_speed * speedFactor);
            stepper.moveTo( (long) (offset_position * posFactor) );
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
            stepper.setAcceleration(base_accel * accelFactor);
            stepper.setMaxSpeed(reset_speed * speedFactor);
            stepper.move(-1*reset_steps);
            stepper.runToPosition();
            
            if (!debugGoingup) {
                LOG("Going up to reset ^ ");
                debugGoingup = true;
            }
            stepper_unlock();
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
    stepper_execute( seq_initStep() );
    stepper_manual = true;
}

void stepper_reset() {
    LOG("Reset (manual) ");
    stepper_manual = true;
    stepper_animate = false;
    stepper_ready = false;
}

void stepper_goto(step_t step) {
    LOG("Go (manual) ");
    stepper_execute( step );
    stepper_animate = false;
    stepper_manual = true;
}