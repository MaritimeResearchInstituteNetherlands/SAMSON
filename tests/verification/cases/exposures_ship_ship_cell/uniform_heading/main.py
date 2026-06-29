import os
import subprocess

# Workspace settings
WORKSPACE = os.getcwd()

# Build command as a list
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
