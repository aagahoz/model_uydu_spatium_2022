from dronekit import connect
import serial
import time

pixhawk_connection_state = True
esp32_connection_state = True

# Connect to the Vehicle (in this case a UDP endpoint)
try:
    vehicle = connect("/dev/ttyACM0", wait_ready=True)
except serial.serialutil.SerialException:
    pixhawk_connection_state = False


# seri portu acma
try:
    ser = serial.Serial('/dev/serial0', 9600, timeout=1)
    
    # vehicle a baglanana kadar bekle
    vehicle.wait_ready('autopilot_version') 

except serial.serialutil.SerialException:
    esp32_connection_state = False

payload = ""

while True:
    pitch = ""
    roll = ""
    yaw = ""
    enlem = ""
    boylam = ""
    yukseklik = ""
    
    if pixhawk_connection_state:
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
    if esp32_connection_state:
        ser.write(payload)  # uarta yazma
        print("USB Success")
    payload = "<,,,,,>"
    time.sleep(0.5)

