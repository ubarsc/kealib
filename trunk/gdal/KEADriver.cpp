
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

        // pointer to open function
        poDriver->pfnOpen = KEADataset::Open;
        // pointer to create function
        poDriver->pfnCreate = KEADataset::Create;

        GetGDALDriverManager()->RegisterDriver( poDriver );
    }
}
