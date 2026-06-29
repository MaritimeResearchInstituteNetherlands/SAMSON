import os
import subprocess
import numpy as np
import pandas as pd

# Constants / Configuration 
NUM_THETAS_I = 5
NUM_THETAS_J = 37

WORKSPACE = os.getcwd()
MODEL_DATA_DIRECTORY = os.path.join(".", "ModelData")
RESULTS_DIRECTORY = os.path.join(".", "Results")

SAMSON_CMD = [
    "SamsonCLI",
    "--config-yaml", "config.yaml",
    "--actions", "actions.txt",
]

RESULTS_FILENAME = "cell.csv"
EXPORT_FILENAME = "shipcelldata.csv"

UIDS = ["SCD0", "SCD1"]
DENSITIES = [1.000e-9, 6.000e-10]
SPEEDS = ["", ""]

# Derived angle arrays 
thetas_i = np.linspace(0, np.pi, num=NUM_THETAS_I, endpoint=True)
thetas_j = np.linspace(0, 2 * np.pi, num=NUM_THETAS_J, endpoint=True)

# Ensure directories exist 
os.makedirs(MODEL_DATA_DIRECTORY, exist_ok=True)

# Collect results 
rows = []

for theta_i in thetas_i:
    for theta_j in thetas_j:
        # Build input table for Samson
        df0 = pd.DataFrame(
            {
                "uid": UIDS,
                "density": DENSITIES,
                "speed": SPEEDS,
                "direction": [theta_i, theta_j],
            }
        )

        export_path = os.path.join(MODEL_DATA_DIRECTORY, EXPORT_FILENAME)
        df0.to_csv(export_path, index=False, header=True)

        # Run external tool
        result = subprocess.run(
            SAMSON_CMD,
            cwd=WORKSPACE,
            capture_output=True,
            text=True,
            check=False,  # set True to raise on non-zero return codes
        )

        # Log output (stdout/stderr)
        if result.stdout:
            print(result.stdout.strip())
        if result.returncode != 0:
            print(f"[SamsonCLI] Return code: {result.returncode}")
            if result.stderr:
                print(result.stderr.strip())

        # Import results
        results_import = os.path.join(RESULTS_DIRECTORY, RESULTS_FILENAME)
        if not os.path.exists(results_import):
            print("Results file not found: cell.csv")
            continue

        df_import = pd.read_csv(results_import)
        # print(df_import)

        if df_import.empty:
            print("Empty cell.csv file.")
            continue

        # Expect one exposure row
        if (
            len(df_import) == 1
        ):
            rows.append(
                {
                    "theta_i": theta_i,
                    "theta_j": theta_j,
                    "area": df_import["area"].iloc[0],
                    "exposures": df_import["exposures"].iloc[0],
                }
            )
        else:
            print("Mismatch of expected exposures.")

# Assemble final DataFrame
df = pd.DataFrame(rows, columns=["theta_i", "theta_j", "area", "exposures"])
print(df)
df.to_pickle("results.pkl")