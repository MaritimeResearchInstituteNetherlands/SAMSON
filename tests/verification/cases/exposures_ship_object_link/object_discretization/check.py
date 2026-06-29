import os
import numpy as np
import pandas as pd

# Workspace / file settings
WORKSPACE_INI = os.getcwd()
OUTPUT_DIR = "Results"
OUTPUT_FILE = "linkobject.csv"

# Load data
exposures_csv = os.path.join(WORKSPACE_INI, OUTPUT_DIR, OUTPUT_FILE)
df = pd.read_csv(exposures_csv, delimiter=",", skipinitialspace=True)

results = np.zeros(8)

# Check objects east
ram_exposures_0 = []
for i in range(3):
    uid_str = "OB" + str(i) + "0"
    subset = df.loc[df["objectUID"] == uid_str, "ramExposure"]
    if len(subset) == 1:
        ram_exposures_0.append(subset.iloc[0])
    else:
        ram_exposures_0.append(0)

check0 = np.sum(ram_exposures_0) == ram_exposures_0[0] * len(ram_exposures_0)
if np.prod(check0):
    results[0] = 1
else:
    results[0] = 0
    print("No identical exposures for the three objects east.")

if np.prod(ram_exposures_0) != 0:
    results[1] = 1
else:
    results[1] = 0
    print("Zero exposure detected for objects east.")

# Check objects north
ram_exposures_1 = []
for i in range(3):
    uid_str = "OB" + str(i) + "1"
    subset = df.loc[df["objectUID"] == uid_str, "ramExposure"]
    if len(subset) == 1:
        ram_exposures_1.append(subset.iloc[0])
    else:
        ram_exposures_1.append(0)

check1 = np.sum(ram_exposures_1) == ram_exposures_1[0] * len(ram_exposures_1)
if np.prod(check1):
    results[2] = 1
else:
    results[2] = 0
    print("No identical exposures for the three objects north.")

if np.prod(ram_exposures_1) != 0:
    results[3] = 1
else:
    results[3] = 0
    print("Zero exposure detected for objects north.")

# Check objects west
ram_exposures_2 = []
for i in range(3):
    uid_str = "OB" + str(i) + "2"
    subset = df.loc[df["objectUID"] == uid_str, "ramExposure"]
    if len(subset) == 1:
        ram_exposures_2.append(subset.iloc[0])
    else:
        ram_exposures_2.append(0)

check2 = np.sum(ram_exposures_2) == ram_exposures_2[0] * len(ram_exposures_2)
if np.prod(check2):
    results[4] = 1
else:
    results[4] = 0
    print("No identical exposures for the three objects west.")

if np.prod(ram_exposures_2) != 0:
    results[5] = 1
else:
    results[5] = 0
    print("Zero exposure detected for objects west.")

# Check objects south
ram_exposures_3 = []
for i in range(3):
    uid_str = "OB" + str(i) + "3"
    subset = df.loc[df["objectUID"] == uid_str, "ramExposure"]
    if len(subset) == 1:
        ram_exposures_3.append(subset.iloc[0])
    else:
        ram_exposures_3.append(0)

# Note: check3 below mirrors original logic (no change of behavior)
check3 = ram_exposures_3 == ram_exposures_3[0] * len(ram_exposures_3)

if np.sum(ram_exposures_3) == 0:
    results[6] = 1
else:
    results[6] = 0
    print("Non zero exposure detected for objects south.")

# Check exposure symmetry objects west and east
sumResults = 0
for i in range(3):
    if (abs(ram_exposures_0[i] / ram_exposures_2[i] - 1) < 0.01):
        sumResults += 1
if sumResults == 3:
    results[7] = 1
else:
    results[7] = 0
    print("Exposures with objects west and east must be equal.")

if np.prod(results) > 0.5:
    print("ok")