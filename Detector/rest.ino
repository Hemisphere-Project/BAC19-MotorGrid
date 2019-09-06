
#include <aREST.h>

aREST rest = aREST();
WiFiServer restServer(3704);

// REST setup
void rest_setup() 
{
  rest.function("test",testRequest);
  rest.function("animate",rest_animateRequest);
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

// '/animate' &param=[0|1] 
int rest_animateRequest(String param) {
    Serial.print("/animate.. nothing to do yet ");
}

