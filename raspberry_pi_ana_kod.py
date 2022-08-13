def converter(old_value, old_min, old_max, new_min, new_max):
    new_value = ( (old_value - old_min) / (old_max - old_min) ) * (new_max - new_min) + new_min
    return new_value

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
        
        pitch = converter(pitch, -1.64, 1.62, 0, 360)
        roll = converter(roll, -3, 3, -180, 180)
        yaw = converter(yaw, -3, 3, -180, 180)
        pitch = int(pitch)
        roll = int(roll)
        yaw = int(yaw)

        #speed
        speed = vehicle.airspeed
        
        # gps
        enlem = vehicle.location.global_relative_frame.lat
        boylam = vehicle.location.global_relative_frame.lon
        yukseklik = vehicle.location.global_relative_frame.alt
        
        
    # birlestirilmis
    payload = "<" + str(enlem) + "," + str(boylam) + "," + str(yukseklik) + "," + str(speed) + "," + str(pitch) + "," + str(roll) + "," + str(yaw) + ">"
#     payload = "$RASPI" + "," + str(pitch) + "," + str(roll) + "," + str(yaw) + "," + str(enlem) + "," + str(boylam) + "," + str(yukseklik) + "," + str(speed) + ">"
    print(payload)
    
    # birlestirilmis veriyi uarta yazma
    
    ser.write(payload.encode())  # uarta yazma
    x = ser.read().decode("utf=8")
    print("GelenData: ", x)
         
#         print("USB Success")
    payload = "<,,,,,>"
    time.sleep(0.5)