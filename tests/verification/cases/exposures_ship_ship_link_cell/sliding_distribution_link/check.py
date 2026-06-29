import os
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt         

# Tolerance
TOL = 1e-20            # [s-1] applied tolerance for comparison

# Load results
df = pd.read_pickle("results.pkl")
df.columns = df.columns.str.strip()

mid = len(df) // 2

results = np.zeros(1)

if df["exposures"].loc[mid] == np.max(df["exposures"]):
    results[0] = 1
else:
    print("Mid element of the exeriments should return the largest exposure value.")

for i in range(0, mid-1):
    left = df["exposures"].iloc[i]
    right = df["exposures"].iloc[-i-1]
    if np.abs(1 - left/right) < TOL:
        results = np.append(results, 1)
    else:
        results = np.append(results, 0)
        print("Non-symmetric exposures,")

# Final verdict
if np.prod(results) > 0.5:
    print("ok")

# Plotting
fig, ax = plt.subplots(1, 1, figsize=(6, 4), dpi=160)

ax.plot(df["mu"].values,
        df["exposures"].values,
        color="blue",
        linestyle = "-",
        marker="o",
        markersize=6,
        markerfacecolor="none",
        markeredgecolor="blue",
        label="samsonCLI")

ax.set_xlabel(r'$\mu$ value link distribution $[m]$')
ax.set_ylabel(r'Exposure frequency $[s^{-1}]$')
ax.yaxis.set_ticks([])
ax.yaxis.set_ticklabels([])
ax.legend()

fig.savefig("figure1.svg", bbox_inches="tight")