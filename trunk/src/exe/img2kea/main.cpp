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
#include "gdal_rat.h"
#include "libkea/KEAImageIO.h"
#include "libkea/KEAAttributeTable.h"
#include "libkea/KEAAttributeTableInMem.h"

// function for converting a GDAL type to a libkea type
// copied from KEADataset.cpp - naughty naughty
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
                if( nOverview == -1 )
                    fprintf( stderr, "band %d %d%%\r", nBand, nPercent );
                else
                    fprintf( stderr, "band %d overview %d %d%%\r", nBand, nOverview, nPercent );
                nLastPercent = nPercent;
            }
        }
    }
    
    CPLFree( pData );
    fprintf( stderr, "\n" );
}

// copies the raster attribute table
void CopyRAT(GDALRasterBand *pBand, libkea::KEAImageIO *pImageIO, int nBand)
{
    const GDALRasterAttributeTable *gdalAtt = pBand->GetDefaultRAT();
    if((gdalAtt != NULL) && (gdalAtt->GetRowCount() > 0))
    {
        libkea::KEAAttributeTable *keaAtt = new libkea::KEAAttributeTableInMem();
        
        bool redDef = false;
        int redIdx = -1;
        bool greenDef = false;
        int greenIdx = -1;
        bool blueDef = false;
        int blueIdx = -1;
        bool alphaDef = false;
        int alphaIdx = -1;
        bool histoDef = false;
        int histoIdx = -1;
        
        int numCols = gdalAtt->GetColumnCount();
        std::vector<libkea::KEAATTField*> *fields = new std::vector<libkea::KEAATTField*>();
        libkea::KEAATTField *field;
        for(int i = 0; i < numCols; ++i)
        {
            field = new libkea::KEAATTField();
            field->name = gdalAtt->GetNameOfCol(i);
            
            field->dataType = libkea::kea_att_string;
            switch(gdalAtt->GetTypeOfCol(i))
            {
                case GFT_Integer:
                    field->dataType = libkea::kea_att_int;
                    break;
                case GFT_Real:
                    field->dataType = libkea::kea_att_float;
                    break;
                case GFT_String:
                    field->dataType = libkea::kea_att_string;
                    break;
                default:
                    // leave as "kea_att_string"
                    break;
            }
            
            if(field->name == "Histogram")
            {
                field->usage = "PixelCount";
                field->dataType = libkea::kea_att_int;
                histoDef = true;
                histoIdx = i;
            }
            else if(field->name == "Opacity")
            {
                field->name = "Alpha";
                field->usage = "Alpha";
                field->dataType = libkea::kea_att_int;
                alphaDef = true;
                alphaIdx = i;
            }
            else
            {
                field->usage = "Generic";
                switch(gdalAtt->GetUsageOfCol(i))
                {
                    case GFU_PixelCount:
                        field->usage = "PixelCount";
                        break;
                    case GFU_Name:
                        field->usage = "Name";
                        break;
                    case GFU_Red:
                        field->usage = "Red";
                        field->dataType = libkea::kea_att_int;
                        redDef = true;
                        redIdx = i;
                        break;
                    case GFU_Green:
                        field->usage = "Green";
                        field->dataType = libkea::kea_att_int;
                        greenDef = true;
                        greenIdx = i;
                        break;
                    case GFU_Blue:
                        field->usage = "Blue";
                        field->dataType = libkea::kea_att_int;
                        blueDef = true;
                        blueIdx = i;
                        break;
                    case GFU_Alpha:
                        field->usage = "Alpha";
                        break;
                    default:
                        // leave as "Generic"
                        break;
                }
            }
            
            fields->push_back(field);
        }
        
        keaAtt->addFields(fields); // This function will populate the field indexs used within the KEA RAT.
        
        int numRows = gdalAtt->GetRowCount();
        keaAtt->addRows(numRows);
        
        libkea::KEAATTFeature *keaFeat = NULL;
        for(int i = 0; i < numRows; ++i)
        {
            keaFeat = keaAtt->getFeature(i);
            for(int j = 0; j < numCols; ++j)
            {
                field = fields->at(j);
                
                if(redDef && (redIdx == j))
                {
                    keaFeat->intFields->at(field->idx) = (int)(gdalAtt->GetValueAsDouble(i, j)*255);
                }
                else if(greenDef && (greenIdx == j))
                {
                    keaFeat->intFields->at(field->idx) = (int)(gdalAtt->GetValueAsDouble(i, j)*255);
                }
                else if(blueDef && (blueIdx == j))
                {
                    keaFeat->intFields->at(field->idx) = (int)(gdalAtt->GetValueAsDouble(i, j)*255);
                }
                else if(alphaDef && (alphaIdx == j))
                {
                    keaFeat->intFields->at(field->idx) = (int)(gdalAtt->GetValueAsDouble(i, j)*255);
                }
                else
                {
                    switch(field->dataType)
                    {
                        case libkea::kea_att_int:
                            keaFeat->intFields->at(field->idx) = gdalAtt->GetValueAsInt(i, j);
                            break;
                        case libkea::kea_att_float:
                            keaFeat->floatFields->at(field->idx) = gdalAtt->GetValueAsDouble(i, j);
                            break;
                        case libkea::kea_att_string:
                            keaFeat->strFields->at(field->idx) = std::string(gdalAtt->GetValueAsString(i, j));
                            break;
                        default:
                            // Ignore as data type is not known or available from a HFA/GDAL RAT."
                            break;
                    }
                }
            }
        }
        
        pImageIO->setAttributeTable(keaAtt, nBand);
        
        delete keaAtt;
        for(std::vector<libkea::KEAATTField*>::iterator iterField = fields->begin(); iterField != fields->end(); ++iterField)
        {
            delete *iterField;
        }
        delete fields;
    }
}

