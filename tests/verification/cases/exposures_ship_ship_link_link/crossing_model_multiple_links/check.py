import os
import numpy as np
import pandas as pd

# Workspace and file paths
WORKSPACE_INI = os.getcwd()
OUTPUT_DIR = "Results"
OUTPUT_FILE = "linklink.csv"

# Load data
exposures_csv = os.path.join(WORKSPACE_INI, OUTPUT_DIR, OUTPUT_FILE)
df = pd.read_csv(exposures_csv, delimiter=",", skipinitialspace=True)

# Result array
results = np.zeros(4)

# ---------------------------------------------------------------------------
# Check the total number of exposures
# ---------------------------------------------------------------------------
if len(df) == 9:
    results[0] = 1
else:
    print("Total number of exposures is incorrect.")

# ---------------------------------------------------------------------------
# Check the presence of the single non-symmetric crossing exposure
# ---------------------------------------------------------------------------
pair0 = pd.concat([
    df[(df["linkA_uid"] == "L1") & (df["linkB_uid"] == "L3")],
    df[(df["linkA_uid"] == "L3") & (df["linkB_uid"] == "L1")]
])

# Unique link pair must appear exactly once
if len(pair0) == 1:
    results[1] = 1
else:
    print("Unique link pair not detected.")

# Exposure must be non-zero
if pair0["exposures"].sum() != 0:
    results[2] = 1
else:
    print("Unique link pair should have a non-zero crossing exposure.")

# Angle must be non-zero (likely intentional duplicate of exposure check)
if pair0["angle"].sum() != 0:
    results[3] = 1
else:
    print("Unique link pair should have a non-zero crossing angle.")

# ---------------------------------------------------------------------------
# Check symmetric crossing exposures
# ---------------------------------------------------------------------------
pair1 = np.array([["L0", "L1"], ["L0", "L2"], ["L0", "L3"]])
pair2 = np.array([["L3", "L4"], ["L2", "L4"], ["L1", "L4"]])

results_aux = np.zeros((3, 4))

for i, (ilinks, jlinks) in enumerate(zip(pair1, pair2)):

    cross0 = pd.concat([
        df[(df["linkA_uid"] == ilinks[0]) & (df["linkB_uid"] == ilinks[1])],
        df[(df["linkA_uid"] == ilinks[1]) & (df["linkB_uid"] == ilinks[0])]
    ])

    cross1 = pd.concat([
        df[(df["linkA_uid"] == jlinks[0]) & (df["linkB_uid"] == jlinks[1])],
        df[(df["linkA_uid"] == jlinks[1]) & (df["linkB_uid"] == jlinks[0])]
    ])

    # Check setup detection
    if len(cross0) == 1:
        results_aux[i, 0] = 1
    else:
        print("First link pair is not detected.")

    if len(cross1) == 1:
        results_aux[i, 1] = 1
    else:
        print("Second link pair is not detected.")

    # Compare exposures
    cross0_exp = cross0["exposures"].sum()
    cross1_exp = cross1["exposures"].sum()
    exp_ratio = cross0_exp / cross1_exp

    if np.abs(1 - exp_ratio) < 1e-3:
        results_aux[i, 2] = 1
    else:
        print("Mismatch of compared crossing exposures.")

    # Compare angles
    cross0_angle = cross0["angle"].sum()
    cross1_angle = cross1["angle"].sum()
    angle_ratio = cross0_angle / cross1_angle

    if np.abs(1 - angle_ratio) < 1e-3:
        results_aux[i, 3] = 1
    else:
        print("Mismatch of compared crossing angles.")

# ---------------------------------------------------------------------------
# Combine and check final result
# ---------------------------------------------------------------------------
results = np.concatenate([results, results_aux.ravel()])

if np.prod(results) > 0.5:
    print("ok")