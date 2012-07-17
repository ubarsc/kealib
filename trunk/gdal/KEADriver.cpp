
#include "KEADataset.h"

CPL_C_START
void CPL_DLL GDALRegister_KEA(void);
CPL_C_END

// method to register this driver
void GDALRegister_KEA()
{
    GDALDriver  *poDriver;

    if (! GDAL_CHECK_VERSION("KEA"))
        return;

    if( GDALGetDriverByName( "KEA" ) == NULL )
    {
        poDriver = new GDALDriver();
        
        poDriver->SetDescription( "KEA" );
        poDriver->SetMetadataItem( GDAL_DMD_LONGNAME, 
                                   "KEA Image Format (.kea)" );
        poDriver->SetMetadataItem( GDAL_DMD_EXTENSION, "kea" );
        poDriver->SetMetadataItem( GDAL_DMD_CREATIONDATATYPES, 
                            "Byte Int16 UInt16 Int32 UInt32 Float32 Float64" );
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
</CreationOptionList>" );

        // pointer to open function
        poDriver->pfnOpen = KEADataset::Open;
        // pointer to create function
        poDriver->pfnCreate = KEADataset::Create;

        GetGDALDriverManager()->RegisterDriver( poDriver );
    }
}
