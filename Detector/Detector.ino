// #define GROUPID 100 // Fixe
// #define NODEID 7  // PIR 1 to 7

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

// MAPPING Detector -> Motor Group
int mapping[ 7 ][ 2 ] = { { 2,3 }, { 1,0 }, { 0,0 }, { 7,0 }, { 5,6 }, { 0,0 }, { 4,0 } };

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
    Serial.println("Trigger Action");
    triggerAction();
  }

  if( ((Tnow-TlastBtn>timeLineDuration+pauseDuration)||(startingBtn==true)) && (digitalRead(BTN_PIN)==LOW)){
    TlastBtn = Tnow;
    startingBtn = false;
    Serial.println("Trigger Action w BTN");
    triggerAction();
  }

}

void triggerAction(){

  digitalWrite(LED_PIN, HIGH); delay(100); digitalWrite(LED_PIN, LOW);

  // MAPPING : PIR nodeid -> Motor groupid
  // if(nodeid!=2){
  //   for (int j = 0; j < 2; j++) { triggerGroup(mapping[nodeid-1][j]); }
  // }

  // PIR 1
  if(nodeid==1){
    for (int j = 0; j < 2; j++) { triggerGroup(mapping[nodeid-1][j], 0); }
    delay(5000);
    for (int j = 0; j < 2; j++) { triggerGroup(mapping[nodeid-1][j], 1); }
    delay(5000);
    for (int j = 0; j < 2; j++) { triggerGroup(mapping[nodeid-1][j], 2); }
    delay(5000);

    for (int i = 1; i < 8; i++) { triggerGroup(i,0); }

    delay(10000);
    for (int i = 1; i < 8; i++) { stopGroup(i); }
  }

  // PIR 2
  if(nodeid==2){
    triggerGroup(10,0);
    delay(10000);
    triggerGroup(1,0);
    delay(2000);
    triggerGroup(10,1);
    delay(2000);
    triggerGroup(1,1);
    delay(2000);
    triggerGroup(10,2);
    delay(2000);
    triggerGroup(1,2);

    delay(2000);
    for (int i = 1; i < 8; i++) { triggerGroup(i,4); }
    delay(2000);
    for (int i = 1; i < 8; i++) { triggerGroup(i,5); }
    delay(2000);
    for (int i = 1; i < 8; i++) { triggerGroup(i,6); }

    delay(10000);
    for (int i = 1; i < 8; i++) { stopGroup(i); }

  }

  // PIR 4
  if(nodeid==4){

  }
  // PIR 5
  if(nodeid==5){

  }
  // PIR 7
  if(nodeid==7){

  }

}

void triggerGroup(int grp, int seq){

  if(grp==0) return;
  if(!eth_isConnected) return;

  HTTPClient http;

  for (int i = 1; i < 8; i++) {
    // GET
    String playUrl = "http://10.0."+String(grp)+"."+String(i)+"/play?seq="+seq;
    http.begin(playUrl);
    http.setConnectTimeout(100);
    int httpCode = http.GET();
    // LOG
    if(httpCode == HTTP_CODE_OK) { Serial.println("ok"); }
    else { Serial.println("connection pb"); }
    // END
    http.end();

  }

}

void triggerMyGroup(){
  if(!eth_isConnected) return;
  HTTPClient http;
  for (int i = 1; i < 8; i++) {
    String playUrl = "http://10.0."+String(nodeid)+"."+String(i)+"/play";
    http.begin(playUrl);
    http.setConnectTimeout(100);
    http.GET();
    http.end();
  }
}


void stopGroup(int grp){
  if(!eth_isConnected) return;
  HTTPClient http;
  for (int i = 1; i < 8; i++) {
    String stopUrl = "http://10.0."+String(grp)+"."+String(i)+"/stop";
    http.begin(stopUrl);
    http.setConnectTimeout(100);
    http.GET();
    http.end();
  }
}

void stopMyGroup(){
  if(!eth_isConnected) return;
  HTTPClient http;
  for (int i = 1; i < 8; i++) {
    String stopUrl = "http://10.0."+String(nodeid)+"."+String(i)+"/stop";
    http.begin(stopUrl);
    http.setConnectTimeout(100);
    http.GET();
    http.end();
  }
}
void resetMyGroup(){
  if(!eth_isConnected) return;
  HTTPClient http;
  for (int i = 1; i < 8; i++) {
    String resetUrl = "http://10.0."+String(nodeid)+"."+String(i)+"/reset";
    http.begin(resetUrl);
    http.setConnectTimeout(100);
    http.GET();
    http.end();
  }
}
