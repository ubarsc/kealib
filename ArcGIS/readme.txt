
This directory contains scripts for compiling
the KEA GDAL driver against the version of GDAL distributed
with ArcMap. 

See http://www.arcgis.com/home/item.html?id=cb7065146659428e9eadc9cd88268b9f
http://www.arcgis.com/home/item.html?id=9a1de2cfeced4f21956342ae91ef89e3
http://www.arcgis.com/home/item.html?id=bbe841db0b144578a05d85735010596e
http://www.arcgis.com/home/item.html?id=90bcc66343ea446ab24edc0d0178dd2d
http://www.arcgis.com/home/item.html?id=cf91203a1c1a4f0f8ce8c22e5e84877c
http://www.arcgis.com/home/item.html?id=96c49d7624674191b3ca902bad889f7e
https://www.arcgis.com/home/item.html?id=b346c508082942548541e2ba6e218261
https://www.arcgis.com/home/item.html?id=9789afe1ae9744a994af997a388088af
https://www.arcgis.com/home/item.html?id=051482dc4fa34383b733a594c1a96f9f
https://www.arcgis.com/home/item.html?id=9fea572de2e2492f833b990ee8d9077e
https://www.arcgis.com/home/item.html?id=2b1d72b7a1bd4526809054fcf0e1b595
https://www.arcgis.com/home/item.html?id=752b2c4cd8c24539bc9ca6f3afa522ea
https://www.arcgis.com/home/item.html?id=7736fe9995ea4a5fa85705c8ce5eddd4

Arc 9.3 - use 'gdal14' with Visual Studio 2008
Arc 10.0 - use 'gdal16' with Visual Studio 2008
Arc 10.1-10.3 - use 'gdal18' with Visual Studio 2008
Arc 10.4 - use 'gdal18b' with Visual Studio 2013
Arc 10.5 - use 'gdal201' with Visual Studio 2013 (vc12 subdir)
Arc 10.5.1 - use 'gdal211' with Visual Studio 2013 (vc12 subdir)
Arc Pro 1.4 - use 'gdal201' with Visual Studio 2015 (vc14 subdir)
Arc Pro 2.0 - use 'gdal211' with Visual Studio 2015 (vc14 subdir)
Arc 10.6 & Arc Pro 2.1 - use 'gdal211b' with Visual Studio 2017
Arc 10.6.1 & Arc Pro 2.2 - use 'gdal211c' with Visual Studio 2017
Arc 10.7.1 & Arc Pro 2.4 use 'gdal211e' with Visual Studio 2017
Arc 10.8.0 & Arc Pro 2.5 use 'gdal233e' with Visual Studio 2019
Arc 10.8.1 & Arc Pro 2.6 use 'gdal233e_2' with Visual Studio 2019
Arc Pro 2.7 use 'gdal233e_3' with Visual Studio 2019
Arc Pro 2.8 use 'gdal233e_4' with Visual Studio 2019
Arc 10.9 use 'gdal233e_5' with Visual Studio 2019
Arc 11 and Arc Pro3.0 use 'gdal34' with Visual Studio 2022

First run buildzlibhdf5.bat, then buildkea.bat and lastly buildkeaforarc.bat. 
You will need to have the indicated version(s) of Visual Studio installed.

The arcgdalforcompilation.zip available from the github releases contains an up
to date version of the headers and import libraries.
