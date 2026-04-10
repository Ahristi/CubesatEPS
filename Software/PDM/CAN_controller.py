import json
import can
import ctypes
from datetime import datetime

CONFIG_PATH = "conversions.json"

# CANable settings
CAN_INTERFACE = "gs_usb"
CAN_CHANNEL   = 0
CAN_BITRATE   = 125000


def load_config(path: str) -> dict:
    with open(path, "r") as f:
        return json.load(f)


def read_u16(data: bytes, offset: int) -> int:
    return (data[offset]>>8) | (data[offset + 1])


def decode_message(msg: can.Message, config: dict):
    can_id = f"0x{msg.arbitration_id:03X}"
    print(can_id)
    if can_id not in config:
        return None

    msg_cfg = config[can_id]
    result = {
        "rail": msg_cfg["name"],
        "can_id": can_id,
    }

    for sig in msg_cfg["signals"]:
        offset = sig["offset"]

        if offset + 1 >= len(msg.data):
            result[sig["name"]] = "INVALID_PAYLOAD"
            continue

        if sig["type"] != "u16":
            result[sig["name"]] = "UNSUPPORTED_TYPE"
            continue

        raw = read_u16(msg.data, offset)
        scale = sig.get("scale", 1.0)
        value = raw * scale

        result[sig["name"]] = value
        result[f"{sig['name']}_raw"] = raw
        result[f"{sig['name']}_unit"] = sig.get("unit", "")

    return result


def format_telemetry(decoded: dict) -> str:
    rail = decoded["rail"]
    can_id = decoded["can_id"]

    parts = [f"[{can_id}] {rail}"]

    for key, value in decoded.items():
        if key in ("rail", "can_id"):
            continue
        if key.endswith("_raw") or key.endswith("_unit"):
            continue

        raw_key = f"{key}_raw"
        unit_key = f"{key}_unit"

        raw = decoded.get(raw_key, "")
        unit = decoded.get(unit_key, "")

        if isinstance(value, (int, float)):
            parts.append(f"{key}={value:.3f} {unit} (raw={raw})")
        else:
            parts.append(f"{key}={value}")

    return " | ".join(parts)


def main():
    config = load_config(CONFIG_PATH)

    bus = can.Bus(
        interface=CAN_INTERFACE,
        channel=CAN_CHANNEL,
        bitrate=CAN_BITRATE,
    )

    print(f"Listening on {CAN_INTERFACE}:{CAN_CHANNEL} at {CAN_BITRATE} bit/s")
    print("Waiting for EPS telemetry...")

    try:
        while True:
            msg = bus.recv(timeout=1.0)
            print(msg)
    except KeyboardInterrupt:
        print("\nStopping CAN listener.")

    finally:
        bus.shutdown()


if __name__ == "__main__":
    main()