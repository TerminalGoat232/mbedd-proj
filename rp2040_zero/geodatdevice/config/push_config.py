import serial
import serial.tools.list_ports as ls_ports
import time 
import pathlib
import json

def search_for_port():
    ports = ls_ports.comports()
    for p in ports:
        if ("RP2040" in p.description) or ("CDC" in p.description) or ("ACM" in p.device):
            return p.device 
    return None 
port = search_for_port()

if not port: raise RuntimeError("Where RP2040?")
ser_rp = serial.Serial(port, 115200)
time.sleep(2)
print("Waiting...")

while 1:
    print("ok buddy rp2040")
    with open(pathlib.Path('config.json'), "r") as file:
        c = []
        dat = file.read()
        config_dat = json.loads(dat)
        for x in range(3):
            c.append(int(config_dat[f"K_{x}"], 16))
        msg_sr = bytes(c);
        ser_rp.write(msg_sr);
        print(f"Successfully pushed key configuration to GeodatDevice: \n {msg_sr}")
        ser_rp.close()
        break
else: pass


