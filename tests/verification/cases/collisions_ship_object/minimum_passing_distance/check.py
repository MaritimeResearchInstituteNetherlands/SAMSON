import os
import sys
import tomllib

import geopandas as gpd
import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns

with open("main.toml", "rb") as f:
    metadata = tomllib.load(f)

path = metadata["externals"][0]

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), path)))

from utils.samson import ram_probability

WORKSPACE = os.getcwd()
case_name = 'case1'
project_name = os.path.basename(WORKSPACE)

case_path = f"{WORKSPACE}/{case_name}"

TOL = 1e-9
tol_ratio = 1.01
# Load results
ok = True
# ---------------------------------------------------------------------------
# Calculate theoretic drift probabilties
# ---------------------------------------------------------------------------
# assume only one shiptype

platform_gdf = gpd.read_file(f"{case_path}/Export/objects.shp").to_crs(epsg=32631)
shiptypes_df = pd.read_csv(f"{case_path}/Traffic/shipcategories.csv")
link_gdf = gpd.read_file(f"{case_path}/Export/links.shp").to_crs(epsg=32631)
linkobject_exposures = pd.read_csv(f"{case_path}/Results/linkobject.csv")
platform_gdf['distance'] = platform_gdf.distance(link_gdf.geometry.iloc[0])
causation_factors_df = pd.read_csv(f"{case_path}/Causation/causationfactors.csv")

results_df = platform_gdf.merge(linkobject_exposures[['objectUID', 'driftCollisions', 'ramCollisions']], left_on='uid',
                                right_on='objectUID')
for collision_type in ['driftCollisions']:
    collisions_at500 = results_df[(results_df['distance'] - 500).abs() < 1][collision_type].iloc[0]
    if ((results_df[results_df['distance'] <= 500][collision_type] - collisions_at500) / collisions_at500).max() > 0.01:
        print(f'{collision_type} minimum passing distance does not work as expected')
        print(results_df[results_df['distance'] <= 501][['distance', collision_type]])
        ok = False

my_link = link_gdf.geometry.iloc[0]
ship_link = pd.read_csv('case1/modeldata/shiplinkdata.csv')
shiptypes_df = pd.read_csv('case1/Traffic/shipcategories.csv')
causation_factors = pd.read_csv('case1/Causation/causationfactors.csv')

exposure, events, collisions = ram_probability(link_gdf.geometry.iloc[0], shiptypes_df.default_speed.iloc[0],
                                               shiptypes_df.length.iloc[0], causation_factors.ram_factor.iloc[0],
                                               ship_link.frequency.iloc[0], platform_gdf, minimum_passing_distance=500)

results_df['manual_ram'] = collisions
# This check is currently not very strict. The mpd lowers the ram collisions, which is good.
# But is not yet completely agreeing with the manual calculated values
# Further research is needed to investigate this
if (results_df['ramCollisions'] - results_df['manual_ram']).iloc[0] / results_df['ramCollisions'].iloc[0] > 0.02:
    ok = False
    print('Minimum passing distance for ram not correctly implemented')
    print(results_df[['distance', 'ramCollisions', 'manual_ram']])
if ok:
    print('ok')

results_df['ramCollisions'] = results_df['ramCollisions'] / 10000
plot_df = results_df.melt(id_vars=['distance'], value_vars=['driftCollisions', 'ramCollisions'], value_name='frequency',
                          var_name='Collision type')

fig, ax = plt.subplots(1, 1, figsize=(6, 8), dpi=160, sharex=True)
sns.lineplot(data=plot_df, x='distance', y='frequency', hue='Collision type', ax=ax)
ax.legend()

fig.savefig("figure1.svg", bbox_inches="tight")
plt.close(fig)
