import os
import numpy as np
import pandas as pd

# Workspace settings
WORKSPACE_INI = os.getcwd()
OUTPUT_DIR = "Results"
OUTPUT_FILE = "cell.csv"

# Constants
A = 1e6                  # [m2] cell area
RHO1 = 0.0010e-6         # [s s-1 m-2] cell density ship
RHO2 = 0.0006e-6         # [s s-1 m-2] cell density ship
MU_U1 = 3.7039968        # [m s-1] mean ship speed
MU_U2 = 4.9386624        # [m s-1] mean ship speed
SIGMA_U1 = 1.4918876     # [m s-1] std dev ship speed
SIGMA_U2 = 2.4178868     # [m s-1] std dev ship speed
L1 = 25.0                # [m] ship length
L2 = 70.0                # [m] ship length
B1 = 6.0                 # [m] ship breadth
B2 = 16.0                # [m] ship breadth
N = 32                   # [-] number of angles

# Tolerances
TOL_GENERAL = 1e-3
TOL_OVERTAKING = 1e-18
TOL_HEADON = 1e-14
TOL_CROSSING = 1e-18


def harmonic_speed(mu, sigma):
    """Compute harmonic mean-based speed approximation."""
    return mu - (sigma ** 2 / mu)


# Load CSV
exposures_csv = os.path.join(WORKSPACE_INI, OUTPUT_DIR, OUTPUT_FILE)
df = pd.read_csv(exposures_csv, delimiter=",", skipinitialspace=True)

# Ship geometric parameters
B12 = (B1 + B2) / 2

# First-order speed approximations
u1 = harmonic_speed(MU_U1, SIGMA_U1)
u2 = harmonic_speed(MU_U2, SIGMA_U2)

Q1 = RHO1 * u1 * A / np.sqrt(A)
Q2 = RHO2 * u2 * A / np.sqrt(A)

# Angular distributions
thetas1 = np.linspace(0, 2 * np.pi, N, endpoint=False)
P1s = np.ones(N) / N
thetas2 = np.linspace(0, 2 * np.pi, N, endpoint=False)
P2s = np.ones(N) / N

# Result containers
exposures_cross = []
exposures_head = []
exposures_over = []
thetas = []

# Loop over angle pairs
for theta2, P2 in zip(thetas2, P2s):
    for theta1, P1 in zip(thetas1, P1s):
        x1 = np.cos(theta1)
        y1 = np.sin(theta1)
        x2 = np.cos(theta2)
        y2 = np.sin(theta2)

        dot = np.dot([x1, y1], [x2, y2])
        dot = np.clip(dot, a_min=-1, a_max=1)

        theta = np.arccos(dot)
        thetas.append(theta)

        # Overtaking
        if np.abs(theta - 0) < TOL_GENERAL:
            ndot = (
                P1 * P2 * 2 * B12 * RHO1 * RHO2 * u1 * u2 * A *
                np.abs(u2 - u1) / (u1 * u2)
            )
            exposures_over.append(ndot)

        # Head-on
        if np.abs(theta - np.pi) < TOL_GENERAL:
            ndot = (
                P1 * P2 * 2 * B12 * RHO1 * RHO2 * u1 * u2 * A *
                (u2 + u1) / (u1 * u2)
            )
            exposures_head.append(ndot)

        # General crossing case
        u12 = np.sqrt(u1**2 + u2**2 - 2 * u1 * u2 * np.cos(theta))

        d1_1 = L1 * u2 * np.sin(theta) / u12
        d1_2 = B1 * np.sqrt(1 - (np.sin(theta) * u2 / u12) ** 2)
        d1 = d1_1 + d1_2

        d2_1 = L2 * u1 * np.sin(theta) / u12
        d2_2 = B2 * np.sqrt(1 - (np.sin(theta) * u1 / u12) ** 2)
        d2 = d2_1 + d2_2

        d12 = d1 + d2

        ndot = P2 * Q2 * d12 * P1 * Q1 * u12 / (u1 * u2)
        exposures_cross.append(ndot)

# Summaries
exposures_over_sum = np.sum(exposures_over)
exposures_head_sum = np.sum(exposures_head)
exposures_cross_sum = np.sum(exposures_cross)

# Unique angles
thetas = np.around(thetas, 6)
thetas_uni = np.unique(thetas)

exposures_uni = []
for theta_uni in thetas_uni:
    valid = thetas == theta_uni
    exposures_uni.append(np.sum(valid * exposures_cross))

# Test array
results = np.zeros(4)

# Return index of valid row
idx = df.index[df.isin(["SC0"]).sum(axis=1) == 1][0]

# Model comparisons
if len(df) == 3:
    results[0] = 1
else:
    print("Mismatch of logged exposures.")

if np.abs(exposures_over_sum - exposures_uni[0]) < TOL_OVERTAKING:
    results[1] = 1
else:
    print("Mismatch of exposures: overtaking vs crossing model")

if np.abs(exposures_head_sum - exposures_uni[1]) < TOL_HEADON:
    results[2] = 1
else:
    print("Mismatch of exposures: head-on vs crossing model")

if np.abs(exposures_cross_sum / A - df.exposures[idx] / df.area[idx]) < TOL_CROSSING:
    results[3] = 1
else:
    print("Mismatch of computed exposures in cell per area unit")

# Final verdict
if np.prod(results) > 0.5:
    print("ok")