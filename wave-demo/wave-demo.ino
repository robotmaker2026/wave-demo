#include <ESP32Servo.h>

// ── Pin Configuration ──────────────────────────────────────────
#define TRIG_PIN     5    // HC-SR04 trigger
#define ECHO_PIN     18   // HC-SR04 echo
#define SERVO_PIN    13   // SG90 signal wire
#define LED_PIN      2    // Onboard LED

// ── Settings ───────────────────────────────────────────────────
#define DETECT_DISTANCE_CM  0       // Trigger if object is within this range
#define SERVO_CENTER        90      // Neutral/resting position
#define SERVO_RIGHT         180     // +90° sweep position
#define SWEEP_DELAY_MS      600     // Time to wait after each servo move
#define COOLDOWN_MS         0       // Cooldown after trigger
#define BLINK_INTERVAL_MS   1000    // Blink + sweep every 1 second

Servo servo;
unsigned long cooldownStart = 0;
unsigned long lastBlinkTime = 0;
bool cooling = false;
bool ledState = false;

// ── Sensor Reading ─────────────────────────────────────────────
long readDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 26000); // ~4.5m max timeout
  if (duration == 0) return -1;                   // Nothing detected / out of range
  return (duration * 0.0343) / 2;
}

// ── Servo Sweep ────────────────────────────────────────────────
void sweepServo() {
  Serial.println(">> Sweeping servo...");
  servo.write(SERVO_RIGHT);           // Swing 90° to the right
  delay(SWEEP_DELAY_MS);
  servo.write(SERVO_CENTER);          // Return 90° back to center
  delay(SWEEP_DELAY_MS);
}

// ── Setup ──────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  ESP32PWM::allocateTimer(0);
  servo.setPeriodHertz(50);
  servo.attach(SERVO_PIN, 500, 2400);
  servo.write(SERVO_CENTER);          // Start at neutral position
  delay(500);

  Serial.println("=== Motion Servo System Ready ===");
  Serial.printf("Trigger distance: %d cm\n", DETECT_DISTANCE_CM);
  Serial.println("Watching...");
}

// ── Main Loop ──────────────────────────────────────────────────
void loop() {
  unsigned long now = millis();

  // Blink LED and sweep servo every 1 second
  if (now - lastBlinkTime >= BLINK_INTERVAL_MS) {
    lastBlinkTime = now;
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
    if (ledState) sweepServo();       // Sweep on LED-on, skip on LED-off
  }

  // Wait out the cooldown before scanning again
  if (cooling) {
    if (now - cooldownStart >= COOLDOWN_MS) {
      cooling = false;
      Serial.println(">> Cooldown done. Watching again...");
    }
    return;
  }

  long dist = readDistanceCm();
  Serial.printf("Distance: %ld cm\n", dist);

  if (dist > 0 && dist < DETECT_DISTANCE_CM) {
    sweepServo();
    cooldownStart = millis();
    cooling = true;
    Serial.printf(">> 10s cooldown started. Triggered at: %ld cm\n", dist);
  }

  delay(100); // Poll sensor every 100ms
}
