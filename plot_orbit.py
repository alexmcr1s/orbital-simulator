import pandas as pd
import matplotlib.pyplot as plt

# Visualizing orbit from .csv file
data = pd.read_csv("orbit.csv")

x_km = data["x"] / 1000.0
y_km = data["y"] / 1000.0

radius_km = (x_km**2 + y_km**2) ** 0.5

periapsis_index = radius_km.idxmin()
apoapsis_index = radius_km.idxmax()

# Periapsis point
plt.scatter(
    x_km.loc[periapsis_index],
    y_km.loc[periapsis_index],
    label="Periapsis"
)

# Apoapsis point
plt.scatter(
    x_km.loc[apoapsis_index],
    y_km.loc[apoapsis_index],
    label="Apoapsis"
)

plt.plot(x_km, y_km, label="Trajectory")

earth = plt.Circle((0, 0), 6371, fill=False)
plt.gca().add_patch(earth)

plt.xlabel("X Position (km)")
plt.ylabel("Y Position (km)")
plt.title("Simulated Orbit")

# Starting point
plt.scatter(x_km.iloc[0], y_km.iloc[0], label="Start")

# Final point
plt.scatter(x_km.iloc[-1], y_km.iloc[-1], label="End")

plt.legend()

plt.axis("equal")

plt.savefig("orbit.png", dpi=300)