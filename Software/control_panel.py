import serial

import serial

# ---- CONFIG ----
PORT = "COM3"         
BAUD = 115200
TIMEOUT = 1.0

READ_SINGLE = 0x01
CMD_ERROR = 0x07

# ----------------

def main():
    reg_input = input("Enter register address (hex, e.g. 0F): ").strip()
    reg = int(reg_input, 16)

    with serial.Serial(PORT, BAUD, timeout=TIMEOUT) as ser:

        # Build command packet
        cmd = bytes([READ_SINGLE, reg])

        # Send command
        ser.write(cmd)

        # Expect 3 bytes back
        resp = ser.read(3)

        if len(resp) == 0:
            print("No response")
            return

        if resp[0] == CMD_ERROR:
            print("Device returned ERROR")
            return

        if resp[0] != READ_SINGLE:
            print("Unexpected response:", resp)
            return

        value = (resp[1] << 8) | resp[2]

        print(f"Register 0x{reg:02X} = 0x{value:04X} ({value})")


if __name__ == "__main__":
    main()