from setuptools import setup
from glob import glob
from pybind11.setup_helpers import Pybind11Extension, build_ext
from pybind11 import get_cmake_dir
from itertools import chain
import sys

__version__ = "0.0.1"
paths = ["src/pybind11/*.cpp", "src/core/*.cpp", "src/util/*.cpp"]
files = sorted(chain.from_iterable([glob(i) for i in paths]))

ext_modules = [
	Pybind11Extension("MEL", files, cxx_std=17, include_dirs=['include']),
]

setup(
	name     = "MEL",
	version  = __version__,
	author   = "Kieran Weaver",
	author_email = "weaver2@student.ubc.ca",
	url      = "https://github.com/Kieran-Weaver/StarrySky",
	description = "A python binding to the Starry Sky collision engine",
	long_description = "",
	ext_modules = ext_modules,
	zip_safe = False,
)
