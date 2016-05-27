
#  Permission is hereby granted, free of charge, to any person 
#  obtaining a copy of this software and associated documentation 
#  files (the "Software"), to deal in the Software without restriction, 
#  including without limitation the rights to use, copy, modify, 
#  merge, publish, distribute, sublicense, and/or sell copies of the 
#  Software, and to permit persons to whom the Software is furnished 
#  to do so, subject to the following conditions:
#
#  The above copyright notice and this permission notice shall be 
#  included in all copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
#  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
#  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
#  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR 
#  ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF 
#  CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION 
#  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

import os
import sys
from distutils.core import setup, Extension

def getGDALFlags():
    """
    Return the flags needed to link in GDAL as a dictionary
    """
    extraargs = {}
    if sys.platform == 'win32':
        # Windows - rely on %GDAL_HOME% being set and set 
        # paths appropriately
        gdalhome = os.getenv('GDAL_HOME')
        if gdalhome is None:
            raise SystemExit("need to define %GDAL_HOME%")
        extraargs['include_dirs'] = [os.path.join(gdalhome, 'include')]
        extraargs['library_dirs'] = [os.path.join(gdalhome, 'lib')]
        extraargs['libraries'] = ['gdal_i']
    else:
        # Unix - can do better with actual flags using gdal-config
        import subprocess
        try:
            cflags = subprocess.check_output(['gdal-config', '--cflags'])
            if sys.version_info[0] >= 3:
                cflags = cflags.decode()
            extraargs['extra_compile_args'] = cflags.strip().split()

            ldflags = subprocess.check_output(['gdal-config', '--libs'])
            if sys.version_info[0] >= 3:
                ldflags = ldflags.decode()
            extraargs['extra_link_args'] = ldflags.strip().split()
        except OSError:
            raise SystemExit("can't find gdal-config - GDAL development files need to be installed")
    return extraargs

def getKEAFlags():
    """
    Return the flags needed to link in GDAL as a dictionary
    """
    extraargs = {}
    if sys.platform == 'win32':
        # Windows - rely on %KEA_HOME% being set and set 
        # paths appropriately
        gdalhome = os.getenv('KEA_HOME')
        if gdalhome is None:
            raise SystemExit("need to define %KEA_HOME%")
        extraargs['include_dirs'] = [os.path.join(gdalhome, 'include')]
        extraargs['library_dirs'] = [os.path.join(gdalhome, 'lib')]
        extraargs['libraries'] = ['libkea']
    else:
        # Unix - can do better with actual flags using kea-config
        import subprocess
        try:
            cflags = subprocess.check_output(['kea-config', '--cflags'])
            if sys.version_info[0] >= 3:
                cflags = cflags.decode()
            extraargs['extra_compile_args'] = cflags.strip().split()

            ldflags = subprocess.check_output(['kea-config', '--libs'])
            if sys.version_info[0] >= 3:
                ldflags = ldflags.decode()
            extraargs['extra_link_args'] = ldflags.strip().split()
        except OSError:
            raise SystemExit("can't find kea-config - KEA development files need to be installed")
    return extraargs

def mergeFlags(a, b):
    """
    Merges the dictonaries a and b and returns a new one.
    Assumes that the dictonary values are lists.
    If a key exists in both then the new dictionary will have a list
    with the contents of both input lists in it.
    """
    result = {}
    result.update(a)
    for key in b.keys():
        if key in result:
            result[key].extend(b[key])
        else:
            result[key] = b[key]
    return result

# get the flags for GDAL
gdalargs = getGDALFlags()

# get the flags for KEA
keaargs = getKEAFlags()

# merge them
args = mergeFlags(gdalargs, keaargs)

# create our extension
extkwargs = {'name':'neighbours', 'sources':['src/neighbours.cpp']}
# add gdalargs
extkwargs.update(args)

cmodule = Extension(**extkwargs)
ext_modules = [cmodule]

setup(name='kealib', 
    version='0.1', 
    description='Access to parts of kealib not exposed by GDAL',
    author='Sam Gillingham',
    author_email='gillingham.sam@gmail.com',
    ext_package = 'kealib',
    ext_modules = ext_modules,
    license='LICENSE.txt',
    url='https://bitbucket.org/chchrsc/kealib',
    classifiers=['Intended Audience :: Developers',
          'Operating System :: OS Independent',
          'Programming Language :: Python :: 2',
          'Programming Language :: Python :: 2.7',
          'Programming Language :: Python :: 3',
          'Programming Language :: Python :: 3.2',
          'Programming Language :: Python :: 3.3',
          'Programming Language :: Python :: 3.4',
          'Programming Language :: Python :: 3.5'])


