#define LED_PIN 2

unsigned long lastBlinkTime = 0;
bool ledState = false;

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  unsigned long now = millis();

  if (now - lastBlinkTime >= 1000) {
    lastBlinkTime = now;
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
  }
}
