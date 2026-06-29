import os
from pathlib import Path

import numpy as np
import pandas as pd

WORKSPACE = os.getcwd()
project_name = os.path.basename(WORKSPACE)
main_samson_folder = str(Path(WORKSPACE).parents[0])
TOL = 1.5 * 1e-2
case_path = f"{WORKSPACE}/case1"

linkobject_exposures = pd.read_csv(f"{case_path}/Results/linkobject.csv")
cellobject_exposures = pd.read_csv(f"{case_path}/Results/cellobject.csv")
shiplinkdata = pd.read_csv(f"{case_path}/ModelData/shiplinkdata.csv")
shiplinkdata.frequency.iloc[0], shiplinkdata.frequency.iloc[1:].sum()

ok = True
for i, my_df in linkobject_exposures.groupby('objectUID'):
    for use_col in ['ramCollisions', 'driftCollisions']:
        x = my_df.iloc[0][use_col]
        y = my_df.iloc[1:][use_col].sum()
        if np.abs(x - y) / (x + y) > TOL:
            ok = False
            print(f'{use_col} result for larger sigma is not as was expected')
            print(use_col, x / y)

for i, my_df in cellobject_exposures.groupby('objectUID'):
    large_cell_df = my_df[my_df.cell_uid == 'C1']
    small_cells_df = my_df[my_df.cell_uid != 'C1']

    for my_col in ['driftExposure', 'ramExposure']:
        x = large_cell_df.driftExposure.sum()
        y = small_cells_df.driftExposure.sum()
        if (x - y) / (y + y) > TOL:
            ok = False
            print(f'difference between {my_col} is too large:')
            print(f"4 cells gives {y}, one large cell gives {x}")

if ok:
    print('ok')
