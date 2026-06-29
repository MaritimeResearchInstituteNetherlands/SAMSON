import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns

TOL = 1e-3

# Load results
ok = True
results_df_links = pd.read_pickle('results_combined.pkl')
results_df_cells = pd.read_pickle('results_combined_cells.pkl')
link_exposure_per_case = results_df_links.pivot(index=['distance'], columns=['case_name'], values='driftExposure')
cell_exposure_per_case = results_df_cells.pivot(index=['distance'], columns=['case_name'], values='driftExposure')
if link_exposure_per_case.std(axis=1).sum() > TOL:
    print("Too large difference between current and wind for links")
    ok = False

if link_exposure_per_case.std(axis=1).sum() > TOL:
    print("Too large difference between current and wind for cells")
    ok = False

if ok:
    print("ok")

# Plotting
plot_df = link_exposure_per_case.reset_index()
fig, axs = plt.subplots(1, 1, figsize=(16, 10))
sns.lineplot(data=results_df_links, x='distance', y='driftExposure', hue='case_name')
fig.savefig("figure1.svg", bbox_inches="tight")
plt.close(fig)
