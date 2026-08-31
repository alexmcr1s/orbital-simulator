import pandas as pd
import matplotlib

matplotlib.use("TkAgg")

import matplotlib.pyplot as plt
import math
from matplotlib.animation import FuncAnimation

# Visualizing orbit from .csv file
data = pd.read_csv("orbit.csv")

# Transferring metadata
metadata = pd.read_csv("simulation_metadata.csv")

trajectory_type = metadata["trajectory_type"].iloc[0]
simulation_result = metadata["simulation_result"].iloc[0]
integrator = metadata["integrator"].iloc[0]

periapsis_altitude_km = metadata["periapsis_altitude_km"].iloc[0]
apoapsis_altitude_km = metadata["apoapsis_altitude_km"].iloc[0]
periapsis_direction_deg = metadata["periapsis_direction_deg"].iloc[0]

x_km = data["x"] / 1000.0
y_km = data["y"] / 1000.0

max_frames = 300
frame_step = max(1, len(data) // max_frames)
animation_frames = range(0, len(data), frame_step)

trajectory_line, = plt.plot([], [], label="Trajectory")
spacecraft, = plt.plot([], [], marker="o", label="Spacecraft")

earth = plt.Circle((0, 0), 6371, fill=False)
plt.gca().add_patch(earth)

plt.xlabel("X Position (km)")
plt.ylabel("Y Position (km)")
plt.title(f"Simulated Orbit — {trajectory_type} | "f"{simulation_result} | {integrator}")

# Starting point
plt.scatter(x_km.iloc[0], y_km.iloc[0], label="Start")

# Final point
if simulation_result == "Impact":
    end_label = "Impact"
elif simulation_result == "Escape":
    end_label = "Escape Limit"
else:
    end_label = "End"
    
plt.scatter(x_km.iloc[-1], y_km.iloc[-1], label=end_label)

# Periapsis point
if (simulation_result == "Orbit" and pd.notna(periapsis_altitude_km) and pd.notna(periapsis_direction_deg)):
    periapsis_radius_km = 6371 + periapsis_altitude_km

    periapsis_angle_rad = math.radians(periapsis_direction_deg)

    periapsis_x = periapsis_radius_km * math.cos(periapsis_angle_rad)
    periapsis_y = periapsis_radius_km * math.sin(periapsis_angle_rad)

    plt.scatter(periapsis_x, periapsis_y, label="Periapsis")

# Apoapsis point
if (simulation_result == "Orbit" and pd.notna(apoapsis_altitude_km) and pd.notna(periapsis_direction_deg)):
    apoapsis_radius_km = 6371 + apoapsis_altitude_km

    apoapsis_angle_deg = periapsis_direction_deg + 180.0
    apoapsis_angle_rad = math.radians(apoapsis_angle_deg)

    apoapsis_x = apoapsis_radius_km * math.cos(apoapsis_angle_rad)
    apoapsis_y = apoapsis_radius_km * math.sin(apoapsis_angle_rad)

    plt.scatter(apoapsis_x, apoapsis_y, label="Apoapsis")

def update(frame):
    trajectory_line.set_data(
        x_km.iloc[:frame + 1],
        y_km.iloc[:frame + 1]
    )
    
    spacecraft.set_data(
        [x_km.iloc[frame]],
        [y_km.iloc[frame]]
    )
    
    return trajectory_line, spacecraft

animation = FuncAnimation(
    plt.gcf(), 
    update, 
    frames=animation_frames, 
    interval=20, 
    repeat=False
)

plt.legend()
plt.axis("equal")

# plt.savefig("orbit.png", dpi=300)
animation.save("orbit.gif", writer="pillow", fps=30)
plt.close()