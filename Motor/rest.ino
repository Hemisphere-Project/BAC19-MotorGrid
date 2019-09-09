
#include <aREST.h>
#include "StringSplitter.h"

aREST rest = aREST();
WiFiServer restServer(3704);

// REST setup
void rest_setup() 
{
  rest.function("test",   testRequest);
  rest.function("play",   [](String param)->int { stepper_play(); });
  rest.function("stop",   [](String param)->int { stepper_stop(); });
  rest.function("pause",  [](String param)->int { stepper_pause(); });
  rest.function("reset",  [](String param)->int { stepper_reset(); });
  rest.function("go",     [](String param)->int { 
                                                    StringSplitter *splitter = new StringSplitter(param, ';', 4);
                                                    int to = (splitter->getItemCount() > 0)     ? splitter->getItemAtIndex(0).toInt() : 0;
                                                    int speed = (splitter->getItemCount() > 1)  ? splitter->getItemAtIndex(1).toInt() : 0;
                                                    int accel = (splitter->getItemCount() > 2)  ? splitter->getItemAtIndex(2).toInt() : 0;
                                                    if (to > 0) {
                                                      step_t step = {to, speed, accel};
                                                      stepper_go(step);
                                                    }

                                                });


}

void rest_begin()
{
    restServer.begin();
    LOG("REST server started");
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
  LOGINL("Received REST /test request: ");
  LOG(param);
}

