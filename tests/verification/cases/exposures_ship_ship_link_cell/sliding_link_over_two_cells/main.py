import os
import subprocess

import numpy as np
import pandas as pd


# ---------------------------------------------------------------------------
# Constants / Configuration
# ---------------------------------------------------------------------------

LON_START = 3.82
LON_DELTA = 0.04
LON_END = 3.90
NUM_LONS = 10

WORKSPACE = os.getcwd()
MODEL_DATA_DIR = os.path.join(".", "ModelData")
RESULTS_DIR = os.path.join(".", "Results")
AREA_DIR = os.path.join(".", "Area")

SAMSON_CMD = [
    "SamsonCLI",
    "--config-yaml",
    "config.yaml",
    "--actions",
    "actions.txt",
]

RESULTS_FILENAME = "linkcell.csv"
VERTICES_FILENAME = "vertices.csv"


# ---------------------------------------------------------------------------
# Load input vertices
# ---------------------------------------------------------------------------

vertices_path = os.path.join(AREA_DIR, VERTICES_FILENAME)

# Load vertices file once
df_vertices = pd.read_csv(vertices_path)

# Generate longitude values
lons = np.linspace(
    LON_START,
    LON_END,
    num=NUM_LONS,
    endpoint=True,
)


# ---------------------------------------------------------------------------
# Translation loop and external tool execution
# ---------------------------------------------------------------------------

df = pd.DataFrame()

for ilon, lon in enumerate(lons, start=0):

    # Update vertices
    df_vertices.loc[0, "lon"] = lon
    df_vertices.loc[1, "lon"] = lon + LON_DELTA

    # Write updated vertices
    df_vertices.to_csv(vertices_path, index=False, header=True)

    # Run SamsonCLI
    result = subprocess.run(
        SAMSON_CMD,
        cwd=WORKSPACE,
        capture_output=True,
        text=True,
        check=False,
    )

    if result.stdout:
        print(result.stdout.strip())

    if result.returncode != 0:
        print(f"[SamsonCLI] Return code: {result.returncode}")
        if result.stderr:
            print(result.stderr.strip())

    # Read results
    results_path = os.path.join(RESULTS_DIR, RESULTS_FILENAME)
    if not os.path.exists(results_path):
        print("Results file not found: linkcell.csv")
        continue

    df_import = pd.read_csv(results_path)
    print(df_import)

    # Add experiment metadata
    df_import["experiment"] = ilon
    df_import["longitude"] = lon

    # Append to global table
    df = pd.concat([df, df_import], ignore_index=True)


# ---------------------------------------------------------------------------
# Final results
# ---------------------------------------------------------------------------

print(df)
df.to_pickle("results.pkl")