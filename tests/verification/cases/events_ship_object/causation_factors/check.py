import os
import numpy as np
import pandas as pd

# Workspace settings
WORKSPACE_INI = os.getcwd()
OUTPUT_DIR = "Results"
OUTPUT_FILE = "linkobject.csv"

# Load results
exposures_csv = os.path.join(WORKSPACE_INI, OUTPUT_DIR, OUTPUT_FILE)
df = pd.read_csv(exposures_csv, delimiter=",", skipinitialspace=True)

# Constants
DRIFT_CAUS = 1.0e-5
RAM_CAUS = 1.0e-6

results = np.zeros(5)

# Check number of exposures/events
if len(df) == 1:
    results[0] = 1
else:
    results[0] = 0
    print("Incorrect number of exposures/events.")

# Check drift event
drift_exposure = df["driftExposure"].iloc[0]

if drift_exposure != 0:
    results[1] = 1

    drift_event = DRIFT_CAUS * drift_exposure
    drift_ratio = df["driftEvents"].iloc[0] / drift_event
    drift_tol = 1e-6

    if abs(1 - drift_ratio) < drift_tol:
        results[2] = 1
    else:
        results[2] = 0
        print("Incorrect drift event.")
else:
    results[1:3] = 0
    print("Drift exposure should not be equal to zero.")

# Check ram event
ram_exposure = df["ramExposure"].iloc[0]

if ram_exposure != 0:
    results[3] = 1

    ram_event = RAM_CAUS * ram_exposure
    ram_ratio = df["ramEvents"].iloc[0] / ram_event
    ram_tol = 1e-6

    if abs(1 - ram_ratio) < ram_tol:
        results[4] = 1
    else:
        results[4] = 0
        print("Incorrect ram event.")
else:
    results[3:5] = 0
    print("Ram exposure should not be equal to zero.")

# Final check
if np.prod(results) > 0.5:
    print("ok")