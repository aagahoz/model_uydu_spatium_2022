"""
Client that sends the file (uploads)
"""
import socket
import tqdm
import os
import argparse

SEPARATOR = "<SEPARATOR>"
BUFFER_SIZE = 1024 * 4 #4KB

def send_file(filename, host, port):
    # get the file size
    filesize = os.path.getsize(filename)
    # create the client socket
    s = socket.socket()
    print(f"[+] Connecting to {host}:{port}")
    s.connect((host, port))
    print("[+] Connected.")

    # send the filename and filesize
    s.send(f"{filename}{SEPARATOR}{filesize}".encode())

    # start sending the file
    progress = tqdm.tqdm(range(filesize), f"Sending {filename}", unit="B", unit_scale=True, unit_divisor=1024)
    with open(filename, "rb") as f:
        while True:
            # read the bytes from the file
            bytes_read = f.read(BUFFER_SIZE)
            if not bytes_read:
                # file transmitting is done
                break
            # we use sendall to assure transimission in 
            # busy networks
            s.sendall(bytes_read)
            # update the progress bar
            progress.update(len(bytes_read))

    # close the socket
    s.close()

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description="Simple File Sender")

    args = parser.parse_args()
    filename = "C:/Users/aagahoz\Desktop/gah.exe"
    host = "192.168.31.151"
    port = 5001
    send_file(filename, host, port)