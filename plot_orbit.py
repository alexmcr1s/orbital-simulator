import pandas as pd
import matplotlib.pyplot as plt

# Visualizing orbit from .csv file
data = pd.read_csv("orbit.csv")

x_km = data["x"] / 1000.0
y_km = data["y"] / 1000.0

plt.plot(x_km, y_km)

earth = plt.Circle((0, 0), 6371, fill=False)
plt.gca().add_patch(earth)

plt.xlabel("X Position (km)")
plt.ylabel("Y Position (km)")
plt.title("Simulated Orbit")

# Starting point
plt.scatter(x_km.iloc[0], y_km.iloc[0], label="Start")

plt.legend()

plt.axis("equal")

plt.savefig("orbit.png", dpi=300)