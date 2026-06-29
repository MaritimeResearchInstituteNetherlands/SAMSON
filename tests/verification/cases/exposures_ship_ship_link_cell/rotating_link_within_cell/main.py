import os
import subprocess

import geopandas as gpd
import numpy as np
import pandas as pd
from shapely.affinity import rotate
from shapely.geometry import LineString

# ---------------------------------------------------------------------------
# Constants / Configuration
# ---------------------------------------------------------------------------

NUM_THETAS = 37

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

# Derived angle array (radians)
THETAS = np.linspace(0, 2 * np.pi, num=NUM_THETAS, endpoint=True)

# ---------------------------------------------------------------------------
# Load input vertices
# ---------------------------------------------------------------------------

vertices_path = os.path.join(AREA_DIR, VERTICES_FILENAME)
df_vertices = pd.read_csv(vertices_path)

x0, y0 = df_vertices.loc[0, ["lon", "lat"]]
x1, y1 = df_vertices.loc[1, ["lon", "lat"]]

link = LineString([(x0, y0), (x1, y1)])

gdf = gpd.GeoDataFrame(
    {"name": ["link"]},
    geometry=[link],
    crs="EPSG:4326",
)

gdf_metric = gdf.to_crs("EPSG:32631")

geom = gdf_metric.geometry.iloc[0]
length = gdf_metric.geometry.iloc[0].length

print('Link length in meters:')

# ---------------------------------------------------------------------------
# Rotation loop and external tool execution
# ---------------------------------------------------------------------------

rows = []

for theta in THETAS:
    # Rotate link (rotate expects degrees)
    geom_rot = rotate(geom, theta, origin="center", use_radians=True)
    gdf_rot = gpd.GeoDataFrame({"geometry": [geom_rot]}, crs="EPSG:32631").to_crs("EPSG:4326")

    # Extract rotated coordinates
    coords = list(gdf_rot.geometry.iloc[0].coords)
    df_vertices.loc[0, ["lon", "lat"]] = coords[0]
    df_vertices.loc[1, ["lon", "lat"]] = coords[1]
    
    print(np.degrees(theta))
    print(df_vertices)

    df_vertices.to_csv(vertices_path, index=False, header=True)

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
        print("Results file not found: linkcell.csv")
        continue

    df_import = pd.read_csv(results_path)

    if df_import.empty:
        print("Empty linkcell.csv file.")
        continue

    # Expect exactly one exposure row
    if len(df_import) == 1:
        rows.append(
            {"exposures": df_import.loc[0, "exposures"]}
        )
    else:
        print("Mismatch of expected exposures.")

# ---------------------------------------------------------------------------
# Final results
# ---------------------------------------------------------------------------

df = pd.DataFrame(rows, columns=["exposures"])
print(df)

df.to_pickle("results.pkl")