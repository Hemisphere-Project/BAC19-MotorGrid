
#include <aREST.h>

aREST rest = aREST();
WiFiServer restServer(3704);

// REST setup
void rest_setup() 
{
  rest.function("test",testRequest);
  rest.function("bounce",rest_bounceRequest);
}

void rest_begin()
{
    restServer.begin();
    Serial.println("REST server started");
}

void rest_loop() 
{
    WiFiClient client = restServer.available();
    if (client) {
        int timeout = 0;
        while(!client.available() && timeout < 200) {
            delay(5);
            timeout += 1;
        }
        if (client.available()) rest.handle(client);
    }
}

/*
    REQUESTS
*/

// '/test' &param=
int testRequest(String param) {
  Serial.print("Received REST /test request: ");
  Serial.println(param);
}

// '/bounce' &param=[0|1] 
int rest_bounceRequest(String param) {
 
  if (param == "0") stepper_stopBounce();
  else stepper_startBounce();

}