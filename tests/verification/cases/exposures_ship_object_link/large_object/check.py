import os
import numpy as np
import pandas as pd

# Workspace and file settings
WORKSPACE_INI = os.getcwd()
OUTPUT_DIR = "Results"
OUTPUT_FILE = "linkobject.csv"


def checkResult(vector):
    for i in range(0,len(vector)):
        if (vector[i] <= 0.0):
            return False
    return True

# Load exposure results
exposures_csv = os.path.join(WORKSPACE_INI, OUTPUT_DIR, OUTPUT_FILE)
df = pd.read_csv(exposures_csv, delimiter=",", skipinitialspace=True)

df0 = df[df['trafficUID'] == 'sl0001']
df1 = df[df['trafficUID'] == 'sl0002']

results = np.zeros((2,2))
results[0][0] = checkResult(df0['driftExposure'].to_numpy())
results[0][1] = checkResult(df0['ramExposure'].to_numpy())
results[1][0] = checkResult(df1['driftExposure'].to_numpy())
results[1][1] = checkResult(df1['ramExposure'].to_numpy())

# Flatten results
results = results.ravel()

if np.prod(results) > 0.5:
    print("ok")