import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

TOL = 1e-5

# Load results
df = pd.read_pickle("results.pkl")
df.columns = df.columns.str.strip()

longitude = []
exposures = []
c0_list = []
c1_list = []

experiments = np.unique(df["experiment"])

for exp in experiments:
    filtered_df = df[df["experiment"] == exp]
    longitude.append(filtered_df["longitude"].iloc[0])

    c0_exposures = filtered_df[filtered_df["cell_uid"] == "C0"]["exposures"].sum()
    c1_exposures = filtered_df[filtered_df["cell_uid"] == "C1"]["exposures"].sum()

    c0_list.append(c0_exposures)
    c1_list.append(c1_exposures)
    exposures.append(c0_exposures + c1_exposures)

results = np.zeros(len(experiments))
exposure_mean = np.mean(exposures)

for i, exposure in enumerate(exposures):
    if np.abs(1.0 - exposure / exposure_mean) < TOL:
        results[i] = 1
    else:
        print("Sum of exposures not constant for all experiments.")
        results[i] = 0

# Final verdict
if np.prod(results) > 0.5:
    print("ok")

# Plotting
fig, ax = plt.subplots(figsize=(6, 4), dpi=160)

ax.stackplot(longitude, c0_list, c1_list, colors=["blue", "lightblue"], labels=["samsonCLI cell C0", "samsonCLI cell C1"])
ax.set_xlabel("Longitude Value First Waypoint of Link [deg]")
ax.set_ylabel("Exposure frequency [s⁻¹]")
ax.yaxis.set_ticks([])
ax.yaxis.set_ticklabels([])
ax.legend()

fig.savefig("figure1.svg", bbox_inches="tight")
