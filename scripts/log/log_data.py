import serial
import csv
import time

# Configure the serial port
serial_port = 'COM3'  # Change this to your serial port
baud_rate = 115200  # Change this to your baud rate
output_file = 'output/esp_log.csv'

# Open the serial port
ser = serial.Serial(serial_port, baud_rate)

# Open the CSV file for writing
with open(output_file, mode='w', newline='') as file:
    writer = csv.writer(file)
    writer.writerow(["Timestamp", "Data"])  # Write the header

    try:
        while True:
            # Read a line from the serial port
            line = ser.readline().decode('utf-8').strip()
            
            # Get the current timestamp
            timestamp = time.strftime('%Y-%m-%d %H:%M:%S')

            # Write the timestamp and data to the CSV file
            writer.writerow([timestamp, line])
            print(f"{timestamp}, {line}")
    except KeyboardInterrupt:
        print("Logging stopped by user")

# Close the serial port
ser.close()
