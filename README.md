# E-Bike BMS Display Capstone Project

Using ESP32C3 controller to communicate with a Daly BMS connected to an E-Bike and display readings from the BMS onto the Adafruit 320x480 Capactive Touch Screen: https://www.adafruit.com/product/5846

Readings from the BMS are logged to an SD Card. Additionally, from the cell voltage readings we are using characterized data collected by the BMS Lab at the University of Windsor to display a much more accurate State of Charge of the pack.

SOC Data can be found here: https://github.com/soorajsunil/Piecewise-OCV-SOC

# Dependencies
ESP-IDF >= 5.2
- Previous versions might work but haven't checked

ESP-DSP = 1.4.12 
- Using linear algebra from this library to calculate SOH

LVGL >= 9.0
- Previous versions will not work, as v9 had big API changes


