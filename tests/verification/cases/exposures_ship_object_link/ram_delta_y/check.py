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

from utils.samson import ram_probability

TOL = 1e-9
WORKSPACE = os.getcwd()
case_name = 'case1'
project_name = os.path.basename(WORKSPACE)

case_path = f"{WORKSPACE}/{case_name}"
# Load results
ok = True
platform_gdf = gpd.read_file(f"{case_path}/Export/objects.shp").to_crs(epsg=32631)
link_gdf = gpd.read_file(f"{case_path}/Export/links.shp").to_crs(epsg=32631)
linkobject_exposures = pd.read_csv(f"{case_path}/Results/linkobject.csv")
cellobject_exposures = pd.read_csv(f"{case_path}/Results/cellobject.csv")

platform_gdf['x_off'] = (platform_gdf.centroid.x - link_gdf.get_coordinates()['x'].max()).round(-2).astype(int)
platform_gdf['y_off'] = (platform_gdf.centroid.y - link_gdf.get_coordinates()['y'].max()).round(-2).astype(int)

link_results = platform_gdf.merge(linkobject_exposures, left_on='uid', right_on='objectUID', how='outer')
cell_results = platform_gdf.merge(cellobject_exposures, left_on='uid', right_on='objectUID', how='outer')

zero_x_offset_dict = {'Link': link_results[(link_results.x_off == 0) & (link_results.y_off > 0)],
                      'Cell': cell_results[(cell_results.x_off == 0) & (cell_results.y_off > 0)]}

for key, value_df in zero_x_offset_dict.items():
    for my_col in ['Collisions']:
        if (value_df[f"ram{my_col}"].shift(1) - value_df[f"ram{my_col}"]).dropna().min() < 0:
            print(f"{key} object ram {my_col} should be decreasing")
            ok = False

for traffic_object, my_df in {'link': link_results, 'cell': cell_results}.items():
    for key in ['ram', 'drift']:
        for my_col in ['Events', 'Collisions']:
            if my_df[my_df[f"{key}Exposure"] == 0][f"{key}{my_col}"].abs().sum() > 0:
                print(f"There are objects for which {traffic_object} {key}Exposure is 0, "
                      f"but {key}{my_col} has values larger than 0")
                ok = False
        if my_df[f"{key}Exposure"].min() < 0:
            print(f"There are objects for which {traffic_object} {key}Exposure is negative")
            ok = False

results_pivot_dict = {}
for key, results_df in {'link': link_results, 'cell': cell_results}.items():
    value_df = results_df.copy()
    value_df['left_side'] = value_df.x_off < 0
    value_df['x_distance'] = value_df.x_off.abs()
    link_pivot = value_df[(value_df.x_distance != 0) & (value_df.ramExposure > 0)].pivot(index=['x_distance', 'y_off'],
                                                                                         columns=['left_side'],
                                                                                         values='ramExposure')
    link_pivot['ratio'] = link_pivot[False] / link_pivot[True]
    results_pivot_dict[key] = link_pivot

for key, value_df in results_pivot_dict.items():
    if np.abs(value_df.ratio.mean() - 1) > 0.05:
        print(f"The difference in {key} ram exposure between left and right objects is too large ")
        ok = False

# Plotting
fig, ax = plt.subplots()
sns.lineplot(data=link_results[link_results.x_off == 500], x='y_off', y='ramExposure', ax=ax, label='Link')
sns.lineplot(data=cell_results[cell_results.x_off == 500], x='y_off', y='ramExposure', ax=ax, label='Cell')
plt.savefig('link_vs_cell_ramexp.svg')
plt.close(fig)

fig, ax = plt.subplots()
sns.lineplot(data=link_results[link_results.x_off == 500], x='y_off', y='ramCollisions', ax=ax, label='Link')
sns.lineplot(data=cell_results[cell_results.x_off == 500], x='y_off', y='ramCollisions', ax=ax, label='Cell')
plt.savefig('link_vs_cell_ramcol.svg')
plt.close(fig)

plot_df = results_pivot_dict['cell'].drop(columns=['ratio']).reset_index().melt(id_vars=['x_distance', 'y_off'],
                                                                                value_name='driftExposure')
fig, ax = plt.subplots()
sns.lineplot(data=plot_df, x='x_distance', y='driftExposure', hue='left_side', ax=ax)
plt.tight_layout()
plt.savefig('test.png')
plt.close(fig)


fig, [ax1,ax2] = plt.subplots(1,2,figsize = (15,6),sharey=True)
plot_df = results_pivot_dict['link'].reset_index()
ax1.set_title('Link ratio')
sns.lineplot(data=plot_df[plot_df.y_off.isin([500, 1500, 3100, 5100])], x='x_distance', y='ratio', hue='y_off', ax=ax1)
plot_df = results_pivot_dict['cell'].reset_index()
ax2.set_title('Cell ratio')
sns.lineplot(data=plot_df[plot_df.y_off.isin([500, 1500, 3100, 5100])], x='x_distance', y='ratio', hue='y_off', ax=ax2)
plt.tight_layout()
plt.savefig('test.svg')
plt.close(fig)

