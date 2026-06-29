import os
from pathlib import Path

import geopandas as gpd
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

WORKSPACE = os.getcwd()
project_name = os.path.basename(WORKSPACE)
main_samson_folder = str(Path(WORKSPACE).parents[0])

case_path = f"{WORKSPACE}/case1"

linkobject_exposures = pd.read_csv(f"{case_path}/Results/linkobject.csv")
platform_gdf = gpd.read_file(f"{case_path}/Export/objects.shp").to_crs(epsg=32631)
platform_gdf['orientation'] = np.arange(0, 360)
link_gdf = gpd.read_file(f"{case_path}/Export/links.shp").to_crs(epsg=32631)

results_df = linkobject_exposures.merge(platform_gdf, left_on='objectUID', right_on='uid').drop(
    columns=['geometry', 'uid'])

TOL = 1e-9

# Load results
ok = True

drift_signal_mean = results_df.driftExposure.values.mean()
ram_signal_mean = results_df.ramExposure.values.mean()
drift_exposure_difference = results_df.driftExposure.values.reshape(-1, 4, order='F').std(axis=1).mean() / drift_signal_mean > 2e-5
ram_exposure_difference = results_df.ramExposure.values.reshape(-1, 4, order='F').std(axis=1).mean() / ram_signal_mean > 2e-5

if drift_exposure_difference or ram_exposure_difference:
    ok = False

if ok:
    print("ok")

# Plotting

fig, axs = plt.subplots(2, 1, figsize=(6, 8), dpi=160, sharex=True)

for ax, col in zip(axs, ['driftExposure', 'ramExposure']):
    ax.plot(results_df["orientation"], results_df[col])
    ax.set_xlabel("orientation")
    ax.set_ylabel(col)
    ax.set_title(col)
plt.tight_layout()
fig.savefig("figure1.svg", bbox_inches="tight")
plt.close(fig)
