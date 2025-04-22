from setuptools import Extension, setup

setup(
    ext_modules=[
        Extension(
            name="pimsim",
            sources=["pymod.cpp"],
        ),
    ]
)
