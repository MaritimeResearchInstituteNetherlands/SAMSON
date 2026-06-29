import os
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

plt.close("all")

# Workspace and file settings
WORKSPACE_INI = os.getcwd()
OUTPUT_DIR = "Results"
OUTPUT_FILE = "linkobject.csv"

# Load data
exposures_csv = os.path.join(WORKSPACE_INI, OUTPUT_DIR, OUTPUT_FILE)
df = pd.read_csv(exposures_csv, delimiter=",", skipinitialspace=True)

# Prepare arrays
rows = len(df)
uid = np.flip(np.arange(rows))

ram_exposure_prev = 0
drift_exposure_prev = 0

ram_arr = []
drift_arr = []
uid_arr = []

results = np.zeros((2, rows))

# Check the ram and drift exposure gradient
if len(uid) != 0:
    for i, uid_i in enumerate(uid):
        uid_str = f"OB{uid_i:02d}"

        ram_exposure = df.loc[df["objectUID"] == uid_str, "ramExposure"].iloc[0]
        drift_exposure = df.loc[df["objectUID"] == uid_str, "driftExposure"].iloc[0]

        # Ram exposure gradient
        if ram_exposure > ram_exposure_prev:
            results[0, i] = 1
        else:
            results[0, i] = 0
            print("Incorrect gradient ram exposures")

        # Drift exposure gradient
        if drift_exposure > drift_exposure_prev:
            results[1, i] = 1
        else:
            results[1, i] = 0
            print("Incorrect gradient drift exposures")

        # Update previous exposures
        ram_exposure_prev = ram_exposure
        drift_exposure_prev = drift_exposure

        # Store results
        ram_arr.append(ram_exposure)
        drift_arr.append(drift_exposure)
        uid_arr.append(uid_str)

# Flatten to 1D for final test
results = results.ravel()

if np.prod(results) > 0.5:
    print("ok")

# Plotting
fig, ax = plt.subplots(1, 1, figsize=(6, 4), dpi=160)

ax.plot(
    uid,
    ram_arr,
    linestyle="-",
    marker="o",
    color="blue",
    label="samsonCLI Ram",
)

ax.plot(
    uid,
    drift_arr,
    linestyle="-",
    marker="o",
    color="green",
    label="samsonCLI Drift",
)

ax.set_xticks(uid, labels=uid_arr, rotation=45)
ax.set_ylabel('Exposure frequency $[s^{-1}]$')
ax.yaxis.set_ticks([])
ax.yaxis.set_ticklabels([])
ax.legend()

fig.savefig("figure1.svg", bbox_inches="tight")