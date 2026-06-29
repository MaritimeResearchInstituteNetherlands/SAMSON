import geopandas as gpd
import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns

TOL = 1e-3
ok = True
case_path = 'case1'
# Load results
platform_gdf = gpd.read_file(f"{case_path}/Export/objects.shp").to_crs(epsg=32631)
shiptypes_df = pd.read_csv(f"{case_path}/Traffic/shipcategories.csv")
link_gdf = gpd.read_file(f"{case_path}/Export/links.shp").to_crs(epsg=32631)
linkobject_exposures = pd.read_csv(f"{case_path}/Results/linkobject.csv").assign(traffic_type='link').rename(
    columns={'link_uid': 'traffic_uid'})
cellobject_exposures = pd.read_csv(f"{case_path}/Results/cellobject.csv").assign(traffic_type='cell').rename(
    columns={'cell_uid': 'traffic_uid'})
object_exposures = pd.concat([linkobject_exposures, cellobject_exposures])
platform_gdf['width'] = (platform_gdf.geometry.length / 4).round(-1).astype(int)

platform_gdf['x_off'] = (platform_gdf.centroid.x - link_gdf.get_coordinates()['x'].max()).round(-2).astype(int)
platform_gdf['y_off'] = (platform_gdf.centroid.y - link_gdf.get_coordinates()['y'].max()).round(-2).astype(int)
platform_gdf['distance'] = platform_gdf['x_off'].abs()
results = platform_gdf.merge(object_exposures[
                                 ['traffic_uid', 'traffic_type', 'objectUID', 'driftCollisions', 'driftExposure',
                                  'ramExposure', 'ramCollisions']], left_on='uid', right_on='objectUID')

# Plotting
for traffic_type, my_df in results.groupby('traffic_type'):
    plot_df = my_df.sort_values(by=['x_off', 'width'])
    fig, axs = plt.subplots(2, 2, figsize=(16, 10), sharex=True)
    axs = axs.ravel()
    sns.lineplot(data=plot_df, x='x_off', y='driftExposure', hue='width', ax=axs[0])
    sns.lineplot(data=plot_df, x='x_off', y='driftCollisions', hue='width', ax=axs[2])
    sns.lineplot(data=plot_df, x='x_off', y='ramExposure', hue='width', ax=axs[1])
    sns.lineplot(data=plot_df, x='x_off', y='ramCollisions', hue='width', ax=axs[3])
    for ax in axs:
        ax.get_legend().remove()
    handles, labels = axs[-1].get_legend_handles_labels()
    fig.legend(handles, labels, loc='upper right', ncol=1, bbox_to_anchor=(0.9, 1), frameon=True,
               title='Platform width [m]')
    fig.savefig(f"sliding_{traffic_type}.svg", bbox_inches="tight")
    plt.close(fig)

frequency_cols = ['driftCollisions', 'driftExposure', 'ramExposure']

for key, sub_df in object_exposures.groupby('traffic_type'):
    for my_col in frequency_cols:
        if (sub_df[my_col] == '-nan(ind)').sum() > 0:
            print(f"'-nan(ind)' value for {my_col} for {key}")
            ok = False
        elif sub_df[my_col].min() < 0:
            print(f"negative value for {my_col} for {key}")
            ok = False

exposures_merged = results[results.x_off < 0].merge(results[results.x_off > 0],
                                                    on=['distance', 'width', 'traffic_type'],
                                                    suffixes=("_left", "_right"))
for key, sub_df in object_exposures.groupby('traffic_type'):
    for my_col in frequency_cols:
        sub_df = exposures_merged[exposures_merged[f"{my_col}_right"] != 0]
        description = (sub_df[f"{my_col}_left"] / sub_df[f"{my_col}_right"]).dropna().describe()
        if description['mean'] < 0.9 or description['mean'] > 1.15:
            print(f"Too large difference left vs right value for {my_col} for {key}")
            ok = False

if linkobject_exposures.driftExposure.max() > 1e6:
    ok = False
    print("driftExposure larger than frequency*link_length")

if linkobject_exposures.ramExposure.max() > 1e6:
    ok = False
    print("driftExposure larger than frequency*link_length")

if ok:
    print("ok")
