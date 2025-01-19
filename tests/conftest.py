import os
import subprocess

import pytest

from .config import INPUT_FILENAME, MAKEFILE_PATH, OUTPUT_FILENAME


@pytest.fixture(scope="session", autouse=True)
def build_compiler():
    subprocess.run(["make", "clean"], cwd=MAKEFILE_PATH, check=True)
    subprocess.run(["make"], cwd=MAKEFILE_PATH, check=True)


@pytest.fixture(scope="session", autouse=True)
def cleanup_files():
    yield
    if os.path.exists(INPUT_FILENAME):
        os.remove(INPUT_FILENAME)
    if os.path.exists(OUTPUT_FILENAME):
        os.remove(OUTPUT_FILENAME)
