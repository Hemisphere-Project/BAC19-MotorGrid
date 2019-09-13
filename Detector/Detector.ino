// #define GROUPID 7 // 1 TO 7
// #define NODEID 254

#define DETECTORS_VERSION 0.1  // Init

#define BTN_PIN     34
#define LED_PIN     33
#define DETECT_PIN  36

#include <ETH.h>
#include <HTTPClient.h>
#include <Preferences.h>

// PREFS
Preferences preferences;
unsigned int nodeid = 0;
unsigned int groupid = 0;

// UTILS
bool detectState = false;
bool starting = true;
bool startingBtn = true;
unsigned long Tnow = 0;
unsigned long Tlast = 0;
unsigned long TlastBtn = 0;

// TIMING
unsigned long timeLineDuration = 10000;
unsigned long pauseDuration = 2000;

void setup() {

  // PINS
  pinMode(LED_PIN, OUTPUT);
  pinMode(DETECT_PIN, INPUT_PULLUP);
  pinMode(BTN_PIN, INPUT);

  // SERIAL
  Serial.begin(115200);

  // GROUP and ID
  preferences.begin("Detectors", false);
  #ifdef NODEID
    preferences.putUInt("nodeid", NODEID);
  #endif
  #ifdef GROUPID
    preferences.putUInt("groupid", GROUPID);
  #endif
  nodeid = preferences.getUInt("nodeid", 254);
  groupid = preferences.getUInt("groupid", 254);
  preferences.end();

  // ETHERNET
  ethernet_setup();

}

void loop() {

  Tnow = millis();
  detectState = digitalRead(DETECT_PIN);

  if( ((Tnow-Tlast>timeLineDuration+pauseDuration)||(starting==true)) && (detectState==HIGH) ){
    Tlast = Tnow;
    starting = false;
    callStart();
  }

  if( ((Tnow-TlastBtn>timeLineDuration+pauseDuration)||(startingBtn==true)) && (digitalRead(BTN_PIN)==LOW)){
    Serial.println("CALL START w BTN");
    TlastBtn = Tnow;
    startingBtn = false;
    callStart();
  }

}

void callStart(){

  Serial.println("SOMEONE'S HERE");
  digitalWrite(LED_PIN, HIGH);
  delay(100);
  digitalWrite(LED_PIN, LOW);

  if(!eth_isConnected){
    return;
  }

  HTTPClient http;

  for (int i = 1; i < 8; i++) {
    String playUrl = "http://10.0."+String(groupid)+"."+String(i)+"/play";
    http.begin(playUrl);
    http.setConnectTimeout(100);
    int httpCode = http.GET();
    if(httpCode == HTTP_CODE_OK) { Serial.println("ok"); }
    else { Serial.println("connection pb"); }
    http.end();
  }


}
