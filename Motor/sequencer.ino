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
        if (steps[x].type == SEQ_STOP) break; //sequence += "stop; ";
        else if (steps[x].type == SEQ_GOTO) sequence += "goto; ";
        else if (steps[x].type == SEQ_WAIT) sequence += "wait; ";
        else if (steps[x].type == SEQ_REPEAT) sequence += "repeat; ";
        else sequence += String(steps[x].type);
        
        sequence += String(steps[x].pos)+"; "+String(steps[x].speed)+"; "+String(steps[x].accel)+"; ";
        sequence += String(steps[x].param1)+"; "+String(steps[x].param2)+"; "+String(steps[x].param3)+"\n";

        // if (steps[x].type == SEQ_STOP) break;
    }
    return sequence;
}

bool seq_import(String sequence) {
    int lineCount = 0;
    int seqCount = 0;
    String line = "";
    String type = "";
    seq_clear(false);

    // split lines
    while(true) {
        line = getValueSplit(sequence, '\n', lineCount);
        if (line == "") break;
        
        // line type 
        type = getValueSplit(line, ';', 0);
        type.toLowerCase();
        if (type == "stop") {
            seq_setStep(seqCount, SEQ_STOP, 0, 0, 0, 0, 0, 0);
            seqCount+=1;
        }
        else if (type == "goto") {
            int pos = getValueSplit(line, ';', 1).toInt();
            int spe = getValueSplit(line, ';', 2).toInt();
            int acc = getValueSplit(line, ';', 3).toInt();
            int p1 = getValueSplit(line, ';', 4).toInt();
            int p2 = getValueSplit(line, ';', 5).toInt();
            int p3 = getValueSplit(line, ';', 6).toInt();
            if (pos > 0) {
                seq_setStep(seqCount, SEQ_GOTO, pos, spe, acc, p1, p2, p3);
                seqCount+=1;
            }
        }
        else if (type == "wait") {
            int p1 = getValueSplit(line, ';', 4).toInt();
            if (p1 > 0) {
                seq_setStep(seqCount, SEQ_WAIT, 0, 0, 0, p1, 0, 0);
                seqCount+=1;
            }
        }
        else if (type == "repeat") {
            int p1 = getValueSplit(line, ';', 4).toInt();
            if (p1 > 0) {
                seq_setStep(seqCount, SEQ_REPEAT, 0, 0, 0, p1, 0, 0);
                seqCount+=1;
            }
        }
        else {
            LOGINL("Unknown type: ");
            LOG(line);
        }

        // next line
        lineCount +=1;
    }
    seq_save();
    return true;
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

int seq_step() {
    return current_step;
}


/// UTILS

String getValueSplit(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  String ret = found>index ? data.substring(strIndex[0], strIndex[1]) : "";
  ret.trim();
  return ret;
}


