import os
import numpy as np
import pandas as pd
from scipy.stats import norm

# Workspace and file configuration
WORKSPACE_INI = os.getcwd()
OUTPUT_DIR = "Results"
OUTPUT_FILE = "link.csv"

# Ship spacing and velocity constants
B0 = 30         # [m]
B1 = 40         # [m]
V0 = 11         # [m/s]
V1 = 12         # [m/s]
MU0 = -200      # [m]
MU1 = -300      # [m]
SIGMA0 = 100    # [m]
SIGMA1 = 150    # [m]
Q0 = 0.005      # [s-1]
Q1 = 0.010      # [s-1]
WP0 = [52, 3.5] # latitude, longitude
WP1 = [52, 4.0] # latitude, longitude


def haversine(coord0, coord1):
    """Compute distance using the Haversine formula (in meters)."""
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


# Load exposures data
exposures_csv = os.path.join(WORKSPACE_INI, OUTPUT_DIR, OUTPUT_FILE)
df = pd.read_csv(exposures_csv, delimiter=",", skipinitialspace=True)

# Analytical overtaking exposure model
v01 = np.abs(V0 - V1)
b01 = (B0 + B1) / 2.0
mu01 = max(MU0, MU1) - min(MU0, MU1)
sigma01 = np.sqrt(SIGMA0**2 + SIGMA1**2)

term1 = norm.cdf(+b01, loc=mu01, scale=sigma01)
term2 = norm.cdf(-b01, loc=mu01, scale=sigma01)
p_ij = term1 - term2

link_length = haversine(WP0, WP1)

n_rate = link_length * p_ij * Q0 * Q1 * v01 / (V0 * V1)

# Validation array
results = np.zeros(1)

# Compare the overtaking exposure rates
if np.abs(n_rate - df["exposures"].sum()) < 1e-6:
    results[0] = 1
else:
    print("Mismatch of computed overtaking exposure rates.")

# Final test
if np.prod(results) > 0.5:
    print("ok")