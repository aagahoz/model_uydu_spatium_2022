from dronekit import connect

import time

# Connect to the Vehicle (in this case a UDP endpoint)
vehicle = connect("	127.0.0.1:14550", wait_ready=True)
print(vehicle.mode.name)

vehicle.wait_ready('autopilot_version')

payload = ""

while True:
    pitch = vehicle.attitude.pitch
    roll = vehicle.attitude.roll
    yaw = vehicle.attitude.yaw
    enlem = vehicle.location.global_relative_frame.lat
    boylam = vehicle.location.global_relative_frame.lon
    yukseklik = vehicle.location.global_relative_frame.alt
    payload = "<" + str(pitch) + "," + str(roll) + "," + str(yaw) + "," + str(enlem) + "," + str(boylam) + "," + str(yukseklik)  + ">"
    print(payload)
    time.sleep(0.5)
