import re
import matplotlib.pyplot as plt
from dataclasses import dataclass
from typing import List
import os

# Define the dataclass for storing data
@dataclass
class ADCData:
    voltage: float
    current: float
    time: float

# Function to parse the text file and return a list of ADCData
def parse_data(filename: str) -> List[ADCData]:
    data = []
    with open(filename, 'r') as file:
        for line in file:
            match = re.search(r"Volt = (\d+) mV, Current = (\d+) mA, Time = (\d+) ms", line)
            if match:
                voltage = float(match.group(1))
                current = float(match.group(2))
                time = float(match.group(3))
                data.append(ADCData(voltage, current, time))
    return data

# Function to plot the data
def plot_data(data: List[ADCData]):
    times = [d.time for d in data]
    voltages = [d.voltage for d in data]
    currents = [d.current for d in data]

    fig, (ax1, ax2) = plt.subplots(2, 1, sharex=True)

    ax1.plot(times, voltages, label='Voltage (mV)')
    ax1.set_ylabel('Voltage (mV)')
    ax1.legend(loc='upper right')
    
    ax2.plot(times, currents, label='Current (mA)', color='orange')
    ax2.set_xlabel('Time (ms)')
    ax2.set_ylabel('Current (mA)')
    ax2.legend(loc='upper right')

    plt.show()

# Main execution
if __name__ == "__main__":
    filename = os.getcwd() + '/scripts/data.txt'  # Replace with your file name
    data = parse_data(filename)
    plot_data(data)
