import json
import can
import time
import tkinter as tk
from tkinter import ttk
from dataclasses import dataclass

CONFIG_PATH = "conversions.json"

CAN_INTERFACE = "pcan"
CAN_CHANNEL = "PCAN_USBBUS1"
CAN_BITRATE = 125000

STALE_TIMEOUT_S = 2.0
REOPEN_DELAY_S = 0.5

EFUSE_BUTTONS = [
    ("3V3 CH2", 0x01),
    ("5V CH1",  0x02),
    ("5V CH2",  0x04),
    ("6V CH1",  0x08),
    ("12V CH1", 0x10),
    ("12V CH2", 0x20),
]

CONVERTER_BUTTONS = [
    ("5V Converter",  0x01),
    ("6V Converter",  0x02),
    ("12V Converter", 0x04),
]

EFUSE_STATUS_BITS = [
    ("3V3 CH2",  0x01),
    ("5V CH1",   0x02),
    ("5V CH2",   0x04),
    ("6V CH1",   0x08),
    ("6V CH2",   0x00),  # unused
    ("12V CH1",  0x10),
    ("12V CH2",  0x20),
]


@dataclass
class RegulatorTelemetry:
    name: str
    voltage: float
    currentCH1: float
    currentCH2: float


@dataclass
class BMSTelemetry:
    name: str
    battery_voltage: float
    battery_current: float
    sys_voltage: float
    battery_temp: float
    die_temp: float


@dataclass
class SystemTelemetry:
    name: str
    efuse_states_mask: int
    mcu_temp: float
    efuse_faults_mask: int


def load_config(path: str) -> dict:
    with open(path, "r") as f:
        return json.load(f)


def open_bus():
    return can.Bus(
        interface=CAN_INTERFACE,
        channel=CAN_CHANNEL,
        bitrate=CAN_BITRATE,
    )


def read_u16(data: bytes, offset: int) -> int:
    return (data[offset] << 8) | data[offset + 1]


def read_u8(data: bytes, offset: int) -> int:
    return data[offset]


def read_s16(data: bytes, offset: int) -> int:
    value = (data[offset] << 8) | data[offset + 1]
    if value & 0x8000:
        value -= 0x10000
    return value


def read_s8(data: bytes, offset: int) -> int:
    value = data[offset]
    if value & 0x80:
        value -= 0x100
    return value


def decode_regulator_telem(msg: can.Message, msg_cfg: dict) -> RegulatorTelemetry:
    signals = msg_cfg["signals"]

    raw_voltage = read_u16(msg.data, signals["rail_voltage"]["offset"])
    raw_currentCH1 = read_u16(msg.data, signals["current_ch1"]["offset"])
    raw_currentCH2 = read_u16(msg.data, signals["current_ch2"]["offset"])

    return RegulatorTelemetry(
        name=msg_cfg["name"],
        voltage=raw_voltage * signals["rail_voltage"]["scale"],
        currentCH1=raw_currentCH1 * signals["current_ch1"]["scale"],
        currentCH2=raw_currentCH2 * signals["current_ch2"]["scale"],
    )


def decode_bms_telem(msg: can.Message, msg_cfg: dict) -> BMSTelemetry:
    signals = msg_cfg["signals"]

    raw_battery_voltage = read_s16(msg.data, signals["battery_voltage"]["offset"])
    raw_battery_current = read_s16(msg.data, signals["battery_current"]["offset"])
    raw_sys_voltage = read_s16(msg.data, signals["sys_voltage"]["offset"])
    raw_battery_temp = read_s8(msg.data, signals["battery_temp"]["offset"])
    raw_die_temp = read_s8(msg.data, signals["die_temp"]["offset"])

    return BMSTelemetry(
        name=msg_cfg["name"],
        battery_voltage=raw_battery_voltage * signals["battery_voltage"]["scale"],
        battery_current=raw_battery_current * signals["battery_current"]["scale"],
        sys_voltage=raw_sys_voltage * signals["sys_voltage"]["scale"],
        battery_temp=raw_battery_temp * signals["battery_temp"]["scale"],
        die_temp=raw_die_temp * signals["die_temp"]["scale"],
    )


