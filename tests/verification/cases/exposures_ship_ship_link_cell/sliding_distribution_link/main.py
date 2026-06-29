import os
import subprocess

import geopandas as gpd
import numpy as np
import pandas as pd

# ---------------------------------------------------------------------------
# Constants / Configuration
# ---------------------------------------------------------------------------

NUM_MUS = 25

WORKSPACE = os.getcwd()
DATA_DIR = os.path.join(".", "ModelData")
RESULTS_DIR = os.path.join(".", "Results")
AREA_DIR = os.path.join(".", "Area")

SAMSON_CMD = [
    "SamsonCLI",
    "--config-yaml", "config.yaml",
    "--actions", "actions.txt",
]

DATA_FILENAME = "shiplinkdata.csv"
RESULTS_FILENAME = "linkcell.csv"
VERTICES_FILENAME = "vertices.csv"

def haversine(coord0, coord1):
    """Compute geodesic distance (Haversine formula) in meters."""
    lat0, lon0 = coord0
    lat1, lon1 = coord1

    r_earth = 6_371_000  # [m]
    phi0 = np.radians(lat0)
    phi1 = np.radians(lat1)

    delta_phi = np.radians(lat1 - lat0)
    delta_lambda = np.radians(lon1 - lon0)

    a = (
        np.sin(delta_phi / 2.0) ** 2
        + np.cos(phi0) * np.cos(phi1)
        * np.sin(delta_lambda / 2.0) ** 2
    )
    c = 2 * np.arctan2(np.sqrt(a), np.sqrt(1 - a))

    return r_earth * c

# ---------------------------------------------------------------------------
# Load input vertices
# ---------------------------------------------------------------------------

data_path = os.path.join(DATA_DIR, DATA_FILENAME)
df_data = pd.read_csv(data_path)
df_data["mu"] = df_data["mu"].astype(float)
df_data["sigma"] = df_data["sigma"].astype(float)

vertices_path = os.path.join(AREA_DIR, VERTICES_FILENAME)
df_vertices = pd.read_csv(vertices_path)

x0, y0 = df_vertices.loc[2, ["lon", "lat"]]
x1, y1 = df_vertices.loc[4, ["lon", "lat"]]

y = (y0 + y1) / 2
print(y)

distance = haversine([y, x0], [y, x1])
print(distance)

sigma = distance / 6
mu_start = -distance
mu_end = +distance

# Generate longitude values
mus = np.linspace(mu_start, mu_end, num=NUM_MUS, endpoint=True)

# ---------------------------------------------------------------------------
# Translation loop and external tool execution
# ---------------------------------------------------------------------------

df = pd.DataFrame()

for mu in mus:
    
    print(mu)
    
    df_data.loc[0, "mu"] = mu
    df_data.loc[0, "sigma"] = sigma
    df_data.to_csv(data_path, index=False, header=True)

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

    # Read result file
    results_path = os.path.join(RESULTS_DIR, RESULTS_FILENAME)
    if not os.path.exists(results_path):
        print("Results file not found: linkcell.csv")
        continue

    df_import = pd.read_csv(results_path)

    # Concatenate results
    df = pd.concat([df, df_import], ignore_index=True)

# ---------------------------------------------------------------------------
# Final results
# ---------------------------------------------------------------------------

df["mu"] = mus 
print(df)
df.to_pickle("results.pkl")