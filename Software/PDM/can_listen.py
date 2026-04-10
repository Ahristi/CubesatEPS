import can
from datetime import datetime

CAN_INTERFACE = "gs_usb"
CAN_CHANNEL   = 0
CAN_BITRATE   = 125000


def main():
    bus = can.Bus(
        interface=CAN_INTERFACE,
        channel=CAN_CHANNEL,
        bitrate=CAN_BITRATE,
    )

    print(f"Listening on {CAN_INTERFACE}:{CAN_CHANNEL} at {CAN_BITRATE} bit/s")

    try:
        while True:
            msg = bus.recv(timeout=1.0)
            if msg is None:
                continue

            # format data bytes
            data_str = " ".join(f"{b:02X}" for b in msg.data)

            timestamp = datetime.now().strftime("%H:%M:%S.%f")[:-3]

            print(
                f"{timestamp} | ID=0x{msg.arbitration_id:03X} | "
                f"DLC={msg.dlc} | DATA=[{data_str}]"
            )

    except KeyboardInterrupt:
        print("\nStopping CAN listener.")

    finally:
        bus.shutdown()


if __name__ == "__main__":
    main()