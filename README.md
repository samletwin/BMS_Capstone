# E-Bike BMS Display Capstone Project

Using ESP32C3 controller to communicate with a Daly BMS connected to an E-Bike and display readings from the BMS onto the Adafruit 320x480 Capactive Touch Screen: https://www.adafruit.com/product/5846

Readings from the BMS are logged to an SD Card. Additionally, from the cell voltage readings we are using characterized data collected by the BMS Lab at the University of Windsor to display a much more accurate State of Charge of the pack.

SOC Data can be found here: https://github.com/soorajsunil/Piecewise-OCV-SOC

# Dependencies
ESP-IDF = 5.1.X
- Arduino Core for ESP-IDF only works with ESP-IDF version of 5.1.X

Arduino-ESP32 >= 3.0.X
https://github.com/espressif/arduino-esp32
- Using Arduino Core for communicate with the Daly BMS over UART
- Github used for Daly is here: https://github.com/maland16/daly-bms-uart
- This Daly library could be implemented in ESP-IDF but due to time constraints it was just easier to import it as an arduino component

ESP-DSP = 1.4.12 
- Using linear algebra from this library to calculate SOH

LVGL >= 9.0
- Previous versions will not work, as v9 had big API changes


