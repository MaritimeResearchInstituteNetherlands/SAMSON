import os
import subprocess

import geopandas as gpd
import numpy as np
import pandas as pd

# ---------------------------------------------------------------------------
# Constants / Configuration
# ---------------------------------------------------------------------------

HEIGHT = 8
START_DRAFT = 13
END_DRAFT = -5
NUM_DRAFTS = 10

WORKSPACE = os.getcwd()
TRAFFIC_DIR = os.path.join(".", "Traffic")
RESULTS_DIR = os.path.join(".", "Results")

SAMSON_CMD = [
    "SamsonCLI",
    "--config-yaml", "config.yaml",
    "--actions", "actions.txt",
]

RESULTS_FILENAME = "linkobject.csv"
SHIPS_FILENAME = "shipcategories.csv"

# Derived draft array (meters)
drafts = np.linspace(START_DRAFT, END_DRAFT, num=NUM_DRAFTS, endpoint=True)

# ---------------------------------------------------------------------------
# Load ship's draft
# ---------------------------------------------------------------------------

ships_path = os.path.join(TRAFFIC_DIR, SHIPS_FILENAME)
df_ships = pd.read_csv(ships_path)

# ---------------------------------------------------------------------------
# Draft loop and external tool execution
# ---------------------------------------------------------------------------

df = pd.DataFrame()

for idraft, draft in enumerate(drafts):
    
    df_ships.loc[0, "draft"] = draft
    df_ships.to_csv(ships_path, index=False, header=True)

    # Run Samson
    result = subprocess.run(
        SAMSON_CMD,
        cwd=WORKSPACE,
        capture_output=True,
        text=True,
        check=False,  # set True for strict failure handling
    )

    if result.stdout:
        print(result.stdout.strip())

    if result.returncode != 0:
        print(f"[SamsonCLI] Return code: {result.returncode}")
        if result.stderr:
            print(result.stderr.strip())

    # Read result
    results_path = os.path.join(RESULTS_DIR, RESULTS_FILENAME)
    if not os.path.exists(results_path):
        print("Results file not found: linkobject.csv")
        continue

    df_import = pd.read_csv(results_path)

    if df_import.empty:
        print("Empty linkcell.csv file.")
        continue
    else:
        df_import["draft"] = draft
        df_import["experiment"] = idraft
        df = pd.concat([df, df_import])
        
    #Remove file
    os.remove(results_path)

# ---------------------------------------------------------------------------
# Final results
# ---------------------------------------------------------------------------

print(df.loc[:, ["driftExposure", "draft", "experiment"]])

df.to_pickle("results.pkl")