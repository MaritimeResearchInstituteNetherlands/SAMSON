import os
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt         

# Tolerance
TOL = 1e-20 # [s-1] applied tolerance for comparison

# Workspace and file settings
WORKSPACE_INI = os.getcwd()
OUTPUT_FILE = "results.pkl"

# Load results
df = pd.read_pickle(OUTPUT_FILE)
df.columns = df.columns.str.strip()

mid = len(df) // 2

results = np.zeros(3)

if df["driftExposures"].iloc[0] == 0:
    results[0] = 1
else:
    print("First element of the experiments should return a zero exposure value.")

if df["driftExposures"].iloc[mid] == np.max(df["driftExposures"]):
    results[1] = 1
else:
    print("Mid element of the experiments should return the largest exposure value.")

if df["driftExposures"].iloc[-1] == 0:
    results[2] = 1
else:
    print("Last element of the experiments should return a zero exposure value.")

for i in range(1, mid):
    left = df["driftExposures"].iloc[i]
    right = df["driftExposures"].iloc[-i-1]
    if np.abs(1 - left/right) < TOL:
        results = np.append(results, 1)
    else:
        results = np.append(results, 0)
        print("Non-symmetric driftExposure")

# Final verdict
if np.prod(results) > 0.5:
    print("ok")

# Plotting
fig, ax = plt.subplots(1, 1, figsize=(6, 4), dpi=160)

ax.plot(df["longitude"].values,
        df["driftExposures"].values,
        color = "blue",
        linestyle = "-",
        marker="o",
        markersize=6,
        markerfacecolor="none",
        markeredgecolor="blue",
        label="samsonCLI")

ax.set_xlabel(r'Longitude value left bottom corner cell $[deg]$')
ax.set_ylabel(r'Drift exposure frequency $[s^{-1}]$')
ax.yaxis.set_ticks([])
ax.yaxis.set_ticklabels([])
ax.legend()

fig.savefig("figure1.svg", bbox_inches="tight")