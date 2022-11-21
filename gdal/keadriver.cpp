/*
 *  keadriver.cpp
 *
 *  Created by Pete Bunting on 01/08/2012.
 *  Copyright 2012 LibKEA. All rights reserved.
 *
 *  This file is part of LibKEA.
 *
 *  Permission is hereby granted, free of charge, to any person 
 *  obtaining a copy of this software and associated documentation 
 *  files (the "Software"), to deal in the Software without restriction, 
 *  including without limitation the rights to use, copy, modify, 
 *  merge, publish, distribute, sublicense, and/or sell copies of the 
 *  Software, and to permit persons to whom the Software is furnished 
 *  to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be 
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
 *  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR 
 *  ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF 
 *  CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION 
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "keaband.h" // for HAVE_64BITIMAGES

CPL_C_START
void CPL_DLL GDALRegister_KEA(void);
CPL_C_END

// method to register this driver
void GDALRegister_KEA()
{
    GDALDriver  *poDriver;

    if (! GDAL_CHECK_VERSION("KEA"))
        return;

    if( GDALGetDriverByName( "KEA" ) == nullptr )
    {
        poDriver = new GDALDriver();
        
        poDriver->SetDescription( "KEA" );
        poDriver->SetMetadataItem( GDAL_DMD_LONGNAME, 
                                   "KEA Image Format (.kea)" );
        poDriver->SetMetadataItem( GDAL_DMD_EXTENSION, "kea" );
        poDriver->SetMetadataItem( GDAL_DMD_CREATIONDATATYPES, 
                            "Byte Int16 UInt16 Int32 UInt32 "
#ifdef HAVE_64BITIMAGES
                            "Int64 UInt64 "
#endif
                            "Float32 Float64" );
        poDriver->SetMetadataItem( GDAL_DMD_CREATIONOPTIONLIST, "\
<CreationOptionList> \
<Option name='IMAGEBLOCKSIZE' type='int' description='The size of each block for image data'/> \
<Option name='ATTBLOCKSIZE' type='int' description='The size of each block for attribute data'/> \
<Option name='MDC_NELMTS' type='int' description='Number of elements in the meta data cache'/> \
<Option name='RDCC_NELMTS' type='int' description='Number of elements in the raw data chunk cache'/> \
<Option name='RDCC_NBYTES' type='int' description='Total size of the raw data chunk cache, in bytes'/> \
<Option name='RDCC_W0' type='float' description='Preemption policy'/> \
<Option name='SIEVE_BUF' type='int' description='Sets the maximum size of the data sieve buffer'/> \
<Option name='META_BLOCKSIZE' type='int' description='Sets the minimum size of metadata block allocations'/> \
<Option name='DEFLATE' type='int' description='0 (no compression) to 9 (max compression)'/> \
<Option name='THEMATIC' type='boolean' description='If YES then all bands are set to thematic'/> \
</CreationOptionList>" );

        // pointer to open function
        poDriver->pfnOpen = KEADataset::Open;
        // pointer to identify function
        poDriver->pfnIdentify = KEADataset::Identify;
        // pointer to create function
        poDriver->pfnCreate = KEADataset::Create;
        // pointer to create copy function
        poDriver->pfnCreateCopy = KEADataset::CreateCopy;

        GetGDALDriverManager()->RegisterDriver( poDriver );
    }
}
