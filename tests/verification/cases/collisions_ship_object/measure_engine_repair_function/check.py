import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import sys
import tomllib
import os

with open("main.toml", "rb") as f:
    metadata = tomllib.load(f)

path = metadata["externals"][0]

sys.path.append(
    os.path.abspath(os.path.join(os.path.dirname(__file__), path))
)

from utils.gis import haversine
from utils.samson import drift_speed
from utils.samson import engine_repair

TOL = 5e-1

BFT = 5
DRAFT = 10.82
HEIGHT = 18.35
LENGTH = 170.06
FREQUENCY = 1000
CAUSATION_FACTOR_DRIFT = 1.6252699784017279e-09
LINK_LON = 3.3250
LINK_LAT0 = 52.9100
LINK_LAT1 = 52.9200

# Load results
df = pd.read_pickle("results.pkl")
df.columns = df.columns.str.strip()

link_length = haversine([LINK_LAT0, LINK_LON], [LINK_LAT1, LINK_LON])

lon_objects = df['longitude'].values
link_lat = (LINK_LAT0 + LINK_LAT1) / 2 
exposures = []
collisions = []
checks = []
for i, lon_link in enumerate(lon_objects):
    valid = df['longitude'].iloc[i] > LINK_LON
    
    distance = haversine([link_lat, LINK_LON], [link_lat, df['longitude'].iloc[i]])

    speed = drift_speed(BFT, DRAFT, HEIGHT)
    time = distance / speed
    p_en = engine_repair(time)
    
    exposure = FREQUENCY * link_length
    event = CAUSATION_FACTOR_DRIFT * exposure
    
    if valid:
        collision = p_en * event
        if df["driftCollisions"].iloc[i] != 0:
            diff = np.abs(1 - (collision / df["driftCollisions"].iloc[i]))
            check =  diff < TOL
            if check:
                checks.append(check)
            else:
                checks.append(False)
                print("Deviation for the computated and compared collisions.")
    else:
        exposure = 0
        collision = 0
    
    exposures.append(exposure)
    collisions.append(collision)

if np.prod(np.array(checks))==1:
    print("ok")   

# Plotting
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(6, 4), dpi=160)

ax1.plot(
    df["longitude"],
    df["driftExposure"],
    color="blue",
    linestyle="-",
    alpha=0.8,
    label="samsonCLI",
)

ax1.plot(
    df["longitude"],
    exposures,
    color="orange",
    linestyle="--",
    alpha=0.8,
    label="Test script",
)

ax1.set_ylabel("Exposures [s⁻¹]")
ax1.set_yticks([])
ax1.set_yticklabels([])
ax1.legend()

ax2.plot(
    df["longitude"],
    df["driftCollisions"],
    color="blue",
    linestyle="-",
    alpha=0.8,
    label="samsonCLI",
)

ax2.plot(
    df["longitude"],
    collisions,
    color="orange",
    linestyle="--",
    alpha=0.8,
    label="Test script",
)

ax2.set_xlabel("Longitude value object [deg]")
ax2.set_ylabel("Collisions [s⁻¹]")
ax2.set_yticks([])
ax2.set_yticklabels([])
ax2.legend()

fig.savefig("figure1.svg", bbox_inches="tight")
plt.close(fig)
