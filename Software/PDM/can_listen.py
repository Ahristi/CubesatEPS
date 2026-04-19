import can

def main():
    channel = "PCAN_USBBUS1"   
    bitrate = 125000           

    try:
        bus = can.Bus(interface="pcan", channel=channel, bitrate=bitrate)
        print(f"Listening on {channel} at {bitrate} bit/s...")
        print("Press Ctrl+C to stop.\n")

        while True:
            msg = bus.recv(timeout=1.0)
            if msg is None:
                continue

            data_str = " ".join(f"{b:02X}" for b in msg.data)
            frame_type = "EXT" if msg.is_extended_id else "STD"

            print(
                f"ID: 0x{msg.arbitration_id:X} | "
                f"{frame_type} | "
                f"DLC: {msg.dlc} | "
                f"Data: {data_str}"
            )

    except KeyboardInterrupt:
        print("\nStopped by user.")
    except can.CanError as e:
        print(f"CAN error: {e}")
    finally:
        try:
            bus.shutdown()
        except:
            pass

if __name__ == "__main__":
    main()