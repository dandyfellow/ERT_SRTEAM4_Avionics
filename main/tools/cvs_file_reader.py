import os
import glob
import pandas as pd
import matplotlib.pyplot as plt
from datetime import datetime

# === 1. Find the latest flight log ===
latest_file = "unknown"

def get_latest_log_file(log_dir="logs"):
    global latest_file
    files = glob.glob(os.path.join(log_dir, "flight_*.csv"))
    if not files:
        print("[!] No flight logs found.")
        return None
    latest_file = max(files, key=os.path.getmtime)
    print(f"[✓] Loaded: {latest_file}")
    return latest_file

    #latest_file = NAME   #change here

# === 2. Load CSV into DataFrame ===
def load_log_data(filename):
    df = pd.read_csv(filename)
    df["timestamp"] = pd.to_datetime(df["timestamp"])
    return df

# === 3. Plot the telemetry ===
def plot_telemetry(df):
    time = df["timestamp"]
    global latest_file
    fig, axs = plt.subplots(4, 1, figsize=(12, 10), sharex=True)
    fig.suptitle(f"Flight Telemetry : {latest_file}")

    # Orientation
    axs[0].plot(time, df["roll"], label="Roll [x]")
    axs[0].plot(time, df["pitch"], label="Pitch [y]")
    axs[0].plot(time, df["yaw"], label="Yaw [z]")

    axs[0].set_ylabel("Radians")
    axs[0].legend()
    axs[0].grid(True)

    # Acceleration
    axs[1].plot(time, df["ax"], label="Ax")
    axs[1].plot(time, df["ay"], label="Ay")
    axs[1].plot(time, df["az"], label="Az")
    axs[1].set_ylabel("Accel (g)")
    axs[1].legend()
    axs[1].grid(True)

    # Altitude & Max Altitude + Deployment Marker
    axs[2].plot(time, df["altitude"], label="Altitude", color='tab:blue')
    axs[2].plot(time, df["max_altitude"], label="Max Altitude", linestyle="--", color='tab:orange')

    # Highlight when max altitude is first reached
    if "max_altitude_reached" in df.columns:
        reached_indices = df.index[df["max_altitude_reached"] == 1].tolist()
        if reached_indices:
            i = reached_indices[0]
            axs[2].axvline(time[i], color="black",  label="Max Alt Reached")

    # Highlight parachute deployment
    if "parachute_deployed" in df.columns:
        deploy_indices = df.index[df["parachute_deployed"] == 1].tolist()
        if deploy_indices:
            i = deploy_indices[0]
            axs[2].axvline(time[i], color="red", linestyle="--", label="Parachute Deployed")

    axs[2].set_ylabel("Altitude (m)")
    axs[2].legend()
    axs[2].grid(True)


# Temperature & Pressure
# Plot Temperature on primary y-axis
    axs[3].plot(time, df["temp"], label="Temperature (°C)", color='tab:red')
    axs[3].set_ylabel("Temperature (°C)", color='tab:red')
    axs[3].tick_params(axis='y', labelcolor='tab:red')

    # Create a twin y-axis for Pressure
    ax3_twin = axs[3].twinx()
    ax3_twin.plot(time, df["pressure"], label="Pressure (Pa)", color='tab:blue')
    ax3_twin.set_ylabel("Pressure (Pa)", color='tab:blue')
    ax3_twin.tick_params(axis='y', labelcolor='tab:blue')

    # Set common x-axis label and grid
    axs[3].set_xlabel("Time")
    axs[3].grid(True)

    # Optional: add legends
    lines_1, labels_1 = axs[3].get_legend_handles_labels()
    lines_2, labels_2 = ax3_twin.get_legend_handles_labels()
    axs[3].legend(lines_1 + lines_2, labels_1 + labels_2, loc='upper left')


    plt.tight_layout(rect=[0, 0, 1, 0.96])
    plt.xticks(rotation=30)
    plt.show()

# === Main runner ===
if __name__ == "__main__":
    log_path = get_latest_log_file()
    if log_path:
        df = load_log_data(log_path)
        plot_telemetry(df)
