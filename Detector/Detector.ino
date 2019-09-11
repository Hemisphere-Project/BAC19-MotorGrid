#define LED_PIN     33
#define DETECT_PIN  36

bool detectState = false;
bool starting = true;

unsigned long Tnow = 0;
unsigned long Tlast = 0;
unsigned long timeLineDuration = 20000;
unsigned long pauseDuration = 10000;

void setup() {

  pinMode(LED_PIN, OUTPUT);
  pinMode(DETECT_PIN, INPUT_PULLUP);
  Serial.begin(115200);

}

void loop() {

  Tnow = millis();
  detectState = digitalRead(DETECT_PIN);


  if( ((Tnow-Tlast>timeLineDuration+pauseDuration)||(starting==true)) && (detectState==HIGH) ){
    Tlast = Tnow;
    starting = false;
    callStart();
  }

}

void callStart(){

  Serial.println("SOMEONE'S HERE");
  digitalWrite(LED_PIN, HIGH);
  delay(100);
  digitalWrite(LED_PIN, LOW);
}
