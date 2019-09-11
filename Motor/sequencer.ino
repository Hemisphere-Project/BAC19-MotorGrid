#include <Preferences.h>
Preferences prefs;

#define MAXSTEPS  128

step_t steps[MAXSTEPS];
int current_step = -1;

void seq_setup() {
    prefs.begin("seq");

    // reset 
    if (CLEARMEM) seq_clear(true);

    size_t schLen = prefs.getBytesLength("seq");

    // Check validity
    if (schLen == 0 || schLen % sizeof(step_t)) {
        LOG("SEQ Data wrong size! resetting memory...");
        prefs.putBytes("seq", steps, MAXSTEPS*sizeof(step_t));
    }   
    else {
        prefs.getBytes("seq", steps, schLen);
        LOG("SEQ loaded");
        // for (int x=0; x<MAXSTEPS; x++) {
        //     LOGINL(steps[x].pos); LOGINL(" "); 
        //     LOGINL(steps[x].speed); LOGINL(" "); 
        //     LOG(steps[x].accel);
        // }
    }
    prefs.end();
}

void seq_save() {
    prefs.begin("seq");
    prefs.putBytes("seq", steps, MAXSTEPS*sizeof(step_t));
    prefs.end();
}

String seq_export() {
    String sequence = "";
    for (int x=0; x<MAXSTEPS; x++) {
        if (steps[x].type == SEQ_STOP) sequence += "stop; ";
        else if (steps[x].type == SEQ_GOTO) sequence += "goto; ";
        else if (steps[x].type == SEQ_WAIT) sequence += "wait; ";
        else if (steps[x].type == SEQ_REPEAT) sequence += "repeat; ";
        else sequence += String(steps[x].type);
        sequence += String(steps[x].pos)+"; "+String(steps[x].speed)+"; "+String(steps[x].accel)+"; ";
        sequence += String(steps[x].param1)+"; "+String(steps[x].param2)+"; "+String(steps[x].param3)+"\n";
    }
    return sequence;
}

void seq_clear(bool save) {
    LOG("SEQ: clear memory...");
    for (int x=0; x<MAXSTEPS; x++) steps[x] = {SEQ_STOP, 0, 0, 0, 0, 0, 0};
    if (save) seq_save();
}


void seq_setStep(int i, int type, int pos, int speed, int accel, int param1, int param2, int param3) {
    if (i >= MAXSTEPS) return;
    step_t step = {type, pos, speed, accel, param1, param2, param3};
    steps[i] = step;
}


step_t seq_getStep(int i) {
    if (i < 0 || i >= MAXSTEPS) return {SEQ_STOP, 0, 0, 0, 0, 0};
    return steps[i];
}

step_t seq_beforeStep() {
    return seq_getStep(current_step-1);
}

step_t seq_nextStep() {
    current_step += 1;
    return seq_getStep(current_step);
}

step_t seq_initStep() {
    current_step = 0;
    return seq_getStep(current_step);
}



