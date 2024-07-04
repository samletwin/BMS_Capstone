import pandas as pd
import matplotlib.pyplot as plt
import os

# Predefined folder containing CSV files
folder_path = os.path.join(os.getcwd(),"scripts","log","output")

# Function to list all CSV files in the predefined folder
def list_csv_files(folder_path):
    return [f for f in os.listdir(folder_path) if f.endswith('.csv')]

# Function to read and plot data from a CSV file with custom axis limits
def plot_csv(file_path):
    df = pd.read_csv(file_path, header=0, names=['Time', 'Voltage', 'Current'])

    # Determine min and max values for voltage and time
    min_voltage = df['Voltage'].min() - 50
    min_voltage = 0 if min_voltage < 0 else min_voltage
    max_voltage = df['Voltage'].max() + 50
    min_time = df['Time'].min()
    max_time = df['Time'].max()

    plt.figure(figsize=(10, 6))
    
    # Plot Voltage
    plt.subplot(2, 1, 1)
    plt.plot(df['Time'], df['Voltage'], label='Voltage', color='blue')
    plt.xlabel('Time (s)')
    plt.ylabel('Voltage (V)')
    plt.title('Voltage vs Time')
    plt.grid(True)
    plt.legend()
    plt.xlim(min_time, max_time)
    plt.ylim(min_voltage, max_voltage)
    
    # Determine min and max values for current
    min_current = df['Current'].min() - 50
    max_current = df['Current'].max() + 50
    
    # Plot Current
    plt.subplot(2, 1, 2)
    plt.plot(df['Time'], df['Current'], label='Current', color='red')
    plt.xlabel('Time (s)')
    plt.ylabel('Current (mA)')
    plt.title('Current vs Time')
    plt.grid(True)
    plt.legend()
    plt.xlim(min_time, max_time)
    plt.ylim(min_current, max_current)
    
    plt.tight_layout()
    plt.show()

# List CSV files in the predefined folder
csv_files = list_csv_files(folder_path)

# Display the CSV files for the user to choose
print("Select a CSV file to plot:")
for idx, file in enumerate(csv_files):
    print(f"{idx + 1}. {file}")

print(f"{len(csv_files) + 1}. Enter full path to a different CSV file")

# Get user's choice
choice = int(input("Enter the number corresponding to your choice: "))

if choice == len(csv_files) + 1:
    # User wants to enter a full path to a different CSV file
    file_path = input("Enter the full path to the CSV file: ")
else:
    # User selects from the predefined folder
    file_path = os.path.join(folder_path, csv_files[choice - 1])

# Plot the selected CSV file
plot_csv(file_path)
