from distutils.core import setup, Extension
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

# Construct the modules that we find in the "build/cython" directory.
ext_modules = []
build_dir = os.path.abspath(os.path.join(__file__, '..', 'build', 'cython'))
for dirname, dirnames, filenames in os.walk(build_dir):
    for filename in filenames:
        if filename.startswith('.') or os.path.splitext(filename)[1] != '.c':
            continue

        path = os.path.join(dirname, filename)
        name = os.path.splitext(os.path.relpath(path, build_dir))[0].replace('/', '.')
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
    
    ext_modules=ext_modules,

)
