#define TRIG_PIN            5
#define ECHO_PIN            18
#define LED_PIN             2

#define DETECT_DISTANCE_CM  30
#define COOLDOWN_MS         1000

unsigned long cooldownStart = 0;
bool cooling = false;

long readDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 26000);
  if (duration == 0) return -1;
  return (duration * 0.0343) / 2;
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  Serial.println("=== Distance Blink Ready ===");
  Serial.printf("Trigger threshold: %d cm\n", DETECT_DISTANCE_CM);
}

void loop() {
  unsigned long now = millis();

  if (cooling) {
    if (now - cooldownStart >= COOLDOWN_MS) {
      cooling = false;
    }
    return;
  }

  long dist = readDistanceCm();

  if (dist < 0) {
    Serial.println("Distance: out of range");
  } else if (dist < DETECT_DISTANCE_CM) {
    Serial.printf("Distance: %ld cm -- UNDER %dcm threshold! Blinking\n", dist, DETECT_DISTANCE_CM);
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    cooldownStart = millis();
    cooling = true;
  } else {
    Serial.printf("Distance: %ld cm -- clear\n", dist);
  }

  delay(100);
}
