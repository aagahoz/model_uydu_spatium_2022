from dronekit import connect
import serial

import time

# Connect to the Vehicle (in this case a UDP endpoint)
vehicle = connect("	127.0.0.1:14550", wait_ready=True)

# seri portu acma
ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=1)

# vehicle a baglanana kadar bekle
vehicle.wait_ready('autopilot_version')  

payload = ""

while True:
    
    # imu
    pitch = vehicle.attitude.pitch
    roll = vehicle.attitude.roll
    yaw = vehicle.attitude.yaw
    
    # gps
    enlem = vehicle.location.global_relative_frame.lat
    boylam = vehicle.location.global_relative_frame.lon
    yukseklik = vehicle.location.global_relative_frame.alt
    
    # birlestirilmis
    payload = "<" + str(pitch) + "," + str(roll) + "," + str(yaw) + "," + str(enlem) + "," + str(boylam) + "," + str(yukseklik)  + ">"
    print(payload)
    
    # birlestirilmis veriyi uarta yazma
    ser.write(payload)  # uarta yazma
    payload = "<,,,,,>"
    time.sleep(0.5)