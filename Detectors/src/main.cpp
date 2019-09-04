#define NODEID 1
#define GROUPID 1

#define MG_VERSION 0.1  // Init & test

#include <Arduino.h>
#include <ETH.h>
#include <ArduinoOTA.h>
#include <Preferences.h>
#include <HTTPClient.h>
#include <ArduinoOSC.h>


Preferences preferences;
unsigned int nodeid = 0;
unsigned int groupid = 0;

static bool eth_connected = false;
unsigned long lastTest = 0;
String broadcastIP;

OscWiFi osc;
int oscPort = 3737;

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

void testClient(const char * host, uint16_t port)
{
  Serial.print("\nconnecting to ");
  Serial.println(host);

  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    return;
  }
  client.printf("GET / HTTP/1.1\r\nHost: %s\r\n\r\n", host);
  while (client.connected() && !client.available());
  while (client.available()) {
    Serial.write(client.read());
  }

  Serial.println("closing connection\n");
  client.stop();
}

bool testRequest(String url) {
  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();                                        

  if (httpCode > 0) { 
      String payload = http.getString();
      Serial.println(httpCode);
      Serial.println(payload);
  }
  else {
    Serial.println("Error on HTTP request");
  }
  http.end();
  return (httpCode > 0);
}

void setup()
{
  Serial.begin(115200);
  
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
  broadcastIP = "10.0.255.255";

  // ETHERNET CONNECT
  WiFi.onEvent(WiFiEvent);
  ETH.config(localIP, gateway, subnet);
  ETH.begin();

  // OTA setup
  String devicename = "detector-" + String(groupid)+"."+String(nodeid) + "-v" + String(MG_VERSION, 2);
  ArduinoOTA.setHostname(devicename.c_str());
  
}

void loop()
{
  if (eth_connected) {

    // Test internet link
    if ((lastTest == 0 || millis()-lastTest>10000)) {
      testClient("google.com", 80);
      testRequest("http://10.0.1.1:3704/test");
      osc.send(broadcastIP, oscPort, "/lambda", 1, 3.3, "string");
      lastTest = millis();
    }

    // OTA handle
    ArduinoOTA.handle();
  }
  delay(1);
}

