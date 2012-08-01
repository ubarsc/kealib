/*
 *  main.cpp
 *  img2kea
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

#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include "gdal_priv.h"
#include "libkea/KEAImageIO.h"

// function for converting a GDAL type to a libkea type
// copied from 
libkea::KEADataType GDAL_to_KEA_Type( GDALDataType gdalType )
{
    libkea::KEADataType keaType = libkea::kea_undefined;
    switch( gdalType )
    {
        case GDT_Byte:
            keaType = libkea::kea_8uint;
            break;
        case GDT_Int16:
            keaType = libkea::kea_16int;
            break;
        case GDT_Int32:
            keaType = libkea::kea_32int;
            break;
        case GDT_UInt16:
            keaType = libkea::kea_16uint;
            break;
        case GDT_UInt32:
            keaType = libkea::kea_32uint;
            break;
        case GDT_Float32:
            keaType = libkea::kea_32float;
            break;
        case GDT_Float64:
            keaType = libkea::kea_64float;
            break;
        default:
            keaType = libkea::kea_undefined;
            break;
    }
    return keaType;
}

// Copies GDAL Band to KEA Band if nOverview == -1
// Otherwise it is assumed we are writing to the specified overview
void CopyRasterData( GDALRasterBand *pBand, libkea::KEAImageIO *pImageIO, int nBand, int nOverview)
{
    // get some info
    libkea::KEADataType eKeaType = pImageIO->getImageBandDataType(nBand);
    unsigned int nBlockSize;
    if( nOverview == -1 )
        nBlockSize = pImageIO->getImageBlockSize( nBand );
    else
        nBlockSize = pImageIO->getOverviewBlockSize(nBand, nOverview);

    GDALDataType eGDALType = pBand->GetRasterDataType();
    unsigned int nXSize = pBand->GetXSize();
    unsigned int nYSize = pBand->GetYSize();

    // allocate some space
    int nPixelSize = GDALGetDataTypeSize( eGDALType ) / 8;
    void *pData = CPLMalloc( nPixelSize * nBlockSize * nBlockSize);
    // for progress
    int nTotalBlocks = std::ceil( (double)nXSize / (double)nBlockSize ) * std::ceil( (double)nYSize / (double)nBlockSize );
    int nBlocksComplete = 0;
    int nLastPercent = -1;
    // go through the image
    for( unsigned int nY = 0; nY < nYSize; nY += nBlockSize )
    {
        // adjust for edge blocks
        unsigned int ysize = nBlockSize;
        unsigned int ytotalsize = nY + nBlockSize;
        if( ytotalsize > nYSize )
            ysize -= (ytotalsize - nYSize);
        for( unsigned int nX = 0; nX < nXSize; nX += nBlockSize )
        {
            // adjust for edge blocks
            unsigned int xsize = nBlockSize;
            unsigned int xtotalsize = nX + nBlockSize;
            if( xtotalsize > nXSize )
                xsize -= (xtotalsize - nXSize);

            // read in from GDAL
            if( pBand->RasterIO( GF_Read, nX, nY, xsize, ysize, pData, xsize, ysize, eGDALType, nPixelSize, nPixelSize * nBlockSize) != CE_None )
            {
                fprintf( stderr, "Failed to read block at %d %d\n", nX, nY );
                exit(1);
            }
            // write out to KEA
            if( nOverview == -1 )
                pImageIO->writeImageBlock2Band( nBand, pData, nX, nY, xsize, ysize, nBlockSize, nBlockSize, eKeaType);
            else
                pImageIO->writeToOverview( nBand, nOverview, pData,  nX, nY, xsize, ysize, nBlockSize, nBlockSize, eKeaType);

            // progress
            nBlocksComplete++;
            int nPercent = std::ceil( ((double)nBlocksComplete / (double)nTotalBlocks) * 100.0);
            if( nPercent != nLastPercent )
            {
                fprintf( stderr, "band %d overview %d %d%%\r", nBand, nOverview, nPercent );
                nLastPercent = nPercent;
            }
        }
    }
    
    CPLFree( pData );
    fprintf( stderr, "\n" );
}

void CopyBand( GDALRasterBand *pBand, libkea::KEAImageIO *pImageIO, int nBand)
{
    // first copy the raster data over
    CopyRasterData( pBand, pImageIO, nBand, -1);

    // are there any overviews?
    int nOverviews = pBand->GetOverviewCount();
    for( int nOverviewCount = 0; nOverviewCount < nOverviews; nOverviewCount++ )
    {
        GDALRasterBand *pOverview = pBand->GetOverview(nOverviewCount);
        int nOverviewXSize = pOverview->GetXSize();
        int nOverviewYSize = pOverview->GetYSize();
        pImageIO->createOverview( nBand, nOverviewCount + 1, nOverviewXSize, nOverviewYSize);
        CopyRasterData( pOverview, pImageIO, nBand, nOverviewCount + 1);
    }

    // now metadata 

    // and attributes
}

void CopySpatialInfo(GDALDataset *pDataset, libkea::KEAImageIO *pImageIO)
{
    libkea::KEAImageSpatialInfo *pSpatialInfo = pImageIO->getSpatialInfo();

    double padfTransform[6];
    if( pDataset->GetGeoTransform(padfTransform) == CE_None )
    {
        // convert back from GDAL's array format
        pSpatialInfo->tlX = padfTransform[0];
        pSpatialInfo->xRes = padfTransform[1];
        pSpatialInfo->xRot = padfTransform[2];
        pSpatialInfo->tlY = padfTransform[3];
        pSpatialInfo->yRot = padfTransform[4];
        pSpatialInfo->yRes = padfTransform[5];
    }

    const char *pszProjection = pDataset->GetProjectionRef();
    pSpatialInfo->wktString = pszProjection;

    pImageIO->setSpatialInfo( pSpatialInfo );
}


int main (int argc, char * const argv[]) 
{
    if( argc != 3 )
    {
        fprintf( stderr, "usage: img2kea infile.img outfile.kea\n" );
        exit(1);
    }

    const char *pszInfile = argv[1];
    const char *pszOutfile = argv[2];

    GDALAllRegister();

    // open input dataset 
    GDALDataset *pDataset = (GDALDataset*)GDALOpen(pszInfile, GA_ReadOnly);
    if( pDataset == NULL )
    {
        fprintf( stderr, "Unable to open %s\n", pszInfile );
        exit(2);
    }

    // open output dataset - assume all bands same data type
    GDALDataType eGDALType = pDataset->GetRasterBand(1)->GetRasterDataType();
    libkea::KEADataType eKeaType = GDAL_to_KEA_Type( eGDALType );
    int nXSize = pDataset->GetRasterXSize();
    int nYSize = pDataset->GetRasterYSize();
    int nBands = pDataset->GetRasterCount();
    libkea::KEAImageIO *pImageIO = NULL;
    try
    {
        // must support setting the creation options at some stage
        H5::H5File *keaImgH5File = libkea::KEAImageIO::createKEAImage( pszOutfile,
                                                    eKeaType, nXSize, nYSize, nBands);

        // create the KEAImageIO class
        pImageIO = new libkea::KEAImageIO();

        // open the file
        pImageIO->openKEAImageHeader( keaImgH5File );

        // copy accross the spatial info
        CopySpatialInfo( pDataset, pImageIO);


        // now copy all the bands over
        for( int nBand = 0; nBand < nBands; nBand++ )
        {
            GDALRasterBand *pBand = pDataset->GetRasterBand(nBand + 1);
            CopyBand( pBand, pImageIO, nBand +1 );
        }

    }
    catch (libkea::KEAException &e)
    {
        fprintf( stderr, "Caught KEA exception: %s\n", e.what() );
        // ignore and close datasets as per normal
    }

    delete pImageIO;
    GDALClose( (GDALDatasetH)pDataset );
}
