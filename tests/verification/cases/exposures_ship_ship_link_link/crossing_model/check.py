import os
import numpy as np
import pandas as pd

# Workspace and file settings
WORKSPACE_INI = os.getcwd()
OUTPUT_DIR = ""
OUTPUT_FILE = "results.pkl"

# Constants
Q1 = 0.001       # [s-1] passing ships per second
Q2 = 0.002       # [s-1] passing ships per second
U1 = 9           # [m/s] ship speed
U2 = 7           # [m/s] ship speed
L1 = 100.0       # [m] ship length
L2 = 200.0       # [m] ship length
B1 = 30.0        # [m] ship breadth
B2 = 40.0        # [m] ship breadth
TOL = 5.0e-4     # exposure tolerance

# Load results
exposures_pkl = os.path.join(WORKSPACE_INI, OUTPUT_DIR, OUTPUT_FILE)
df = pd.read_pickle(exposures_pkl)
df.columns = df.columns.str.strip()

angles = np.array(df["alpha"])
exposures = np.array(df["exposures"])

results = []

# ---------------------------------------------------------------------
# Main exposure validation loop
# ---------------------------------------------------------------------
for angle, exposure in zip(angles, exposures):

    theta = np.deg2rad(angle)
    angle_check = angle % 180

    if 10 < angle_check < 170:
        u12 = np.sqrt(U1**2 + U2**2 - 2 * U1 * U2 * np.cos(theta))

        d1_1 = L1 * U2 * np.sin(theta) / u12
        d1_2 = B1 * np.sqrt(1 - (np.sin(theta) * U2 / u12) ** 2)
        d1 = d1_1 + d1_2

        d2_1 = L2 * U1 * np.sin(theta) / u12
        d2_2 = B2 * np.sqrt(1 - (np.sin(theta) * U1 / u12) ** 2)
        d2 = d2_1 + d2_2

        d12 = d1 + d2

        p12 = d12 / np.sin(theta)
        ndot = p12 * Q1 * Q2 * u12 / (U1 * U2)  # [s-1] exposures per second

    else:
        ndot = 0

    if np.abs(exposure - ndot) < TOL:
        results.append(1)
    else:
        results.append(0)
        print(f"Mismatch for computed exposure for angle: {angle} deg.")

# ---------------------------------------------------------------------
# Final result
# ---------------------------------------------------------------------
if np.prod(results) > 0.5:
    print("ok")