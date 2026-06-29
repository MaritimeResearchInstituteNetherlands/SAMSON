import os
import sys
import tomllib

import geopandas as gpd
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns

with open("main.toml", "rb") as f:
    metadata = tomllib.load(f)

path = metadata["externals"][0]

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), path)))

from utils.samson import drift_speed

TOL = 1e-3
ok = True
case_path = 'case1'
# Load results
platform_gdf = gpd.read_file(f"{case_path}/Export/objects.shp").to_crs(epsg=32631)
shiptypes_df = pd.read_csv(f"{case_path}/Traffic/shipcategories.csv")
cells_gdf = gpd.read_file(f"{case_path}/Export/cells.shp").to_crs(epsg=32631)
cellobject_exposures = pd.read_csv(f"{case_path}/Results/cellobject.csv").rename(columns={'cell_uid': 'traffic_uid'})
object_exposures = pd.concat([cellobject_exposures])
platform_gdf['width'] = (platform_gdf.geometry.length / 4).round(-1).astype(int)
platform_gdf['angle'] = platform_gdf.uid.str[1:].astype(int) - 1

frequency_cols = ['driftCollisions', 'driftExposure', 'ramExposure', 'ramCollisions']
cell_results = platform_gdf.merge(cellobject_exposures[['traffic_uid', 'objectUID'] + frequency_cols], left_on='uid',
                                  right_on='objectUID', how='outer').assign(traffic_type='cell')
results = pd.concat([cell_results])

cell_ram_results = results[results.traffic_type == 'cell'][['angle', 'ramExposure']]
cell_ram_description = cell_ram_results['ramExposure'].describe()
if cell_ram_description['mean'] == 0:
    print("The ram exposure for a uniform cell is 0")
    ok = False
elif cell_ram_description['std'] / cell_ram_description['mean'] > 5:
    print("The ram exposure for a uniform cell deviates too much")
    ok = False

fig, ax = plt.subplots()
sns.lineplot(data=cell_ram_results, x='angle', y='ramExposure', ax=ax)
plt.savefig('ramexposure_uniform_cell.svg')
plt.close(fig)

cell_drift_exposure = results[results.traffic_type == 'cell'].set_index('angle')['driftExposure']
cell_drift_dir_prob = cell_drift_exposure / cell_drift_exposure.sum()

drag_direction = np.deg2rad(np.linspace(0, 360, num=360 * 360, endpoint=False))
my_ship = shiptypes_df.iloc[0]
drag_velocity = drift_speed(5, my_ship.draft, my_ship.height)
current_direction = np.deg2rad(0)
current_velocity = 1

# --- Convert to vector components ---

# Drag components
drag_x = drag_velocity * np.cos(drag_direction)
drag_y = drag_velocity * np.sin(drag_direction)

# Current components (constant for all theta)
current_x = current_velocity * np.cos(current_direction)
current_y = current_velocity * np.sin(current_direction)

# --- Resultant vector ---
result_x = drag_x + current_x
result_y = drag_y + current_y

# --- Convert back to magnitude and direction ---
drift_velocity = np.sqrt(result_x ** 2 + result_y ** 2)
drift_direction = np.rad2deg(np.arctan2(result_y, result_x))

# Normalize direction to 0–360
drift_direction = (drift_direction + 360) % 360

drift_velocity
manual_dir_prob = pd.Series(drift_direction).round().astype(int).value_counts(normalize=True).sort_index()
manual_dir_prob.loc[0] = manual_dir_prob.loc[0] + manual_dir_prob.loc[360]
manual_dir_prob = manual_dir_prob.drop(360)
my_df = cell_drift_dir_prob.to_frame().join(manual_dir_prob).reset_index()
my_df.columns = ['angle', 'SAMSON', 'Manual']
my_df['Manual'] = my_df['Manual'].fillna(0)

my_df['SAMSON'] = my_df['SAMSON'].fillna(0)
my_df['Manual'] = my_df['Manual'].fillna(0)

fig, ax = plt.subplots(figsize=(30, 20))
sns.lineplot(data=my_df.melt(id_vars=['angle'], value_name='Proportion', var_name='Drift direction'), x='angle',
             y='Proportion', hue='Drift direction', ax=ax, marker='.')
plt.savefig('drift_direction_proportions.svg')
plt.close(fig)

description = (my_df['Manual'] - my_df['SAMSON']).abs().describe()
if description['mean'] > 0.0005 or description['std'] > 0.003:
    print("SAMSON drift direction proportions deviate from the expected values")
    ok = False

if ok:
    print('ok')
