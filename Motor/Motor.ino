#define NODEID 3
#define GROUPID 0

#define CLEARMEM 0
#define MICROSTEP 4

#define MG_VERSION 0.2  // Init & test
#define MG_VERSION 0.3  // Calibration

// MAX SETTINGS
long  MAX_STEPPOS   = 25000;
float MAX_STEPSPEED = 4500.0;
float MAX_STEPACCEL = MAX_STEPSPEED;

// FACTOR CALCULATION (100%)
long  posFactor   = MAX_STEPPOS * MICROSTEP / 1000;             // 0 -> 1000 for 10m
float speedFactor = MAX_STEPSPEED * MICROSTEP / 256;            // 0 -> 256  for MAXSPEED
float accelFactor = MAX_STEPACCEL * MICROSTEP / 256;            // 0 -> 256  for MAXSPEED


#include <ETH.h>
#include <Preferences.h>
#include "debug.h"

Preferences preferences;
unsigned int nodeid = 0;
unsigned int groupid = 0;

unsigned long lastTest = 0;


// SEQUENCER TYPE
#define SEQ_STOP  0
#define SEQ_GOTO  1
#define SEQ_WAIT  2

typedef struct {
  int type;
  int pos;
  int speed;
  int accel;
  int param1;
  int param2;
  int param3;
} step_t;


// PROGRAM

void setup()
{
  LOGSETUP();

  // BTN
  pinMode(34, INPUT);
  
  // GROUP and ID
  preferences.begin("MotorGrid", false);
  #ifdef NODEID
    preferences.putUInt("nodeid", NODEID);
  #endif
  #ifdef GROUPID
    preferences.putUInt("groupid", GROUPID);
  #endif
  nodeid = preferences.getUInt("nodeid", 254);
  groupid = preferences.getUInt("groupid", 254);
  preferences.end();

  seq_setup();
  ethernet_setup();  
  stepper_setup();

  seq_clear(false);
  for (int x=0; x<20; x++) 
    seq_setStep(x, SEQ_GOTO, 5, 50, 100, 12, 256, 5000 );

  seq_save();

}

void loop()
{

  stepper_loop();
}