def decode_sys_telem(msg: can.Message, msg_cfg: dict) -> SystemTelemetry:
    signals = msg_cfg["signals"]

    efuse_states_mask = read_u8(msg.data, signals["efuse_states_mask"]["offset"])
    efuse_faults_mask = read_u8(msg.data, signals["efuse_faults_mask"]["offset"])

    raw_mcu_temp = read_u8(msg.data, signals["MCU_temp"]["offset"])
    mcu_temp = raw_mcu_temp * signals["MCU_temp"].get("scale", 1.0)

    return SystemTelemetry(
        name=msg_cfg["name"],
        efuse_states_mask=efuse_states_mask,
        mcu_temp=mcu_temp,
        efuse_faults_mask=efuse_faults_mask,
    )


def send_efuse_command(bus: can.Bus, enable_mask: int):
    msg = can.Message(
        arbitration_id=0x47,
        data=[enable_mask & 0xFF],
        is_extended_id=False,
    )
    bus.send(msg)


def send_converter_command(bus: can.Bus, enable_mask: int):
    msg = can.Message(
        arbitration_id=0x65,
        data=[enable_mask & 0xFF],
        is_extended_id=False,
    )
    bus.send(msg)


class TelemetryUI:
    def __init__(self, root: tk.Tk, config: dict, efuse_send_callback, converter_send_callback):
        self.root = root
        self.config = config
        self.efuse_send_callback = efuse_send_callback
        self.converter_send_callback = converter_send_callback

        self.root.title("EPS Telemetry Monitor")
        self.root.geometry("550x850")

        self.widgets = {}
        self.diag_widgets = {}
        self.status_var = tk.StringVar(value="Status: starting...")
        self.command_status_var = tk.StringVar(value="No command sent yet")

        notebook = ttk.Notebook(root)
        notebook.pack(fill="both", expand=True, padx=10, pady=10)

        self.telemetry_tab = ttk.Frame(notebook)
        self.command_tab = ttk.Frame(notebook)
        self.diagnostics_tab = ttk.Frame(notebook)

        notebook.add(self.telemetry_tab, text="Telemetry")
        notebook.add(self.command_tab, text="eFuse Control")
        notebook.add(self.diagnostics_tab, text="System Diagnostics")

        self._build_telemetry_tab()
        self._build_command_tab()
        self._build_diagnostics_tab()

    def _build_telemetry_tab(self):
        container = ttk.Frame(self.telemetry_tab, padding=10)
        container.pack(fill="both", expand=True)

        ttk.Label(container, textvariable=self.status_var).grid(
            row=0, column=0, sticky="w", pady=(0, 8)
        )

        row = 1
        for can_id, msg_cfg in self.config.items():
            msg_type = msg_cfg.get("type")
            name = msg_cfg.get("name", can_id)

            if msg_type not in ("regulator_telem", "BMS_telem"):
                continue

            frame = ttk.LabelFrame(container, text=name, padding=10)
            frame.grid(row=row, column=0, sticky="ew", pady=5)
            container.columnconfigure(0, weight=1)

            if msg_type == "regulator_telem":
                voltage_var = tk.StringVar(value="--- V")
                ch1_var = tk.StringVar(value="--- A")
                ch2_var = tk.StringVar(value="--- A")

                ttk.Label(frame, text="Voltage:").grid(row=0, column=0, sticky="w")
                ttk.Label(frame, textvariable=voltage_var).grid(row=0, column=1, sticky="w", padx=10)

                ttk.Label(frame, text="CH1 Current:").grid(row=1, column=0, sticky="w")
                ttk.Label(frame, textvariable=ch1_var).grid(row=1, column=1, sticky="w", padx=10)

                ttk.Label(frame, text="CH2 Current:").grid(row=2, column=0, sticky="w")
                ttk.Label(frame, textvariable=ch2_var).grid(row=2, column=1, sticky="w", padx=10)

                self.widgets[can_id] = {
                    "type": "regulator_telem",
                    "voltage": voltage_var,
                    "ch1": ch1_var,
                    "ch2": ch2_var,
                }

            elif msg_type == "BMS_telem":
                batt_voltage_var = tk.StringVar(value="--- V")
                batt_current_var = tk.StringVar(value="--- A")
                sys_voltage_var = tk.StringVar(value="--- V")
                batt_temp_var = tk.StringVar(value="--- °C")
                die_temp_var = tk.StringVar(value="--- °C")

                ttk.Label(frame, text="Battery Voltage:").grid(row=0, column=0, sticky="w")
                ttk.Label(frame, textvariable=batt_voltage_var).grid(row=0, column=1, sticky="w", padx=10)

                ttk.Label(frame, text="Battery Current:").grid(row=1, column=0, sticky="w")
                ttk.Label(frame, textvariable=batt_current_var).grid(row=1, column=1, sticky="w", padx=10)

                ttk.Label(frame, text="System Voltage:").grid(row=2, column=0, sticky="w")
                ttk.Label(frame, textvariable=sys_voltage_var).grid(row=2, column=1, sticky="w", padx=10)

                ttk.Label(frame, text="Battery Temp:").grid(row=3, column=0, sticky="w")
                ttk.Label(frame, textvariable=batt_temp_var).grid(row=3, column=1, sticky="w", padx=10)

                ttk.Label(frame, text="Die Temp:").grid(row=4, column=0, sticky="w")
                ttk.Label(frame, textvariable=die_temp_var).grid(row=4, column=1, sticky="w", padx=10)

                self.widgets[can_id] = {
                    "type": "BMS_telem",
                    "battery_voltage": batt_voltage_var,
                    "battery_current": batt_current_var,
                    "sys_voltage": sys_voltage_var,
                    "battery_temp": batt_temp_var,
                    "die_temp": die_temp_var,
                }

            row += 1

    def _build_command_tab(self):
        container = ttk.Frame(self.command_tab, padding=15)
        container.pack(fill="both", expand=True)

        ttk.Label(container, text="eFuse Controls").grid(
            row=0, column=0, columnspan=3, sticky="w", pady=(0, 10)
        )

        for i, (label, mask) in enumerate(EFUSE_BUTTONS):
            btn = ttk.Button(
                container,
                text=f"{label}\n0x{mask:02X}",
                command=lambda m=mask, l=label: self._send_efuse(m, l)
            )
            btn.grid(
                row=1 + i // 3,
                column=i % 3,
                padx=5,
                pady=5,
                sticky="nsew"
            )

        efuse_last_row = 1 + (len(EFUSE_BUTTONS) - 1) // 3

        converter_title_row = efuse_last_row + 2
        ttk.Label(container, text="Converter Controls").grid(
            row=converter_title_row, column=0, columnspan=3, sticky="w", pady=(15, 10)
        )

        for i, (label, mask) in enumerate(CONVERTER_BUTTONS):
            btn = ttk.Button(
                container,
                text=f"{label}\n0x{mask:02X}",
                command=lambda m=mask, l=label: self._send_converter(m, l)
            )
            btn.grid(
                row=converter_title_row + 1 + i // 3,
                column=i % 3,
                padx=5,
                pady=5,
                sticky="nsew"
            )

        for c in range(3):
            container.columnconfigure(c, weight=1)

        status_row = converter_title_row + 3
        ttk.Label(container, textvariable=self.command_status_var).grid(
            row=status_row, column=0, columnspan=3, sticky="w", pady=(15, 0)
        )

    def _build_diagnostics_tab(self):
        container = ttk.Frame(self.diagnostics_tab, padding=15)
        container.pack(fill="both", expand=True)

        title = ttk.Label(container, text="System Diagnostics")
        title.grid(row=0, column=0, columnspan=3, sticky="w", pady=(0, 12))

        mcu_temp_var = tk.StringVar(value="--- °C")
        ttk.Label(container, text="MCU Temperature:").grid(row=1, column=0, sticky="w", pady=(0, 10))
        ttk.Label(container, textvariable=mcu_temp_var).grid(row=1, column=1, sticky="w", pady=(0, 10))

        ttk.Label(container, text="eFuse States").grid(row=2, column=0, columnspan=3, sticky="w", pady=(10, 5))
        state_leds = {}

        state_start_row = 3
        for i, (label, mask) in enumerate(EFUSE_STATUS_BITS):
            row = state_start_row + i

            led = tk.Canvas(container, width=18, height=18, highlightthickness=0)
            led.grid(row=row, column=0, sticky="w", padx=(0, 8), pady=2)
            led_id = led.create_oval(2, 2, 16, 16, fill="red", outline="black")

            ttk.Label(container, text=label).grid(row=row, column=1, sticky="w", pady=2)

            state_leds[mask] = (led, led_id)

        fault_title_row = state_start_row + len(EFUSE_STATUS_BITS) + 1
        ttk.Label(container, text="eFuse Faults").grid(
            row=fault_title_row, column=0, columnspan=3, sticky="w", pady=(14, 5)
        )

        fault_leds = {}
        fault_start_row = fault_title_row + 1
        for i, (label, mask) in enumerate(EFUSE_STATUS_BITS):
            row = fault_start_row + i

            led = tk.Canvas(container, width=18, height=18, highlightthickness=0)
            led.grid(row=row, column=0, sticky="w", padx=(0, 8), pady=2)
            led_id = led.create_oval(2, 2, 16, 16, fill="green", outline="black")

            ttk.Label(container, text=label).grid(row=row, column=1, sticky="w", pady=2)

            fault_leds[mask] = (led, led_id)

        self.diag_widgets["sys_telem"] = {
            "mcu_temp": mcu_temp_var,
            "state_leds": state_leds,
            "fault_leds": fault_leds,
        }

    def _send_efuse(self, mask: int, label: str):
        try:
            self.efuse_send_callback(mask)
            self.command_status_var.set(f"Sent eFuse command: {label} -> 0x{mask:02X}")
        except Exception as e:
            self.command_status_var.set(f"eFuse send failed: {e}")

    def _send_converter(self, mask: int, label: str):
        try:
            self.converter_send_callback(mask)
            self.command_status_var.set(f"Sent converter command: {label} -> 0x{mask:02X}")
        except Exception as e:
            self.command_status_var.set(f"Converter send failed: {e}")

    def set_status(self, text: str):
        self.status_var.set(text)

    def update_regulator(self, can_id: str, reg: RegulatorTelemetry):
        if can_id not in self.widgets:
            return
        w = self.widgets[can_id]
        if w["type"] != "regulator_telem":
            return

        w["voltage"].set(f"{reg.voltage:.3f} V")
        w["ch1"].set(f"{reg.currentCH1:.3f} A")
        w["ch2"].set(f"{reg.currentCH2:.3f} A")

    def update_bms(self, can_id: str, bms: BMSTelemetry):
        if can_id not in self.widgets:
            return
        w = self.widgets[can_id]
        if w["type"] != "BMS_telem":
            return

        w["battery_voltage"].set(f"{bms.battery_voltage:.3f} V")
        w["battery_current"].set(f"{bms.battery_current:.3f} A")
        w["sys_voltage"].set(f"{bms.sys_voltage:.3f} V")
        w["battery_temp"].set(f"{bms.battery_temp:.1f} °C")
        w["die_temp"].set(f"{bms.die_temp:.1f} °C")

    def update_system(self, sys_telem: SystemTelemetry):
        if "sys_telem" not in self.diag_widgets:
            return

        w = self.diag_widgets["sys_telem"]

        w["mcu_temp"].set(f"{sys_telem.mcu_temp:.1f} °C")

        for mask, (canvas, led_id) in w["state_leds"].items():
            is_on = (sys_telem.efuse_states_mask & mask) != 0
            canvas.itemconfig(led_id, fill="green" if is_on else "red")

        for mask, (canvas, led_id) in w["fault_leds"].items():
            has_fault = (sys_telem.efuse_faults_mask & mask) != 0
            canvas.itemconfig(led_id, fill="red" if has_fault else "green")


