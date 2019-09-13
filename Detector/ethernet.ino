#include <ETH.h>
#include <ArduinoOTA.h>

static bool eth_connected = false;

void ethernet_setup() {

    // STATIC IP
    IPAddress localIP(10, 0, groupid, nodeid);
    IPAddress gateway(10, 0, 0, 1);
    IPAddress subnet(255, 255, 0, 0);
    IPAddress dns1(8, 8, 8, 8);
    IPAddress dns2(8, 8, 4, 4);

    // OTA SETUP
    String devicename = "detector-" + String(groupid)+"."+String(nodeid) + "-v" + String(DETECTORS_VERSION, 2);
    ArduinoOTA.setHostname(devicename.c_str());

    // ETHERNET CONNECT
    WiFi.onEvent(WiFiEvent);
    ETH.begin();
    ETH.config(localIP, gateway, subnet, dns1, dns2);

    webserver_setup();

    xTaskCreatePinnedToCore(
      ethernet_task, /* Function to implement the task */
      "networkTask", /* Name of the task */
      15000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      1,  /* Priority of the task */
      NULL,  /* Task handle. */
      0); /* Core where the task should run */
}

void ethernet_task( void * parameter) {

    while(true) {
        if (eth_connected) {
            ArduinoOTA.handle();
            webserver_loop();
            delay(1);
        }
        else delay(100);
    }

    vTaskDelete(NULL);
}

void ethernet_connected() {

    ArduinoOTA.begin();
    Serial.println("ArduinoOTA started");
}


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
      if (eth_connected) return;
      eth_connected = true;

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

      ethernet_connected();
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

bool eth_isConnected(){
 return eth_connected;
}
