from setuptools import setup, Extension
import sys

#My C module name and definition.
extension_module = Extension(
    'tempstats',
    sources=['temp_stats.c'],
    extra_compile_args=['-std=c99'] if sys.platform != 'win32' else [],
    libraries=[],
    include_dirs=[],
)

setup(
    name='tempstats',
    version='1.0',
    description='C extension for efficient temperature statistics',
    ext_modules=[extension_module],
    zip_safe=False,
)