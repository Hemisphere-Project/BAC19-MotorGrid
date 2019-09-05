#define LED_PIN     33
#define BTN_PIN     34

unsigned long TstartClick = 0;
unsigned long Tnow = 0;

bool buttonState;
bool resetting = false;
bool resetDone = false;
int resetDelay = 5000;

void setup() {

  pinMode(LED_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT);
  Serial.begin(115200);
}


void loop() {
  resetProcedure();
}

void resetProcedure(){

  Tnow = millis();
  buttonState = digitalRead(BTN_PIN);


  if((buttonState == LOW)&&(resetting==false)){
    TstartClick = Tnow;
    resetting = true;
    // MOTOR STOP
    Serial.println("MOTOR STOP");
  }
  if(buttonState == HIGH){
    resetting = false;
    // MOTOR MOVE UP
    Serial.println("MOTOR MOVE UP");
  }
  if((Tnow-TstartClick>resetDelay)&&(resetting==true)&&(resetDone==false)){
    resetting = false;
    resetDone = true;
    Serial.println("reset Validated");
    // trigOrigin();
    // MOTOR MOVE DOWN 10CM
    Serial.println("MOTOR MOVE DOWN 10CM");
  }
}