link_pivot = link_results.pivot(index=['x_off'], columns=['y_off'], values='ramExposure')
cell_pivot = cell_results.pivot(index=['x_off'], columns=['y_off'], values='ramExposure')
fig, ax = plt.subplots(figsize=(20, 20))
sns.heatmap(link_pivot, ax=ax)
ax.set_title("Heatmap of ramExposure for link object")
fig.savefig('link_heatmap.svg')
plt.close(fig)
fig, ax = plt.subplots()
sns.lineplot(link_pivot[[100, 300, 500]])
ax.set_title("The ramExposure for link object as a function of the lateral distance\n"
             "between link and object for three different values\n"
             "of the longitudinal distance between link and object")
plt.savefig('link_slice.svg')
plt.close(fig)

fig, ax = plt.subplots()
sns.lineplot(cell_pivot[[100, 300, 500]])
ax.set_title("The ramExposure for cell object as a function of the lateral distance\n"
             "between the midpoint of the cell and object for three different values\n"
             "of the longitudinal distance between cell and object")
plt.savefig('cell_slice.svg')
plt.close(fig)

# Testing (assumes that the heading of the link is to the north - 0.5*np.pi)
my_link = link_gdf.geometry.iloc[0]
ship_link = pd.read_csv('case1/modeldata/shiplinkdata.csv')
shiptypes_df = pd.read_csv('case1/Traffic/shipcategories.csv')
causation_factors = pd.read_csv('case1/Causation/causationfactors.csv')
platform_selection = platform_gdf[platform_gdf.y_off == 1100].copy()
exposure, events, collisions = ram_probability(link_gdf.geometry.iloc[0], shiptypes_df.default_speed.iloc[0],
                                               shiptypes_df.length.iloc[0], causation_factors.ram_factor.iloc[0],
                                               ship_link.frequency.iloc[0], platform_selection)

platform_selection.loc[:, 'calculated_exposure'] = exposure
platform_selection.loc[:, 'calculated_events'] = events
platform_selection.loc[:, 'calculated_collisions'] = collisions
compare_df = platform_selection.merge(linkobject_exposures, left_on='uid', right_on='objectUID')
compare_df['exposure_ratio'] = compare_df.ramExposure / compare_df.calculated_exposure
compare_df['collision_ratio'] = compare_df.ramCollisions / compare_df.calculated_collisions
compare_df['x'] = compare_df.geometry.centroid.x
compare_df['y'] = compare_df.geometry.centroid.y

exposure_ratio_description = compare_df[compare_df.x_off != 0].exposure_ratio.describe()
if (np.abs(exposure_ratio_description['mean'] - 1) > 1.03) or (exposure_ratio_description['std'] > 0.03):
    print("The average samson exposures deviates too much from the values calculated by hand")
    ok = False

plot_df = compare_df.melt(id_vars=['x_off'], value_vars=['exposure_ratio', 'collision_ratio'], var_name='Type',
                          value_name='Ratio')
plot_df['Type'] = plot_df['Type'].replace({'exposure_ratio': 'Exposure', 'collision_ratio': "Collision"})
fig, ax = plt.subplots()
sns.lineplot(data=plot_df, x='x_off', y='Ratio', hue='Type', ax=ax)
fig.savefig("exposure_ratio.svg")
plt.close(fig)

link_vs_cell = linkobject_exposures.merge(cellobject_exposures, on='objectUID', suffixes=('_link', '_cell'),
                                          how='outer').merge(platform_gdf, left_on='objectUID', right_on='uid',
                                                             how='outer')
link_vs_cell['drift_exposure_ratio'] = link_vs_cell['driftExposure_link'] / link_vs_cell['driftExposure_cell']

for collision_type in ['drift', 'ram']:
    for result_type in ['Exposure', 'Collisions']:
        use_col = f"{collision_type}{result_type}"
        non_neg_values = (link_vs_cell[f"{use_col}_link"] > 0) | (link_vs_cell[f"{use_col}_cell"] > 0)
        link_vs_cell[f"{use_col}_ratio"] = np.where(non_neg_values,
                                                    link_vs_cell[f"{use_col}_link"] / link_vs_cell[f"{use_col}_cell"],
                                                    None).astype(float)

for my_col in ['driftExposure', 'driftCollisions', 'ramExposure', 'ramCollisions']:
    description = link_vs_cell[(link_vs_cell.x_off != 0) & (link_vs_cell.y_off < 10000)][f"{my_col}_ratio"].replace(
        np.inf, None).astype(float).dropna().describe()
    if (np.abs(description['mean'] - 1) > 1.1) or (description['std'] > 0.4):
        print(f"The difference in {my_col} between cell and link is too large:"
              f"The average ratio is {description['mean'].round(2)} and "
              f"the standard deviation of the ratio is {description['std'].round(2)}")
        ok = False

for my_col in ['ramExposure', 'driftExposure']:
    fig, ax = plt.subplots()
    ax.set_title(f'{my_col}: Link vs Cell ratio')
    sns.heatmap(
        link_vs_cell.pivot(index=['x_off'], columns=['y_off'], values=f'{my_col}_ratio').fillna(0).replace(np.inf, 0),
        ax=ax, vmax=2, cmap='viridis')
    plt.tight_layout()
    plt.savefig(f'{my_col}_ratio_heatmap.svg')
    plt.close(fig)

if ok:
    print("ok")
