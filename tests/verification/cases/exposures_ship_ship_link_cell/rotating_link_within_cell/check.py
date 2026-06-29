import os
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

# Constants
Q1 = 0.00091          # [s-1] link frequency
RHO2 = 1.000e-009     # [s s-1 m-2] cell density ship
U1 = 5                # [m s-1] ship speed
U2 = 3.1031           # [m s-1] ship speed
L1 = 25.0             # [m] ship length
L2 = 25.0             # [m] ship length
B1 = 6.0              # [m] ship breadth
B2 = 6.0              # [m] ship breadth
L = 2753.7767         # [m] link length
SIGMA = 100           # [m] Sigma value of link's lateral distribution

# Tolerance
TOL = 1e-2            # [s-1] applied tolerance for comparison

# Load results
df = pd.read_pickle("results.pkl")
df.columns = df.columns.str.strip()

# Geometry
width = 6.0 * SIGMA
area = L * width
rho1 = Q1 / (U1 * width)
q1q2 = rho1 * RHO2 * U1 * U2 * area

# Initial values for test array
THETA_I = 0.0
NUM_THETAS = 37
thetas = np.linspace(0, 2 * np.pi, num=NUM_THETAS, endpoint=True)
results = np.zeros(NUM_THETAS)

# Iterate over rows
ndots = []
for index, row in df.iterrows():
    exposures = row["exposures"]

    theta = thetas[index] - THETA_I
    theta = np.abs( (theta + np.pi) % (2 * np.pi) - np.pi )

    U12 = np.sqrt(U1 ** 2 + U2 ** 2 - 2 * U1 * U2 * np.cos(theta))

    # Ship domain sizes
    d1_1 = L1 * U2 * np.sin(theta) / U12
    d1_2 = B1 * np.sqrt(1 - (np.sin(theta) * U2 / U12) ** 2)
    d1 = d1_1 + d1_2

    d2_1 = L2 * U1 * np.sin(theta) / U12
    d2_2 = B2 * np.sqrt(1 - (np.sin(theta) * U1 / U12) ** 2)
    d2 = d2_1 + d2_2

    d12 = d1 + d2

    ndot = d12 * q1q2 * U12 / (U1 * U2)
    ndots.append(ndot)
    
    if np.abs(1.0 - ndot / exposures) < TOL:
        results[index] = 1
    else:
        print(
            "Mismatch of exposures derived from crossing model in cell/link with:"
            f"theta: {theta} [rad]"
        )
        print(
            "Exposures computed by SamsonCLI: "
            f"{exposures} [s-1]"
        )
        print(
            "Exposures computed by test script: "
            f"{ndot} [s-1]"
        )

# Final verdict
if np.prod(results) > 0.5:
    print("ok")

# Plotting
fig, ax = plt.subplots(1, 1, figsize=(6, 4), dpi=160)

ax.plot(np.degrees(thetas),
        df["exposures"].values,
        color = "blue",
        linestyle = "-",
        alpha = 0.8,        
        marker="o",
        markersize=6,
        markerfacecolor="none",
        markeredgecolor="blue",
        label="samsonCLI")

ax.plot(np.degrees(thetas),
        ndots,
        color = "orange",
        linestyle = "--",
        alpha = 0.8, 
        marker="o",
        markersize=10,
        markerfacecolor="none",
        markeredgecolor="orange",
        label="Test script")

ax.set_xlabel('Absolute link angle $[deg]$')
ax.set_ylabel('Exposure frequency $[s^{-1}]$')
ax.legend()

fig.savefig("figure1.svg", bbox_inches="tight")