#define NODEID 1
#define GROUPID 1

#define MG_VERSION 0.1  // Init & test


#include <ETH.h>
#include <Preferences.h>

Preferences preferences;
unsigned int nodeid = 0;
unsigned int groupid = 0;

unsigned long lastTest = 0;


void setup()
{
  Serial.begin(115200);

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

  ethernet_setup();  
  stepper_setup();
}

void loop()
{
  stepper_loop();

}
