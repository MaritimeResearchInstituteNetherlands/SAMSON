import os
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from scipy.stats import linregress


# Tolerances
TOL_STDERR = 1e-004
TOL_SLOPE = 1e-008         

# Load results
df = pd.read_pickle("results.pkl")
df.columns = df.columns.str.strip()

x_left = df["longitude"].iloc[0:4]
y_left = df["exposures"].iloc[0:4]
left = linregress(x_left, y_left)

x_mid = df["longitude"].iloc[3:7]
y_mid = df["exposures"].iloc[3:7]
mid = linregress(x_mid, y_mid)

x_right = df["longitude"].iloc[6:]
y_right = df["exposures"].iloc[6:]
right = linregress(x_right, y_right)

results = np.zeros(7)

if len(df) == 10:
    results[0] = 1
else:
    print("Mismatch of link cell exposure number.")

if np.any(df["exposures"].iloc[1:-2] != 0):
    results[1] = 1
else:
    print("Non-zero exposures expected.")

if left.stderr < TOL_STDERR:
    results[2] = 1
else:
    print("Standard deviation of left 4 samples too large.")

if mid.stderr < TOL_STDERR:
    results[3] = 1
else:
    print("Standard deviation of mid 4 samples too large.")

if right.stderr < TOL_STDERR:
    results[4] = 1
else:
    print("Standard deviation of right 4 samples too large.")
 
if mid.slope < TOL_SLOPE:
    results[5] = 1
else:
    print("Slope mid 4 samples should be zero.")

if np.abs(left.slope + right.slope) < TOL_SLOPE:
    results[6] = 1
else:
    print("The left slope gradient is not opposite to the right slope gradient.")    
 
# Final verdict
if np.prod(results) > 0.5:
    print("ok")

# Plotting
fig, ax = plt.subplots(1, 1, figsize=(6, 4), dpi=160)

ax.plot(df["longitude"].values,
        df["exposures"].values,
        color = "blue",
        linestyle = "-",
        marker="o",
        markersize=6,
        markerfacecolor="none",
        markeredgecolor="blue",
        label="samsonCLI")

ax.set_xlabel('First Longitude Waypoint Link $[deg]$')
ax.set_ylabel('Exposure frequency $[s^{-1}]$')
ax.yaxis.set_ticks([])
ax.yaxis.set_ticklabels([])
ax.legend()

fig.savefig("figure1.svg", bbox_inches="tight")