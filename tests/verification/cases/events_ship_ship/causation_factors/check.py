import os
import numpy as np
import pandas as pd

# Workspace and files
WORKSPACE_INI = os.getcwd()
OUTPUT_DIR = ""
OUTPUT_FILE = "results.pkl"

# Constants
Q1 = 0.001        # [s-1] passing ships per second
Q2 = 0.002        # [s-1] passing ships per second
U1 = 9            # [m/s] ship speed
U2 = 7            # [m/s] ship speed
L1 = 100.0        # [m] ship length
L2 = 200.0        # [m] ship length
B1 = 30.0         # [m] ship breadth
B2 = 40.0         # [m] ship breadth

CF1 = 1e-4
CF2 = 1e-3

# Load input data
results_pkl = os.path.join(WORKSPACE_INI, OUTPUT_DIR, OUTPUT_FILE)
df = pd.read_pickle(results_pkl)
df.columns = df.columns.str.strip()

angles = np.array(df["alpha"])
thetas = np.array(df["angle"])
struck_i = np.array(df["struck_chance_i"])
struck_j = np.array(df["struck_chance_j"])
exposures = np.array(df["exposures"])
events_i = np.array(df["events_i"])
events_j = np.array(df["events_j"])

results = []

# -------------------------------------------------------------------------
# Main check loop
# -------------------------------------------------------------------------
for theta, angle, exposure, s_i, s_j, event_i, event_j in zip(
    thetas,
    angles,
    exposures,
    struck_i,
    struck_j,
    events_i,
    events_j,
):

    u12 = np.sqrt(U1**2 + U2**2 - 2 * U1 * U2 * np.cos(theta))

    d1_1 = L2 * U1 * np.sin(theta) / u12
    d1_2 = B2 * np.sqrt(1 - (np.sin(theta) * U1 / u12) ** 2)
    d1 = d1_1 + d1_2

    d2_1 = L1 * U2 * np.sin(theta) / u12
    d2_2 = B1 * np.sqrt(1 - (np.sin(theta) * U2 / u12) ** 2)
    d2 = d2_1 + d2_2

    d12 = d1 + d2

    struck1 = d2 / d12
    struck2 = d1 / d12

    event_check = (struck1 * CF1 + struck2 * CF2) * exposure
    event_total = event_i + event_j

    if np.abs(event_check - event_total) < 1.0e-10:
        results.append(1)
    else:
        results.append(0)
        print(f"Mismatch for computed event for angle: {angle} deg.")
        print(f"Event frequency {event_total} computed, {event_check} expected.")

# -------------------------------------------------------------------------
# Final status
# -------------------------------------------------------------------------
if np.prod(results) > 0.5:
    print("ok")