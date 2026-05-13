import serial
import pyautogui
import time

# ── Setup ──────────────────────────────────────
pyautogui.FAILSAFE = False  # prevents crash if mouse hits corner
ser = serial.Serial('/dev/cu.usbserial-0001', 115200, timeout=1)
time.sleep(2)
print("Connected! Listening...")

last_state = None
last_forward_time = 0
forward_cooldown = 0.8  # seconds between forward triggers

# ── Main Loop ──────────────────────────────────
while True:
    try:
        raw = ser.readline()
        if not raw:
            continue

        data = raw.decode('utf-8', errors='ignore').strip()
        if not data:
            continue

        print(f"Received: {data}")

        # ── PAUSE ────────────────────────────
        if data == "PAUSE" and last_state != "PAUSE":
            pyautogui.press("space")
            last_state = "PAUSE"
            print("Action: PAUSE")

        # ── PLAY ─────────────────────────────
        elif data == "PLAY" and last_state != "PLAY":
            pyautogui.press("space")
            last_state = "PLAY"
            print("Action: PLAY")

        # ── SCREENSHOT ───────────────────────
        elif data == "SHOT":
            pyautogui.hotkey("command", "shift", "3")
            print("Action: Screenshot taken")

        # ── FORWARD ──────────────────────────
        elif data == "FORWARD":
            now = time.time()
            if now - last_forward_time > forward_cooldown:
                pyautogui.press("right")
                last_forward_time = now
                print("Action: >> Forward")
            else:
                print("Action: FORWARD skipped (cooldown)")

    except serial.SerialException as e:
        print(f"Serial error: {e}")
        time.sleep(1)

    except UnicodeDecodeError:
        continue