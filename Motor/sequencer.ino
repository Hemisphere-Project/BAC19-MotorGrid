#include <Preferences.h>
Preferences prefs;

#define MAXSTEPS  128

step_t steps[MAXSTEPS];
int current_step = -1;

void seq_setup() {
    prefs.begin("sequence");
    size_t schLen = prefs.getBytesLength("sequence");

    // Check validity
    if (schLen == 0 || schLen % sizeof(step_t)) {
        LOG("SEQ Data wrong size! resetting memory...");
        prefs.putBytes("sequence", steps, MAXSTEPS*sizeof(step_t));
    }   
    else {
        char buffer[schLen];
        prefs.getBytes("sequence", buffer, schLen);
        LOG("SEQ loaded...");
        step_t *steps = (step_t *) buffer;
        for (int x=0; x<schLen; x++) LOGF("%02X ", steps[x]);
    }
    prefs.end();
}


void seq_save() {
    prefs.begin("sequence");
    prefs.putBytes("sequence", steps, MAXSTEPS*sizeof(step_t));
    prefs.end();
}


void seq_setStep(int i, uint32_t pos, uint32_t speed, uint32_t accel) {
    if (i >= MAXSTEPS) return;
    step_t step = {pos, speed, accel};
    steps[i] = step;
}


step_t seq_getStep(int i) {
    if (i < 0 || i >= MAXSTEPS) return {0, 0, 0};
    return steps[i];
}

step_t seq_nextStep() {
    current_step += 1;
    step_t next = seq_getStep(current_step);
    return next;
}

step_t seq_initStep() {
    current_step = -1;
    return seq_nextStep();
}

void seq_manualStep() {
    current_step = -1;
}


