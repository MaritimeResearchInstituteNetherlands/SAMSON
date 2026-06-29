import os
from pathlib import Path

import geopandas as gpd
import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns

TOL = 1e-9
threshold_ratio = 1.07
# Load results
ok = True

WORKSPACE = os.getcwd()
project_name = os.path.basename(WORKSPACE)
main_samson_folder = str(Path(WORKSPACE).parents[0])
case_list = ['standard_wind', 'simple_wind', 'current']
my_list = []
for case_name in case_list:
    case_path = f"{WORKSPACE}/{case_name}"
    my_list.append(pd.read_csv(f"{case_path}/Results/linkobject.csv").assign(case_name=case_name))

linkobject_exposures = pd.concat(my_list)
platform_gdf = gpd.read_file(f"{case_path}/Export/objects.shp").to_crs(epsg=32631)
link_gdf = gpd.read_file(f"{case_path}/Export/links.shp").to_crs(epsg=32631)

results_combined = (linkobject_exposures.merge(platform_gdf, left_on='objectUID', right_on='uid').drop(
    columns=['geometry', 'uid']).merge(link_gdf, left_on='link_uid', right_on='uid')).set_geometry('geometry')

results_combined['splitted_line'] = results_combined.geometry.length.round().astype(int) != 10000

my_cols = ['driftExposure', 'ramExposure']  # , 'driftEvents', 'ramEvents', 'driftCollisions', 'ramCollisions']
results_df = results_combined.groupby(['case_name', 'objectUID', 'splitted_line'])[my_cols].sum()

results_summary = results_df.groupby(['case_name', 'splitted_line'])[my_cols].sum()

for col in my_cols:
    for case_name in results_summary.index.get_level_values('case_name').unique():
        drift_splitted_link = results_summary.loc[(case_name, True), col]
        drift_unsplitted_link = results_summary.loc[(case_name, False), col]
        ratio = drift_splitted_link / drift_unsplitted_link
        if ratio > threshold_ratio or 1 / ratio > threshold_ratio:
            print(
                f"For case {case_name} the ratio between {col} for splitted vs unsplitted links is too large: {ratio}")
            ok = False

if ok:
    print("ok")

# Plotting
plot_df = results_summary.reset_index()
fig, axs = plt.subplots(1, 2, figsize=(16, 10))
for col, ax in zip(my_cols, axs):
    sns.barplot(plot_df, x='case_name', y=col, hue='splitted_line', ax=ax)
    ax.set_title(col)

fig.savefig("figure1.png", bbox_inches="tight")
fig.savefig("figure1.svg", bbox_inches="tight")
plt.close(fig)
