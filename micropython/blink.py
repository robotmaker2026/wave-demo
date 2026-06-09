from machine import Pin
import time

LED_PIN = 2

led = Pin(LED_PIN, Pin.OUT)

last_blink_time = 0
led_state = False

while True:
    now = time.ticks_ms()

    if time.ticks_diff(now, last_blink_time) >= 1000:
        last_blink_time = now
        led_state = not led_state
        led.value(led_state)
