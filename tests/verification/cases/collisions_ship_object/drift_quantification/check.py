import os

import geopandas as gpd
import numpy as np
import seaborn as sns


def repair_function(t):
    return np.clip(1 / (1.5 * t + 0.625), 0, 1)


def avoidance_function(distance, ship_length, a=0.1):
    return np.exp(-a * distance / ship_length)


def get_wave_drift_coefficient(kT):
    a = -1.4736
    b = 2.4765
    c = -0.0315
    return a * kT ** 3 + b * kT ** 2 + c * kT


def get_v_wind_ms(bft_scale, draft, total_height):
    """
    Calculate the contribution of the wind to the drift speed for a specified bft scale and N different shiptypes
    :param bft_scale:
    :param draft: np.array(N)
    :param total_height: np.array(N)
    :return:
    """
    rkt = omega[bft_scale][0, 0] ** 2 / g * draft
    r = np.where(rkt <= 1, get_wave_drift_coefficient(rkt), 1)
    t1 = (rho_air * (total_height - draft) * c_dwind) / (rho_w * draft * c_d) * v_b[bft_scale][0, 0] ** 2
    t3 = z_b[bft_scale][0, 0] ** 2 * g * r ** 2 / (8 * draft * c_d)
    return np.sqrt(t1 + t3)


def get_allision_probability(n_passages, drift_factor, distance, drift_speed, bft_scale, effective_length,
                             anchor_measure=False):
    repair_probability = repair_function(distance * drift_speed ** -1 / 3600)
    anchor_failure_probabilty = 1 - anchor_measure * anchor_succes_probability[bft_scale]
    collision_frequency = n_passages * drift_factor * effective_length * repair_probability * anchor_failure_probabilty
    return np.where(distance > 0, collision_frequency, 0)


# Constants
nautical_mile = 1852
g = 9.813
pi = 3.14159
ms_to_knot = 1.944
rho_air = 1.23
rho_w = 1025
c_dwind = 0.9  # Lateral wind resistance coefficient ship
c_d = 0.8  # lateral resistance coeficient of the underwater body of the ship
T_p = np.array([0.001, .78, 1.87, 3.06, 4.62, 6.21, 7.70, 9.24, 10.81, 12.44, 14.09, 15.79]).reshape(-1, 1,
                                                                                                     1)  # mean wave period
v_b = np.array([.001, 1.03, 2.57, 4.37, 6.94, 9.77, 12.59, 15.68, 19.02, 22.62, 26.47, 30.58]).reshape(-1, 1, 1)
z_b = np.array([0., .1, .3, .5, 1., 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5]).reshape(-1, 1, 1)  # significant wave amplitude
anchor_succes_probability = np.array([0.99, 0.99, 0.99, 0.99, 0.965, 0.93, 0.874, 0.79, 0.65, 0.51, 0.37, 0.3, 0])
omega = 2 * pi / T_p  # Frequency
wave_number = (4 * np.pi ** 2) / (T_p ** 2 * g)
factor = 4.0 * np.pi * np.pi
import matplotlib.pyplot as plt
import pandas as pd

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

platform_gdf

shiptype = shiptypes_df.iloc[0]
for col in ['height', 'draft']:
    platform_gdf[col] = shiptype[col]
platform_gdf['drift_factor'] = causation_factors_df['drift_factor'].iloc[0]
platform_gdf['passages'] = 1000
bft_scale = 5
v_wind_ms = get_v_wind_ms(bft_scale, platform_gdf.draft, platform_gdf.height)
platform_gdf['manual_p'] = get_allision_probability(platform_gdf.passages, platform_gdf.drift_factor,
                                                    platform_gdf['distance'], v_wind_ms, bft_scale, 100,
                                                    anchor_measure=True)

results_df = platform_gdf.merge(linkobject_exposures[['objectUID', 'driftCollisions', 'driftExposure']], left_on='uid',
                                right_on='objectUID')
results_df['ratio'] = results_df['driftCollisions'] / results_df['manual_p']

if np.max(np.abs((results_df[results_df['driftExposure'] > 0].driftExposure / 100000) - 1)) > 0.01:
    print('exposure is not what we expected')
    ok = False

if results_df[results_df['distance'] < 0].driftCollisions.sum() > 0:
    print("objects to the left should have zero collisions")
    ok = False

avg_ratio = results_df[results_df['distance'] > 0].ratio.mean()
if avg_ratio > tol_ratio or 1 / avg_ratio > tol_ratio:
    print("drift collisions should not deviate too much from the theoretically expected result")
    ok = False

if results_df.driftExposure.std() > 100:
    print("Driftexposure should not deviate too much as a function of the distance between link and object")
    ok = False

if ok:
    print("ok")

# Plotting

fig, [ax, ax2] = plt.subplots(2, 1, figsize=(6, 8), dpi=160, sharex=True)

ax.plot(results_df["distance"], results_df["driftCollisions"], label="SamsonCLI")
ax.plot(results_df["distance"], results_df["manual_p"], label="Theoretical results")
sns.lineplot(data=results_df, x='distance', y='driftExposure', ax=ax2)
ax2.set_xlabel("distance")
ax.set_ylabel("Drift collision frequency")  # adjust if needed
ax.legend()

fig.savefig("figure1.svg", bbox_inches="tight")
plt.close(fig)
