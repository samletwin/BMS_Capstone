import serial
import csv
import re
import time
from datetime import datetime
import os
import signal

# Configure the serial port
serial_port = 'COM3'  # Change this to your serial port
baud_rate = 115200  # Change this to your baud rate

# Regular expression to match the expected format
pattern = re.compile(r'Battery_Voltage:(\d+\.\d+),Battery_Current:(-?\d+\.\d+)')

# Flag to control logging state
logging_enabled = False
log_file = None
writer = None

# Start time for the measurements and the print interval
start_time = time.time()
print_interval = 1  # Change this to the desired number of seconds
last_print_time = start_time

# Function to handle Ctrl-C
def signal_handler(sig, frame):
    global logging_enabled, log_file, writer

    if logging_enabled:
        logging_enabled = False
        if log_file:
            log_file.close()
            log_file = None
            writer = None
        print("\nLogging paused. Press Ctrl-C to resume logging or exit.")
    else:
        user_input = input("\nDo you want to start/resume logging or exit (s/r/e)?: ")
        if user_input.lower() == 's' or user_input.lower() == 'r':
            start_logging()
        elif user_input.lower() == 'e':
            print("Exiting...")
            if log_file:
                log_file.close()
            ser.close()
            exit()

# Function to start logging
def start_logging():
    global logging_enabled, log_file, writer

    if not log_file:
        current_time = datetime.now().strftime("%Y-%m-%d_%H-%M")
        output_file = os.path.join(os.getcwd(), "scripts", "log", "output", f'SingleCellAdcLog_{current_time}.csv')
        log_file = open(output_file, mode='w', newline='')
        writer = csv.writer(log_file)
        writer.writerow(["Elapsed_Time", "Battery_Voltage", "Battery_Current"])  # Write the header

    logging_enabled = True
    print("Logging resumed")

# Function to process a single line of data
def process_line(line):
    global logging_enabled, writer, last_print_time

    match = pattern.match(line)
    if match:
        battery_voltage = match.group(1)
        battery_current = match.group(2)
        elapsed_time = round(time.time() - start_time, 2)  # Calculate elapsed time
        current_time = time.time()

        if logging_enabled:
            writer.writerow([elapsed_time, battery_voltage, battery_current])
        # Print only if the interval has passed
        if current_time - last_print_time >= print_interval:
            last_print_time = current_time
            
            print(f"Elapsed_Time={elapsed_time}s, Battery_Voltage={battery_voltage}, Battery_Current={battery_current}")
    else:
        print(f"Ignored: {line}")

# Setup signal handler for Ctrl-C
signal.signal(signal.SIGINT, signal_handler)

# Open the serial port
ser = serial.Serial(serial_port, baud_rate)

# Main loop to read and process serial data
try:
    print("Press Ctrl-C to start logging")
    while True:
        line = ser.readline().decode('utf-8').strip()
        process_line(line)
except KeyboardInterrupt:
    print("\nExiting...")
finally:
    # Close the serial port
    ser.close()
    if log_file:
        log_file.close()
    print("Serial port closed.")
