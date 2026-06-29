import os
import sys
import tomllib
from pathlib import Path

with open("main.toml", "rb") as f:
    metadata = tomllib.load(f)

path = metadata["externals"][0]

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), path)))

from utils.testing import run_samson_test

WORKSPACE = os.getcwd()
project_name = os.path.basename(WORKSPACE)
main_samson_folder = str(Path(WORKSPACE).parents[0])

case_list = ['standard_wind', 'simple_wind', 'current']

for case_name in case_list:
    case_path = f"{WORKSPACE}/{case_name}"
    run_samson_test(case_path)
