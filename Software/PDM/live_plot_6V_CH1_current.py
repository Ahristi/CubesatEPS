import json
import time
import csv
from datetime import datetime

import can
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

CONFIG_PATH = "conversions.json"

CAN_INTERFACE = "pcan"
CAN_CHANNEL = "PCAN_USBBUS1"
CAN_BITRATE = 125000

TARGET_CAN_ID = 0x322
PLOT_WINDOW_S = 20.0
RECV_TIMEOUT_S = 0.05
REOPEN_DELAY_S = 0.5


def load_config(path: str) -> dict:
    with open(path, "r") as f:
        return json.load(f)


def open_bus() -> can.Bus:
    return can.Bus(
        interface=CAN_INTERFACE,
        channel=CAN_CHANNEL,
        bitrate=CAN_BITRATE,
    )


def read_u16_be(data: bytes, offset: int) -> int:
    return (data[offset] << 8) | data[offset + 1]


def decode_6v_ch1_current(msg: can.Message, msg_cfg: dict) -> float:
    sig = msg_cfg["signals"]["current_ch1"]
    raw = read_u16_be(msg.data, sig["offset"])
    return raw * sig["scale"]


class LiveCurrentPlot:
    def __init__(self):
        self.config = load_config(CONFIG_PATH)

        if "0x322" not in self.config:
            raise KeyError("0x322 not found in conversions.json")

        self.msg_cfg = self.config["0x322"]
        self.bus = None
        self.start_time = time.time()
        self.last_rx_time = 0.0

        # FULL DATASET (no truncation)
        self.times = []
        self.currents = []

        self.fig, self.ax = plt.subplots()
        (self.line,) = self.ax.plot([], [])
        self.status_text = self.ax.text(
            0.02, 0.95, "Starting...", transform=self.ax.transAxes, va="top"
        )

        self.ax.set_title("6V CH1 Current")
        self.ax.set_xlabel("Time (s)")
        self.ax.set_ylabel("Current (A)")
        self.ax.grid(True)

        self.connect_bus()

    def connect_bus(self):
        self.close_bus()
        self.bus = open_bus()
        self.last_rx_time = time.time()
        print("CAN bus opened")

    def close_bus(self):
        if self.bus is not None:
            try:
                self.bus.shutdown()
            except Exception:
                pass
            self.bus = None

    def reconnect_bus(self):
        print("Reconnecting CAN...")
        self.status_text.set_text("Reconnecting CAN...")
        self.close_bus()
        time.sleep(REOPEN_DELAY_S)
        self.connect_bus()

    def poll_messages(self):
        if self.bus is None:
            self.connect_bus()

        now = time.time()
        got_frame = False

        while True:
            msg = self.bus.recv(timeout=RECV_TIMEOUT_S)
            if msg is None:
                break

            if msg.arbitration_id != TARGET_CAN_ID:
                continue

            current = decode_6v_ch1_current(msg, self.msg_cfg)
            t = time.time() - self.start_time

            self.times.append(t)
            self.currents.append(current)
            self.last_rx_time = time.time()
            got_frame = True

        if got_frame:
            self.status_text.set_text(f"Receiving | {self.currents[-1]:.3f} A")
        elif (now - self.last_rx_time) > 2.0:
            self.status_text.set_text("No frames, reconnecting...")
            self.reconnect_bus()
        else:
            self.status_text.set_text("Waiting for 0x322...")

    def update_plot(self, _):
        try:
            self.poll_messages()
        except Exception as e:
            print(f"CAN error: {e}")
            self.status_text.set_text(f"Error: {e}")
            self.reconnect_bus()
            return self.line, self.status_text

        if not self.times:
            return self.line, self.status_text

        latest_t = self.times[-1]
        xmin = max(0.0, latest_t - PLOT_WINDOW_S)
        xmax = max(PLOT_WINDOW_S, latest_t)

        visible_x = []
        visible_y = []
        for t, y in zip(self.times, self.currents):
            if t >= xmin:
                visible_x.append(t)
                visible_y.append(y)

        self.line.set_data(visible_x, visible_y)
        self.ax.set_xlim(xmin, xmax)

        ymin = min(visible_y)
        ymax = max(visible_y)
        pad = 0.1 * (ymax - ymin) if ymax != ymin else 0.05
        self.ax.set_ylim(ymin - pad, ymax + pad)

        return self.line, self.status_text

    def save_to_csv(self):
        filename = f"6V_CH1_current_{datetime.now().strftime('%Y%m%d_%H%M%S')}.csv"
        print(f"Saving to {filename}")

        with open(filename, "w", newline="") as f:
            writer = csv.writer(f)
            writer.writerow(["time_s", "current_A"])

            for t, i in zip(self.times, self.currents):
                writer.writerow([t, i])

        print("Save complete")

    def run(self):
        try:
            self.ani = FuncAnimation(self.fig, self.update_plot, interval=50)
            plt.show()
        finally:
            self.save_to_csv()
            self.close_bus()


def main():
    app = LiveCurrentPlot()
    app.run()


if __name__ == "__main__":
    main()