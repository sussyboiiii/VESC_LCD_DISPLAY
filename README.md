# About

This repository hosts the code for an Arduino-based project that enables you to view statistics from your VESC (Vedder Electronic Speed Controller) on an I2C LCD screen. The project is designed for electric bikes (E-bikes) and is inspired by [Tom Stanton's VESC E-bike](https://github.com/TomStanton/VESC_LCD_EBIKE).

The code allows you to monitor various stats about your E-bike in real-time, including speed, power, current, temperature, and more. It's a great tool for E-bike enthusiasts who want to keep track of their bike's performance and health.

The project uses a VESC 6 to control the E-bike and an I2C 4x20 LCD screen to display the stats. It also includes an optional Python-based LCD emulator for testing the LCD display functions without needing an actual Arduino or LCD screen.

## Components

The project requires the following components:

- [VESC 6 MKVI HP](https://trampaboards.com/)
- [Arduino Nano](https://amzn.eu/d/a3JlUzB)
- [4x20 I2C LCD](https://amzn.eu/d/3YCoh1k)


Optional components include (adjustments to `Ebike.ino` may be required):

- [DS3231 I2C Real Time Clock](https://amzn.eu/d/5yu6arG)
- [Buck Boost Converter](https://amzn.eu/d/1i72zAF)
- [10k at 25°C NTC Thermistor](https://amzn.eu/d/2FOsBCA) (Adjust B value in `Ebike.ino` to your sensor!)

## Main Arduino Sketch

The main Arduino sketch for the project is `Ebike.ino`. This sketch controls the behavior of the E-Bike, including reading data from the VESC and displaying it on the LCD screen.

## LCD Emulator for testing

The `lcdem` directory contains a Python-based LCD emulator. This emulator can be used to test the LCD display functions without needing an actual Arduino or LCD screen. The main scripts for the emulator are `Ebike.py` and `lcd.py`. To run the emulator you need to type `python3 ebike.py` in your terminal and everything should be up and running!

`Ebike.py` is a script that simulates the data that would be sent from the Arduino to the LCD screen. It sends data to `lcd.py`, which emulates the LCD screen and displays the data.

## Contributing

If you encounter any bugs, please feel free to open an issue or make a pull request. Thanks!
