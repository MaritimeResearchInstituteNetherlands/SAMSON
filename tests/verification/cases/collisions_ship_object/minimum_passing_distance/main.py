import os
import sys
import tomllib

with open("main.toml", "rb") as f:
    metadata = tomllib.load(f)

path = metadata["externals"][0]

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), path)))

from utils.testing import run_samson_test

WORKSPACE = os.getcwd()
case_name = 'case1'
project_name = os.path.basename(WORKSPACE)

case_path = f"{WORKSPACE}/{case_name}"

run_samson_test(case_path)
