import serial
import time

# ---- CONFIG ----
PORT = 'COM4'          
BAUD = 115200

CMD_EFUSE = 0x47      
MASK_12V_CH1 = 0x0010

# ---- BUILD PACKET ----
data_msb = (MASK_12V_CH1 >> 8) & 0xFF
data_lsb = MASK_12V_CH1 & 0xFF



packet = bytes([
    CMD_EFUSE,
    data_msb,
    data_lsb,
    ord('\n')         
])

with serial.Serial(PORT, BAUD, timeout=1) as ser:
    print(f"Sending: {[hex(b) for b in packet]}")
    ser.write(packet)
    response = ser.readline()
