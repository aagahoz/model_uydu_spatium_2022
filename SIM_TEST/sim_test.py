from dronekit import Command, connect, VehicleMode, LocationGlobalRelative
import time
from pymavlink import mavutil

uav = connect("127.0.0.1:14550", wait_ready=True)


def takeoff(irtifa):
    while uav.is_armable is not True:
        print("IHA arm edilebilir durumda degil.")
        time.sleep(1)

    print("IHA arm edilebilir.")

    uav.mode = VehicleMode("GUIDED")

    uav.armed = True

    while uav.armed is not True:
        print("IHA arm ediliyor...")
        time.sleep(0.5)

    print("IHA arm edildi.")

    uav.simple_takeoff(irtifa)

    while uav.location.global_relative_frame.alt < irtifa * 0.9:
        print("IHA hedefe yukseliyor.")
        time.sleep(1)


def gorev_ekle():
    global komut
    komut = uav.commands
    komut.clear()
    time.sleep(1)

    global GOREV_SAYISI
    GOREV_SAYISI = 6
    # 500m TAKEOFF MISSION
    komut.add(
        Command(0, 0, 0, mavutil.mavlink.MAV_FRAME_GLOBAL_RELATIVE_ALT, mavutil.mavlink.MAV_CMD_NAV_TAKEOFF, 0, 0, 0, 0,
                0, 0, 0, 0, 500))


    # HIZI AYARLA, 200M'YE KADAR İN. 30 SN BEKLE.
    komut.add(
        Command(0, 0, 0, mavutil.mavlink.MAV_FRAME_GLOBAL_RELATIVE_ALT, mavutil.mavlink.MAV_CMD_DO_CHANGE_SPEED, 0, 0,
                0, 15, 0, 0, 0, 0, 0))
    komut.add(
        Command(0, 0, 0, mavutil.mavlink.MAV_FRAME_GLOBAL_RELATIVE_ALT, mavutil.mavlink.MAV_CMD_NAV_WAYPOINT, 0, 0, 30,
                0, 0, 0, 0, 0, 200))


    # HIZI AYARLA, İNİŞ KOORDİNATINA 100M'YE İNEREK GİT. İN.
    komut.add(
        Command(0, 0, 0, mavutil.mavlink.MAV_FRAME_GLOBAL_RELATIVE_ALT, mavutil.mavlink.MAV_CMD_DO_CHANGE_SPEED, 0, 0,
                0, 8, 0, 0, 0, 0, 0))
    komut.add(
        Command(0, 0, 0, mavutil.mavlink.MAV_FRAME_GLOBAL_RELATIVE_ALT, mavutil.mavlink.MAV_CMD_NAV_WAYPOINT, 0, 0, 0,
                0, 0, 0, -35.36284149, 149.16516464, 100))
    komut.add(
        Command(0, 0, 0, mavutil.mavlink.MAV_FRAME_GLOBAL_RELATIVE_ALT, mavutil.mavlink.MAV_CMD_NAV_LAND, 0, 0, 0, 0, 0,
                0, 0, 0, 0))






    # BURASI ÖYLESİNE BİR YERE GİTMEK İÇİN VAR
    # komut.add(
    #     Command(0, 0, 0, mavutil.mavlink.MAV_FRAME_GLOBAL_RELATIVE_ALT, mavutil.mavlink.MAV_CMD_DO_CHANGE_SPEED, 0, 0,
    #             0, 15, 0, 0, 0, 0, 0))
    # komut.add(
    #     Command(0, 0, 0, mavutil.mavlink.MAV_FRAME_GLOBAL_RELATIVE_ALT, mavutil.mavlink.MAV_CMD_NAV_TAKEOFF, 0, 0, 0, 0,
    #             0, 0, 0, 0, 50))
    # komut.add(
    #     Command(0, 0, 0, mavutil.mavlink.MAV_FRAME_GLOBAL_RELATIVE_ALT, mavutil.mavlink.MAV_CMD_NAV_WAYPOINT, 0, 0, 0,
    #             0, 0, 0, -35.36300937, 149.16517533, 0))
    # komut.add(
    #     Command(0, 0, 0, mavutil.mavlink.MAV_FRAME_GLOBAL_RELATIVE_ALT, mavutil.mavlink.MAV_CMD_NAV_LAND, 0, 0, 0, 0, 0,
    #             0, 0, 0, 0))
    # global GOREV_SAYISI = None

    komut.upload()
    print("Komutlar yukleniyor...")

#İLK YÜKSELME
takeoff(450)

#GÖREVLERİ EKLE
gorev_ekle()

komut.next = 0

uav.mode = VehicleMode("AUTO")

while True:
    next_waypoint = komut.next

    print(f"Siradaki komut {next_waypoint}")

    # BURASI GEREKSİZ.
    if (next_waypoint == 2):
        time.sleep(10)
        print("Irtifa sabitleme gorevi")

    time.sleep(1)

    if next_waypoint is GOREV_SAYISI:
        print("Gorev bitti.")
        break

print("Donguden cikildi.")