/*
 *  KEAImageIO.cpp
 *  LibKEA
 *
 *  Created by Pete Bunting on 02/07/2012.
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

#include "libkea/KEAImageIO.h"

#include <string.h>
#include <stdlib.h>

namespace kealib{
    
    KEAImageIO::KEAImageIO()
    {
        this->fileOpen = false;
    }
    
    void KEAImageIO::openKEAImageHeader(H5::H5File *keaImgH5File)throw(KEAIOException)
    {
        try 
        {
            this->keaImgFile = keaImgH5File;
            this->spatialInfoFile = new KEAImageSpatialInfo();
            
            // READ KEA VERSION NUMBER
            try
            {
                hid_t nativeVerStrType;
                H5::DataSet datasetFileVersion = keaImgH5File->openDataSet( KEA_DATASETNAME_HEADER_VERSION );
                H5::DataType strVerDataType = datasetFileVersion.getDataType();
                char **strVerData = new char*[1];
                if((nativeVerStrType=H5Tget_native_type(strVerDataType.getId(), H5T_DIR_DEFAULT))<0)
                {
                    throw KEAIOException("Could not define a native string type");
                }
                datasetFileVersion.read((void*)strVerData, strVerDataType);
                keaVersion = std::string(strVerData[0]);
                free(strVerData[0]);
                delete[] strVerData;
                datasetFileVersion.close();
            }
            catch ( H5::Exception &e)
            {
                throw KEAIOException("The number of image bands was not specified.");
            }
            
            
            // READ NUMBER OF IMAGE BANDS
            try 
            {
                hsize_t dimsValue[1];
                dimsValue[0] = 1;
                H5::DataSpace valueDataSpace(1, dimsValue);
                uint32_t value[1];
                H5::DataSet datasetNumImgBands = this->keaImgFile->openDataSet( KEA_DATASETNAME_HEADER_NUMBANDS );
                datasetNumImgBands.read(value, H5::PredType::NATIVE_UINT32, valueDataSpace);
                this->numImgBands = value[0];
                datasetNumImgBands.close();
                valueDataSpace.close();
            } 
            catch ( H5::Exception &e) 
            {
                throw KEAIOException("The number of image bands was not specified.");
            }
                        
            // READ TL COORDINATES
            try 
            {
                hsize_t dimsValue[1];
                dimsValue[0] = 2;
                H5::DataSpace valueDataSpace(1, dimsValue);
                double *values = new double[2];
                H5::DataSet datasetSpatialTL = this->keaImgFile->openDataSet( KEA_DATASETNAME_HEADER_TL );
                datasetSpatialTL.read(values, H5::PredType::NATIVE_DOUBLE, valueDataSpace);
                this->spatialInfoFile->tlX = values[0];
                this->spatialInfoFile->tlY = values[1];
                delete[] values;
                datasetSpatialTL.close();
                valueDataSpace.close();
            } 
            catch ( H5::Exception &e) 
            {
                throw KEAIOException("The TL coordinate is not specified.");
            }
            
            // READ RESOLUTION
            try 
            {
                hsize_t dimsValue[1];
                dimsValue[0] = 2;
                H5::DataSpace valueDataSpace(1, dimsValue);
                double *values = new double[2];
                H5::DataSet spatialResDataset = this->keaImgFile->openDataSet( KEA_DATASETNAME_HEADER_RES );
                spatialResDataset.read(values, H5::PredType::NATIVE_DOUBLE, valueDataSpace);
                this->spatialInfoFile->xRes = values[0];
                this->spatialInfoFile->yRes = values[1];
                delete[] values;
                spatialResDataset.close();
                valueDataSpace.close();
            } 
            catch ( H5::Exception &e) 
            {
                throw KEAIOException("The pixel resolution was not specified.");
            }
            
            // READ ROTATION
            try 
            {
                hsize_t dimsValue[1];
                dimsValue[0] = 2;
                H5::DataSpace valueDataSpace(1, dimsValue);
                double *values = new double[2];
                H5::DataSet spatialRotDataset = this->keaImgFile->openDataSet( KEA_DATASETNAME_HEADER_ROT );
                spatialRotDataset.read(values, H5::PredType::NATIVE_DOUBLE, valueDataSpace);
                this->spatialInfoFile->xRot = values[0];
                this->spatialInfoFile->yRot = values[1];
                delete[] values;
                spatialRotDataset.close();
                valueDataSpace.close();
            } 
            catch ( H5::Exception &e) 
            {
                throw KEAIOException("The image resolution was not specified.");
            }
            
            // READ IMAGE SIZE
            try 
            {
                hsize_t dimsValue[1];
                dimsValue[0] = 2;
                H5::DataSpace valueDataSpace(1, dimsValue);
                uint64_t *values = new uint64_t[2];
                H5::DataSet spatialSizeDataset = this->keaImgFile->openDataSet( KEA_DATASETNAME_HEADER_SIZE );
                spatialSizeDataset.read(values, H5::PredType::NATIVE_UINT64, valueDataSpace);
                this->spatialInfoFile->xSize = values[0];
                this->spatialInfoFile->ySize = values[1];
                delete[] values;
                spatialSizeDataset.close();
                valueDataSpace.close();
            } 
            catch ( H5::Exception &e) 
            {
                throw KEAIOException("The image resolution was not specified.");
            }
            
            // READ WKT STRING
            try 
            {
                hid_t nativeStrType;
                H5::DataSet datasetSpatialReference = this->keaImgFile->openDataSet( KEA_DATASETNAME_HEADER_WKT );
                H5::DataType strDataType = datasetSpatialReference.getDataType();
                char **strData = new char*[1];
                if((nativeStrType=H5Tget_native_type(strDataType.getId(), H5T_DIR_DEFAULT))<0)
                {
                    throw KEAIOException("Could not define a native string type");
                }
                datasetSpatialReference.read((void*)strData, strDataType);
                this->spatialInfoFile->wktString = std::string(strData[0]);
                free(strData[0]);
                delete[] strData;
                datasetSpatialReference.close();
            } 
            catch ( H5::Exception &e) 
            {
                throw KEAIOException("The spatial reference was not specified.");
            }
        } 
        catch ( KEAIOException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        this->fileOpen = true;
    }
    
    void KEAImageIO::writeImageBlock2Band(uint32_t band, void *data, uint64_t xPxlOff, uint64_t yPxlOff, uint64_t xSizeOut, uint64_t ySizeOut, uint64_t xSizeBuf, uint64_t ySizeBuf, KEADataType inDataType)throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        try 
        {
            // CHECK PARAMETERS PROVIDED FIT WITHIN IMAGE
            if(band == 0)
            {
                throw KEAIOException("KEA Image Bands start at 1.");
            }
            else if(band > this->numImgBands)
            {
               throw KEAIOException("Band is not present within image."); 
            }
            
            uint64_t endXPxl = xPxlOff + xSizeOut;
            uint64_t endYPxl = yPxlOff + ySizeOut;
            
            if(xPxlOff > this->spatialInfoFile->xSize)
            {
               throw KEAIOException("Start X Pixel is not within image.");  
            }
            
            if(endXPxl > this->spatialInfoFile->xSize)
            {
                throw KEAIOException("End X Pixel is not within image.");  
            }
            
            if(yPxlOff > this->spatialInfoFile->ySize)
            {
                throw KEAIOException("Start Y Pixel is not within image.");  
            }
            
            if(endYPxl > this->spatialInfoFile->ySize)
            {
                throw KEAIOException("End Y Pixel is not within image.");  
            }
            
            // GET NATIVE DATASET
            H5::DataType imgBandDT = convertDatatypeKeaToH5Native(inDataType);

            // OPEN BAND DATASET AND WRITE IMAGE DATA
            try 
            {
                std::string imageBandPath = KEA_DATASETNAME_BAND + uint2Str(band);
                H5::DataSet imgBandDataset = this->keaImgFile->openDataSet( imageBandPath + KEA_BANDNAME_DATA );
                H5::DataSpace imgBandDataspace = imgBandDataset.getSpace();                
                
                hsize_t imgOffset[2];
                imgOffset[0] = yPxlOff;
                imgOffset[1] = xPxlOff;
                hsize_t dataDims[2];
                dataDims[0] = ySizeBuf;
                dataDims[1] = xSizeBuf;
                H5::DataSpace write2BandDataspace = H5::DataSpace(2, dataDims);
                
                if((ySizeOut != ySizeBuf) | (xSizeOut != xSizeBuf))
                {                    
                    hsize_t dataSelectMemDims[2];
                    dataSelectMemDims[0] = ySizeOut;
                    dataSelectMemDims[1] = 1;

                    hsize_t dataOffDims[2];
                    dataOffDims[0] = 0;
                    dataOffDims[1] = 0;

                    hsize_t dataSelectStrideDims[2];
                    dataSelectStrideDims[0] = 1;
                    if(xSizeBuf == xSizeOut)
                    {
                        dataSelectStrideDims[1] = 1;
                    }
                    else
                    {
                        dataSelectStrideDims[1] = xSizeBuf - xSizeOut;
                    }

                    hsize_t dataSelectBlockSizeDims[2];
                    dataSelectBlockSizeDims[0] = 1;
                    dataSelectBlockSizeDims[1] = xSizeOut;
                    write2BandDataspace.selectHyperslab(H5S_SELECT_SET, dataSelectMemDims, dataOffDims, dataSelectStrideDims, dataSelectBlockSizeDims);
                    
                    hsize_t dataOutDims[2];
                    dataOutDims[0] = ySizeOut;
                    dataOutDims[1] = xSizeOut;
                    imgBandDataspace.selectHyperslab( H5S_SELECT_SET, dataOutDims, imgOffset);
                }
                else
                {
                    imgBandDataspace.selectHyperslab( H5S_SELECT_SET, dataDims, imgOffset);
                }
                
                imgBandDataset.write( data, imgBandDT, write2BandDataspace, imgBandDataspace);
                                
                imgBandDataset.close();
                imgBandDataspace.close();
                write2BandDataspace.close();
                
                this->keaImgFile->flush(H5F_SCOPE_GLOBAL);
            } 
            catch ( H5::Exception &e) 
            {
                throw KEAIOException("Could not write image data.");
            }            
        }
        catch(KEAIOException &e)
        {
            throw e;
        }
        catch( H5::FileIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( H5::DataSetIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( H5::DataSpaceIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( H5::DataTypeIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }
    
    void KEAImageIO::readImageBlock2Band(uint32_t band, void *data, uint64_t xPxlOff, uint64_t yPxlOff, uint64_t xSizeIn, uint64_t ySizeIn, uint64_t xSizeBuf, uint64_t ySizeBuf, KEADataType inDataType)throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        try 
        {
            // CHECK PARAMETERS PROVIDED FIT WITHIN IMAGE
            if(band == 0)
            {
                throw KEAIOException("KEA Image Bands start at 1.");
            }
            else if(band > this->numImgBands)
            {
                throw KEAIOException("Band is not present within image."); 
            }
            
            uint64_t endXPxl = xPxlOff + xSizeIn;
            uint64_t endYPxl = yPxlOff + ySizeIn;
            
            if(xPxlOff > this->spatialInfoFile->xSize)
            {
                throw KEAIOException("Start X Pixel is not within image.");  
            }
            
            if(endXPxl > this->spatialInfoFile->xSize)
            {
                throw KEAIOException("End X Pixel is not within image.");  
            }
            
            if(yPxlOff > this->spatialInfoFile->ySize)
            {
                throw KEAIOException("Start Y Pixel is not within image.");  
            }
            
            if(endYPxl > this->spatialInfoFile->ySize)
            {
                throw KEAIOException("End Y Pixel is not within image.");  
            }
            
            // GET NATIVE DATASET
            H5::DataType imgBandDT = convertDatatypeKeaToH5Native(inDataType);

            // OPEN BAND DATASET AND READ IMAGE DATA
            try 
            {
                std::string imageBandPath = KEA_DATASETNAME_BAND + uint2Str(band);
                H5::DataSet imgBandDataset = this->keaImgFile->openDataSet( imageBandPath + KEA_BANDNAME_DATA );
                H5::DataSpace imgBandDataspace = imgBandDataset.getSpace();                
                
                hsize_t dataOffset[2];
                dataOffset[0] = yPxlOff;
                dataOffset[1] = xPxlOff;
                hsize_t dataDims[2];
                dataDims[0] = ySizeBuf;
                dataDims[1] = xSizeBuf;
                H5::DataSpace read2BandDataspace = H5::DataSpace(2, dataDims);
                
                if((ySizeBuf != ySizeIn) | (xSizeBuf != xSizeIn))
                {
                    hsize_t dataSelectMemDims[2];
                    dataSelectMemDims[0] = ySizeIn;
                    dataSelectMemDims[1] = 1;

                    hsize_t dataOffDims[2];
                    dataOffDims[0] = 0;
                    dataOffDims[1] = 0;

                    hsize_t dataSelectStrideDims[2];
                    dataSelectStrideDims[0] = 1;
                    if(xSizeBuf == xSizeIn)
                    {
                        dataSelectStrideDims[1] = 1;
                    }
                    else
                    {
                        dataSelectStrideDims[1] = xSizeBuf - xSizeIn;
                    }

                    hsize_t dataSelectBlockSizeDims[2];
                    dataSelectBlockSizeDims[0] = 1;
                    dataSelectBlockSizeDims[1] = xSizeIn;
                    read2BandDataspace.selectHyperslab(H5S_SELECT_SET, dataSelectMemDims, dataOffDims, dataSelectStrideDims, dataSelectBlockSizeDims);
                    
                    hsize_t dataInDims[2];
                    dataInDims[0] = ySizeIn;
                    dataInDims[1] = xSizeIn;
                    imgBandDataspace.selectHyperslab( H5S_SELECT_SET, dataInDims, dataOffset);
                }
                else
                {
                    imgBandDataspace.selectHyperslab( H5S_SELECT_SET, dataDims, dataOffset);
                }
                
                imgBandDataset.read( data, imgBandDT, read2BandDataspace, imgBandDataspace);
                
                imgBandDataset.close();
                imgBandDataspace.close();
                read2BandDataspace.close();
            } 
            catch ( H5::Exception &e) 
            {
                throw KEAIOException("Could not read image data.");
            }            
        }
        catch(KEAIOException &e)
        {
            throw e;
        }
        catch( H5::FileIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( H5::DataSetIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( H5::DataSpaceIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( H5::DataTypeIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }
  
    
    
    void KEAImageIO::createMask(uint32_t band, uint32_t deflate)throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        if(!this->maskCreated(band))
        {
            uint32_t blockSize2Use = getImageBlockSize(band);
            int initFillVal = 255;
            hsize_t dimsImageBandChunk[] = { blockSize2Use, blockSize2Use };
            H5::DSetCreatPropList initParamsImgBand;
            initParamsImgBand.setChunk(2, dimsImageBandChunk);
            initParamsImgBand.setShuffle();
            initParamsImgBand.setDeflate(deflate);
            initParamsImgBand.setFillValue( H5::PredType::NATIVE_INT, &initFillVal);
            
            H5::StrType strdatatypeLen6(H5::PredType::C_S1, 6);
            H5::StrType strdatatypeLen4(H5::PredType::C_S1, 4);
            const H5std_string strClassVal ("IMAGE");
            const H5std_string strImgVerVal ("1.2");
            H5::DataSpace attr_dataspace = H5::DataSpace(H5S_SCALAR);
            
            std::string imageBandPath = KEA_DATASETNAME_BAND + uint2Str(band);
            hsize_t imageBandDims[] = { spatialInfoFile->ySize, spatialInfoFile->xSize };
            H5::DataSpace imgBandDataSpace(2, imageBandDims);
            H5::DataSet imgBandDataSet = this->keaImgFile->createDataSet((imageBandPath+KEA_BANDNAME_MASK), H5::PredType::STD_U8LE, imgBandDataSpace, initParamsImgBand);
            H5::Attribute classAttribute = imgBandDataSet.createAttribute(KEA_ATTRIBUTENAME_CLASS, strdatatypeLen6, attr_dataspace);
            classAttribute.write(strdatatypeLen6, strClassVal);
            classAttribute.close();
            
            H5::Attribute imgVerAttribute = imgBandDataSet.createAttribute(KEA_ATTRIBUTENAME_IMAGE_VERSION, strdatatypeLen4, attr_dataspace);
            imgVerAttribute.write(strdatatypeLen4, strImgVerVal);
            imgVerAttribute.close();
            
            imgBandDataSet.close();
            imgBandDataSpace.close();
        }
    }
    
    void KEAImageIO::writeImageBlock2BandMask(uint32_t band, void *data, uint64_t xPxlOff, uint64_t yPxlOff, uint64_t xSizeOut, uint64_t ySizeOut, uint64_t xSizeBuf, uint64_t ySizeBuf, KEADataType inDataType)throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        try
        {
            // CHECK PARAMETERS PROVIDED FIT WITHIN IMAGE
            if(band == 0)
            {
                throw KEAIOException("KEA Image Bands start at 1.");
            }
            else if(band > this->numImgBands)
            {
                throw KEAIOException("Band is not present within image.");
            }
            
            uint64_t endXPxl = xPxlOff + xSizeOut;
            uint64_t endYPxl = yPxlOff + ySizeOut;
            
            if(xPxlOff > this->spatialInfoFile->xSize)
            {
                throw KEAIOException("Start X Pixel is not within image.");
            }
            
            if(endXPxl > this->spatialInfoFile->xSize)
            {
                throw KEAIOException("End X Pixel is not within image.");
            }
            
            if(yPxlOff > this->spatialInfoFile->ySize)
            {
                throw KEAIOException("Start Y Pixel is not within image.");
            }
            
            if(endYPxl > this->spatialInfoFile->ySize)
            {
                throw KEAIOException("End Y Pixel is not within image.");
            }
            
            // GET NATIVE DATASET
            H5::DataType imgBandDT = convertDatatypeKeaToH5Native(inDataType);
            
            // OPEN BAND DATASET AND WRITE IMAGE DATA
            try
            {
                std::string imageBandPath = KEA_DATASETNAME_BAND + uint2Str(band);
                H5::DataSet imgBandDataset = this->keaImgFile->openDataSet( imageBandPath + KEA_BANDNAME_MASK );
                H5::DataSpace imgBandDataspace = imgBandDataset.getSpace();
                
                hsize_t imgOffset[2];
                imgOffset[0] = yPxlOff;
                imgOffset[1] = xPxlOff;
                hsize_t dataDims[2];
                dataDims[0] = ySizeBuf;
                dataDims[1] = xSizeBuf;
                H5::DataSpace write2BandDataspace = H5::DataSpace(2, dataDims);
                
                if((ySizeOut != ySizeBuf) | (xSizeOut != xSizeBuf))
                {
                    hsize_t dataSelectMemDims[2];
                    dataSelectMemDims[0] = ySizeOut;
                    dataSelectMemDims[1] = 1;
                    
                    hsize_t dataOffDims[2];
                    dataOffDims[0] = 0;
                    dataOffDims[1] = 0;
                    
                    hsize_t dataSelectStrideDims[2];
                    dataSelectStrideDims[0] = 1;
                    if(xSizeBuf == xSizeOut)
                    {
                        dataSelectStrideDims[1] = 1;
                    }
                    else
                    {
                        dataSelectStrideDims[1] = xSizeBuf - xSizeOut;
                    }
                    
                    hsize_t dataSelectBlockSizeDims[2];
                    dataSelectBlockSizeDims[0] = 1;
                    dataSelectBlockSizeDims[1] = xSizeOut;
                    write2BandDataspace.selectHyperslab(H5S_SELECT_SET, dataSelectMemDims, dataOffDims, dataSelectStrideDims, dataSelectBlockSizeDims);
                    
                    hsize_t dataOutDims[2];
                    dataOutDims[0] = ySizeOut;
                    dataOutDims[1] = xSizeOut;
                    imgBandDataspace.selectHyperslab( H5S_SELECT_SET, dataOutDims, imgOffset);
                }
                else
                {
                    imgBandDataspace.selectHyperslab( H5S_SELECT_SET, dataDims, imgOffset);
                }
                
                imgBandDataset.write( data, imgBandDT, write2BandDataspace, imgBandDataspace);
                
                imgBandDataset.close();
                imgBandDataspace.close();
                write2BandDataspace.close();
                
                this->keaImgFile->flush(H5F_SCOPE_GLOBAL);
            }
            catch ( H5::Exception &e)
            {
                throw KEAIOException("Could not write image data.");
            }
        }
        catch(KEAIOException &e)
        {
            throw e;
        }
        catch( H5::FileIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( H5::DataSetIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( H5::DataSpaceIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( H5::DataTypeIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }
    
    void KEAImageIO::readImageBlock2BandMask(uint32_t band, void *data, uint64_t xPxlOff, uint64_t yPxlOff, uint64_t xSizeIn, uint64_t ySizeIn, uint64_t xSizeBuf, uint64_t ySizeBuf, KEADataType inDataType)throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        try
        {
            // CHECK PARAMETERS PROVIDED FIT WITHIN IMAGE
            if(band == 0)
            {
                throw KEAIOException("KEA Image Bands start at 1.");
            }
            else if(band > this->numImgBands)
            {
                throw KEAIOException("Band is not present within image.");
            }
            
            uint64_t endXPxl = xPxlOff + xSizeIn;
            uint64_t endYPxl = yPxlOff + ySizeIn;
            
            if(xPxlOff > this->spatialInfoFile->xSize)
            {
                throw KEAIOException("Start X Pixel is not within image.");
            }
            
            if(endXPxl > this->spatialInfoFile->xSize)
            {
                throw KEAIOException("End X Pixel is not within image.");
            }
            
            if(yPxlOff > this->spatialInfoFile->ySize)
            {
                throw KEAIOException("Start Y Pixel is not within image.");
            }
            
            if(endYPxl > this->spatialInfoFile->ySize)
            {
                throw KEAIOException("End Y Pixel is not within image.");
            }
            
            // GET NATIVE DATASET
            H5::DataType imgBandDT = convertDatatypeKeaToH5Native(inDataType);
            
            // OPEN BAND DATASET AND READ IMAGE DATA
            try
            {
                std::string imageBandPath = KEA_DATASETNAME_BAND + uint2Str(band);
                H5::DataSet imgBandDataset = this->keaImgFile->openDataSet( imageBandPath + KEA_BANDNAME_MASK );
                H5::DataSpace imgBandDataspace = imgBandDataset.getSpace();
                
                hsize_t dataOffset[2];
                dataOffset[0] = yPxlOff;
                dataOffset[1] = xPxlOff;
                hsize_t dataDims[2];
                dataDims[0] = ySizeBuf;
                dataDims[1] = xSizeBuf;
                H5::DataSpace read2BandDataspace = H5::DataSpace(2, dataDims);
                
                if((ySizeBuf != ySizeIn) | (xSizeBuf != xSizeIn))
                {
                    hsize_t dataSelectMemDims[2];
                    dataSelectMemDims[0] = ySizeIn;
                    dataSelectMemDims[1] = 1;
                    
                    hsize_t dataOffDims[2];
                    dataOffDims[0] = 0;
                    dataOffDims[1] = 0;
                    
                    hsize_t dataSelectStrideDims[2];
                    dataSelectStrideDims[0] = 1;
                    if(xSizeBuf == xSizeIn)
                    {
                        dataSelectStrideDims[1] = 1;
                    }
                    else
                    {
                        dataSelectStrideDims[1] = xSizeBuf - xSizeIn;
                    }
                    
                    hsize_t dataSelectBlockSizeDims[2];
                    dataSelectBlockSizeDims[0] = 1;
                    dataSelectBlockSizeDims[1] = xSizeIn;
                    read2BandDataspace.selectHyperslab(H5S_SELECT_SET, dataSelectMemDims, dataOffDims, dataSelectStrideDims, dataSelectBlockSizeDims);
                    
                    hsize_t dataInDims[2];
                    dataInDims[0] = ySizeIn;
                    dataInDims[1] = xSizeIn;
                    imgBandDataspace.selectHyperslab( H5S_SELECT_SET, dataInDims, dataOffset);
                }
                else
                {
                    imgBandDataspace.selectHyperslab( H5S_SELECT_SET, dataDims, dataOffset);
                }
                
                imgBandDataset.read( data, imgBandDT, read2BandDataspace, imgBandDataspace);
                
                imgBandDataset.close();
                imgBandDataspace.close();
                read2BandDataspace.close();
            }
            catch ( H5::Exception &e)
            {
                throw KEAIOException("Could not read image data.");
            }
        }
        catch(KEAIOException &e)
        {
            throw e;
        }
        catch( H5::FileIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( H5::DataSetIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( H5::DataSpaceIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( H5::DataTypeIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }
    
    bool KEAImageIO::maskCreated(uint32_t band)throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        // CHECK PARAMETERS PROVIDED FIT WITHIN IMAGE
        if(band == 0)
        {
            throw KEAIOException("KEA Image Bands start at 1.");
        }
        else if(band > this->numImgBands)
        {
            throw KEAIOException("Band is not present within image.");
        }
        
        bool maskPresent = false;
        std::string imageBandPath = KEA_DATASETNAME_BAND + uint2Str(band);
        try
        {
            H5::Group imgBandGrp = this->keaImgFile->openGroup(imageBandPath);
            hsize_t numItems = imgBandGrp.getNumObjs();
            for(hsize_t i = 0; i < numItems; ++i)
            {
                //std::cout << "Item: " << imgBandGrp.getObjnameByIdx(i) << std::endl;
                if(imgBandGrp.getObjnameByIdx(i) == "MASK")
                {
                    maskPresent = true;
                    break;
                }
            }
            imgBandGrp.close();
            //H5::DataSet imgBandDataset = this->keaImgFile->openDataSet(imageBandPath+KEA_BANDNAME_MASK);
            //imgBandDataset.close();
            
        }
        catch (H5::Exception &e)
        {
            maskPresent = false;
        }
        
        return maskPresent;
    }
    
    
    void KEAImageIO::setImageMetaData(std::string name, std::string value)throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        // FORM META-DATA PATH WITHIN THE H5 FILE 
        std::string metaDataH5Path = KEA_DATASETNAME_METADATA + std::string("/") + name;
        
        // WRITE IMAGE META DATA
        try 
        {
            // OPEN DATASET OR CREATE NEW DATASET IF IT DOES NOT EXIST
            H5::StrType strTypeAll(0, H5T_VARIABLE);
            H5::DataSet datasetMetaData;
            try 
            {
                datasetMetaData = this->keaImgFile->openDataSet( metaDataH5Path );
            }
            catch (H5::Exception &e)
            {
                hsize_t	dimsForStr[1];
                dimsForStr[0] = 1; // number of lines;
                H5::DataSpace dataspaceStrAll(1, dimsForStr);
                datasetMetaData = this->keaImgFile->createDataSet(metaDataH5Path, strTypeAll, dataspaceStrAll);

            }
            // WRITE DATA INTO THE DATASET
            const char **wStrdata = new const char*[1];
            wStrdata[0] = value.c_str();			
            datasetMetaData.write((void*)wStrdata, strTypeAll);
            datasetMetaData.close();
            delete[] wStrdata;
            
            this->keaImgFile->flush(H5F_SCOPE_GLOBAL);
        }
        catch (H5::Exception &e) 
        {
            throw KEAIOException("Could not set image meta-data.");
        }
        catch ( KEAIOException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }
    
    std::string KEAImageIO::getImageMetaData(std::string name)throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        std::string metaDataH5Path = KEA_DATASETNAME_METADATA + std::string("/") + name;
        std::string value = "";
        // READ IMAGE META-DATA
        try 
        {
            hid_t nativeStrType;
            H5::DataSet datasetMetaData = this->keaImgFile->openDataSet( metaDataH5Path );
            H5::DataType strDataType = datasetMetaData.getDataType();
            char **strData = new char*[1];
            if((nativeStrType=H5Tget_native_type(strDataType.getId(), H5T_DIR_DEFAULT))<0)
            {
                throw KEAIOException("Could not define a native string type");
            }
            datasetMetaData.read((void*)strData, strDataType);
            value = std::string(strData[0]);
            delete strData[0];
            delete[] strData;
            datasetMetaData.close();
        } 
        catch ( H5::Exception &e) 
        {
            throw KEAIOException("Meta-data variable was not accessable.");
        }
        catch ( KEAIOException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return value;
    }
    
    std::vector<std::string> KEAImageIO::getImageMetaDataNames()throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        std::vector<std::string> metaDataNames;
        
        try 
        {
            // Try to open dataset with overviewName
            H5::Group imgBandMetaDataGrp = this->keaImgFile->openGroup(KEA_DATASETNAME_METADATA);
            hsize_t numMetaDataItems = imgBandMetaDataGrp.getNumObjs();
            
            for(hsize_t i = 0; i < numMetaDataItems; ++i)
            {
                metaDataNames.push_back(imgBandMetaDataGrp.getObjnameByIdx(i));
            }
        }
        catch (H5::Exception &e)
        {
            throw KEAIOException("Could not retrieve image meta data.");
        }
        catch ( KEAIOException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return metaDataNames;
    }
    
    std::vector< std::pair<std::string, std::string> > KEAImageIO::getImageMetaData()throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        std::vector< std::pair<std::string, std::string> > metaData;
        
        try 
        {
            // Try to open dataset with overviewName
            H5::Group imgBandMetaDataGrp = this->keaImgFile->openGroup(KEA_DATASETNAME_METADATA);
            hsize_t numMetaDataItems = imgBandMetaDataGrp.getNumObjs();
            std::string name = "";
            std::string value = "";
            
            for(hsize_t i = 0; i < numMetaDataItems; ++i)
            {
                name = imgBandMetaDataGrp.getObjnameByIdx(i);
                value = this->getImageMetaData(name);
                metaData.push_back(std::pair<std::string, std::string>(name,value));
            }
        }
        catch (KEAIOException &e)
        {
            throw e;
        }
        catch (H5::Exception &e)
        {
            throw KEAIOException("Could not retrieve image meta data.");
        }
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return metaData;
    }
    
    void KEAImageIO::setImageMetaData(std::vector< std::pair<std::string, std::string> > data)throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        try 
        {
            for(std::vector< std::pair<std::string, std::string> >::iterator iterMetaData = data.begin(); iterMetaData != data.end(); ++iterMetaData)
            {
                this->setImageMetaData(iterMetaData->first, iterMetaData->second);
            }
            
            this->keaImgFile->flush(H5F_SCOPE_GLOBAL);
        }
        catch (H5::Exception &e)
        {
            throw KEAIOException("Could not set image band meta data.");
        }
        catch ( KEAIOException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }
    
    void KEAImageIO::setImageBandMetaData(uint32_t band, std::string name, std::string value)throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        // FORM META-DATA PATH WITHIN THE H5 FILE 
        std::string metaDataH5Path = KEA_DATASETNAME_BAND + uint2Str(band) + KEA_BANDNAME_METADATA + std::string("/") + name;
        
        // WRITE IMAGE BAND META DATA
        try 
        {
            // OPEN DATASET OR CREATE NEW DATASET IF IT DOES NOT EXIST
            H5::StrType strTypeAll(0, H5T_VARIABLE);
            H5::DataSet datasetMetaData;
            try 
            {
                datasetMetaData = this->keaImgFile->openDataSet( metaDataH5Path );
            }
            catch (H5::Exception &e)
            {
                hsize_t	dimsForStr[1];
                dimsForStr[0] = 1; // number of lines;
                H5::DataSpace dataspaceStrAll(1, dimsForStr);
                datasetMetaData = this->keaImgFile->createDataSet(metaDataH5Path, strTypeAll, dataspaceStrAll);
                
            }
            // WRITE DATA INTO THE DATASET
            const char **wStrdata = new const char*[1];
            wStrdata[0] = value.c_str();			
            datasetMetaData.write((void*)wStrdata, strTypeAll);
            datasetMetaData.close();
            delete[] wStrdata;
            
            this->keaImgFile->flush(H5F_SCOPE_GLOBAL);
        }
        catch (H5::Exception &e) 
        {
            throw KEAIOException("Could not set image band meta-data.");
        }
        catch ( KEAIOException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }
    
    std::string KEAImageIO::getImageBandMetaData(uint32_t band, std::string name)throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        std::string metaDataH5Path = KEA_DATASETNAME_BAND + uint2Str(band) + KEA_BANDNAME_METADATA + std::string("/") + name;
        std::string value = "";
        // READ IMAGE BAND META-DATA
        try 
        {
            hid_t nativeStrType;
            H5::DataSet datasetMetaData = this->keaImgFile->openDataSet( metaDataH5Path );
            H5::DataType strDataType = datasetMetaData.getDataType();
            char **strData = new char*[1];
            if((nativeStrType=H5Tget_native_type(strDataType.getId(), H5T_DIR_DEFAULT))<0)
            {
                throw KEAIOException("Could not define a native string type");
            }
            datasetMetaData.read((void*)strData, strDataType);
            value = std::string(strData[0]);
            free(strData[0]);
            delete[] strData;
            datasetMetaData.close();
        } 
        catch ( H5::Exception &e) 
        {
            throw KEAIOException("Meta-data variable was not accessable.");
        }
        catch ( KEAIOException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return value;
    }
    
    std::vector<std::string> KEAImageIO::getImageBandMetaDataNames(uint32_t band)throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        std::vector<std::string> metaDataNames;
        
        std::string metaDataGroupName = KEA_DATASETNAME_BAND + uint2Str(band) + KEA_BANDNAME_METADATA;
        try 
        {
            // Try to open dataset with overviewName
            H5::Group imgBandMetaDataGrp = this->keaImgFile->openGroup(metaDataGroupName);
            hsize_t numMetaDataItems = imgBandMetaDataGrp.getNumObjs();
            
            for(hsize_t i = 0; i < numMetaDataItems; ++i)
            {
                metaDataNames.push_back(imgBandMetaDataGrp.getObjnameByIdx(i));
            }
        }
        catch (H5::Exception &e)
        {
            throw KEAIOException("Could not retrieve image band meta data.");
        }
        catch ( KEAIOException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return metaDataNames;
    }
    
    std::vector< std::pair<std::string, std::string> > KEAImageIO::getImageBandMetaData(uint32_t band)throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        std::vector< std::pair<std::string, std::string> > metaData;
        
        std::string metaDataGroupName = KEA_DATASETNAME_BAND + uint2Str(band) + KEA_BANDNAME_METADATA;
        try 
        {
            // Try to open dataset with overviewName
            H5::Group imgBandMetaDataGrp = this->keaImgFile->openGroup(metaDataGroupName);
            hsize_t numMetaDataItems = imgBandMetaDataGrp.getNumObjs();
            std::string name = "";
            std::string value = "";
            
            for(hsize_t i = 0; i < numMetaDataItems; ++i)
            {
                name = imgBandMetaDataGrp.getObjnameByIdx(i);
                value = this->getImageBandMetaData(band, name);
                metaData.push_back(std::pair<std::string, std::string>(name,value));
            }
        }
        catch (H5::Exception &e)
        {
            throw KEAIOException("Could not retrieve image band meta data.");
        }
        catch ( KEAIOException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return metaData;
    }
    
    void KEAImageIO::setImageBandMetaData(uint32_t band, std::vector< std::pair<std::string, std::string> > data)throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        try 
        {
            for(std::vector< std::pair<std::string, std::string> >::iterator iterMetaData = data.begin(); iterMetaData != data.end(); ++iterMetaData)
            {
                this->setImageBandMetaData(band, iterMetaData->first, iterMetaData->second);
            }
            
            this->keaImgFile->flush(H5F_SCOPE_GLOBAL);
        }
        catch (H5::Exception &e)
        {
            throw KEAIOException("Could not set image band meta data.");
        }
        catch ( KEAIOException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }
    
    void KEAImageIO::setImageBandDescription(uint32_t band, std::string description)throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        std::string bandName = KEA_DATASETNAME_BAND + uint2Str(band);
        
        // WRITE IMAGE BAND DESCRIPTION
        try 
        {
            H5::StrType strTypeAll(0, H5T_VARIABLE);
            H5::DataSet datasetBandDescription = this->keaImgFile->openDataSet( bandName+KEA_BANDNAME_DESCRIP );
            const char **wStrdata = new const char*[1];
            wStrdata[0] = description.c_str();			
            datasetBandDescription.write((void*)wStrdata, strTypeAll);
            datasetBandDescription.close();
            delete[] wStrdata;
            this->keaImgFile->flush(H5F_SCOPE_GLOBAL);
        }
        catch (H5::Exception &e) 
        {
            throw KEAIOException("Could not set image band description.");
        }
        catch ( KEAIOException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }
    
    std::string KEAImageIO::getImageBandDescription(uint32_t band)throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        std::string bandName = KEA_DATASETNAME_BAND + uint2Str(band) + std::string("/") + KEA_BANDNAME_DESCRIP;
        std::string description = "";
        // READ IMAGE BAND DESCRIPTION
        try 
        {
            hid_t nativeStrType;
            H5::DataSet datasetBandDescription = this->keaImgFile->openDataSet( bandName );
            H5::DataType strDataType = datasetBandDescription.getDataType();
            char **strData = new char*[1];
            if((nativeStrType=H5Tget_native_type(strDataType.getId(), H5T_DIR_DEFAULT))<0)
            {
                throw KEAIOException("Could not define a native string type");
            }
            datasetBandDescription.read((void*)strData, strDataType);
            description = std::string(strData[0]);
            free(strData[0]);
            delete[] strData;
            datasetBandDescription.close();
        } 
        catch ( H5::Exception &e) 
        {
            throw KEAIOException("Could not read band description.");
        }
        catch ( KEAIOException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return description;
    }
    
    
    
    void KEAImageIO::setNoDataValue(uint32_t band, const void *data, KEADataType inDataType)throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        // READ IMAGE DATA TYPE
        try 
        {    
            std::string noDataValPath = KEA_DATASETNAME_BAND + uint2Str(band) + KEA_BANDNAME_NO_DATA_VAL;
            H5::DataSet datasetImgNDV;
            H5::Attribute noDataDefAttribute;
            
            try 
            {
                datasetImgNDV = this->keaImgFile->openDataSet( noDataValPath );
            }
            catch (H5::Exception &e)
            {
                hsize_t	dimsForNDV[1];
                dimsForNDV[0] = 1; // number of lines;
                H5::DataSpace dataspaceNDV(1, dimsForNDV);
                
                KEADataType imgDataType = this->getImageBandDataType(band);
                H5::DataType imgBandDT = convertDatatypeKeaToH5STD(imgDataType);

                datasetImgNDV = this->keaImgFile->createDataSet(noDataValPath, imgBandDT, dataspaceNDV);
            }
            
            try
            {
                noDataDefAttribute = datasetImgNDV.openAttribute(KEA_NODATA_DEFINED);
            }
            catch (H5::Exception &e)
            {
                H5::DataSpace attr_dataspace = H5::DataSpace(H5S_SCALAR);
                noDataDefAttribute = datasetImgNDV.createAttribute(KEA_NODATA_DEFINED, H5::PredType::STD_I8LE, attr_dataspace);
            }
            
            int val = 1;
            noDataDefAttribute.write(H5::PredType::NATIVE_INT, &val);
            noDataDefAttribute.close();
            
            H5::DataType dataDT = convertDatatypeKeaToH5Native(inDataType);
            datasetImgNDV.write( data, dataDT );
            datasetImgNDV.close();
            this->keaImgFile->flush(H5F_SCOPE_GLOBAL);
        } 
        catch ( H5::Exception &e) 
        {
            throw KEAIOException("The image data type was not specified.");
        }
        catch ( KEAIOException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }
    
    void KEAImageIO::getNoDataValue(uint32_t band, void *data, KEADataType inDataType)throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        
        // READ IMAGE BAND NO DATA VALUE
        try 
        {            
            H5::DataType imgBandDT = convertDatatypeKeaToH5Native(inDataType);
            hsize_t dimsValue[1];
            dimsValue[0] = 1;
            H5::DataSpace valueDataSpace(1, dimsValue);
            H5::DataSet datasetImgNDV = this->keaImgFile->openDataSet( KEA_DATASETNAME_BAND + uint2Str(band) + KEA_BANDNAME_NO_DATA_VAL );
            
            bool noDataDefined = true;
            try
            {
                H5::Attribute noDataDefAttribute = datasetImgNDV.openAttribute(KEA_NODATA_DEFINED);
                int val = 1;
                noDataDefAttribute.read(H5::PredType::NATIVE_INT, &val);
                noDataDefAttribute.close();
                
                if(val == 0)
                {
                    noDataDefined = false;
                }
            }
            catch ( H5::Exception &e)
            {
                noDataDefined = true;
            }
            
            if(noDataDefined)
            {
                datasetImgNDV.read(data, imgBandDT, valueDataSpace);
            }
            else
            {
                throw KEAIOException("The image band no data value was not defined.");
            }
            datasetImgNDV.close();
            valueDataSpace.close();
        } 
        catch ( H5::Exception &e) 
        {
            throw KEAIOException("The image band no data value was not specified.");
        }
        catch ( KEAIOException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }
    
    void KEAImageIO::undefineNoDataValue(uint32_t band)throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        
        // UNDEFINE THE NO DATA VALUE
        try
        {
            H5::DataSet datasetImgNDV = this->keaImgFile->openDataSet( KEA_DATASETNAME_BAND + uint2Str(band) + KEA_BANDNAME_NO_DATA_VAL );
            try
            {
                H5::Attribute noDataDefAttribute = datasetImgNDV.openAttribute(KEA_NODATA_DEFINED);
                int val = 0;
                noDataDefAttribute.write(H5::PredType::NATIVE_INT, &val);
                noDataDefAttribute.close();
            }
            catch ( H5::Exception &e)
            {
                H5::DataSpace attr_dataspace = H5::DataSpace(H5S_SCALAR);
                H5::Attribute noDataDefAttribute = datasetImgNDV.createAttribute(KEA_NODATA_DEFINED, H5::PredType::STD_I8LE, attr_dataspace);
                int val = 0;
                noDataDefAttribute.write(H5::PredType::NATIVE_INT, &val);
                noDataDefAttribute.close();
            }
            
            datasetImgNDV.close();
        }
        catch ( H5::Exception &e)
        {
            throw KEAIOException("The image band no data value had not been created.");
        }
        catch ( KEAIOException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }
    
    std::vector<KEAImageGCP*>* KEAImageIO::getGCPs()throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        std::vector<KEAImageGCP*> *gcps = new std::vector<KEAImageGCP*>();
        
        
        try
        {            
            uint32_t numGCPs = 0;
            // Get the number of GCPs
            try
            {
                hsize_t dimsValue[1];
                dimsValue[0] = 1;
                H5::DataSpace valueDataSpace(1, dimsValue);
                uint32_t value[1];
                H5::DataSet datasetNumGCPs = this->keaImgFile->openDataSet( KEA_GCPS_NUM );
                datasetNumGCPs.read(value, H5::PredType::NATIVE_UINT32, valueDataSpace);
                numGCPs = value[0];
                datasetNumGCPs.close();
                valueDataSpace.close();
            }
            catch (H5::Exception &e)
            {
                throw KEAIOException("Could not read the number of GCPs.");
            }
            catch ( KEAIOException &e)
            {
                throw e;
            }
            catch ( std::exception &e)
            {
                throw KEAIOException(e.what());
            }

            KEAImageGCP_HDF5 *gcpsHDF = new KEAImageGCP_HDF5[numGCPs];
            
            try
            {
                H5::CompType *fieldDtMem = this->createGCPCompTypeMem();
                H5::DataSet gcpsDataset = this->keaImgFile->openDataSet( KEA_GCPS_DATA );
                
                H5::DataSpace gcpsDataspace = gcpsDataset.getSpace();
                hsize_t boolFieldOff[1];
                boolFieldOff[0] = 0;
                
                hsize_t boolFieldsDims[1];
                boolFieldsDims[0] = numGCPs;
                H5::DataSpace gcpsMemspace(1, boolFieldsDims);
                
                gcpsDataspace.selectHyperslab( H5S_SELECT_SET, boolFieldsDims, boolFieldOff );
                gcpsDataset.read(gcpsHDF, *fieldDtMem, gcpsMemspace, gcpsDataspace);
                
                gcpsDataset.close();
                gcpsDataspace.close();
                gcpsMemspace.close();
            }
            catch( H5::Exception &e )
            {
                throw KEAIOException(e.getDetailMsg());
            }
            
            KEAImageGCP *tmpGCP = NULL;
            for(uint32_t i = 0; i < numGCPs; ++i)
            {
                tmpGCP = new KEAImageGCP();
                tmpGCP->pszId = std::string(gcpsHDF[i].pszId);
                tmpGCP->pszInfo = std::string(gcpsHDF[i].pszInfo);
                tmpGCP->dfGCPLine = gcpsHDF[i].dfGCPLine;
                tmpGCP->dfGCPPixel = gcpsHDF[i].dfGCPPixel;
                tmpGCP->dfGCPX = gcpsHDF[i].dfGCPX;
                tmpGCP->dfGCPY = gcpsHDF[i].dfGCPY;
                tmpGCP->dfGCPZ = gcpsHDF[i].dfGCPZ;
                gcps->push_back(tmpGCP);
            }
            
            delete[] gcpsHDF;
        }
        catch (H5::Exception &e)
        {
            throw KEAIOException(e.getCDetailMsg());
        }
        catch ( KEAIOException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return gcps;
    }
    
    void KEAImageIO::setGCPs(std::vector<KEAImageGCP*> *gcps, std::string projWKT)throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        uint32_t numGCPs = gcps->size();
        
        KEAImageGCP_HDF5 *gcpsHDF = new KEAImageGCP_HDF5[numGCPs];
        
        uint32_t i = 0;
        for(std::vector<KEAImageGCP*>::iterator iterGCP = gcps->begin(); iterGCP != gcps->end(); ++iterGCP)
        {
            // Copy the char from one to the other for PSZ ID.
            const size_t lenPSZId = strlen((*iterGCP)->pszId.c_str());
            gcpsHDF[i].pszId = new char[lenPSZId + 1];
            strncpy(gcpsHDF[i].pszId, (*iterGCP)->pszId.c_str(), lenPSZId);
            gcpsHDF[i].pszId[lenPSZId] = '\0';
            
            // Copy the char from one to the other for PSZ INFO.
            const size_t lenPSZInfo = strlen((*iterGCP)->pszInfo.c_str());
            gcpsHDF[i].pszInfo = new char[lenPSZInfo + 1];
            strncpy(gcpsHDF[i].pszInfo, (*iterGCP)->pszInfo.c_str(), lenPSZInfo);
            gcpsHDF[i].pszInfo[lenPSZInfo] = '\0';
            
            gcpsHDF[i].dfGCPPixel = (*iterGCP)->dfGCPPixel;
            gcpsHDF[i].dfGCPLine = (*iterGCP)->dfGCPLine;
            gcpsHDF[i].dfGCPX = (*iterGCP)->dfGCPX;
            gcpsHDF[i].dfGCPY = (*iterGCP)->dfGCPY;
            gcpsHDF[i].dfGCPZ = (*iterGCP)->dfGCPZ;            
            
            ++i;
        }
        
        try
        {
            // Open or create the GCPs dataset 
            try
            {
                H5::DataSet gcpsDataset = this->keaImgFile->openDataSet(KEA_GCPS_DATA);
                H5::DataSpace gcpsWriteDataSpace = gcpsDataset.getSpace();
                
                H5::CompType *fieldDtMem = this->createGCPCompTypeMem();
                
                hsize_t gcpsDataDims[1];
                gcpsWriteDataSpace.getSimpleExtentDims(gcpsDataDims);
                
                if(numGCPs > gcpsDataDims[0])
                {
                    hsize_t extendGCPsDatasetTo[1];
                    extendGCPsDatasetTo[0] = numGCPs;
                    gcpsDataset.extend( extendGCPsDatasetTo );
                }
                
                hsize_t gcpsOffset[1];
                gcpsOffset[0] = 0;
                gcpsDataDims[0] = numGCPs;
                
                gcpsWriteDataSpace.close();
                gcpsWriteDataSpace = gcpsDataset.getSpace();
                gcpsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, gcpsDataDims, gcpsOffset);
                H5::DataSpace newGCPsDataspace = H5::DataSpace(1, gcpsDataDims);
                
                gcpsDataset.write(gcpsHDF, *fieldDtMem, newGCPsDataspace, gcpsWriteDataSpace);
                
                gcpsWriteDataSpace.close();
                newGCPsDataspace.close();
                gcpsDataset.close();
            }
            catch ( H5::Exception &e)
            {
                H5::CompType *fieldDtDisk = this->createGCPCompTypeDisk();
                H5::CompType *fieldDtMem = this->createGCPCompTypeMem();
                
                hsize_t initDimsGCPsDS[1];
                initDimsGCPsDS[0] = numGCPs;
                hsize_t maxDimsGCPsDS[1];
                maxDimsGCPsDS[0] = H5S_UNLIMITED;
                H5::DataSpace gcpsDataSpace = H5::DataSpace(1, initDimsGCPsDS, maxDimsGCPsDS);
                
                hsize_t dimsGCPsChunk[1];
                dimsGCPsChunk[0] = numGCPs;
                
                H5::DSetCreatPropList creationGCPsDSPList;
                creationGCPsDSPList.setChunk(1, dimsGCPsChunk);
                creationGCPsDSPList.setShuffle();
                creationGCPsDSPList.setDeflate(1);
                H5::DataSet gcpsDataset = this->keaImgFile->createDataSet(KEA_GCPS_DATA, *fieldDtDisk, gcpsDataSpace, creationGCPsDSPList);
                
                hsize_t gcpsOffset[1];
                gcpsOffset[0] = 0;
                hsize_t gcpsDataDims[1];
                gcpsDataDims[0] = numGCPs;
                
                H5::DataSpace gcpsWriteDataSpace = gcpsDataset.getSpace();
                gcpsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, gcpsDataDims, gcpsOffset);
                H5::DataSpace newGCPsDataspace = H5::DataSpace(1, gcpsDataDims);
                
                gcpsDataset.write(gcpsHDF, *fieldDtMem, newGCPsDataspace, gcpsWriteDataSpace);
                
                gcpsDataSpace.close();
                gcpsWriteDataSpace.close();
                newGCPsDataspace.close();
                gcpsDataset.close();                
            }
            
            
            // Set the number of GCPs
            try
            {
                H5::DataSet numBandsDataset;
                try
                {
                    // open the dataset
                    numBandsDataset = this->keaImgFile->openDataSet(KEA_GCPS_NUM);
                }
                catch (H5::Exception &e)
                {
                    // create the dataset if it does not exist
                    hsize_t dimsNumBands[] = { 1 };
                    H5::DataSpace numBandsDataSpace(1, dimsNumBands);
                    numBandsDataset = this->keaImgFile->createDataSet(KEA_GCPS_NUM, H5::PredType::STD_U32LE, numBandsDataSpace);
                    numBandsDataSpace.close();
                }
                numBandsDataset.write(&numGCPs, H5::PredType::NATIVE_UINT32);
                numBandsDataset.close();
            }
            catch (H5::Exception &e)
            {
                throw KEAIOException("Could not write the number of GCPs.");
            }
            catch ( KEAIOException &e)
            {
                throw e;
            }
            catch ( std::exception &e)
            {
                throw KEAIOException(e.what());
            }
        }
        catch (H5::Exception &e)
        {
            throw KEAIOException(e.getCDetailMsg());
        }
        catch ( KEAIOException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        delete[] gcpsHDF;
        
        
        // Set the projection string
        try
        {
            this->setGCPProjection(projWKT);
        }
        catch ( KEAIOException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
    }
    
    uint32_t KEAImageIO::getGCPCount()throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        // READ NUMBER OF GCPs
        uint32_t numGCPs = 0;
        try
        {
            hsize_t dimsValue[1];
            dimsValue[0] = 1;
            H5::DataSpace valueDataSpace(1, dimsValue);
            uint32_t value[1];
            H5::DataSet datasetNumGCPs = this->keaImgFile->openDataSet( KEA_GCPS_NUM );
            datasetNumGCPs.read(value, H5::PredType::NATIVE_UINT32, valueDataSpace);
            numGCPs = value[0];
            datasetNumGCPs.close();
            valueDataSpace.close();
        }
        catch ( H5::Exception &e)
        {
            throw KEAIOException("The number of image bands was not specified.");
        }
        
        return numGCPs;
    }
    
    std::string KEAImageIO::getGCPProjection()throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        // READ WKT STRING
        std::string gcpProj = "";
        try
        {
            hid_t nativeStrType;
            H5::DataSet datasetGCPSpatialReference = this->keaImgFile->openDataSet( KEA_GCPS_PROJ );
            H5::DataType strDataType = datasetGCPSpatialReference.getDataType();
            char **strData = new char*[1];
            if((nativeStrType=H5Tget_native_type(strDataType.getId(), H5T_DIR_DEFAULT))<0)
            {
                throw KEAIOException("Could not define a native string type");
            }
            datasetGCPSpatialReference.read((void*)strData, strDataType);
            gcpProj = std::string(strData[0]);
            delete strData[0];
            delete[] strData;
            datasetGCPSpatialReference.close();
        }
        catch ( H5::Exception &e)
        {
            throw KEAIOException("The spatial reference was not specified.");
        }
        
        return gcpProj;
    }
    
    void KEAImageIO::setGCPProjection(std::string projWKT)throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        // SET THE WKT STRING SPATAIL REFERENCE IN GLOBAL HEADER
        try
        {
            const char **wStrdata = NULL;
            H5::DataSet datasetSpatialReference = this->keaImgFile->openDataSet(KEA_GCPS_PROJ);
            H5::DataType strDataType = datasetSpatialReference.getDataType();
            wStrdata = new const char*[1];
            wStrdata[0] = projWKT.c_str();
            datasetSpatialReference.write((void*)wStrdata, strDataType);
            datasetSpatialReference.close();
            delete[] wStrdata;
            this->keaImgFile->flush(H5F_SCOPE_GLOBAL);
        }
        catch (H5::Exception &e)
        {
            const char **wStrdata = NULL;
			hsize_t	dimsForStr[1];
			dimsForStr[0] = 1; // number of lines;
            H5::DataSpace dataspaceStrAll(1, dimsForStr);
            H5::StrType strTypeAll(0, H5T_VARIABLE);
            H5::DataSet datasetSpatialReference = this->keaImgFile->createDataSet(KEA_GCPS_PROJ, strTypeAll, dataspaceStrAll);
			wStrdata = new const char*[1];
			wStrdata[0] = projWKT.c_str();
			datasetSpatialReference.write((void*)wStrdata, strTypeAll);
			datasetSpatialReference.close();
			delete[] wStrdata;
        }
        catch ( KEAIOException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }
    
    void KEAImageIO::setSpatialInfo(KEAImageSpatialInfo *inSpatialInfo)throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        try 
        {
            // SET X AND Y TL IN GLOBAL HEADER
            double *doubleVals = new double[2];
            doubleVals[0] = inSpatialInfo->tlX;
            doubleVals[1] = inSpatialInfo->tlY;
            H5::DataSet spatialTLDataset = this->keaImgFile->openDataSet(KEA_DATASETNAME_HEADER_TL);
			spatialTLDataset.write( doubleVals, H5::PredType::NATIVE_DOUBLE );
            spatialTLDataset.close();
            
            // SET X AND Y RESOLUTION IN GLOBAL HEADER
            doubleVals[0] = inSpatialInfo->xRes;
            doubleVals[1] = inSpatialInfo->yRes;
            H5::DataSet spatialResDataset = this->keaImgFile->openDataSet(KEA_DATASETNAME_HEADER_RES);
			spatialResDataset.write( doubleVals, H5::PredType::NATIVE_DOUBLE );
            spatialResDataset.close();
            
            // SET X AND Y ROTATION IN GLOBAL HEADER
            doubleVals[0] = inSpatialInfo->xRot;
            doubleVals[1] = inSpatialInfo->yRot;
            H5::DataSet spatialRotDataset = this->keaImgFile->openDataSet(KEA_DATASETNAME_HEADER_ROT);
			spatialRotDataset.write( doubleVals, H5::PredType::NATIVE_DOUBLE );
            spatialRotDataset.close();
            delete[] doubleVals;
            
            // SET THE WKT STRING SPATAIL REFERENCE IN GLOBAL HEADER
			const char **wStrdata = NULL;
            H5::DataSet datasetSpatialReference = this->keaImgFile->openDataSet(KEA_DATASETNAME_HEADER_WKT);
            H5::DataType strDataType = datasetSpatialReference.getDataType();
			wStrdata = new const char*[1];
			wStrdata[0] = inSpatialInfo->wktString.c_str();			
			datasetSpatialReference.write((void*)wStrdata, strDataType);
			datasetSpatialReference.close();
			delete[] wStrdata;
            
            this->keaImgFile->flush(H5F_SCOPE_GLOBAL);
        } 
        catch (H5::Exception &e)
        {
            throw KEAIOException(e.getCDetailMsg());
        }
        catch ( KEAIOException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }
    
    KEAImageSpatialInfo* KEAImageIO::getSpatialInfo() throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        return spatialInfoFile;
    }
    
    uint32_t KEAImageIO::getNumOfImageBands() throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        return this->numImgBands;
    }
    
    uint32_t KEAImageIO::getImageBlockSize(uint32_t band) throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        uint32_t imgBlockSize = 0;
        
        try 
        {
            // CHECK PARAMETERS PROVIDED FIT WITHIN IMAGE
            if(band == 0)
            {
                throw KEAIOException("KEA Image Bands start at 1.");
            }
            else if(band > this->numImgBands)
            {
                throw KEAIOException("Band is not present within image."); 
            }
            
            // OPEN BAND DATASET
            try 
            {
                std::string imageBandPath = KEA_DATASETNAME_BAND + uint2Str(band);
                H5::DataSet imgBandDataset = this->keaImgFile->openDataSet( imageBandPath + KEA_BANDNAME_DATA );
                H5::Attribute blockSizeAtt = imgBandDataset.openAttribute(KEA_ATTRIBUTENAME_BLOCK_SIZE);
                blockSizeAtt.read(H5::PredType::NATIVE_UINT32, &imgBlockSize);
                imgBandDataset.close();
                blockSizeAtt.close();
            } 
            catch ( H5::Exception &e) 
            {
                throw KEAIOException("Could not get image block size.");
            }            
        }
        catch(KEAIOException &e)
        {
            throw e;
        }
        catch( H5::FileIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( H5::DataSetIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( H5::DataSpaceIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( H5::DataTypeIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return imgBlockSize;
    }
    
    KEADataType KEAImageIO::getImageBandDataType(uint32_t band) throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        KEADataType imgDataType = kealib::kea_undefined;
        
        // READ IMAGE DATA TYPE
        try 
        {
            hsize_t dimsValue[1];
            dimsValue[0] = 1;
            H5::DataSpace valueDataSpace(1, dimsValue);
            uint32_t value[1];
            H5::DataSet datasetImgDT = this->keaImgFile->openDataSet( KEA_DATASETNAME_BAND + uint2Str(band) + KEA_BANDNAME_DT );
            datasetImgDT.read(value, H5::PredType::NATIVE_UINT32, valueDataSpace);
            imgDataType = (KEADataType)value[0];
            datasetImgDT.close();
            valueDataSpace.close();
        } 
        catch ( H5::Exception &e) 
        {
            throw KEAIOException("The image band data type was not specified.");
        }
        catch ( KEAIOException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return imgDataType;
    }
    
    std::string KEAImageIO::getKEAImageVersion() throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        return this->keaVersion;
    }
    
    void KEAImageIO::setImageBandLayerType(uint32_t band, KEALayerType imgLayerType) throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
                
        // WRITE IMAGE LAYER TYPE
        try 
        {
            uint32_t value = (uint32_t)imgLayerType;
            H5::DataSet datasetImgLT = this->keaImgFile->openDataSet( KEA_DATASETNAME_BAND + uint2Str(band) + KEA_BANDNAME_TYPE );
            datasetImgLT.write(&value, H5::PredType::NATIVE_UINT32);
            datasetImgLT.close();
            this->keaImgFile->flush(H5F_SCOPE_GLOBAL);
        } 
        catch ( H5::Exception &e) 
        {
            throw KEAIOException("The image band data type was not specified.");
        }
        catch ( KEAIOException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }
    
    KEALayerType KEAImageIO::getImageBandLayerType(uint32_t band) throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        KEALayerType imgLayerType = kea_continuous;
        
        // READ IMAGE LAYER TYPE
        try 
        {
            hsize_t dimsValue[1];
            dimsValue[0] = 1;
            H5::DataSpace valueDataSpace(1, dimsValue);
            uint32_t value[1];
            H5::DataSet datasetImgLT = this->keaImgFile->openDataSet( KEA_DATASETNAME_BAND + uint2Str(band) + KEA_BANDNAME_TYPE );
            datasetImgLT.read(value, H5::PredType::NATIVE_UINT32, valueDataSpace);
            imgLayerType = (KEALayerType)value[0];
            datasetImgLT.close();
            valueDataSpace.close();
        } 
        catch ( H5::Exception &e) 
        {
            throw KEAIOException("The image band data type was not specified.");
        }
        catch ( KEAIOException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return imgLayerType;
    }
    
    void KEAImageIO::setImageBandClrInterp(uint32_t band, KEABandClrInterp imgLayerClrInterp) throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        // WRITE IMAGE LAYER USAGE
        uint32_t value = (uint32_t) imgLayerClrInterp;
        try 
        {
            H5::DataSet datasetImgLU = this->keaImgFile->openDataSet( KEA_DATASETNAME_BAND + uint2Str(band) + KEA_BANDNAME_USAGE );
            datasetImgLU.write(&value, H5::PredType::NATIVE_UINT32);
            datasetImgLU.close();
            this->keaImgFile->flush(H5F_SCOPE_GLOBAL);
        } 
        catch ( H5::Exception &e) 
        {
            hsize_t dimsUsage[1];
            dimsUsage[0] = 1;
            H5::DataSpace usageDataSpace(1, dimsUsage);
            H5::DataSet usageDataset = this->keaImgFile->createDataSet((KEA_DATASETNAME_BAND + uint2Str(band) + KEA_BANDNAME_USAGE), H5::PredType::STD_U8LE, usageDataSpace);
            usageDataset.write( &value, H5::PredType::NATIVE_UINT32 );
            usageDataset.close();
        }
        catch ( KEAIOException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }
    
    KEABandClrInterp KEAImageIO::getImageBandClrInterp(uint32_t band) throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        KEABandClrInterp imgLayerClrInterp = kea_generic;
        
        // READ IMAGE LAYER USAGE
        try 
        {
            hsize_t dimsValue[1];
            dimsValue[0] = 1;
            H5::DataSpace valueDataSpace(1, dimsValue);
            uint32_t value[1];
            H5::DataSet datasetImgLU = this->keaImgFile->openDataSet( KEA_DATASETNAME_BAND + uint2Str(band) + KEA_BANDNAME_USAGE );
            datasetImgLU.read(value, H5::PredType::NATIVE_UINT32, valueDataSpace);
            imgLayerClrInterp = (KEABandClrInterp)value[0];
            datasetImgLU.close();
            valueDataSpace.close();
        } 
        catch ( H5::Exception &e) 
        {
            //throw KEAIOException("The image band data type was not specified.");
            imgLayerClrInterp = kea_generic; // Field was not present within the file.
        }
        catch ( KEAIOException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return imgLayerClrInterp;
    }
    
    void KEAImageIO::createOverview(uint32_t band, uint32_t overview, uint64_t xSize, uint64_t ySize) throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        std::string overviewName = KEA_DATASETNAME_BAND + uint2Str(band) + KEA_OVERVIEWSNAME_OVERVIEW + uint2Str(overview);
                
        try 
        {
            // Try to open dataset with overviewName
            H5::DataSet imgBandDataset = this->keaImgFile->openDataSet( overviewName );
            this->keaImgFile->unlink(overviewName);
        }
        catch (H5::Exception &e)
        {
            // Do nothing as dataset does not exist.
        }
        
        try 
        {
            KEADataType imgDataType = this->getImageBandDataType(band);
            
            H5::DataType imgBandDT = convertDatatypeKeaToH5STD(imgDataType);
            int initFillVal = 0;
            
            hsize_t imageBandDims[2];
            imageBandDims[0] = ySize;
            imageBandDims[1] = xSize;
            H5::DataSpace imgBandDataSpace(2, imageBandDims);
            
            hsize_t dimsImageBandChunk[2];
            // Make sure that the chuck size is not bigger than the dataset.
            uint32_t imgBlockSize = this->getImageBlockSize(band);
            uint64_t smallestAxis = 0;
            if(xSize < ySize)
            {
                smallestAxis = xSize;
            }
            else
            {
                smallestAxis = ySize;
            }
            if(smallestAxis < imgBlockSize)
            {
                dimsImageBandChunk[0] = smallestAxis;
                dimsImageBandChunk[1] = smallestAxis;
            }
            else
            {
                dimsImageBandChunk[0] = imgBlockSize;
                dimsImageBandChunk[1] = imgBlockSize;
            }
			
            
            H5::DSetCreatPropList initParamsImgBand;
			initParamsImgBand.setChunk(2, dimsImageBandChunk);			
			initParamsImgBand.setShuffle();
            initParamsImgBand.setDeflate(KEA_DEFLATE);
			initParamsImgBand.setFillValue( H5::PredType::NATIVE_INT, &initFillVal);
            
            H5::StrType strdatatypeLen6(H5::PredType::C_S1, 6);
            H5::StrType strdatatypeLen4(H5::PredType::C_S1, 4);
            const H5std_string strClassVal ("IMAGE");
            const H5std_string strImgVerVal ("1.2");
            H5::DataSpace attr_dataspace = H5::DataSpace(H5S_SCALAR);
                        
            // CREATE THE IMAGE DATA ARRAY
            H5::DataSet imgBandDataSet = this->keaImgFile->createDataSet(overviewName, imgBandDT, imgBandDataSpace, initParamsImgBand);
            
            H5::Attribute classAttribute = imgBandDataSet.createAttribute(KEA_ATTRIBUTENAME_CLASS, strdatatypeLen6, attr_dataspace);
            classAttribute.write(strdatatypeLen6, strClassVal); 
            classAttribute.close();
            
            H5::Attribute imgVerAttribute = imgBandDataSet.createAttribute(KEA_ATTRIBUTENAME_IMAGE_VERSION, strdatatypeLen4, attr_dataspace);
            imgVerAttribute.write(strdatatypeLen4, strImgVerVal);
            imgVerAttribute.close();
            
            H5::Attribute blockSizeAttribute = imgBandDataSet.createAttribute(KEA_ATTRIBUTENAME_BLOCK_SIZE, H5::PredType::STD_U16LE, attr_dataspace);
            uint32_t blockSizeTmp = dimsImageBandChunk[0]; // copy into a temporary variable to write to the file - fixing a bug on solaris.
            blockSizeAttribute.write(H5::PredType::NATIVE_UINT32, &blockSizeTmp);
            blockSizeAttribute.close();
            
            imgBandDataSet.close();
            
            attr_dataspace.close();
            imgBandDataSet.close();
            
            this->keaImgFile->flush(H5F_SCOPE_GLOBAL);
        }
        catch (H5::Exception &e)
        {
            throw KEAIOException(e.getCDetailMsg());
        }
        catch ( KEAIOException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }
    
    void KEAImageIO::removeOverview(uint32_t band, uint32_t overview) throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        std::string overviewName = KEA_DATASETNAME_BAND + uint2Str(band) + KEA_OVERVIEWSNAME_OVERVIEW + uint2Str(overview);
        
        try 
        {
            // Try to open dataset with overviewName
            H5::DataSet imgBandDataset = this->keaImgFile->openDataSet( overviewName );
            this->keaImgFile->unlink(overviewName);
            this->keaImgFile->flush(H5F_SCOPE_GLOBAL);
        }
        catch (H5::Exception &e)
        {
            // Do nothing as dataset does not exist.
        }
        catch ( KEAIOException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
    }
    
    uint32_t KEAImageIO::getOverviewBlockSize(uint32_t band, uint32_t overview) throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        uint32_t ovBlockSize = 0;
        
        try 
        {
            // CHECK PARAMETERS PROVIDED FIT WITHIN IMAGE
            if(band == 0)
            {
                throw KEAIOException("KEA Image Bands start at 1.");
            }
            else if(band > this->numImgBands)
            {
                throw KEAIOException("Band is not present within image."); 
            }
            
            // OPEN BAND DATASET
            try 
            {
                std::string overviewName = KEA_DATASETNAME_BAND + uint2Str(band) + KEA_OVERVIEWSNAME_OVERVIEW + uint2Str(overview);
                H5::DataSet imgBandDataset = this->keaImgFile->openDataSet( overviewName );
                H5::Attribute blockSizeAtt = imgBandDataset.openAttribute(KEA_ATTRIBUTENAME_BLOCK_SIZE);
                blockSizeAtt.read(H5::PredType::NATIVE_UINT32, &ovBlockSize);
                imgBandDataset.close();
                blockSizeAtt.close();
            } 
            catch ( H5::Exception &e) 
            {
                throw KEAIOException("Could not retrieve the overview block size.");
            }            
        }
        catch(KEAIOException &e)
        {
            throw e;
        }
        catch( H5::FileIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( H5::DataSetIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( H5::DataSpaceIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( H5::DataTypeIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return ovBlockSize;
    }
    
    void KEAImageIO::writeToOverview(uint32_t band, uint32_t overview, void *data, uint64_t xPxlOff, uint64_t yPxlOff, uint64_t xSizeOut, uint64_t ySizeOut, uint64_t xSizeBuf, uint64_t ySizeBuf, KEADataType inDataType) throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        try 
        {
            // CHECK PARAMETERS PROVIDED FIT WITHIN IMAGE
            if(band == 0)
            {
                throw KEAIOException("KEA Image Bands start at 1.");
            }
            else if(band > this->numImgBands)
            {
                throw KEAIOException("Band is not present within image."); 
            }
            
            // GET NATIVE DATASET
            H5::DataType imgBandDT = convertDatatypeKeaToH5Native(inDataType);

            // OPEN BAND DATASET AND WRITE IMAGE DATA
            try 
            {
                std::string overviewName = KEA_DATASETNAME_BAND + uint2Str(band) + KEA_OVERVIEWSNAME_OVERVIEW + uint2Str(overview);
                H5::DataSet imgBandDataset = this->keaImgFile->openDataSet( overviewName );
                H5::DataSpace imgBandDataspace = imgBandDataset.getSpace();                
                
                hsize_t imgOffset[2];
                imgOffset[0] = yPxlOff;
                imgOffset[1] = xPxlOff;
                hsize_t dataDims[2];
                dataDims[0] = ySizeBuf;
                dataDims[1] = xSizeBuf;
                H5::DataSpace write2BandDataspace = H5::DataSpace(2, dataDims);
                
                if((ySizeOut != ySizeBuf) | (xSizeOut != xSizeBuf))
                {                    
                    hsize_t dataSelectMemDims[2];
                    dataSelectMemDims[0] = ySizeOut;
                    dataSelectMemDims[1] = 1;

                    hsize_t dataOffDims[2];
                    dataOffDims[0] = 0;
                    dataOffDims[1] = 0;

                    hsize_t dataSelectStrideDims[2];
                    dataSelectStrideDims[0] = 1;
                    if(xSizeBuf == xSizeOut)
                    {
                        dataSelectStrideDims[1] = 1;
                    }
                    else
                    {
                        dataSelectStrideDims[1] = xSizeBuf - xSizeOut;
                    }

                    hsize_t dataSelectBlockSizeDims[2];
                    dataSelectBlockSizeDims[0] = 1;
                    dataSelectBlockSizeDims[1] = xSizeOut;
                    write2BandDataspace.selectHyperslab(H5S_SELECT_SET, dataSelectMemDims, dataOffDims, dataSelectStrideDims, dataSelectBlockSizeDims);
                    
                    hsize_t dataOutDims[2];
                    dataOutDims[0] = ySizeOut;
                    dataOutDims[1] = xSizeOut;
                    imgBandDataspace.selectHyperslab( H5S_SELECT_SET, dataOutDims, imgOffset);
                }
                else
                {
                    imgBandDataspace.selectHyperslab( H5S_SELECT_SET, dataDims, imgOffset);
                }
                
                imgBandDataset.write( data, imgBandDT, write2BandDataspace, imgBandDataspace);
                
                imgBandDataset.close();
                imgBandDataspace.close();
                write2BandDataspace.close();
            } 
            catch ( H5::Exception &e) 
            {
                throw KEAIOException("Could not write image data.");
            }
            
            this->keaImgFile->flush(H5F_SCOPE_GLOBAL);
        }
        catch(KEAIOException &e)
        {
            throw e;
        }
        catch( H5::FileIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( H5::DataSetIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( H5::DataSpaceIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( H5::DataTypeIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }
    
    void KEAImageIO::readFromOverview(uint32_t band, uint32_t overview, void *data, uint64_t xPxlOff, uint64_t yPxlOff, uint64_t xSizeIn, uint64_t ySizeIn, uint64_t xSizeBuf, uint64_t ySizeBuf, KEADataType inDataType) throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        try 
        {
            // CHECK PARAMETERS PROVIDED FIT WITHIN IMAGE
            if(band == 0)
            {
                throw KEAIOException("KEA Image Bands start at 1.");
            }
            else if(band > this->numImgBands)
            {
                throw KEAIOException("Band is not present within image."); 
            }
            
            // GET NATIVE DATASET
            H5::DataType imgBandDT = convertDatatypeKeaToH5Native(inDataType);
            
            // OPEN BAND DATASET AND READ IMAGE DATA
            try 
            {
                std::string overviewName = KEA_DATASETNAME_BAND + uint2Str(band) + KEA_OVERVIEWSNAME_OVERVIEW + uint2Str(overview);
                H5::DataSet imgBandDataset = this->keaImgFile->openDataSet( overviewName );
                H5::DataSpace imgBandDataspace = imgBandDataset.getSpace();                
                
                hsize_t dataOffset[2];
                dataOffset[0] = yPxlOff;
                dataOffset[1] = xPxlOff;
                hsize_t dataDims[2];
                dataDims[0] = ySizeBuf;
                dataDims[1] = xSizeBuf;
                H5::DataSpace read2BandDataspace = H5::DataSpace(2, dataDims);
                
                if((ySizeBuf != ySizeIn) | (xSizeBuf != xSizeIn))
                {
                    hsize_t dataSelectMemDims[2];
                    dataSelectMemDims[0] = ySizeIn;
                    dataSelectMemDims[1] = 1;

                    hsize_t dataOffDims[2];
                    dataOffDims[0] = 0;
                    dataOffDims[1] = 0;

                    hsize_t dataSelectStrideDims[2];
                    dataSelectStrideDims[0] = 1;
                    if(xSizeBuf == xSizeIn)
                    {
                        dataSelectStrideDims[1] = 1;
                    }
                    else
                    {
                        dataSelectStrideDims[1] = xSizeBuf - xSizeIn;
                    }

                    hsize_t dataSelectBlockSizeDims[2];
                    dataSelectBlockSizeDims[0] = 1;
                    dataSelectBlockSizeDims[1] = xSizeIn;
                    read2BandDataspace.selectHyperslab(H5S_SELECT_SET, dataSelectMemDims, dataOffDims, dataSelectStrideDims, dataSelectBlockSizeDims);
                    
                    hsize_t dataInDims[2];
                    dataInDims[0] = ySizeIn;
                    dataInDims[1] = xSizeIn;
                    imgBandDataspace.selectHyperslab( H5S_SELECT_SET, dataInDims, dataOffset);
                }
                else
                {
                    imgBandDataspace.selectHyperslab( H5S_SELECT_SET, dataDims, dataOffset);
                }
                imgBandDataset.read( data, imgBandDT, read2BandDataspace, imgBandDataspace);
                
                imgBandDataset.close();
                imgBandDataspace.close();
                read2BandDataspace.close();
            } 
            catch ( H5::Exception &e) 
            {
                throw KEAIOException("Could not read from image overview.");
            }            
        }
        catch(KEAIOException &e)
        {
            throw e;
        }
        catch( H5::FileIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( H5::DataSetIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( H5::DataSpaceIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( H5::DataTypeIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }
    
    uint32_t KEAImageIO::getNumOfOverviews(uint32_t band) throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        std::string overviewGroupName = KEA_DATASETNAME_BAND + uint2Str(band) + KEA_BANDNAME_OVERVIEWS;
        uint32_t numOverviews = 0;
        try 
        {
            // Try to open dataset with overviewName
            H5::Group imgOverviewsGrp = this->keaImgFile->openGroup(overviewGroupName);
            numOverviews = imgOverviewsGrp.getNumObjs();
        }
        catch (H5::Exception &e)
        {
            throw KEAIOException("Could not retrieve the number of image band overviews.");
        }
        catch ( KEAIOException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return numOverviews;
    }
    
    void KEAImageIO::getOverviewSize(uint32_t band, uint32_t overview, uint64_t *xSize, uint64_t *ySize) throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        try 
        {
            // CHECK PARAMETERS PROVIDED FIT WITHIN IMAGE
            if(band == 0)
            {
                throw KEAIOException("KEA Image Bands start at 1.");
            }
            else if(band > this->numImgBands)
            {
                throw KEAIOException("Band is not present within image."); 
            }
                        
            // OPEN BAND DATASET AND READ THE IMAGE DIMENSIONS
            try 
            {
                std::string overviewName = KEA_DATASETNAME_BAND + uint2Str(band) + KEA_OVERVIEWSNAME_OVERVIEW + uint2Str(overview);
                H5::DataSet imgBandDataset = this->keaImgFile->openDataSet( overviewName );
                H5::DataSpace imgBandDataspace = imgBandDataset.getSpace();                
                
                uint32_t nDims = imgBandDataspace.getSimpleExtentNdims();
                if(nDims != 2)
                {
                    throw KEAIOException("The number of dimensions for the overview must be 2.");
                }
                hsize_t *dims = new hsize_t[2];
                imgBandDataspace.getSimpleExtentDims(dims);
                
                *xSize = dims[1];
                *ySize = dims[0];
                
                delete[] dims;
                imgBandDataset.close();
            } 
            catch(KEAIOException &e)
            {
                throw e;
            }
            catch ( H5::Exception &e) 
            {
                throw KEAIOException("Could not read from image overview.");
            }            
        }
        catch( H5::Exception &e )
		{
			throw KEAIOException("Could not get the overview size.");
		}
        catch ( KEAIOException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }
    
    KEAAttributeTable* KEAImageIO::getAttributeTable(KEAATTType type, uint32_t band) throw(KEAATTException, KEAIOException)
    {
        KEAAttributeTable *att = NULL;
        try 
        {
            if(type == kea_att_mem)
            {
                att = kealib::KEAAttributeTableInMem::createKeaAtt(this->keaImgFile, band);
            }
            else if(type == kea_att_file)
            {
                att = kealib::KEAAttributeTableFile::createKeaAtt(this->keaImgFile, band);
            }
            else
            {
                throw KEAATTException("The attribute table type was not recognised.");
            }
        }
        catch(KEAATTException &e)
        {
            throw e;
        }
        catch ( KEAIOException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return att;
    }
    
    void KEAImageIO::setAttributeTable(KEAAttributeTable* att, uint32_t band, uint32_t chunkSize, uint32_t deflate) throw(KEAATTException, KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        try 
        {
            att->exportToKeaFile(this->keaImgFile, band, chunkSize, deflate);
            this->keaImgFile->flush(H5F_SCOPE_GLOBAL);
        }
        catch(KEAATTException &e)
        {
            throw e;
        }
        catch ( KEAIOException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }
    
    bool KEAImageIO::attributeTablePresent(uint32_t band)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        bool attPresent = false;
        try 
        {
            std::string bandPathBase = KEA_DATASETNAME_BAND + uint2Str(band);
            hsize_t *attSize = new hsize_t[5];
            try 
            {   
                hsize_t dimsValue[1];
                dimsValue[0] = 5;
                H5::DataSpace valueDataSpace(1, dimsValue);
                H5::DataSet datasetAttSize = this->keaImgFile->openDataSet( bandPathBase + KEA_ATT_SIZE_HEADER );
                datasetAttSize.read(attSize, H5::PredType::STD_U64LE, valueDataSpace);
                datasetAttSize.close();
                valueDataSpace.close();
            } 
            catch (H5::Exception &e) 
            {
                throw KEAIOException("The attribute table size field is not present.");
            }
            
            if(attSize[0] > 0)
            {
                attPresent = true;
            }
            
            delete[] attSize;
        }
        catch(KEAIOException &e)
        {
            throw e;
        }
        catch(KEAATTException &e)
        {
            throw e;
        }
        catch( H5::Exception &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        return attPresent;
    }
    
    void KEAImageIO::close() throw(KEAIOException)
    {
        try 
        {
            delete this->spatialInfoFile;
            this->keaImgFile->close();
            delete this->keaImgFile;
            this->keaImgFile = NULL;
            this->fileOpen = false;
        }
        catch(KEAIOException &e)
        {
            throw e;
        }
        catch( H5::Exception &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }
        
    H5::H5File* KEAImageIO::createKEAImage(std::string fileName, KEADataType dataType, uint32_t xSize, uint32_t ySize, uint32_t numImgBands, std::vector<std::string> *bandDescrips, KEAImageSpatialInfo * spatialInfo, uint32_t imageBlockSize, uint32_t attBlockSize, int mdcElmts, hsize_t rdccNElmts, hsize_t rdccNBytes, double rdccW0, hsize_t sieveBuf, hsize_t metaBlockSize, uint32_t deflate)throw(KEAIOException)
    {
        H5::Exception::dontPrint();
        
        H5::H5File *keaImgH5File = NULL;
        
        try 
        {
            // CREATE HDF FILE ACCESS PROPERTIES - DEFAULT VALUES CAN BE TUNED FROM KEACommon.h
            H5::FileAccPropList keaAccessPlist = H5::FileAccPropList(H5::FileAccPropList::DEFAULT);
            keaAccessPlist.setCache(mdcElmts, rdccNElmts, rdccNBytes, rdccW0);
            keaAccessPlist.setSieveBufSize(sieveBuf);
            keaAccessPlist.setMetaBlockSize(metaBlockSize);
            
            // CREATE THE HDF FILE - EXISTING FILE WILL BE TRUNCATED
            keaImgH5File = new H5::H5File( fileName, H5F_ACC_TRUNC, H5::FileCreatPropList::DEFAULT, keaAccessPlist);
            
            //////////// CREATE GLOBAL HEADER ////////////////
            keaImgH5File->createGroup( KEA_DATASETNAME_HEADER );
            
            bool deleteSpatialInfo = false;
            if(spatialInfo == NULL)
            {
                spatialInfo = new KEAImageSpatialInfo();
                spatialInfo->tlX = 0.0;
                spatialInfo->tlY = 0.0;
                spatialInfo->xRes = 1.0;
                spatialInfo->yRes = -1.0;
                spatialInfo->xRot = 0.0;
                spatialInfo->yRot = 0.0;
                spatialInfo->wktString = "";
                deleteSpatialInfo = true;
            }
            
            spatialInfo->xSize = xSize;
            spatialInfo->ySize = ySize;
                        
            // SET NUMBER OF IMAGE BANDS IN GLOBAL HEADER
            KEAImageIO::setNumImgBandsInFileMetadata(keaImgH5File, numImgBands);
                                    
            // SET X AND Y TL IN GLOBAL HEADER
            double *doubleVals = new double[2];
            doubleVals[0] = spatialInfo->tlX;
            doubleVals[1] = spatialInfo->tlY;
            hsize_t dimsSpatialTL[1];
			dimsSpatialTL[0] = 2;
            H5::DataSpace spatialTLDataSpace(1, dimsSpatialTL);
            H5::DataSet spatialTLDataset = keaImgH5File->createDataSet(KEA_DATASETNAME_HEADER_TL, H5::PredType::IEEE_F64LE, spatialTLDataSpace);
			spatialTLDataset.write( doubleVals, H5::PredType::NATIVE_DOUBLE );
            spatialTLDataset.close();
            spatialTLDataSpace.close();
            delete[] doubleVals;
            
            // SET X AND Y RESOLUTION IN GLOBAL HEADER
            float *floatVals = new float[2];
            floatVals[0] = spatialInfo->xRes;
            floatVals[1] = spatialInfo->yRes;
            hsize_t dimsSpatialRes[1];
			dimsSpatialRes[0] = 2;
            H5::DataSpace spatialResDataSpace(1, dimsSpatialRes);
            H5::DataSet spatialResDataset = keaImgH5File->createDataSet(KEA_DATASETNAME_HEADER_RES, H5::PredType::IEEE_F64LE, spatialResDataSpace);
			spatialResDataset.write( floatVals, H5::PredType::NATIVE_FLOAT );
            spatialResDataset.close();
            spatialResDataSpace.close();
            
            // SET X AND Y ROTATION IN GLOBAL HEADER
            floatVals[0] = spatialInfo->xRot;
            floatVals[1] = spatialInfo->yRot;
            hsize_t dimsSpatialRot[1];
			dimsSpatialRot[0] = 2;
            H5::DataSpace spatialRotDataSpace(1, dimsSpatialRot);
            H5::DataSet spatialRotDataset = keaImgH5File->createDataSet(KEA_DATASETNAME_HEADER_ROT, H5::PredType::IEEE_F64LE, spatialRotDataSpace);
			spatialRotDataset.write( floatVals, H5::PredType::NATIVE_FLOAT );
            spatialRotDataset.close();
            spatialRotDataSpace.close();
            delete[] floatVals;
                        
            // SET NUMBER OF X AND Y PIXELS
            uint64_t *uLongVals = new uint64_t[2];
            uLongVals[0] = spatialInfo->xSize;
            uLongVals[1] = spatialInfo->ySize;
            hsize_t dimsSpatialSize[1];
			dimsSpatialSize[0] = 2;
            H5::DataSpace spatialSizeDataSpace(1, dimsSpatialSize);
            H5::DataSet spatialSizeDataset = keaImgH5File->createDataSet(KEA_DATASETNAME_HEADER_SIZE, H5::PredType::STD_U64LE, spatialSizeDataSpace);
			spatialSizeDataset.write( uLongVals, H5::PredType::NATIVE_UINT64 );
            spatialSizeDataset.close();
            spatialSizeDataSpace.close();
            delete[] uLongVals;
            
            // SET THE WKT STRING SPATAIL REFERENCE IN GLOBAL HEADER
			const char **wStrdata = NULL;
			hsize_t	dimsForStr[1];
			dimsForStr[0] = 1; // number of lines;
            H5::DataSpace dataspaceStrAll(1, dimsForStr);
            H5::StrType strTypeAll(0, H5T_VARIABLE);
            H5::DataSet datasetSpatialReference = keaImgH5File->createDataSet(KEA_DATASETNAME_HEADER_WKT, strTypeAll, dataspaceStrAll);
			wStrdata = new const char*[1];
			wStrdata[0] = spatialInfo->wktString.c_str();			
			datasetSpatialReference.write((void*)wStrdata, strTypeAll);
			datasetSpatialReference.close();
			delete[] wStrdata;
            
            // SET THE FILE TYPE IN GLOBAL HEADER
            H5::DataSet datasetFileType = keaImgH5File->createDataSet(KEA_DATASETNAME_HEADER_FILETYPE, strTypeAll, dataspaceStrAll);
			wStrdata = new const char*[1];
            std::string strVal = "KEA";
			wStrdata[0] = strVal.c_str();			
			datasetFileType.write((void*)wStrdata, strTypeAll);
			datasetFileType.close();
			delete[] wStrdata;
            
            // SET THE FILE GENARATOR IN GLOBAL HEADER
            H5::DataSet datasetGenarator = keaImgH5File->createDataSet(KEA_DATASETNAME_HEADER_GENERATOR, strTypeAll, dataspaceStrAll);
			wStrdata = new const char*[1];
            strVal = "LibKEA";
			wStrdata[0] = strVal.c_str();			
			datasetGenarator.write((void*)wStrdata, strTypeAll);
			datasetGenarator.close();
			delete[] wStrdata;
            
            // SET THE FILE VERSION IN GLOBAL HEADER
            H5::DataSet datasetVersion = keaImgH5File->createDataSet(KEA_DATASETNAME_HEADER_VERSION, strTypeAll, dataspaceStrAll);
			wStrdata = new const char*[1];
            strVal = "1.1";
			wStrdata[0] = strVal.c_str();			
			datasetVersion.write((void*)wStrdata, strTypeAll);
			datasetVersion.close();
			delete[] wStrdata;
            
            if(deleteSpatialInfo)
            {
                delete spatialInfo;
            }
            //////////// CREATED GLOBAL HEADER ////////////////
            
            //////////// CREATE GLOBAL META-DATA ////////////////
			keaImgH5File->createGroup( KEA_DATASETNAME_METADATA );
            //////////// CREATED GLOBAL META-DATA ////////////////
            
            //////////// CREATE GCPS ////////////////
			keaImgH5File->createGroup( KEA_GCPS );
            //////////// CREATED GCPS ////////////////
            
            //////////// CREATE IMAGE BANDS ////////////////
            for(uint32_t i = 0; i < numImgBands; ++i) {
                std::string bandDescription = "";
                if (bandDescrips != NULL && i < bandDescrips->size()) {
                    bandDescription = bandDescrips->at(i);
                }

                addImageBandToFile(keaImgH5File, dataType, xSize, ySize,
                        i+1, bandDescription, imageBlockSize, attBlockSize,
                        deflate);
            }
            //////////// CREATED IMAGE BANDS ////////////////
            
            dataspaceStrAll.close();
            keaImgH5File->flush(H5F_SCOPE_GLOBAL);
        }
        catch (KEAIOException &e) 
        {
            throw e;
        }
        catch( H5::FileIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( H5::DataSetIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( H5::DataSpaceIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( H5::DataTypeIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return keaImgH5File;
    }
    
    H5::H5File* KEAImageIO::openKeaH5RW(std::string fileName, int mdcElmts, hsize_t rdccNElmts, hsize_t rdccNBytes, double rdccW0, hsize_t sieveBuf, hsize_t metaBlockSize)throw(KEAIOException)
    {
        H5::Exception::dontPrint();
        
        H5::H5File *keaImgH5File = NULL;
        try 
        {
            H5::FileAccPropList keaAccessPlist = H5::FileAccPropList(H5::FileAccPropList::DEFAULT);
            keaAccessPlist.setCache(mdcElmts, rdccNElmts, rdccNBytes, rdccW0);
            keaAccessPlist.setSieveBufSize(sieveBuf);
            keaAccessPlist.setMetaBlockSize(metaBlockSize);
            
            const H5std_string keaImgFilePath(fileName);
            keaImgH5File = new H5::H5File(keaImgFilePath, H5F_ACC_RDWR, H5::FileCreatPropList::DEFAULT, keaAccessPlist);
            
        } 
        catch (KEAIOException &e) 
        {
            throw e;
        }
        catch( H5::FileIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( H5::DataSetIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( H5::DataSpaceIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( H5::DataTypeIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return keaImgH5File;
    }
    
    H5::H5File* KEAImageIO::openKeaH5RDOnly(std::string fileName, int mdcElmts, hsize_t rdccNElmts, hsize_t rdccNBytes, double rdccW0, hsize_t sieveBuf, hsize_t metaBlockSize)throw(KEAIOException)
    {
        H5::Exception::dontPrint();
        
        H5::H5File *keaImgH5File = NULL;
        try 
        {
            H5::FileAccPropList keaAccessPlist = H5::FileAccPropList(H5::FileAccPropList::DEFAULT);
            keaAccessPlist.setCache(mdcElmts, rdccNElmts, rdccNBytes, rdccW0);
            keaAccessPlist.setSieveBufSize(sieveBuf);
            keaAccessPlist.setMetaBlockSize(metaBlockSize);
            
            const H5std_string keaImgFilePath(fileName);
            keaImgH5File = new H5::H5File(keaImgFilePath, H5F_ACC_RDONLY, H5::FileCreatPropList::DEFAULT, keaAccessPlist);
            
        }
        catch (KEAIOException &e) 
        {
            throw e;
        }
        catch( H5::FileIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( H5::DataSetIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( H5::DataSpaceIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( H5::DataTypeIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return keaImgH5File;
    }
        
    bool KEAImageIO::isKEAImage(std::string fileName)throw(KEAIOException)
    {
        bool keaImageFound = false;
        H5::Exception::dontPrint();
        
        try 
        {
            H5::H5File *keaImgH5File = NULL;
            const H5std_string keaImgFilePath(fileName);
            keaImgH5File = new H5::H5File(keaImgFilePath, H5F_ACC_RDONLY);
            
            try 
            {
                hid_t nativeFTStrType;
                H5::DataSet datasetFileType = keaImgH5File->openDataSet( KEA_DATASETNAME_HEADER_FILETYPE );
                H5::DataType strFTDataType = datasetFileType.getDataType();
                char **strFTData = new char*[1];
                if((nativeFTStrType=H5Tget_native_type(strFTDataType.getId(), H5T_DIR_DEFAULT))<0)
                {
                    throw KEAIOException("Could not define a native string type");
                }
                datasetFileType.read((void*)strFTData, strFTDataType);
                std::string fileType = std::string(strFTData[0]);
                free(strFTData[0]);
                delete[] strFTData;
                datasetFileType.close();
                                
                try 
                {
                    if(fileType == "KEA")
                    {
                        hid_t nativeVerStrType;
                        H5::DataSet datasetFileVersion = keaImgH5File->openDataSet( KEA_DATASETNAME_HEADER_VERSION );
                        H5::DataType strVerDataType = datasetFileVersion.getDataType();
                        char **strVerData = new char*[1];
                        if((nativeVerStrType=H5Tget_native_type(strVerDataType.getId(), H5T_DIR_DEFAULT))<0)
                        {
                            throw KEAIOException("Could not define a native string type");
                        }
                        datasetFileVersion.read((void*)strVerData, strVerDataType);
                        std::string fileVersion = std::string(strVerData[0]);
                        free(strVerData[0]);
                        delete[] strVerData;
                        datasetFileVersion.close();
                        
                        if((fileVersion == "1.0") || (fileVersion == "1.1"))
                        {
                            keaImageFound = true;
                        }
                        else
                        {
                            keaImageFound = false;
                        }
                    }
                    else
                    {
                        keaImageFound = false;
                    }
                } 
                catch ( H5::Exception &e) // WILL BE THROWN IF THE FILE VERSION DATASET IS NOT PRESENT
                {
                    keaImageFound = false;
                }
                
            } 
            catch ( H5::Exception &e) // WILL BE THROWN IF THE FILE TYPE DATASET IS NOT PRESENT
            {
                keaImageFound = false;
            }
            
            keaImgH5File->close();
            delete keaImgH5File;
        } 
        catch( H5::Exception &e ) // WILL BE THROWN WHEN THE HDF LIBRARY CANNOT OPEN THE FILE - IE IT IS NOT A HDF5 FILE!
		{
			keaImageFound = false;
		}
        catch ( KEAIOException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return keaImageFound;
    }

    KEAImageIO::~KEAImageIO()
    {
        
    }

    void KEAImageIO::addImageBand(const KEADataType dataType, const std::string bandDescrip, const uint32_t imageBlockSize, const uint32_t attBlockSize, const uint32_t deflate) throw(KEAIOException)
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }

        const uint32_t xSize = this->spatialInfoFile->xSize;
        const uint32_t ySize = this->spatialInfoFile->ySize;

        // add a new image band to the file
        KEAImageIO::addImageBandToFile(this->keaImgFile, dataType, xSize, ySize, this->numImgBands + 1, bandDescrip, imageBlockSize, attBlockSize, deflate);
        ++this->numImgBands;

        // update the band counter in the file metadata
        KEAImageIO::setNumImgBandsInFileMetadata(this->keaImgFile, this->numImgBands);

        this->keaImgFile->flush(H5F_SCOPE_GLOBAL);
    }

    H5::DataType KEAImageIO::convertDatatypeKeaToH5STD(const KEADataType dataType) throw(KEAIOException)
    {
        H5::DataType h5Datatype = H5::PredType::IEEE_F32LE;
        switch (dataType)
        {
            case kea_8int:
                h5Datatype = H5::PredType::STD_I8LE; break;
            case kea_16int:
                h5Datatype = H5::PredType::STD_I16LE; break;
            case kea_32int:
                h5Datatype = H5::PredType::STD_I32LE; break;
            case kea_64int:
                h5Datatype = H5::PredType::STD_I64LE; break;
            case kea_8uint:
                h5Datatype = H5::PredType::STD_U8LE; break;
            case kea_16uint:
                h5Datatype = H5::PredType::STD_U16LE; break;
            case kea_32uint:
                h5Datatype = H5::PredType::STD_U32LE; break;
            case kea_64uint:
                h5Datatype = H5::PredType::STD_U64LE; break;
            case kea_32float:
                h5Datatype = H5::PredType::IEEE_F32LE; break;
            case kea_64float:
                h5Datatype = H5::PredType::IEEE_F64LE; break;
            default:
                throw KEAIOException("The specified data type was not recognised.");
        }
        return h5Datatype;
    }

    H5::DataType KEAImageIO::convertDatatypeKeaToH5Native(const KEADataType dataType) throw(KEAIOException)
    {
        H5::DataType h5Datatype = H5::PredType::NATIVE_FLOAT;
        switch (dataType)
        {
            case kea_8int:
                h5Datatype = H5::PredType::NATIVE_INT8; break;
            case kea_16int:
                h5Datatype = H5::PredType::NATIVE_INT16; break;
            case kea_32int:
                h5Datatype = H5::PredType::NATIVE_INT32; break;
            case kea_64int:
                h5Datatype = H5::PredType::NATIVE_INT64; break;
            case kea_8uint:
                h5Datatype = H5::PredType::NATIVE_UINT8; break;
            case kea_16uint:
                h5Datatype = H5::PredType::NATIVE_UINT16; break;
            case kea_32uint:
                h5Datatype = H5::PredType::NATIVE_UINT32; break;
            case kea_64uint:
                h5Datatype = H5::PredType::NATIVE_UINT64; break;
            case kea_32float:
                h5Datatype = H5::PredType::NATIVE_FLOAT; break;
            case kea_64float:
                h5Datatype = H5::PredType::NATIVE_DOUBLE; break;
            default:
                throw KEAIOException("The specified data type was not recognised.");
        }
        return h5Datatype;
    }

    void KEAImageIO::addImageBandToFile(H5::H5File *keaImgH5File, const KEADataType dataType, const uint32_t xSize,   const uint32_t ySize, const uint32_t bandIndex, std::string bandDescrip, const uint32_t imageBlockSize, const uint32_t attBlockSize,  const uint32_t deflate) throw(KEAIOException)
    {
        int initFillVal = 0;

        // Find the smallest axis of the image.
        uint64_t minImgDim = xSize < ySize ? xSize : ySize; 
        uint32_t blockSize2Use = imageBlockSize > minImgDim ? minImgDim : imageBlockSize;

        try
        {
            hsize_t dimsImageBandChunk[] = { blockSize2Use, blockSize2Use };
            H5::DSetCreatPropList initParamsImgBand;
            initParamsImgBand.setChunk(2, dimsImageBandChunk);			
            initParamsImgBand.setShuffle();
            initParamsImgBand.setDeflate(deflate);
            initParamsImgBand.setFillValue( H5::PredType::NATIVE_INT, &initFillVal);

            H5::StrType strdatatypeLen6(H5::PredType::C_S1, 6);
            H5::StrType strdatatypeLen4(H5::PredType::C_S1, 4);
            const H5std_string strClassVal ("IMAGE");
            const H5std_string strImgVerVal ("1.2");
            H5::DataSpace attr_dataspace = H5::DataSpace(H5S_SCALAR);

            uint32_t bandType = kea_continuous;
            uint32_t bandUsage = kea_generic;

            // CREATE IMAGE BAND HDF5 GROUP
            std::string bandName = KEA_DATASETNAME_BAND + uint2Str(bandIndex);
            keaImgH5File->createGroup( bandName );

            // CREATE THE IMAGE DATA ARRAY
            H5::DataType imgBandDT = convertDatatypeKeaToH5STD(dataType);
            hsize_t imageBandDims[] = { ySize, xSize };
            H5::DataSpace imgBandDataSpace(2, imageBandDims);
            H5::DataSet imgBandDataSet = keaImgH5File->createDataSet((bandName+KEA_BANDNAME_DATA), imgBandDT, imgBandDataSpace, initParamsImgBand);
            H5::Attribute classAttribute = imgBandDataSet.createAttribute(KEA_ATTRIBUTENAME_CLASS, strdatatypeLen6, attr_dataspace);
            classAttribute.write(strdatatypeLen6, strClassVal); 
            classAttribute.close();

            H5::Attribute imgVerAttribute = imgBandDataSet.createAttribute(KEA_ATTRIBUTENAME_IMAGE_VERSION, strdatatypeLen4, attr_dataspace);
            imgVerAttribute.write(strdatatypeLen4, strImgVerVal);
            imgVerAttribute.close();

            H5::Attribute blockSizeAttribute = imgBandDataSet.createAttribute(KEA_ATTRIBUTENAME_BLOCK_SIZE, H5::PredType::STD_U16LE, attr_dataspace);
            blockSizeAttribute.write(H5::PredType::NATIVE_UINT32, &blockSize2Use);
            blockSizeAttribute.close();
            imgBandDataSet.close();
            imgBandDataSpace.close();

            // SET BAND NAME / DESCRIPTION
            if (bandDescrip == "")
            {
                bandDescrip = "Band " + uint2Str(bandIndex);
            }

            hsize_t	dimsForStr[] = { 1 }; // number of lines;
            H5::DataSpace dataspaceStrAll(1, dimsForStr);
            H5::StrType strTypeAll(0, H5T_VARIABLE);
            H5::DataSet datasetBandDescription = keaImgH5File->createDataSet((bandName+KEA_BANDNAME_DESCRIP), strTypeAll, dataspaceStrAll);
            const char **wStrdata = new const char*[1];
            wStrdata[0] = bandDescrip.c_str();			
            datasetBandDescription.write((void*)wStrdata, strTypeAll);
            datasetBandDescription.close();
            dataspaceStrAll.close();
            delete[] wStrdata;

            // SET IMAGE BAND DATA TYPE IN IMAGE BAND
            hsize_t dimsDT[] = { 1 };
            H5::DataSpace dtDataSpace(1, dimsDT);
            H5::DataSet dtDataset = keaImgH5File->createDataSet( (bandName+KEA_BANDNAME_DT), H5::PredType::STD_U16LE, dtDataSpace);
            dtDataset.write( &dataType, H5::PredType::NATIVE_UINT );
            dtDataset.close();
            dtDataSpace.close();

            // SET IMAGE BAND TYPE IN IMAGE BAND (I.E., CONTINUOUS (0) OR
            // THEMATIC (1))
            hsize_t dimsType[] = { 1 };
            H5::DataSpace typeDataSpace(1, dimsType);
            H5::DataSet typeDataset = keaImgH5File->createDataSet((bandName+KEA_BANDNAME_TYPE), H5::PredType::STD_U8LE, typeDataSpace);
            typeDataset.write( &bandType, H5::PredType::NATIVE_UINT32 );
            typeDataset.close();
            typeDataSpace.close();

            // SET IMAGE BAND USAGE IN IMAGE BAND
            hsize_t dimsUsage[] = { 1 };
            H5::DataSpace usageDataSpace(1, dimsUsage);
            H5::DataSet usageDataset = keaImgH5File->createDataSet((bandName+KEA_BANDNAME_USAGE), H5::PredType::STD_U8LE, usageDataSpace);
            usageDataset.write( &bandUsage, H5::PredType::NATIVE_UINT32 );
            usageDataset.close();
            usageDataSpace.close();

            // CREATE META-DATA
            keaImgH5File->createGroup( bandName+KEA_BANDNAME_METADATA );

            // CREATE OVERVIEWS GROUP
            keaImgH5File->createGroup( bandName+KEA_BANDNAME_OVERVIEWS );

            // CREATE ATTRIBUTE TABLE GROUP
            keaImgH5File->createGroup( bandName+KEA_BANDNAME_ATT );
            keaImgH5File->createGroup( bandName+KEA_ATT_GROUPNAME_DATA );
            keaImgH5File->createGroup( bandName+KEA_ATT_GROUPNAME_NEIGHBOURS );
            keaImgH5File->createGroup( bandName+KEA_ATT_GROUPNAME_HEADER );

            // SET ATTRIBUTE TABLE CHUNK SIZE
            int attChunkSize = 0;
            hsize_t dimsAttChunkSize[] = { 1 };
            H5::DataSpace attChunkSizeDataSpace(1, dimsAttChunkSize);
            H5::DataSet attChunkSizeDataset = keaImgH5File->createDataSet((bandName+KEA_ATT_CHUNKSIZE_HEADER), H5::PredType::STD_U64LE, attChunkSizeDataSpace);
            attChunkSizeDataset.write( &attChunkSize, H5::PredType::NATIVE_INT);
            attChunkSizeDataset.close();
            attChunkSizeDataSpace.close();

            // SET ATTRIBUTE TABLE SIZE
            int attSize[] = { 0, 0, 0, 0, 0 };
            hsize_t dimsAttSize[] = { 5 };
            H5::DataSpace attSizeDataSpace(1, dimsAttSize);
            H5::DataSet attSizeDataset = keaImgH5File->createDataSet((bandName+KEA_ATT_SIZE_HEADER), H5::PredType::STD_U64LE, attSizeDataSpace);
            attSizeDataset.write( attSize, H5::PredType::NATIVE_INT );
            attSizeDataset.close();
            attSizeDataSpace.close();

            attr_dataspace.close();
        }
        catch (H5::FileIException &e)
        {
            throw KEAIOException(e.getCDetailMsg());
        }
        catch (H5::DataSetIException &e)
        {
            throw KEAIOException(e.getCDetailMsg());
        }
        catch (H5::DataSpaceIException &e)
        {
            throw KEAIOException(e.getCDetailMsg());
        }
        catch (H5::DataTypeIException &e)
        {
            throw KEAIOException(e.getCDetailMsg());
        }
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }

    void KEAImageIO::setNumImgBandsInFileMetadata(H5::H5File *keaImgH5File, const uint32_t numImgBands) throw(KEAIOException)
    {
        try
        {
            H5::DataSet numBandsDataset;
            try
            {
                // open the dataset
                numBandsDataset = keaImgH5File->openDataSet(KEA_DATASETNAME_HEADER_NUMBANDS);
            }
            catch (H5::Exception &e)
            {
                // create the dataset if it does not exist
                hsize_t dimsNumBands[] = { 1 };
                H5::DataSpace numBandsDataSpace(1, dimsNumBands);
                numBandsDataset = keaImgH5File->createDataSet(KEA_DATASETNAME_HEADER_NUMBANDS, H5::PredType::STD_U16LE, numBandsDataSpace);
                numBandsDataSpace.close();
            }
            numBandsDataset.write(&numImgBands, H5::PredType::NATIVE_UINT32);
            numBandsDataset.close();
        }
        catch (H5::Exception &e)
        {
            throw KEAIOException("Could not write the number of bands to the file metadata.");
        }
        catch ( KEAIOException &e)
        {
            throw e;
        }
        catch ( std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }
    
    
    H5::CompType* KEAImageIO::createGCPCompTypeDisk() throw(KEAIOException)
    {
        try
        {
            H5::StrType strTypeDisk(0, H5T_VARIABLE);
            
            H5::CompType *gcpDataType = new H5::CompType( sizeof(KEAImageGCP_HDF5) );
            gcpDataType->insertMember(KEA_GCPS_PSZID, HOFFSET(KEAImageGCP_HDF5, pszId), strTypeDisk);
            gcpDataType->insertMember(KEA_GCPS_PSZINFO, HOFFSET(KEAImageGCP_HDF5, pszInfo), strTypeDisk);
            gcpDataType->insertMember(KEA_GCPS_DFPIXEL, HOFFSET(KEAImageGCP_HDF5, dfGCPPixel), H5::PredType::IEEE_F64LE);
            gcpDataType->insertMember(KEA_GCPS_DFLINE, HOFFSET(KEAImageGCP_HDF5, dfGCPLine), H5::PredType::IEEE_F64LE);            
            gcpDataType->insertMember(KEA_GCPS_DFX, HOFFSET(KEAImageGCP_HDF5, dfGCPX), H5::PredType::IEEE_F64LE);
            gcpDataType->insertMember(KEA_GCPS_DFY, HOFFSET(KEAImageGCP_HDF5, dfGCPY), H5::PredType::IEEE_F64LE);
            gcpDataType->insertMember(KEA_GCPS_DFZ, HOFFSET(KEAImageGCP_HDF5, dfGCPZ), H5::PredType::IEEE_F64LE);
            return gcpDataType;
        }
        catch( H5::FileIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
        catch( H5::DataSetIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
        catch( H5::DataSpaceIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
        catch( H5::DataTypeIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
    }
    
    H5::CompType* KEAImageIO::createGCPCompTypeMem() throw(KEAIOException)
    {
        try
        {
            H5::StrType strTypeMem(0, H5T_VARIABLE);
            
            H5::CompType *gcpDataType = new H5::CompType( sizeof(KEAImageGCP_HDF5) );
            gcpDataType->insertMember(KEA_GCPS_PSZID, HOFFSET(KEAImageGCP_HDF5, pszId), strTypeMem);
            gcpDataType->insertMember(KEA_GCPS_PSZINFO, HOFFSET(KEAImageGCP_HDF5, pszInfo), strTypeMem);
            gcpDataType->insertMember(KEA_GCPS_DFPIXEL, HOFFSET(KEAImageGCP_HDF5, dfGCPPixel), H5::PredType::NATIVE_DOUBLE);
            gcpDataType->insertMember(KEA_GCPS_DFLINE, HOFFSET(KEAImageGCP_HDF5, dfGCPLine), H5::PredType::NATIVE_DOUBLE);
            gcpDataType->insertMember(KEA_GCPS_DFX, HOFFSET(KEAImageGCP_HDF5, dfGCPX), H5::PredType::NATIVE_DOUBLE);
            gcpDataType->insertMember(KEA_GCPS_DFY, HOFFSET(KEAImageGCP_HDF5, dfGCPY), H5::PredType::NATIVE_DOUBLE);
            gcpDataType->insertMember(KEA_GCPS_DFZ, HOFFSET(KEAImageGCP_HDF5, dfGCPZ), H5::PredType::NATIVE_DOUBLE);
            return gcpDataType;
        }
        catch( H5::FileIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
        catch( H5::DataSetIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
        catch( H5::DataSpaceIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
        catch( H5::DataTypeIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
    }
    /*
    H5::CompType* KEAImageIO::createKeaStringCompTypeDisk() throw(KEAIOException)
    {
        try
        {
            H5::StrType strTypeDisk(0, H5T_VARIABLE);
            
            H5::CompType *keaStrDataType = new H5::CompType( sizeof(KEAString) );
            keaStrDataType->insertMember(KEA_ATT_STRING_FIELD, HOFFSET(KEAString, str), strTypeDisk);
            return keaStrDataType;
        }
        catch( H5::FileIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
        catch( H5::DataSetIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
        catch( H5::DataSpaceIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
        catch( H5::DataTypeIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
    }
    
    H5::CompType* KEAImageIO::createKeaStringCompTypeMem() throw(KEAIOException)
    {
        try
        {
            H5::StrType strTypeMem(0, H5T_VARIABLE);
            
            H5::CompType *keaStrDataType = new H5::CompType( sizeof(KEAString) );
            keaStrDataType->insertMember(KEA_ATT_STRING_FIELD, HOFFSET(KEAString, str), strTypeMem);
            return keaStrDataType;
        }
        catch( H5::FileIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
        catch( H5::DataSetIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
        catch( H5::DataSpaceIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
        catch( H5::DataTypeIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
    }
     */

} // namespace libkea

#include "libkea/kea-config.h"
// C function for determining library version
double get_kealibversion()
{
    return double(LIBKEA_VERSION_MAJOR) + double(LIBKEA_VERSION_MINOR) / 10.0 + 
        double(LIBKEA_VERSION_PATCH) / 100.0;
}



