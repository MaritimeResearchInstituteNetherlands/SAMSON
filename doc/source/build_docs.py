import argparse
import os
import shutil
import subprocess
import sys

REL_SCRIPT_PATH = r"\doc\source\build_docs.py"
CONDA_ENV_NAME = "samson_documentation"
OUTPUT_DIR_NAME = "doc_build"


def parse_arguments():
    """Parse command line arguments."""
    parser = argparse.ArgumentParser(description="Build the documentation in a conda environment")
    parser.add_argument('--env-prefix', help="The prefix path for the conda environment.")
    parser.add_argument('--clean', action='store_true', help="Remove conda environment after building")
    parser.add_argument('--debug', action='store_true', help="Enable debug mode to print additional information.")
    return parser.parse_args()


def get_conda_prefix():
    """Set the prefix for the conda env to the parent of the source directory"""
    abs_script_path = os.path.abspath(__file__)
    REL_SCRIPT_PATH = r"\doc\source\build_docs.py"
    if abs_script_path.endswith(REL_SCRIPT_PATH):
        coderoot = abs_script_path[:-len(REL_SCRIPT_PATH)]
        conda_prefix = os.path.dirname(coderoot)
        return conda_prefix
    else:
        print("Error: Can't locate checkout directory")
        sys.exit(1)


def check_conda():
    """Check if conda can be executed successfully and print its version if debug is enabled."""
    if debug:
        print("DEBUG: Start checking Conda")
    command = ['conda', '--version']
    try:
        result = subprocess.run(command, shell=True, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        version = result.stdout.decode().strip()
        if debug:
            print(f"Conda executable is available. Version: {version}")
    except subprocess.CalledProcessError:
        print("Error: conda executable not found or cannot be executed.")
        sys.exit(1)


def check_env_path_for_spaces(env_path):
    """Spaces in the path will result in separate tokens for the subprocess.run method"""
    if debug:
        print("DEBUG: Start checking env_path for spaces")
    if " " in env_path:
        raise Exception(f"No spaces allowedin in env-path\n{env_path}")


def create_conda_env(env_path):
    """Create a conda environment in the specified path."""
    env_yaml = os.path.join(os.path.dirname(os.path.abspath(__file__)), "conda_env.yml")
    if debug:
        print(f"DEBUG: Start creating Conda environment in {env_path}, using {env_yaml}.")
    command = ['conda', 'env', 'create', '-y', '--prefix', env_path, '--file', env_yaml]
    if buildfarm:
        # conda env create without progressbar
        command.append('--quiet')
    try:
        subprocess.run(command, shell=True, text=True)
    except subprocess.CalledProcessError:
        print("Error: Failed to create conda environment.")
        sys.exit(1)


def remove_env(env_path):
    """Remove te conda environment after building."""
    if debug:
        print(f"DEBUG: Start removing Conda environment in {env_path}.")
    command = ['conda', 'env', 'remove', '-y', '--prefix', env_path]
    try:
        subprocess.run(command, shell=True, text=True)
    except subprocess.CalledProcessError:
        print("Error: Failed to remove conda environment.")
        sys.exit(1)


def build_docs(env_path, doc_source, output_dir):
    """Build the documentation in the conda environment"""
    if debug:
        print(f"DEBUG: Building documentation in {output_dir}.")
    # Notice that the following "script" is actually a one-liner
    # with && after every statement:
    script = ['conda', 'activate', env_path, '&&', 'sphinx-build', '-M', 'html', doc_source, output_dir]
    try:
        subprocess.run(script, shell=True, check=True)
    except subprocess.CalledProcessError:
        print("Error: Something went wrong while building the documentation")
        sys.exit(1)


if __name__ == "__main__":
    args = parse_arguments()
    debug = args.debug
    # Check if we are running in Bamboo
    if "BAMBOO_BUILD_WORKING_DIRECTORY" in os.environ:
        buildfarm = True
        if debug:
            print("DEBUG: We are running in bamboo.")
    else:
        buildfarm = False
    check_conda()
    if args.env_prefix:
        conda_prefix = args.env_prefix
    else:
        conda_prefix = get_conda_prefix()
    check_env_path_for_spaces(conda_prefix)
    conda_env = os.path.join(conda_prefix, CONDA_ENV_NAME)
    create_conda_env(conda_env)
    if buildfarm:
        doc_build_dir = os.path.join(os.environ["BAMBOO_BUILD_WORKING_DIRECTORY"], OUTPUT_DIR_NAME)
    else:
        doc_build_dir = os.path.join(conda_prefix, OUTPUT_DIR_NAME)
    doc_source_dir = os.path.dirname(os.path.abspath(__file__))
    build_docs(conda_env, doc_source_dir, doc_build_dir)
    if buildfarm:
        shutil.make_archive('documentation', 'zip', os.environ["BAMBOO_BUILD_WORKING_DIRECTORY"], f"{OUTPUT_DIR_NAME}")
    if args.clean:
        remove_env(conda_env)
