import os
import subprocess

import geopandas as gpd
import numpy as np
import pandas as pd

# ---------------------------------------------------------------------------
# Constants / Configuration
# ---------------------------------------------------------------------------

NUM_LONS = 10

WORKSPACE = os.getcwd()
MODEL_DATA_DIR = os.path.join(".", "ModelData")
RESULTS_DIR = os.path.join(".", "Results")
AREA_DIR = os.path.join(".", "Area")

SAMSON_CMD = [
    "SamsonCLI",
    "--config-yaml", "config.yaml",
    "--actions", "actions.txt",
]

RESULTS_FILENAME = "linkcell.csv"
VERTICES_FILENAME = "vertices.csv"

# ---------------------------------------------------------------------------
# Load input vertices
# ---------------------------------------------------------------------------

vertices_path = os.path.join(AREA_DIR, VERTICES_FILENAME)
df_vertices = pd.read_csv(vertices_path)

x0, y0 = df_vertices.loc[2, ["lon", "lat"]]
x1, y1 = df_vertices.loc[3, ["lon", "lat"]]

# Compute link length
link_length = (x1 - x0) / 2
print(link_length)

# Create longitudinal sampling positions
lon_start = min(x0, x1) - link_length
lon_end = max(x0, x1)
lons = np.linspace(lon_start, lon_end, num=NUM_LONS, endpoint=True)

# ---------------------------------------------------------------------------
# Translation loop and external tool execution
# ---------------------------------------------------------------------------

rows = []

for lon in lons:

    df_vertices.loc[0, "lon"] = lon
    df_vertices.loc[1, "lon"] = lon + link_length

    print(lon)
    print(df_vertices)

    # Write temporary vertices file
    df_vertices.to_csv(vertices_path, index=False, header=True)

    # Reload to ensure consistency
    df_vertices = pd.read_csv(vertices_path)
    print(df_vertices)

    # Run Samson CLI
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

    # Read result file
    results_path = os.path.join(RESULTS_DIR, RESULTS_FILENAME)
    if not os.path.exists(results_path):
        print("Results file not found: linkcell.csv")
        continue

    df_import = pd.read_csv(results_path)
    print(df_import)

    if df_import.empty:
        print("Empty linkcell.csv file.")
        rows.append(
            {"longitude": lon, "exposures": 0}
        )
        continue

    # Expect exactly one row
    if len(df_import) == 1:
        rows.append(
            {
                "longitude": lon,
                "exposures": df_import.loc[0, "exposures"],
            }
        )

# ---------------------------------------------------------------------------
# Final results
# ---------------------------------------------------------------------------

df = pd.DataFrame(rows, columns=["longitude", "exposures"])
print(df)

df.to_pickle("results.pkl")