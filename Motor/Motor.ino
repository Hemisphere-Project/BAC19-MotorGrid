#define NODEID 3
#define GROUPID 1

#define CLEARMEM 0

#define MG_VERSION 0.2  // Init & test

// STEPPER FACTOR  16 x 200 / 0.3
float sfactor = 1000.0;

#include <ETH.h>
#include <Preferences.h>
#include "debug.h"

Preferences preferences;
unsigned int nodeid = 0;
unsigned int groupid = 0;

unsigned long lastTest = 0;

typedef struct {
  uint16_t pos;
  uint16_t speed;
  uint16_t accel;
} step_t;


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

  // seq_setStep(0, 5, 10, 100);
  // seq_setStep(1, 100, 200, 100);
  // seq_save();
  // seq_setStep(2, 10, 10, 100);
}

void loop()
{
  stepper_loop();
}
