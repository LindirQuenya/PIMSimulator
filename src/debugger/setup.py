from setuptools import Extension, setup

setup(
    ext_modules=[
        Extension(
            name="pimsim",
            sources=["pymod.cpp", "pimapi.cpp"],
#            library_dirs=["lib"],
#            libraries=["dramsim2"],
            include_dirs=["../", "../../lib"],
            extra_objects=["../../libdramsim/libdramsim2.a"],
            extra_compile_args=['-std=c++20'],
        ),
    ]
)