// copies the metadata
// pass nBand == -1 to copy a dataset's metadata
// or band index to copy a band's metadata
void CopyMetadata( GDALMajorObject *pObject, libkea::KEAImageIO *pImageIO, int nBand)
{
    char **ppszMetadata = pObject->GetMetadata();
    if( ppszMetadata != NULL )
    {
        char *pszName;
        const char *pszValue;
        int nCount = 0;
        while( ppszMetadata[nCount] != NULL )
        {
            pszValue = CPLParseNameValue( ppszMetadata[nCount], &pszName );

            // it is LAYER_TYPE and a Band? if so handle seperately
            if( ( nBand != -1 ) && EQUAL( pszName, "LAYER_TYPE" ) )
            {
                if( EQUAL( pszValue, "athematic" ) )
                {
                    pImageIO->setImageBandLayerType(nBand, libkea::kea_continuous );
                }
                else
                {
                    pImageIO->setImageBandLayerType(nBand, libkea::kea_thematic );
                }
            }
            else if( ( nBand != -1 ) && EQUAL( pszName, "STATISTICS_HISTOBINVALUES") )
            {
                // this gets copied accross as part of the attributes
                // so ignore for now
            }
            else
            {
                // write it into the image
                if( nBand != -1 )
                    pImageIO->setImageBandMetaData(nBand, pszName, pszValue );
                else
                    pImageIO->setImageMetaData(pszName, pszValue );
            }
            nCount++;
        }
    }
}

// copies the description over
void CopyDescription(GDALRasterBand *pBand, libkea::KEAImageIO *pImageIO, int nBand)
{
    const char *pszDesc = pBand->GetDescription();
    pImageIO->setImageBandDescription(nBand, pszDesc);
}

// copies the no data value accross
void CopyNoData(GDALRasterBand *pBand, libkea::KEAImageIO *pImageIO, int nBand)
{
    int bSuccess = 0;
    double dNoData = pBand->GetNoDataValue(&bSuccess);
    if( bSuccess )
    {
        pImageIO->setNoDataValue(nBand, &dNoData, libkea::kea_64float);
    }
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
    CopyMetadata(pBand, pImageIO, nBand);

    // and attributes
    CopyRAT(pBand, pImageIO, nBand);

    // and description
    CopyDescription(pBand, pImageIO, nBand);

    // and no data
    CopyNoData(pBand, pImageIO, nBand);
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

        // dataset metadata
        CopyMetadata(pDataset, pImageIO, -1);
    
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

    pImageIO->close();
    delete pImageIO;
    GDALClose( (GDALDatasetH)pDataset );
}
