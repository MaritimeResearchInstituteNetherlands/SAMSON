import os
import numpy as np
import pandas as pd

# Workspace and file configuration
WORKSPACE_INI = os.getcwd()
OUTPUT_DIR = "Results"
OUTPUT_FILE = "link.csv"

# Load results
exposures_csv = os.path.join(WORKSPACE_INI, OUTPUT_DIR, OUTPUT_FILE)
df = pd.read_csv(exposures_csv, delimiter=",", skipinitialspace=True)

results = np.zeros(3)

# ----------------------------------------------------------------------
# Check the number of head-on exposures
# ----------------------------------------------------------------------
if len(df) == 6:
    results[0] = 1
else:
    print("Mismatch of head-on exposure number.")

# ----------------------------------------------------------------------
# Check identical link length values
# ----------------------------------------------------------------------
if (df["link_length"].iloc[0] == df["link_length"]).all():
    results[1] = 1
else:
    print("Identical link length values expected.")

# ----------------------------------------------------------------------
# Check identical *non-zero* exposure values
# ----------------------------------------------------------------------
non_zero_exposures = df["exposures"][df["exposures"] != 0]

if (non_zero_exposures == non_zero_exposures.max()).all():
    results[2] = 1
else:
    print("Identical exposure values expected.")

# ----------------------------------------------------------------------
# Final validation
# ----------------------------------------------------------------------
if np.prod(results) > 0.5:
    print("ok")