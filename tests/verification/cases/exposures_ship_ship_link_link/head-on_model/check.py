import os
import numpy as np
import pandas as pd
from scipy.stats import norm

# Workspace and file configuration
WORKSPACE_INI = os.getcwd()
OUTPUT_DIR = "Results"
OUTPUT_FILE = "linklink.csv"

# Ship and distribution constants
B0 = 30          # [m]
B1 = 40          # [m]
V0 = 11          # [m/s]
V1 = 12          # [m/s]
MU0 = -200       # [m]
MU1 = -300       # [m]
SIGMA0 = 100     # [m]
SIGMA1 = 150     # [m]
Q0 = 0.005       # [s-1]
Q1 = 0.010       # [s-1]
WP0 = [52, 3.5]  # latitude, longitude
WP1 = [52, 4.0]  # latitude, longitude


def haversine(coord0, coord1):
    """Compute geodesic distance (Haversine formula) in meters."""
    lat0, lon0 = coord0
    lat1, lon1 = coord1

    r_earth = 6_371_000  # [m]
    phi0 = np.radians(lat0)
    phi1 = np.radians(lat1)

    delta_phi = np.radians(lat1 - lat0)
    delta_lambda = np.radians(lon1 - lon0)

    a = (
        np.sin(delta_phi / 2.0) ** 2
        + np.cos(phi0) * np.cos(phi1)
        * np.sin(delta_lambda / 2.0) ** 2
    )
    c = 2 * np.arctan2(np.sqrt(a), np.sqrt(1 - a))

    return r_earth * c


# Load exposure results
exposures_csv = os.path.join(WORKSPACE_INI, OUTPUT_DIR, OUTPUT_FILE)
df = pd.read_csv(exposures_csv, delimiter=",", skipinitialspace=True)

# Analytical computation
v01 = V0 + V1
b01 = (B0 + B1) / 2.0
mu1_pos = -MU1
mu01 = max(MU0, mu1_pos) - min(MU0, mu1_pos)
sigma01 = np.sqrt(SIGMA0**2 + SIGMA1**2)

term1 = norm.cdf(+b01, loc=mu01, scale=sigma01)
term2 = norm.cdf(-b01, loc=mu01, scale=sigma01)
p_ij = term1 - term2

length = haversine(WP0, WP1)

n_rate = length * p_ij * Q0 * Q1 * v01 / (V0 * V1)

# Check results
results = np.zeros(1)

if np.abs(n_rate - df["exposures"].iloc[0]) < 1e-6:
    results[0] = 1
else:
    print("Mismatch of computed head-on exposure rates.")

if np.prod(results) > 0.5:
    print("ok")