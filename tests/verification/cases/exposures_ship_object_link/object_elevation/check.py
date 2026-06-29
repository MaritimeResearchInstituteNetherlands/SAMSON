import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from scipy import stats

TOL = 1e-9

# Load results
df = pd.read_pickle("results.pkl")
df.columns = df.columns.str.strip()

exp_unique = df["experiment"].unique()
exp_unique = np.sort(exp_unique)

results = np.zeros(2)

drafts = []
occurences = []
for exp in exp_unique:
    
    subset = df["experiment"] == exp
    drafts.append( df[subset]["draft"].values[0] )
    occurences.append( len(df[subset]) )

left = stats.linregress(drafts[0:5], occurences[0:5])
right = stats.linregress(drafts[5:], occurences[5:])

if np.prod(np.array(drafts))>=0:
    results[0] = 1
else:
    print("Negative drafts detected, no wetted area.")
    results[0] = 0    

if (1 + left.slope/right.slope) < TOL:
    results[1] = 1
else:
    print("Slope should be symmetric.")
    results[1] = 0 

if np.prod(results) > 0.5:
    print("ok")

# Plotting
fig, ax = plt.subplots(1, 1, figsize=(6, 4), dpi=160)

ax.plot(drafts,
        occurences,
        color = "blue",
        linestyle = "-",
        marker="o",
        markersize=6,
        markerfacecolor="none",
        markeredgecolor="blue",
        label="samsonCLI")

ax.set_xlabel('Draft $[m]$')
ax.set_ylabel('Number of logged drift exposures $[-]$')
ax.yaxis.set_ticks([0,1,2,3,4,5])
ax.legend()

fig.savefig("figure1.svg", bbox_inches="tight")