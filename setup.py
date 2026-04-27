import sys
from setuptools import setup
from pybind11.setup_helpers import Pybind11Extension, build_ext

cpp_args = ['-std=c++17']

include_dirs = [
    "include",
    "src",
    "utils",
]

ext_modules = [
    Pybind11Extension(
        "bitbarrel",
        [
            "src/bindings.cc", 
            "src/bitbarrel.cc", 
            "src/segment.cc",
        ],
        include_dirs=include_dirs,
        extra_compile_args=cpp_args,
    ),
]

setup(
    name="bitbarrel",
    version="0.1.0",
    description="Python wrapper for the BitBarrel C++ KV store",
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
    zip_safe=False,
)
