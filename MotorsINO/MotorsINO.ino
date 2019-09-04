#define NODEID 1
#define GROUPID 1

#define MG_VERSION 0.1  // Init & test

#include <ETH.h>
#include <ArduinoOTA.h>
#include <Preferences.h>
#include <AccelStepper.h>
#include <aREST.h>
#include <ArduinoOSC.h>

Preferences preferences;
unsigned int nodeid = 0;
unsigned int groupid = 0;

static bool eth_connected = false;
unsigned long lastTest = 0;

aREST rest = aREST();
WiFiServer server(3704);

OscWiFi osc;
int oscPort = 3737;

int stepper_minposition = 100;
int stepper_maxposition = 300;
int stepper_destination = stepper_minposition;
bool stepper_bouncing = false;
const int stepperPin_enable = 10;  // Pin ENA+
AccelStepper stepper(1, 8, 9);      // Type (TB6600: 1) , Pin DIR+ , Pin PUL+


void WiFiEvent(WiFiEvent_t event)
{
  switch (event) {
    case SYSTEM_EVENT_ETH_START:
      Serial.println("ETH Started");
      //set eth hostname here
      ETH.setHostname("esp32-ethernet");
      break;
    case SYSTEM_EVENT_ETH_CONNECTED:
      Serial.println("ETH Connected");
      break;
    case SYSTEM_EVENT_ETH_GOT_IP:
      Serial.print("ETH MAC: ");
      Serial.print(ETH.macAddress());
      Serial.print(", IPv4: ");
      Serial.print(ETH.localIP());
      if (ETH.fullDuplex()) {
        Serial.print(", FULL_DUPLEX");
      }
      Serial.print(", ");
      Serial.print(ETH.linkSpeed());
      Serial.println("Mbps");
      eth_connected = true;

      ArduinoOTA.begin();
      Serial.println("ArduinoOTA started");

      server.begin();
      Serial.println("REST server started");

      digitalWrite(LED_BUILTIN, HIGH);

      break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
      Serial.println("ETH Disconnected");
      eth_connected = false;
      break;
    case SYSTEM_EVENT_ETH_STOP:
      Serial.println("ETH Stopped");
      eth_connected = false;
      break;
    default:
      break;
  }
}

// aREST '/test' &param=
int testRequest(String param) {
  Serial.print("Received REST /test request: ");
  Serial.println(param);
}

// aREST '/bounce' &param=[0|1] 
int bounceRequest(String param) {
  if (param == "0") {
    Serial.println("Stop Bounce ");
    stepper_bouncing = false;
    stepper.stop();
    stepper.runToPosition();
  }
  else {
    Serial.println("Start Bounce ");
    stepper_bouncing = true;
    stepper.moveTo(stepper_destination);
  }
}

void trigOrigin() {
  stepper.stop();
  stepper.runToPosition();
  stepper.setCurrentPosition(0);
}

void setup()
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);

  // STEPPER INIT
  pinMode(stepperPin_enable, OUTPUT);
  digitalWrite(stepperPin_enable, LOW);
  stepper.setCurrentPosition(0);
  stepper.moveTo(0);
  stepper.setMaxSpeed(100);
  stepper.setAcceleration(20);
  
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

  // STATIC IP
  IPAddress localIP(10, 0, groupid, nodeid);
  IPAddress gateway(10, 0, 0, 1);
  IPAddress subnet(255, 255, 0, 0);
  IPAddress dns1(8, 8, 8, 8);
  IPAddress dns2(8, 8, 4, 4);

  // ETHERNET CONNECT
  WiFi.onEvent(WiFiEvent);
  ETH.begin();
  ETH.config(localIP, gateway, subnet, dns1, dns2);

  // OTA SETUP
  String devicename = "motor-" + String(groupid)+"."+String(nodeid) + "-v" + String(MG_VERSION, 2);
  ArduinoOTA.setHostname(devicename.c_str());

  // REST setup
  rest.function("test",testRequest);
  rest.function("bounce",bounceRequest);

  // OSC setup
  osc.begin(oscPort);
  osc.subscribe("/test", [](OscMessage& m)
  {
      // do something with osc message
      Serial.print("OSC /test ");
      Serial.print(m.arg<int>(0));    Serial.print(" ");
      Serial.print(m.arg<float>(1));  Serial.println(" ");
  });
  
  
}

void loop()
{
  if (eth_connected) {

    // dO SOMETHING
    // if ((lastTest == 0 || millis()-lastTest>5000)) {

    //   lastTest = millis();
    // }

    // OTA handle
    ArduinoOTA.handle();

    // OSC handle
    osc.parse();

    // REST handle
    WiFiClient client = server.available();
    if (client) {
      int timeout = 0;
      while(!client.available() && timeout < 200) {
        delay(5);
        timeout += 1;
      }
      if (client.available()) rest.handle(client);
    }

    // STEPPER bounce
    if (stepper_bouncing) {
      if (stepper.distanceToGo() == 0) {
        if (stepper_destination == stepper_minposition) stepper_destination = stepper_maxposition;
        else stepper_destination = stepper_minposition;
        stepper.moveTo(stepper_destination);
        Serial.println("bouncing...");
      }
    }

    // STEPPER run
    stepper.run();

  }
  else 
  delay(10);
}
