import os
import subprocess
import sys
import tomllib
from pathlib import Path

import geopandas as gpd
import pandas as pd

with open("main.toml", "rb") as f:
    metadata = tomllib.load(f)

path = metadata["externals"][0]

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), path)))

from utils.testing import run_samson_test

WORKSPACE = os.getcwd()
project_name = os.path.basename(WORKSPACE)
main_samson_folder = str(Path(WORKSPACE).parents[0])

case_list = ['simple_wind', 'current']

for case_name in case_list:
    case_path = f"{WORKSPACE}/{case_name}"
    run_samson_test(case_path)

my_list = []
for case_name in case_list:
    my_list.append(pd.read_csv(f"{case_path}/Results/linkobject.csv").assign(case_name=case_name))
linkobject_exposures = pd.concat(my_list)
my_list = []
for case_name in case_list:
    my_list.append(pd.read_csv(f"{case_path}/Results/cellobject.csv").assign(case_name=case_name))
cellobject_exposures = pd.concat(my_list)
platform_gdf = gpd.read_file(f"{case_path}/Export/objects.shp").to_crs(epsg=32631)
link_gdf = gpd.read_file(f"{case_path}/Export/links.shp").to_crs(epsg=32631)
platform_gdf['distance'] = platform_gdf.distance(link_gdf.geometry.iloc[0])
results_combined_links = linkobject_exposures.merge(platform_gdf, left_on='objectUID', right_on='uid')
results_combined_cells = cellobject_exposures.merge(platform_gdf, left_on='objectUID', right_on='uid')

results_combined_links.to_pickle('results_combined.pkl')
results_combined_cells.to_pickle('results_combined_cells.pkl')

