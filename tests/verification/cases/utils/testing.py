import os
import subprocess

import geopandas as gpd
import matplotlib.pyplot as plt


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


def run_samson_test(case_path, cli_path=None):
    """
    Wrapper for running samsonCLI
    :param case_path: The folder with the case files
    :param cli_path: Path to the cli, of None, it is assumed that the folder containg the cli is added to PATH.
    This function does not escape spaces in the cli_path, this can be done like this:
    cli_path='C:/Users/"SAMSON 2.0"/marin_SAMSON-0.0.1-poc-2026.1l/bin/Release'
    :return:
    """
    samson_cli = f"{cli_path}/samsoncli.exe" if cli_path else "samsoncli.exe"
    samson_command = f'{samson_cli} --config-yaml config.yaml --actions actions.txt'
    result = subprocess.run(samson_command, shell=True, cwd=case_path, capture_output=True, text=True, check=False)

    if result.stdout:
        print(result.stdout.strip())

    if result.returncode != 0:
        print(f"[SamsonCLI] Return code: {result.returncode}")
        if result.stderr:
            print(result.stderr.strip())

    plot_area_overview(case_path, 'svg')
