from machine import Pin, time_pulse_us
from servo import Servo
import time

# ── Pin Configuration ──────────────────────────────────────────
TRIG_PIN = 5      # HC-SR04 trigger
ECHO_PIN = 18     # HC-SR04 echo
SERVO_PIN = 13    # SG90 signal wire
LED_PIN = 2       # Onboard LED

# ── Settings ───────────────────────────────────────────────────
DETECT_DISTANCE_CM = 0     # Trigger if object is within this range
SERVO_CENTER = 90          # Neutral/resting position
SERVO_RIGHT = 180          # +90° sweep position
SWEEP_DELAY_MS = 600       # Time to wait after each servo move
COOLDOWN_MS = 0            # Cooldown after trigger
BLINK_INTERVAL_MS = 1000   # Blink + sweep every 1 second

# ── Hardware Setup ─────────────────────────────────────────────
trig = Pin(TRIG_PIN, Pin.OUT)
echo = Pin(ECHO_PIN, Pin.IN)
led = Pin(LED_PIN, Pin.OUT)
servo = Servo(SERVO_PIN)


cooldown_start = 0
last_blink_time = 0
cooling = False
led_state = False

# ── Servo Sweep ────────────────────────────────────────────────
def sweep_servo():
    print(">> Sweeping servo...")
    servo.move(SERVO_RIGHT)
    time.sleep_ms(SWEEP_DELAY_MS)
    servo.move(SERVO_CENTER)
    time.sleep_ms(SWEEP_DELAY_MS)

# ── Sensor Reading ─────────────────────────────────────────────
def read_distance_cm():
    trig.off()
    time.sleep_us(2)
    trig.on()
    time.sleep_us(10)
    trig.off()

    try:
        duration = time_pulse_us(echo, 1, 26000)
    except OSError:
        return -1

    if duration <= 0:
        return -1

    return (duration * 0.0343) / 2

# ── Initialization ─────────────────────────────────────────────
servo.move(SERVO_CENTER)
time.sleep_ms(500)

print("=== Motion Servo System Ready ===")
print("Trigger distance:", DETECT_DISTANCE_CM, "cm")
print("Watching...")


# ── Main Loop ──────────────────────────────────────────────────
while True:
    now = time.ticks_ms()

    # Blink LED and sweep servo every 1 second
    if time.ticks_diff(now, last_blink_time) >= BLINK_INTERVAL_MS:
        last_blink_time = now
        led_state = not led_state
        led.value(led_state)
        if led_state:
            sweep_servo()

    # Wait out the cooldown before scanning again
    if cooling:
        if time.ticks_diff(now, cooldown_start) >= COOLDOWN_MS:
            cooling = False
            print(">> Cooldown done. Watching again...")
        continue

    dist = read_distance_cm()
    print("Distance:", dist, "cm")

    if dist > 0 and dist < DETECT_DISTANCE_CM:
        sweep_servo()
        cooldown_start = time.ticks_ms()
        cooling = True
        print(">> Cooldown started. Triggered at:", dist, "cm")

    time.sleep_ms(100)
