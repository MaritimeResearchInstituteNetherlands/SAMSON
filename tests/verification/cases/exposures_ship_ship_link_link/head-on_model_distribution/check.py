import os
import numpy as np
import pandas as pd

# Workspace and file configuration
WORKSPACE_INI = os.getcwd()
OUTPUT_DIR = "Results"
OUTPUT_FILE = "linklink.csv"
TRAFFIC_DIR = "Traffic"
TRAFFIC_FILE = "shiplinks.csv"

# Load exposure results
exposures_csv = os.path.join(WORKSPACE_INI, OUTPUT_DIR, OUTPUT_FILE)
df = pd.read_csv(exposures_csv, delimiter=",", skipinitialspace=True)

# Load traffic data
shiplinks_csv = os.path.join(WORKSPACE_INI, TRAFFIC_DIR, TRAFFIC_FILE)
df_traffic = pd.read_csv(shiplinks_csv, delimiter=",", skipinitialspace=True)

results = np.zeros(8)

# ----------------------------------------------------------------------
# Check the number of head-on exposures
# ----------------------------------------------------------------------
if len(df) == 6:
    results[0] = 1
else:
    print("Mismatch of head-on exposure number.")

# ----------------------------------------------------------------------
# Check unique linkA_uid values
# ----------------------------------------------------------------------
if (df["linkA_uid"].iloc[0] == df["linkA_uid"]).all():
    print("Unique linkA_uid values expected.")
else:
    results[1] = 1

# ----------------------------------------------------------------------
# Check unique linkB_uid values
# ----------------------------------------------------------------------
if (df["linkB_uid"].iloc[0] == df["linkB_uid"]).all():
    print("Unique linkB_uid values expected.")
else:
    results[2] = 1

# ----------------------------------------------------------------------
# Check identical angle values
# ----------------------------------------------------------------------
if (df["angle"].iloc[0] == df["angle"]).all():
    results[3] = 1
else:
    print("Identical angle values expected.")

# ----------------------------------------------------------------------
# Check identical shipA_uid values
# ----------------------------------------------------------------------
traffic_a_data = df_traffic.loc[df_traffic["uid"].isin(df["trafficA_uid"])]
if (traffic_a_data["ship_uid"].iloc[0] == traffic_a_data["ship_uid"]).all():
    results[4] = 1
else:
    print("Identical shipA_uid values expected.")

# ----------------------------------------------------------------------
# Check identical shipB_uid values
# ----------------------------------------------------------------------
traffic_b_data = df_traffic.loc[df_traffic["uid"].isin(df["trafficB_uid"])]
if (traffic_b_data["ship_uid"].iloc[0] == traffic_b_data["ship_uid"]).all():
    results[5] = 1
else:
    print("Identical shipB_uid values expected.")

# ----------------------------------------------------------------------
# Check identical exposure values
# ----------------------------------------------------------------------
if (df["exposures"].iloc[0] == df["exposures"]).all():
    results[6] = 1
else:
    print("Identical exposure values expected.")

# ----------------------------------------------------------------------
# Check non-zero exposure values
# ----------------------------------------------------------------------
if df["exposures"].iloc[0] != 0:
    results[7] = 1
else:
    print("Non-zero exposure values expected.")

# ----------------------------------------------------------------------
# Final result
# ----------------------------------------------------------------------
if np.prod(results) > 0.5:
    print("ok")