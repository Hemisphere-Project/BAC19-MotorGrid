#define LED_PIN     33
#define DETECT_PIN  36

bool buttonState = false;


void setup() {

  pinMode(LED_PIN, OUTPUT);
  pinMode(DETECT_PIN, INPUT);
  Serial.begin(115200);
}


void loop() {

  buttonState = digitalRead(DETECT_PIN);

  if((buttonState == HIGH)&&(true)){
    Serial.println("SOMEONE'S HERE");
    digitalWrite(LED_PIN, HIGH);
  }
  if((buttonState == LOW)&&(true)){
    Serial.println("PARTI");
    digitalWrite(LED_PIN, LOW);
  }

}
