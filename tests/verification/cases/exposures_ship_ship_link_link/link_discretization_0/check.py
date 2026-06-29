import os
import numpy as np
import pandas as pd

# Workspace and file configuration
WORKSPACE_INI = os.getcwd()
OUTPUT_DIR = "Results"
OUTPUT_FILE = "linklink.csv"

# Load exposure results
exposures_csv = os.path.join(WORKSPACE_INI, OUTPUT_DIR, OUTPUT_FILE)
df = pd.read_csv(exposures_csv, delimiter=",", skipinitialspace=True)

results = np.zeros(7)

# ----------------------------------------------------------------------
# Check the total number of exposures
# ----------------------------------------------------------------------
if len(df) == 5:
    results[0] = 1
else:
    print("Total number of exposures is incorrect.")

# ----------------------------------------------------------------------
# Crossing exposures
# ----------------------------------------------------------------------
cross0 = pd.concat([
    df[(df["linkA_uid"] == "L0") & (df["linkB_uid"] == "L2")],
    df[(df["linkA_uid"] == "L2") & (df["linkB_uid"] == "L0")],
])

cross1 = pd.concat([
    df[(df["linkA_uid"] == "L5") & (df["linkB_uid"] == "L6")],
    df[(df["linkA_uid"] == "L6") & (df["linkB_uid"] == "L5")],
])

# First setup: one crossing exposure expected
if len(cross0) == 1:
    results[1] = 1
else:
    print("First set-up should have one crossing exposure.")

# Second setup: one crossing exposure expected
if len(cross1) == 1:
    results[2] = 1
else:
    print("Second set-up should have one crossing exposure.")

# Compare crossing exposures of the two setups
cross0_exp = cross0["exposures"].sum()
cross1_exp = cross1["exposures"].sum()
cross_exp_ratio = cross0_exp / cross1_exp

if np.abs(1 - cross_exp_ratio) < 1e-5:
    results[3] = 1
else:
    print("Mismatch of compared crossing exposures.")

# ----------------------------------------------------------------------
# Head-on exposures
# ----------------------------------------------------------------------
head0 = pd.concat([
    df[(df["linkA_uid"] == "L1") & (df["linkB_uid"] == "L3")],
    df[(df["linkA_uid"] == "L3") & (df["linkB_uid"] == "L1")],
    df[(df["linkA_uid"] == "L2") & (df["linkB_uid"] == "L4")],
    df[(df["linkA_uid"] == "L4") & (df["linkB_uid"] == "L2")],
])

head1 = pd.concat([
    df[(df["linkA_uid"] == "L6") & (df["linkB_uid"] == "L7")],
    df[(df["linkA_uid"] == "L7") & (df["linkB_uid"] == "L6")],
])

# First setup: two head-on exposures expected
if len(head0) == 2:
    results[4] = 1
else:
    print("First set-up should have two head-on exposures.")

# Second setup: one head-on exposure expected
if len(head1) == 1:
    results[5] = 1
else:
    print("Second set-up should have one head-on exposure.")

# Compare head-on exposures of the two setups
head0_exp = head0["exposures"].sum()
head1_exp = head1["exposures"].sum()
head_exp_ratio = head0_exp / head1_exp

if np.abs(1 - head_exp_ratio) < 1e-5:
    results[6] = 1
else:
    print("Mismatch of compared head-on exposures.")

# ----------------------------------------------------------------------
# Final status
# ----------------------------------------------------------------------
if np.prod(results) > 0.5:
    print("ok")