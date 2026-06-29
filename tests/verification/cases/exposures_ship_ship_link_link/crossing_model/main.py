import os
import subprocess
import numpy as np
import pandas as pd
import geopandas as gpd
import shapely

# ------------------------------------------------------------------------------
# Constants and configuration
# ------------------------------------------------------------------------------
ALPHAS = np.arange(10, 360, 10)
ORIGIN = np.array([521854, 5857450])
LENGTH = 1000
CRS = "EPSG:32631"
EPSG = 4326

WORKSPACE = os.getcwd()
AREA_DIRECTORY = "./Area"
MODEL_DATA_DIRECTORY = "./ModelData"
TRAFFIC_DIRECTORY = "./Traffic"
RESULTS_DIRECTORY = "./Results"

# ------------------------------------------------------------------------------
# Result containers
# ------------------------------------------------------------------------------
exposures = []
alphas_filtered = []
df = pd.DataFrame()

# ------------------------------------------------------------------------------
# Main loop
# ------------------------------------------------------------------------------
for alpha in ALPHAS:

    length_arr = np.array([0, LENGTH])
    x1 = ORIGIN - length_arr
    x2 = ORIGIN

    line1 = shapely.LineString([x1, x2])
    line2 = shapely.affinity.rotate(line1, alpha, origin=tuple(ORIGIN))

    node_dict = {
        "0": shapely.get_point(line1, 0),
        "1": shapely.get_point(line1, 1),
        "2": shapely.get_point(line2, 0),
    }

    node_gdf = gpd.GeoSeries(node_dict, crs=CRS).to_crs(EPSG)

    # --------------------------------------------------------------------------
    # Vertices
    # --------------------------------------------------------------------------
    df_vertices = pd.DataFrame(
        {
            "uid": node_gdf.index,
            "lat": node_gdf.geometry.y,
            "lon": node_gdf.geometry.x,
            "name": node_gdf.index,
        }
    )
    vertices_export = os.path.join(AREA_DIRECTORY, "vertices.csv")
    df_vertices.to_csv(vertices_export, index=False)

    # --------------------------------------------------------------------------
    # Waypoints
    # --------------------------------------------------------------------------
    df_waypoints = pd.DataFrame(
        {
            "uid": ["WP0", "WP1", "WP2"],
            "vertex_uid": [0, 1, 2],
            "description": ["WP0", "WP1", "WP2"],
            "radius": [1.0, 1.0, 1.0],
        }
    )
    waypoints_export = os.path.join(AREA_DIRECTORY, "waypoints.csv")
    df_waypoints.to_csv(waypoints_export, index=False)

    # --------------------------------------------------------------------------
    # Links
    # --------------------------------------------------------------------------
    df_links = pd.DataFrame(
        {
            "uid": ["L0", "L1"],
            "start": ["WP0", "WP2"],
            "end": ["WP1", "WP1"],
            "description": ["First link", "Second link"],
        }
    )
    links_export = os.path.join(AREA_DIRECTORY, "links.csv")
    df_links.to_csv(links_export, index=False)

    # --------------------------------------------------------------------------
    # Shiplinks
    # --------------------------------------------------------------------------
    df_shiplinks = pd.DataFrame(
        {
            "uid": ["SL0", "SL1"],
            "ship_uid": ["SCAT0", "SCAT1"],
            "link_uid": df_links["uid"].values,
            "shiplinkdata_uid": ["SLD0", "SLD1"],
        }
    )
    shiplinks_export = os.path.join(TRAFFIC_DIRECTORY, "shiplinks.csv")
    df_shiplinks.to_csv(shiplinks_export, index=False)

    # --------------------------------------------------------------------------
    # Shiplink data
    # --------------------------------------------------------------------------
    df_shiplinkdata = pd.DataFrame(
        {
            "uid": ["SLD0", "SLD1"],
            "frequency": [0.001, 0.002],
            "speed": [9, 7],
            "mu": [-200, -300],
            "sigma": [100, 100],
        }
    )
    shiplinkdata_export = os.path.join(MODEL_DATA_DIRECTORY, "shiplinkdata.csv")
    df_shiplinkdata.to_csv(shiplinkdata_export, index=False)

    # --------------------------------------------------------------------------
    # Run SamsonCLI
    # --------------------------------------------------------------------------
    result = subprocess.run(
        "SamsonCLI --config-yaml config.yaml --actions actions.txt",
        shell=True,
        cwd=WORKSPACE,
        capture_output=True,
        text=True,
    )

    if result.stdout:
        print(result.stdout)

    # --------------------------------------------------------------------------
    # Read results
    # --------------------------------------------------------------------------
    results_import = os.path.join(RESULTS_DIRECTORY, "linklink.csv")

    if os.path.exists(results_import):
        df_import = pd.read_csv(results_import)
        if not df_import.empty:
            df = pd.concat([df, df_import])
            alphas_filtered.append(alpha)

df["alpha"] = alphas_filtered
print(df)

df.to_pickle("results.pkl")