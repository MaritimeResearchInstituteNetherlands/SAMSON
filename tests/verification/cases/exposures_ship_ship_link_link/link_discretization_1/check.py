import os
import numpy as np
import pandas as pd

# Workspace and file configuration
WORKSPACE_INI = os.getcwd()
OUTPUT_DIR = "Results"
OUTPUT_FILE = "link.csv"
TRAFFIC_DIR = "Traffic"
TRAFFIC_FILE = "shiplinks.csv"

# Constants
TOL = 1e-5

# Read exposure results
exposures_csv = os.path.join(WORKSPACE_INI, OUTPUT_DIR, OUTPUT_FILE)
df = pd.read_csv(exposures_csv, delimiter=",", skipinitialspace=True)

# Read traffic data
shiplinks_csv = os.path.join(WORKSPACE_INI, TRAFFIC_DIR, TRAFFIC_FILE)
df_traffic = pd.read_csv(shiplinks_csv, delimiter=",", skipinitialspace=True)

results = np.zeros(7)

# ----------------------------------------------------------------------
# Check the total number of exposures
# ----------------------------------------------------------------------
if len(df) == 3:
    results[0] = 1
else:
    print("Total number of exposures is incorrect.")

# ----------------------------------------------------------------------
# Check overtaking exposures
# ----------------------------------------------------------------------
over0 = df[(df["link_uid"] == "L0") & (df["exposures"] != 0)]

over1 = df[
    ((df["link_uid"] == "L1") | (df["link_uid"] == "L2"))
    & (df["exposures"] != 0)
]

# First setup: expect exactly one overtaking exposure
if len(over0) == 1:
    results[1] = 1
else:
    print("First set-up should have one overtaking exposure.")

# Second setup: expect two overtaking exposures
if len(over1) == 2:
    results[2] = 1
else:
    print("Second set-up should have two overtaking exposures.")

# ----------------------------------------------------------------------
# Compare overtaking exposure magnitudes
# ----------------------------------------------------------------------
over0_exp = over0["exposures"].sum()
over1_exp = over1["exposures"].sum()
over_exp_ratio = over0_exp / over1_exp

if np.abs(1 - over_exp_ratio) < TOL:
    results[3] = 1
else:
    print("Mismatch of compared overtaking exposures.")

# ----------------------------------------------------------------------
# Compare overtaking exposure lengths
# ----------------------------------------------------------------------
over0_len = over0["link_length"].sum()
over1_len = over1["link_length"].sum()
over_len_ratio = over0_len / over1_len

if np.abs(1 - over_len_ratio) < TOL:
    results[4] = 1
else:
    print("Mismatch of compared overtaking exposure lengths.")

# ----------------------------------------------------------------------
# Compare ship categories (A)
# ----------------------------------------------------------------------
shipcat0 = df_traffic.loc[
    df_traffic["uid"].isin(over0["trafficA_uid"].unique()),
    "ship_uid",
].unique()

shipcat1 = df_traffic.loc[
    df_traffic["uid"].isin(over1["trafficA_uid"].unique()),
    "ship_uid",
].unique()

if np.array_equal(shipcat0, shipcat1):
    results[5] = 1
else:
    print("Mismatch of compared ship categories (A).")

# ----------------------------------------------------------------------
# Compare ship categories (B)
# ----------------------------------------------------------------------
shipcat0 = df_traffic.loc[
    df_traffic["uid"].isin(over0["trafficB_uid"].unique()),
    "ship_uid",
].unique()

shipcat1 = df_traffic.loc[
    df_traffic["uid"].isin(over1["trafficB_uid"].unique()),
    "ship_uid",
].unique()

if np.array_equal(shipcat0, shipcat1):
    results[6] = 1
else:
    print("Mismatch of compared ship categories (B).")

# ----------------------------------------------------------------------
# Final status
# ----------------------------------------------------------------------
if np.prod(results) > 0.5:
    print("ok")