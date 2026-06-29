import os
import subprocess

WORKSPACE = os.getcwd()

command = [
    "SamsonCLI",
    "--config-yaml", "config.yaml",
    "--actions", "actions.txt",
]

result = subprocess.run(
    command,
    cwd=WORKSPACE,
    capture_output=True,
    text=True,
)

print(result.stdout)
