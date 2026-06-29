import os
import sys
import tomllib

import matplotlib.pyplot as plt
import seaborn as sns

with open("main.toml", "rb") as f:
    metadata = tomllib.load(f)

path = metadata["externals"][0]

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), path)))
from utils.samson import *

TOL = 1e-6

# Load results
ok = True
drift_factor = pd.read_csv('case1/causation/causationfactors.csv').drift_factor.iloc[0]
results_df = pd.read_pickle('results_combined.pkl')
tide_df = pd.read_csv('case1/area/tide.csv')
amplitude = tide_df.amplitude.iloc[0]
angular_frequency = tide_df.angular_frequency.iloc[0]
t = np.arange(24) * 3600
t_seconds = np.arange(24 * 3600).reshape(-1, 1)
tide_speeds = amplitude * np.cos(angular_frequency * t.reshape(-1, 1) + t[:12].reshape(1, -1) * angular_frequency)

tide_speed_seconds = np.choose((t_seconds / 3600).astype(int), tide_speeds)
tide_speed_seconds2 = amplitude * np.cos(angular_frequency * t_seconds + t[:12].reshape(1, -1) * angular_frequency)
travelled_distance = np.cumsum(tide_speed_seconds, axis=0)
my_list = []
x_p = results_df['distance'].values
for i in range(12):
    x_t = travelled_distance[:, [i]]
    hit_time = np.where(x_p < 0, np.argmax(-1 * x_t > x_p * -1, axis=0), np.argmax(x_t > x_p, axis=0))
    collision_probability = anchor(0) * engine_repair(hit_time) * (hit_time > 0) * 100

    my_list.append(collision_probability.reshape(-1, 1))
final_probability = np.concatenate(my_list, axis=1).mean(axis=1)

max_distance = np.max(travelled_distance)
results_df['manual_p'] = final_probability * drift_factor * results_df.passages
results_df['ratio'] = results_df['driftCollisions'] / results_df['manual_p']
collisions_right = results_df[results_df['distance'] > 0][['driftCollisions']].sum().iloc[0]
collisions_left = results_df[results_df['distance'] < 0][['driftCollisions']].sum().iloc[0]

# add 5% to the max distance because SAMSON approximates the max_distance
if results_df[results_df['distance'].abs() > max_distance * 1.05].driftCollisions.sum() > 0:
    print(f"There should not be collisions at a distance larger than {max_distance}")
    ok = False

if (results_df['manual_p'] - results_df.driftCollisions).abs().mean() > TOL:
    print('driftCollisions deviate too much from theoretical expected values')
if ok:
    print("ok")

# Plotting
fig, ax = plt.subplots(1, 1)
sns.lineplot(data=results_df, x='distance', y='driftCollisions', ax=ax, label='driftCollisions')
sns.lineplot(data=results_df, x='distance', y='manual_p', ax=ax, color='r', label='Manual approximation')
ax.get_legend().remove()
handles, labels = ax.get_legend_handles_labels()
fig.legend(handles, labels, loc='upper right', ncol=1, bbox_to_anchor=(0.9, 1), frameon=True)

fig.savefig("figure1.svg", bbox_inches="tight")
plt.close(fig)
