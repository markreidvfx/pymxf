from distutils.core import setup, Extension
from Cython.Build import cythonize
import os
import subprocess

os.environ['ARCHFLAGS'] ="-arch x86_64"
ext_extra = {
    'include_dirs': ['headers'],
    'library_dirs': [],
    'library_dirs': [],
    'libraries': ['MXF-1.0']
}

def pkgconfig(libname):
    cmd = ['pkg-config', '--cflags-only-I', '--libs-only-L', libname]
    
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    
    stdout, stderr= p.communicate()
    
    if p.returncode != 0:
        raise Exception(stderr)
    include_dirs = []
    library_dirs = []
    for item in stdout.split():
        if item.startswith("-L"):
            library_dirs.append(item[2:])
        elif item.startswith("-I"):
            include_dirs.append(item[2:])
            
    return {'include_dirs': include_dirs, 'library_dirs':library_dirs}
    
libmxf_info = pkgconfig('libMXF-1.0')

ext_extra['include_dirs'].extend(libmxf_info['include_dirs'])
ext_extra['library_dirs'].extend(libmxf_info['library_dirs'])
ext_util_source = [os.path.join('headers', 'label_table.c')]

ext_modules = []

for dirname, dirnames, filenames in os.walk("mxf"):
    for filename in filenames:
        if filename.startswith('.') or os.path.splitext(filename)[1] != '.pyx':
            continue

        path = os.path.join(dirname, filename)
        name = os.path.join("mxf", os.path.splitext(filename)[0])
        sources=[path]
        sources.extend(ext_util_source)
        
        ext_modules.append(Extension(
            name,
            sources=sources,
            **ext_extra
        ))


setup(

    name='mxf',
    version='0.1',
    description='Python bindings libMXF.',
    
    author="Mark Reid",
    author_email="mindmark@gmail.com",
    
    url="",
    license="GPLv2",

    packages=['mxf'],
    ext_modules=cythonize(ext_modules, include_path=["headers"]),

)
