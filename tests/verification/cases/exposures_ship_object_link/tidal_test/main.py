import os
import subprocess

import geopandas as gpd
import matplotlib.pyplot as plt
import pandas as pd
from shapely.geometry import Point, LineString


def side_of_point(line: LineString, point: Point):
    (x1, y1), (x2, y2) = line.coords[:2]
    x, y = point.x, point.y

    cross = (x2 - x1) * (y - y1) - (y2 - y1) * (x - x1)

    if cross > 0:
        return -1
    elif cross < 0:
        return 1
    else:
        return 0


def plot_area_overview(case_folder, file_format='png'):
    export_folder = f"{case_folder}/export"
    shp_files = [f"{export_folder}/{x}" for x in os.listdir(export_folder) if x.endswith('.shp')]
    shapefile_dict = {os.path.basename(x)[:-4]: gpd.read_file(x) for x in shp_files}

    fig, ax = plt.subplots(figsize=(20, 20))

    if not shapefile_dict['links'].empty: shapefile_dict['links'].plot(ax=ax, alpha=0.5, label='Link')
    if not shapefile_dict['objects'].empty: shapefile_dict['objects'].plot(ax=ax)
    if not shapefile_dict['cells'].empty: shapefile_dict['cells'].plot(ax=ax, alpha=0.5, label='Cells')
    ax.set_title(os.path.basename(case_folder))
    fig.savefig(f'{case_folder}/area_overview.{file_format}')
    plt.close(fig)


WORKSPACE = os.getcwd()
case_name = 'case1'
project_name = os.path.basename(WORKSPACE)

case_path = f"{WORKSPACE}/{case_name}"

samson_command = 'samsoncli.exe --config-yaml config.yaml --actions actions.txt'
result = subprocess.run(samson_command, shell=True, cwd=case_path, capture_output=True, text=True, check=False)

if result.stdout:
    print(result.stdout.strip())

if result.returncode != 0:
    print(f"[SamsonCLI] Return code: {result.returncode}")
    if result.stderr:
        print(result.stderr.strip())

# ---------------------------------------------------------------------------
# Calculate theoretic drift probabilties
# ---------------------------------------------------------------------------
# assume only one shiptype
platform_gdf = gpd.read_file(f"{case_path}/Export/objects.shp").to_crs(epsg=32631)
shiptypes_df = pd.read_csv(f"{case_path}/Traffic/shipcategories.csv")
link_gdf = gpd.read_file(f"{case_path}/Export/links.shp").to_crs(epsg=32631)
linkobject_exposures = pd.read_csv(f"{case_path}/Results/linkobject.csv")
platform_gdf['rel_loc'] = platform_gdf.geometry.map(lambda x: side_of_point(link_gdf.geometry.iloc[0], x.centroid))
platform_gdf['distance'] = platform_gdf.distance(link_gdf.geometry.iloc[0]) * platform_gdf['rel_loc']
causation_factors_df = pd.read_csv(f"{case_path}/Causation/causationfactors.csv")

shiptype = shiptypes_df.iloc[0]
for col in ['height', 'draft']:
    platform_gdf[col] = shiptype[col]
platform_gdf['drift_factor'] = causation_factors_df['drift_factor'].iloc[0]
platform_gdf['passages'] = 1000

merged_df = platform_gdf.merge(linkobject_exposures[['objectUID', 'driftCollisions', 'driftExposure']], left_on='uid',
                               right_on='objectUID')
merged_df.to_pickle('results_combined.pkl')
plot_area_overview(case_path, 'svg')