class App:
    def __init__(self, root, config):
        self.root = root
        self.config = config
        self.bus = None
        self.last_rx_time = 0.0
        self.eFuseMasks = 0x00

        self.ui = TelemetryUI(
            root,
            config,
            self.send_efuse_command_wrapper,
            self.send_converter_command_wrapper
        )

        self.connect_bus()
        self.poll_can()

    def connect_bus(self):
        self.close_bus()
        try:
            self.bus = open_bus()
            self.last_rx_time = time.time()
            self.ui.set_status("Status: connected")
            print("CAN bus opened")
        except Exception as e:
            self.bus = None
            self.ui.set_status(f"Status: connect failed ({e})")
            print(f"Failed to open CAN bus: {e}")

    def close_bus(self):
        if self.bus is not None:
            try:
                self.bus.shutdown()
            except Exception:
                pass
            self.bus = None

    def reconnect_bus(self):
        self.ui.set_status("Status: reconnecting...")
        print("Reconnecting CAN bus...")
        self.close_bus()
        time.sleep(REOPEN_DELAY_S)
        self.connect_bus()

    def send_efuse_command_wrapper(self, mask: int):
        if self.bus is None:
            raise RuntimeError("CAN bus not connected")
        self.eFuseMasks ^= mask
        send_efuse_command(self.bus, self.eFuseMasks)
        print(f"Sent eFuse command: 0x{self.eFuseMasks:02X}")

    def send_converter_command_wrapper(self, mask: int):
        if self.bus is None:
            raise RuntimeError("CAN bus not connected")
        send_converter_command(self.bus, mask)
        print(f"Sent converter command: 0x{mask:02X}")

    def poll_can(self):
        try:
            if self.bus is None:
                self.connect_bus()
            else:
                msg = self.bus.recv(timeout=0.01)
                now = time.time()

                if msg is not None:
                    self.last_rx_time = now
                    can_id = f"0x{msg.arbitration_id:03X}"

                    if can_id in self.config:
                        msg_cfg = self.config[can_id]
                        msg_type = msg_cfg.get("type")

                        if msg_type == "regulator_telem":
                            reg = decode_regulator_telem(msg, msg_cfg)
                            self.ui.update_regulator(can_id, reg)

                        elif msg_type == "BMS_telem":
                            bms = decode_bms_telem(msg, msg_cfg)
                            self.ui.update_bms(can_id, bms)

                        elif msg_type == "sys_telem":
                            sys_telem = decode_sys_telem(msg, msg_cfg)
                            self.ui.update_system(sys_telem)

                        self.ui.set_status("Status: receiving")

                elif (now - self.last_rx_time) > STALE_TIMEOUT_S:
                    self.ui.set_status("Status: no frames, reopening adapter")
                    self.reconnect_bus()

        except Exception as e:
            print(f"CAN error: {e}")
            self.ui.set_status(f"Status: error ({e}), reopening")
            self.reconnect_bus()

        self.root.after(20, self.poll_can)

    def shutdown(self):
        self.close_bus()
        self.root.destroy()


def main():
    config = load_config(CONFIG_PATH)

    root = tk.Tk()
    app = App(root, config)

    root.protocol("WM_DELETE_WINDOW", app.shutdown)
    root.mainloop()


if __name__ == "__main__":
    main()