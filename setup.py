from distutils.core import setup, Extension
import os
import subprocess


ext_extra = {
    'include_dirs': ['headers',os.path.join('lib','include')],
    'library_dirs': ['lib'],
    'libraries': ['MXF']
}


# Construct the modules that we find in the "build/cython" directory.
ext_modules = []
build_dir = os.path.abspath(os.path.join(__file__, '..', 'build', 'cython'))
for dirname, dirnames, filenames in os.walk(build_dir):
    for filename in filenames:
        if filename.startswith('.') or os.path.splitext(filename)[1] != '.c':
            continue

        path = os.path.join(dirname, filename)
        name = os.path.splitext(os.path.relpath(path, build_dir))[0].replace('/', '.')

        ext_modules.append(Extension(
            name,
            sources=[path],
            **ext_extra
        ))


setup(

    name='mxf',
    version='0.1',
    description='Python bindings libMXF.',
    
    author="Mark Reid",
    author_email="mindmark@gmail.com",
    
    url="",
    
    ext_modules=ext_modules,

)