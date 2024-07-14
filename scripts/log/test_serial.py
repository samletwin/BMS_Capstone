import serial

# Configure the serial port
serial_port = 'COM3'  # Change this to your serial port
baud_rate = 115200  # Change this to your baud rate

try:
    # Open the serial port
    ser = serial.Serial(serial_port, baud_rate)
    print(f"Connected to {serial_port} at {baud_rate} baud.")

    # Continuously read and print data from the serial port
    while True:
        line = ser.readline().decode('utf-8').strip()
        print(line)
except serial.SerialException as e:
    print(f"Error opening the serial port: {e}")
except KeyboardInterrupt:
    print("\nLogging stopped by user")
finally:
    if 'ser' in locals() and ser.is_open:
        ser.close()
        print("Serial port closed.")
