import os
import numpy as np
import pandas as pd

# Constants
A = 1e6               # [m2] cell area
RHO1 = 0.0010e-6      # [s s-1 m-2] cell density ship
RHO2 = 0.0006e-6      # [s s-1 m-2] cell density ship
MU_U1 = 3.7039968     # [m s-1] mean ship speed
MU_U2 = 4.9386624     # [m s-1] mean ship speed
SIGMA_U1 = 1.4918876  # [m s-1] standard deviation ship speed
SIGMA_U2 = 2.4178868  # [m s-1] standard deviation ship speed
L1 = 25.0             # [m] ship length
L2 = 70.0             # [m] ship length
B1 = 6.0              # [m] ship breadth
B2 = 16.0             # [m] ship breadth

# Tolerance
TOL = 2.0e-5           # [s-1] applied tolerance for comparison


def harmonic_speed(mu, sigma):
    """Return harmonic mean-based approximation of ship speed."""
    return mu - (sigma ** 2 / mu)


# Load results
df = pd.read_pickle("results.pkl")
df.columns = df.columns.str.strip()

# Geometry
B12 = (B1 + B2) / 2

# First-order approximations
u1 = harmonic_speed(MU_U1, SIGMA_U1)
u2 = harmonic_speed(MU_U2, SIGMA_U2)

q1 = RHO1 * u1 * A / np.sqrt(A)
q2 = RHO2 * u2 * A / np.sqrt(A)

# Initial values for test array
THETAS_I_N = 5
THETAS_J_N = 37
results = np.zeros(THETAS_I_N * THETAS_J_N)

# Iterate over rows
for index, row in df.iterrows():
    thetai = row["theta_i"]
    thetaj = row["theta_j"]
    area = row["area"]
    exposures = row["exposures"]

    theta = thetai - thetaj
    theta = np.abs( (theta + np.pi) % (2 * np.pi) - np.pi )

    u12 = np.sqrt(u1 ** 2 + u2 ** 2 - 2 * u1 * u2 * np.cos(theta))

    # Ship domain sizes
    d1_1 = L1 * u2 * np.sin(theta) / u12
    d1_2 = B1 * np.sqrt(1 - (np.sin(theta) * u2 / u12) ** 2)
    d1 = d1_1 + d1_2

    d2_1 = L2 * u1 * np.sin(theta) / u12
    d2_2 = B2 * np.sqrt(1 - (np.sin(theta) * u1 / u12) ** 2)
    d2 = d2_1 + d2_2

    d12 = d1 + d2

    ndot = q2 * d12 * q1 * u12 / (u1 * u2)
    ndot = area / A * ndot

    if np.abs(1.0 - ndot / exposures) < TOL:
        results[index] = 1
    else:
        print(
            "Mismatch of exposures derived from crossing model in cell with "
            f"theta_i: {thetai} [rad] and theta_j: {thetaj} [rad]"
        )

# Final verdict
if np.prod(results) > 0.5:
    print("ok")