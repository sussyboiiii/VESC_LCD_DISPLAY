import subprocess
import socket
import time
import json
from chars import*
host = "localhost"
port = 65432
s1 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

server_process = subprocess.Popen(["python", "/Users/dennisruff/Documents/Programming/Python/LCD/lcd.py"])

# Wait for the server to start
time.sleep(1) 

def writechar(input, row, column):
    data = {
        'input': input,
        'row': row,
        'column': column
    }
    s1.sendto(json.dumps(data).encode(), (host, port))

abtemp = 0
bttemp = 0
SW = 0
previousMillis = 0
erval = 0
date = 0
rpm = 0
voltage = 38.4
current = 0
power = 0
powerpercent = 0
powerfiltered = 0
amphour = 0
tach = 0
watthour = 0
speed = 0
batterypercentage = 0
averagespeed = 0
timedriving = 0
temperature = -99
tempmotor = 0
tempmosfet = 0
tempbattery = 0
tripdistance = 0
lasttrip = 0
istrip = 0

ph = "ph1234567890"
km = "km1234567890"
ah = "ah1234567890"
dgC = "dgC1234567890"
k1 = "k11234567890"
mt = "mt1234567890"
ms = "ms1234567890"
bat = "bat1234567890"


def setup():
    # Speed
    writechar(k1, 5, 0)
    writechar(ph, 6, 0)

    # Battery percentage
    writechar("%", 12, 0)

    # Voltage
    writechar("V", 19, 0)

    # Wattage
    writechar("W", 6, 1)

    # Current
    writechar("A", 12, 1)

    # Amphours
    writechar(ah, 19, 1)

    # Battery temp
    writechar(dgC, 5, 2)
    writechar(bat, 6, 2)

    # Motor temp
    writechar(dgC, 11, 2)
    writechar(mt, 12, 2)

    # Mosfet temp
    writechar(dgC, 18, 2)
    writechar(ms, 19, 2)

    # Trip distance
    writechar(km, 19, 3)

    # Ambient temperature
    writechar(dgC, 12, 3)

def loop():

    global speed, amphour, batterypercentage, powerfiltered, current, tempbattery, tempmotor, tempmosfet, temperature, tripdistance, voltage

    voltage = voltage + 0.1
    voltage = round(voltage, 1)
    if voltage >= 50.4:
        voltage = 38.4
    speed += 1.11
    speed = round(speed, 2)
    batterypercentage += 1
    powerfiltered += 1
    current += 1
    amphour += 1.1
    amphour = round(amphour, 1)
    tempbattery += 1
    tempmotor += 1
    tempmosfet += 1
    temperature += 1
    tripdistance += 1.1
    tripdistance = round(tripdistance, 1)

    if speed <= 0:
        speed = 0
    if speed < 10:
        writechar(f" {speed}", 0, 0)
    elif speed < 100:
        writechar(f"{speed}", 0, 0)
    elif speed > 99:
        speed = 99
        writechar(f"{speed}", 0, 0)

        speed = 0

    if batterypercentage < 10:
        writechar(f"  {batterypercentage}", 9, 0)
    elif batterypercentage < 100:
        writechar(f" {batterypercentage}", 9, 0)
    elif batterypercentage == 100:
        writechar(f"{batterypercentage}", 9, 0)

        batterypercentage = 0


    writechar(voltage, 15, 0)

    # 2nd line
    if powerfiltered < 10:
        writechar(f"   {powerfiltered}", 2, 1)
    elif powerfiltered < 100:
        writechar(f"  {powerfiltered}", 2, 1)
    elif powerfiltered < 1000:
        writechar(f" {powerfiltered}", 2, 1)
    elif powerfiltered > 999:
        writechar(f"{powerfiltered}", 2, 1)

    if powerfiltered > 5000:
        powerfiltered = 0

    if current < 10:
        writechar(f"  {current}", 9, 1)
    elif current < 100:
        writechar(f" {current}", 9, 1)
    elif current > 99:
        writechar(f"{current}", 9, 1)

    if current > 200:
        current = 0

    writechar(amphour, 15, 1)
    if amphour >= 99:
        amphour = 0

    # 3rd line
    if tempbattery < 10:
        writechar(f"  {tempbattery}", 2, 2)
    elif tempbattery < 100:
        writechar(f" {tempbattery}", 2, 2)
    elif tempbattery > 99:
        writechar(f"{tempbattery}", 2, 2)
        
        tempbattery = 0

    if tempmotor < 10:
        writechar(f"  {tempmotor}", 8, 2)
    elif tempmotor < 100:
        writechar(f" {tempmotor}", 8, 2)
    elif tempmotor > 99:
        writechar(f"{tempmotor}", 8, 2)

        tempmotor = 0

    if tempmosfet < 10:
        writechar(f"  {tempmosfet}", 15, 2)
    elif tempmosfet < 100:
        writechar(f" {tempmosfet}", 15, 2)
    elif tempmosfet > 99:
        writechar(f"{tempmosfet}", 15, 2)

        tempmosfet = 0

    # 4th line
        
    writechar("12:58:15", 0, 3)

    if temperature < -99:
        writechar(f"-99", 9, 3)
    elif temperature < -10 and temperature > -100:
        writechar(f"{temperature}", 9, 3)
    elif temperature < 0 and temperature > -9:
        writechar(f" {temperature}", 9, 3)
    elif temperature < 9 and temperature > 0:
        writechar(f"  {temperature}", 9, 3)
    elif temperature > 10 and temperature < 100:
        writechar(f" {temperature}", 9, 3)
    elif temperature > 99:
        writechar(f"{temperature}", 9, 3)

        temperature = -99

    if tripdistance < 10:
        writechar(f"  {tripdistance}", 14, 3)
    elif tripdistance < 100:
        writechar(f" {tripdistance}", 14, 3)
    elif tripdistance > 99:
        writechar(f"{tripdistance}", 14, 3)

    if tripdistance > 999:
        tripdistance = 0


if __name__ == "__main__":
    setup()
    while True:
        if server_process.poll() is not None:
            break
        time.sleep(0.01)
        loop()
