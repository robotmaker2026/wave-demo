from machine import Pin, time_pulse_us
import time

TRIG_PIN = 5
ECHO_PIN = 18
LED_PIN = 2

DETECT_DISTANCE_CM = 30
COOLDOWN_MS = 1000

cooldown_start = 0
cooling = False

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

trig = Pin(TRIG_PIN, Pin.OUT)
echo = Pin(ECHO_PIN, Pin.IN)
led = Pin(LED_PIN, Pin.OUT)
print("=== Distance Blink Ready ===")
print("Trigger threshold:", DETECT_DISTANCE_CM, "cm")



# ── Main Loop ──────────────────────────────────────────────────
while True:
    now = time.ticks_ms()

    if cooling:
        if time.ticks_diff(now, cooldown_start) >= COOLDOWN_MS:
            cooling = False
        continue

    dist = read_distance_cm()

    if dist < 0:
        print("Distance: out of range")
    elif dist < DETECT_DISTANCE_CM:
        print(f"Distance: {dist} cm -- UNDER {DETECT_DISTANCE_CM}cm threshold! Blinking")
        led.on()
        time.sleep_ms(200)
        led.off()
        cooldown_start = time.ticks_ms()
        cooling = True
    else:
        print(f"Distance: {dist} cm -- clear")

    time.sleep_ms(100)
