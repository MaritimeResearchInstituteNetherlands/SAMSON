import os
import subprocess

import numpy as np
import pandas as pd

# ---------------------------------------------------------------------------
# Constants / Configuration
# ---------------------------------------------------------------------------

NUM_LONS = 9

WORKSPACE = os.getcwd()
MODEL_DATA_DIR = os.path.join(".", "ModelData")
RESULTS_DIR = os.path.join(".", "Results")
AREA_DIR = os.path.join(".", "Area")

SAMSON_CMD = [
    "SamsonCLI",
    "--config-yaml", "config.yaml",
    "--actions", "actions.txt",
]

RESULTS_FILENAME = "cellobject.csv"
VERTICES_FILENAME = "vertices.csv"

# ---------------------------------------------------------------------------
# Load input vertices
# ---------------------------------------------------------------------------

vertices_path = os.path.join(AREA_DIR, VERTICES_FILENAME)
df_vertices = pd.read_csv(vertices_path)

# x0, y0 = df_vertices.loc[0, ["lon", "lat"]]
# x1, y1 = df_vertices.loc[1, ["lon", "lat"]]

x0 = 3.8200
x1 = 3.8600

# Compute cell length
cell_length = (x1 - x0)
print(cell_length)

# Create longitudinal sampling positions
lon_start = min(x0, x1) - 2 * cell_length
lon_end = lon_start + 4 * cell_length
lons = np.linspace(lon_start, lon_end, num=NUM_LONS, endpoint=True)

# ---------------------------------------------------------------------------
# Translation loop and external tool execution
# ---------------------------------------------------------------------------

rows = []

for lon in lons:

    df_vertices.loc[0, "lon"] = lon
    df_vertices.loc[1, "lon"] = lon + cell_length
    df_vertices.loc[2, "lon"] = lon + cell_length
    df_vertices.loc[3, "lon"] = lon

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
        print("Results file not found: cellobject.csv")
        continue

    df_import = pd.read_csv(results_path)
    print(df_import)

    if df_import.empty:
        print("Empty cellobject.csv file.")
        rows.append(
            {"longitude": lon, "driftExposures": 0}
        )
        continue

    # Expect exactly one row
    if len(df_import) == 1:
        rows.append(
            {
                "longitude": lon,
                "driftExposures": df_import.loc[0, "driftExposure"],
                "ramExposures": df_import.loc[0, "ramExposure"],
            }
        )

# ---------------------------------------------------------------------------
# Final results
# ---------------------------------------------------------------------------

df = pd.DataFrame(rows, columns=["longitude", "ramExposures", "driftExposures"])
print(df)

df.to_pickle("results.pkl")