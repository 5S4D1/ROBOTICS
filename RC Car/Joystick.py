# laptop_controller.py
import pygame
import serial
import struct
import time
from datetime import datetime

# Initialize Pygame and joystick
pygame.init()
pygame.joystick.init()

# Check if any joystick/game controller is connected
if pygame.joystick.get_count() == 0:
    print("No joystick detected")
    exit()

print(f"[{datetime.now().strftime('%H:%M:%S')}] Found {pygame.joystick.get_count()} joystick(s)")

# Initialize the first joystick
joystick = pygame.joystick.Joystick(0)
joystick.init()
print(f"[{datetime.now().strftime('%H:%M:%S')}] Initialized joystick: {joystick.get_name()}")

# Initialize serial connection to ESP32
SERIAL_PORT = 'COM9'  # Change this to match your ESP32's port
BAUD_RATE = 115200

try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    print(f"[{datetime.now().strftime('%H:%M:%S')}] Successfully connected to {SERIAL_PORT}")
    time.sleep(2)  # Wait for connection to establish
except serial.SerialException as e:
    print(f"Error opening serial port {SERIAL_PORT}: {e}")
    pygame.quit()
    exit()

def map_value(x, in_min, in_max, out_min, out_max):
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min

print("\nController Instructions:")
print("- Use left stick for control")
print("- Press Ctrl+C to exit")
print("\nStarting control loop...")

# Variables for debug printing
last_print_time = time.time()
PRINT_INTERVAL = 0.5  # Print every 0.5 seconds

try:
    while True:
        pygame.event.pump()
        
        # Get joystick values
        throttle = -joystick.get_axis(1)  # Y-axis for forward/backward
        steering = joystick.get_axis(0)   # X-axis for left/right
        
        # Map joystick values (-1 to 1) to motor values (0 to 255)
        throttle_value = int(map_value(throttle, -1, 1, -255, 255))
        steering_value = int(map_value(steering, -1, 1, -255, 255))
        
        # Pack data into bytes
        data = struct.pack('!2h', throttle_value, steering_value)
        ser.write(data)
        
        # Print debug info every PRINT_INTERVAL seconds
        current_time = time.time()
        if current_time - last_print_time >= PRINT_INTERVAL:
            print(f"[{datetime.now().strftime('%H:%M:%S')}] Throttle: {throttle_value:4d}, Steering: {steering_value:4d}")
            last_print_time = current_time
        
        time.sleep(0.05)  # Small delay to prevent overwhelming the ESP32

except KeyboardInterrupt:
    print("\nExiting...")
except Exception as e:
    print(f"\nAn error occurred: {e}")
finally:
    ser.close()
    pygame.quit()
    print("Cleanup complete")