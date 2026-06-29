import os
import subprocess

import geopandas as gpd
import matplotlib.pyplot as plt
import pandas as pd


def plot_area_overview(case_folder, file_format='png'):
    export_folder = f"{case_folder}/export"
    shp_files = [f"{export_folder}/{x}" for x in os.listdir(export_folder) if x.endswith('.shp')]
    shapefile_dict = {os.path.basename(x)[:-4]: gpd.read_file(x) for x in shp_files}

    fig, ax = plt.subplots(figsize=(20, 20))

    if not shapefile_dict['links'].empty: shapefile_dict['links'].plot(ax=ax, alpha=0.5, label='Link')
    if not shapefile_dict['objects'].empty: shapefile_dict['objects'].plot(ax=ax)
    if not shapefile_dict['cells'].empty: shapefile_dict['cells'].plot(ax=ax, alpha=0.5, label='Cells')
    ax.set_title(os.path.basename(case_folder))
    fig.savefig(f'{case_folder}/area_overview.{file_format}')
    plt.close(fig)


WORKSPACE = os.getcwd()
case_name = 'case1'
project_name = os.path.basename(WORKSPACE)

case_path = f"{WORKSPACE}/{case_name}"

samson_command = 'samsoncli.exe --config-yaml config.yaml --actions actions.txt'
result = subprocess.run(samson_command, shell=True, cwd=case_path, capture_output=True, text=True, check=False)

if result.stdout:
    print(result.stdout.strip())

if result.returncode != 0:
    print(f"[SamsonCLI] Return code: {result.returncode}")
    if result.stderr:
        print(result.stderr.strip())

# ---------------------------------------------------------------------------
# Calculate theoretic drift probabilties
# ---------------------------------------------------------------------------
# assume only one shiptype

plot_area_overview(case_path, 'svg')
