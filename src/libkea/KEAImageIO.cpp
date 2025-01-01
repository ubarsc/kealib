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

    static void* kealibmalloc(size_t nSize, void* ignored)
    {
        return malloc(nSize);
    }

    static void kealibfree(void* ptr, void* ignored)
    {
        free(ptr);
    }

    KEAImageIO::KEAImageIO()
    {
        this->fileOpen = false;
    }
    /*
    std::string KEAImageIO::readString(H5::DataSet& dataset, H5::DataType strDataType)
    {
        hid_t nativeVerStrType;
        if((nativeVerStrType=H5Tget_native_type(strDataType.getId(), H5T_DIR_DEFAULT))<0)
        {
            throw KEAIOException("Could not define a native string type");
        }
        H5::DSetMemXferPropList xfer;
        // Ensures that malloc()/free() are from the same C runtime
        xfer.setVlenMemManager(kealibmalloc, nullptr, kealibfree, nullptr);
        char* strData[1];
        dataset.read((void*)strData, strDataType, H5::DataSpace::ALL, H5::DataSpace::ALL, xfer);
        std::string ret = strData[0];
        free(strData[0]);
        H5Tclose(nativeVerStrType);
        return ret;
    }
    */
    
    void KEAImageIO::openKEAImageHeader(HighFive::File *keaImgH5File)
    {
        try 
        {
            kealib::kea_lock lock(*this->m_mutex); 
            //KEAStackPrintState printState;
            
            this->keaImgFile = keaImgH5File;
            this->spatialInfoFile = new KEAImageSpatialInfo();

            // READ KEA File Type - Check it is a KEA file.
            std::string fileType = "";
            if (keaImgH5File->exist(KEA_DATASETNAME_HEADER_FILETYPE))
            {
                auto datasetFileType = keaImgH5File->getDataSet(KEA_DATASETNAME_HEADER_FILETYPE);
                datasetFileType.read(fileType);
            }
            else
            {
                throw KEAIOException("The KEA file type dataset was not found in the file.");
            }
            if (fileType != "KEA")
            {
                throw KEAIOException("The input file cannot be identified as a KEA file.");
            }

            // READ KEA VERSION NUMBER
            if (keaImgH5File->exist(KEA_DATASETNAME_HEADER_VERSION))
            {
                auto datasetFileVersion = keaImgH5File->getDataSet(KEA_DATASETNAME_HEADER_VERSION);
                datasetFileVersion.read(this->keaVersion);
            }
            else
            {
                throw KEAIOException("The KEA file format version number was not specified.");
            }

            // READ NUMBER OF IMAGE BANDS
            if (keaImgH5File->exist(KEA_DATASETNAME_HEADER_NUMBANDS))
            {
                auto datasetNumImgBands = keaImgH5File->getDataSet(KEA_DATASETNAME_HEADER_NUMBANDS);
                datasetNumImgBands.read(this->numImgBands);
            }
            else
            {
                throw KEAIOException("The number of image bands was not specified.");
            }

            // READ TL COORDINATES
            if (keaImgH5File->exist(KEA_DATASETNAME_HEADER_TL))
            {
                std::vector<double> tlCoordsSpatial(2);
                auto datasetSpatialTL = keaImgH5File->getDataSet(KEA_DATASETNAME_HEADER_TL);
                datasetSpatialTL.read(tlCoordsSpatial);
                this->spatialInfoFile->tlX = tlCoordsSpatial[0];
                this->spatialInfoFile->tlY = tlCoordsSpatial[1];
            }
            else
            {
                throw KEAIOException("The TL coordinate is not specified.");
            }
            std::cout << "TL: " << this->spatialInfoFile->tlX << ", " << this->spatialInfoFile->tlY << std::endl;

            
            // READ RESOLUTION
            if (keaImgH5File->exist(KEA_DATASETNAME_HEADER_RES))
            {
                std::vector<float> pxlResSpatial(2);
                auto spatialResDataset = keaImgH5File->getDataSet(KEA_DATASETNAME_HEADER_RES);
                spatialResDataset.read(pxlResSpatial);
                this->spatialInfoFile->xRes = pxlResSpatial[0];
                this->spatialInfoFile->yRes = pxlResSpatial[1];
            }
            else
            {
                throw KEAIOException("The pixel resolution was not specified.");
            }
            std::cout << "Res: " << this->spatialInfoFile->xRes << ", " << this->spatialInfoFile->yRes << std::endl;

            
            // READ ROTATION
            if (keaImgH5File->exist(KEA_DATASETNAME_HEADER_ROT))
            {
                std::vector<float> pxlRotSpatial(2);
                auto spatialRotDataset = keaImgH5File->getDataSet(KEA_DATASETNAME_HEADER_ROT);
                spatialRotDataset.read(pxlRotSpatial);
                this->spatialInfoFile->xRot = pxlRotSpatial[0];
                this->spatialInfoFile->yRot = pxlRotSpatial[1];
            }
            else
            {
                throw KEAIOException("The image rotation was not specified.");
            }
            std::cout << "Rot: " << this->spatialInfoFile->xRot << ", " << this->spatialInfoFile->yRot << std::endl;


            
            // READ IMAGE SIZE
            if (keaImgH5File->exist(KEA_DATASETNAME_HEADER_SIZE))
            {
                std::vector<uint64_t> imgSizeSpatial(2);
                auto spatialRotDataset = keaImgH5File->getDataSet(KEA_DATASETNAME_HEADER_SIZE);
                spatialRotDataset.read(imgSizeSpatial);
                this->spatialInfoFile->xSize = imgSizeSpatial[0];
                this->spatialInfoFile->ySize = imgSizeSpatial[1];
            }
            else
            {
                throw KEAIOException("The image size was not specified.");
            }
            
            // READ WKT STRING
            if (keaImgH5File->exist(KEA_DATASETNAME_HEADER_WKT))
            {
                auto spatialRotDataset = keaImgH5File->getDataSet(KEA_DATASETNAME_HEADER_WKT);
                spatialRotDataset.read(this->spatialInfoFile->wktString);
            }
            else
            {
                throw KEAIOException("The spatial reference (WKT String) was not specified.");
            }
            std::cout << "WKT: " << this->spatialInfoFile->wktString << std::endl;
        }
        catch ( const KEAIOException &e)
        {
            throw e;
        }
        catch ( const HighFive::Exception &e)
        {
            throw KEAIOException(e.what());
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }

        this->fileOpen = true;
    }
    
    void KEAImageIO::writeImageBlock2Band(uint32_t band, void *data, uint64_t xPxlOff, uint64_t yPxlOff, uint64_t xSizeOut, uint64_t ySizeOut, uint64_t xSizeBuf, uint64_t ySizeBuf, KEADataType inDataType)
    {
        kealib::kea_lock lock(*this->m_mutex); 

        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }

        auto imgBandDT = convertDatatypeKeaToH5Native(inDataType);
        
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

            // OPEN BAND DATASET AND WRITE IMAGE DATA
            std::string imageBandPath = KEA_DATASETNAME_BAND + uint2Str(band);
            if (this->keaImgFile->exist(imageBandPath + KEA_BANDNAME_DATA))
            {
                auto imgBandDataset = this->keaImgFile->getDataSet(imageBandPath + KEA_BANDNAME_DATA);
                std::vector<size_t> imgDataDims = imgBandDataset.getDimensions();
                std::cout << "imgDataDims: " << imgDataDims[0] << ", " << imgDataDims[1] << std::endl;

                if ((this->spatialInfoFile->ySize != ySizeBuf) || (this->spatialInfoFile->xSize != xSizeBuf))
                {
                    std::cout << "Writing subset of band " << band << " to image." << std::endl;
                    std::cout << "xSizeBuf: " << xSizeBuf << ", ySizeBuf: " << ySizeBuf << std::endl;
                    std::cout << "xSizeOut: " << xSizeOut << ", ySizeOut: " << ySizeOut << std::endl;

                    std::vector<size_t> startOffset = {yPxlOff, xPxlOff};
                    std::vector<size_t> bufSize = {ySizeBuf, xSizeBuf};
                    imgBandDataset.select(startOffset, bufSize).write_raw(data, imgBandDT);
                }
                else
                {
                    std::cout << "Writing band " << band << " to image." << std::endl;
                    std::cout << "xSizeBuf: " << xSizeBuf << ", ySizeBuf: " << ySizeBuf << std::endl;
                    std::cout << "xSizeOut: " << xSizeOut << ", ySizeOut: " << ySizeOut << std::endl;

                    imgBandDataset.write_raw(data, imgBandDT);
                }

                // Flushing the dataset
                this->keaImgFile->flush();
                std::cout << "Band " << band << " flushed to image." << std::endl;
            }
            else
            {
                throw KEAIOException("Band image dataset does not exist.");
            }
        }
        catch(const KEAIOException &e)
        {
            throw e;
        }
		catch( const HighFive::Exception &e )
		{
			throw KEAIOException(e.what());
		}
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }
    
    void KEAImageIO::readImageBlock2Band(uint32_t band, void *data, uint64_t xPxlOff, uint64_t yPxlOff, uint64_t xSizeIn, uint64_t ySizeIn, uint64_t xSizeBuf, uint64_t ySizeBuf, KEADataType inDataType)
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
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
            catch ( const H5::Exception &e)
            {
                throw KEAIOException("Could not read image data.");
            }            
        }
        catch(const KEAIOException &e)
        {
            throw e;
        }
        catch( const H5::FileIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( const H5::DataSetIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( const H5::DataSpaceIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( const H5::DataTypeIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        */
    }
  
    
    
    void KEAImageIO::createMask(uint32_t band, uint32_t deflate)
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
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
        */
    }
    
    void KEAImageIO::writeImageBlock2BandMask(uint32_t band, void *data, uint64_t xPxlOff, uint64_t yPxlOff, uint64_t xSizeOut, uint64_t ySizeOut, uint64_t xSizeBuf, uint64_t ySizeBuf, KEADataType inDataType)
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
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
            catch ( const H5::Exception &e)
            {
                throw KEAIOException("Could not write image data.");
            }
        }
        catch(const KEAIOException &e)
        {
            throw e;
        }
        catch( const H5::FileIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( const H5::DataSetIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( const H5::DataSpaceIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( const H5::DataTypeIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        */
    }
    
    void KEAImageIO::readImageBlock2BandMask(uint32_t band, void *data, uint64_t xPxlOff, uint64_t yPxlOff, uint64_t xSizeIn, uint64_t ySizeIn, uint64_t xSizeBuf, uint64_t ySizeBuf, KEADataType inDataType)
    {
        /*
        kealib::kea_lock lock(*this->m_mutex);
 
        KEAStackPrintState printState;
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
            catch ( const H5::Exception &e)
            {
                throw KEAIOException("Could not read image data.");
            }
        }
        catch(const KEAIOException &e)
        {
            throw e;
        }
        catch( const H5::FileIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( const H5::DataSetIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( const H5::DataSpaceIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( const H5::DataTypeIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        */
    }
    
    bool KEAImageIO::maskCreated(uint32_t band)
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;

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
        catch (const H5::Exception &e)
        {
            maskPresent = false;
        }
        
        return maskPresent;
        */
    }
    
    
    void KEAImageIO::setImageMetaData(const std::string &name, const std::string &value)
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
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
            catch (const HighFive::File &e)
            {
                hsize_t	dimsForStr[1];
                dimsForStr[0] = 1; // number of lines;
                H5::DataSpace dataspaceStrAll(1, dimsForStr);
                datasetMetaData = this->keaImgFile->createDataSet(metaDataH5Path, strTypeAll, dataspaceStrAll);

            }
            // WRITE DATA INTO THE DATASET
            const char *wStrdata[1];
            wStrdata[0] = value.c_str();			
            datasetMetaData.write((void*)wStrdata, strTypeAll);
            datasetMetaData.close();
            
            this->keaImgFile->flush(H5F_SCOPE_GLOBAL);
        }
        catch (const H5::Exception &e) 
        {
            throw KEAIOException("Could not set image meta-data.");
        }
        catch ( const KEAIOException &e)
        {
            throw e;
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        */
    }
    
    std::string KEAImageIO::getImageMetaData(const std::string &name)
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        std::string metaDataH5Path = KEA_DATASETNAME_METADATA + std::string("/") + name;
        std::string value = "";
        // READ IMAGE META-DATA
        try 
        {
            H5::DataSet datasetMetaData = this->keaImgFile->openDataSet( metaDataH5Path );
            H5::DataType strDataType = datasetMetaData.getDataType();
            value = readString(datasetMetaData, strDataType);
            datasetMetaData.close();
        } 
        catch ( const H5::Exception &e) 
        {
            throw KEAIOException("Meta-data variable was not accessable.");
        }
        catch ( const KEAIOException &e)
        {
            throw e;
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return value;
        */
    }
    
    std::vector<std::string> KEAImageIO::getImageMetaDataNames()
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
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
        catch (const H5::Exception &e)
        {
            throw KEAIOException("Could not retrieve image meta data.");
        }
        catch ( const KEAIOException &e)
        {
            throw e;
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return metaDataNames;
        */
    }
    
    std::vector< std::pair<std::string, std::string> > KEAImageIO::getImageMetaData()
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
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
        catch (const KEAIOException &e)
        {
            throw e;
        }
        catch (const H5::Exception &e)
        {
            throw KEAIOException("Could not retrieve image meta data.");
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return metaData;
        */
    }
    
    void KEAImageIO::setImageMetaData(const std::vector< std::pair<std::string, std::string> > &data)
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        try 
        {
            for(auto iterMetaData = data.begin(); iterMetaData != data.end(); ++iterMetaData)
            {
                this->setImageMetaData(iterMetaData->first, iterMetaData->second);
            }
            
            this->keaImgFile->flush(H5F_SCOPE_GLOBAL);
        }
        catch (const H5::Exception &e)
        {
            throw KEAIOException("Could not set image band meta data.");
        }
        catch ( const KEAIOException &e)
        {
            throw e;
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        */
    }
    
    void KEAImageIO::setImageBandMetaData(uint32_t band, const std::string &name, const std::string &value)
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
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
            catch (const H5::Exception &e)
            {
                hsize_t	dimsForStr[1];
                dimsForStr[0] = 1; // number of lines;
                H5::DataSpace dataspaceStrAll(1, dimsForStr);
                datasetMetaData = this->keaImgFile->createDataSet(metaDataH5Path, strTypeAll, dataspaceStrAll);
                
            }
            // WRITE DATA INTO THE DATASET
            const char *wStrdata[1];
            wStrdata[0] = value.c_str();			
            datasetMetaData.write((void*)wStrdata, strTypeAll);
            datasetMetaData.close();
            
            this->keaImgFile->flush(H5F_SCOPE_GLOBAL);
        }
        catch (const H5::Exception &e) 
        {
            throw KEAIOException("Could not set image band meta-data.");
        }
        catch ( const KEAIOException &e)
        {
            throw e;
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        */
    }
    
    std::string KEAImageIO::getImageBandMetaData(uint32_t band, const std::string &name)
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        std::string metaDataH5Path = KEA_DATASETNAME_BAND + uint2Str(band) + KEA_BANDNAME_METADATA + std::string("/") + name;
        std::string value = "";
        // READ IMAGE BAND META-DATA
        try 
        {
            H5::DataSet datasetMetaData = this->keaImgFile->openDataSet( metaDataH5Path );
            H5::DataType strDataType = datasetMetaData.getDataType();
            value = readString(datasetMetaData, strDataType);
            datasetMetaData.close();
        } 
        catch ( const H5::Exception &e) 
        {
            throw KEAIOException("Meta-data variable was not accessable.");
        }
        catch ( const KEAIOException &e)
        {
            throw e;
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return value;
        */
    }
    
    std::vector<std::string> KEAImageIO::getImageBandMetaDataNames(uint32_t band)
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
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
        catch (const H5::Exception &e)
        {
            throw KEAIOException("Could not retrieve image band meta data.");
        }
        catch ( const KEAIOException &e)
        {
            throw e;
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return metaDataNames;
        */
    }
    
    std::vector< std::pair<std::string, std::string> > KEAImageIO::getImageBandMetaData(uint32_t band)
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
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
        catch (const H5::Exception &e)
        {
            throw KEAIOException("Could not retrieve image band meta data.");
        }
        catch ( const KEAIOException &e)
        {
            throw e;
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return metaData;
        */
    }
    
    void KEAImageIO::setImageBandMetaData(uint32_t band, const std::vector< std::pair<std::string, std::string> > &data)
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        try 
        {
            for(auto iterMetaData = data.begin(); iterMetaData != data.end(); ++iterMetaData)
            {
                this->setImageBandMetaData(band, iterMetaData->first, iterMetaData->second);
            }
            
            this->keaImgFile->flush(H5F_SCOPE_GLOBAL);
        }
        catch (const H5::Exception &e)
        {
            throw KEAIOException("Could not set image band meta data.");
        }
        catch ( const KEAIOException &e)
        {
            throw e;
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        */
    }
    
    void KEAImageIO::setImageBandDescription(uint32_t band, const std::string &description)
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
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
            const char *wStrdata[1];
            wStrdata[0] = description.c_str();			
            datasetBandDescription.write((void*)wStrdata, strTypeAll);
            datasetBandDescription.close();
            this->keaImgFile->flush(H5F_SCOPE_GLOBAL);
        }
        catch (const H5::Exception &e) 
        {
            throw KEAIOException("Could not set image band description.");
        }
        catch ( const KEAIOException &e)
        {
            throw e;
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        */
    }
    
    std::string KEAImageIO::getImageBandDescription(uint32_t band)
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        std::string bandName = KEA_DATASETNAME_BAND + uint2Str(band) + std::string("/") + KEA_BANDNAME_DESCRIP;
        std::string description = "";
        // READ IMAGE BAND DESCRIPTION
        try 
        {
            H5::DataSet datasetBandDescription = this->keaImgFile->openDataSet( bandName );
            H5::DataType strDataType = datasetBandDescription.getDataType();
            description = readString(datasetBandDescription, strDataType);
            datasetBandDescription.close();
        } 
        catch ( const H5::Exception &e) 
        {
            throw KEAIOException("Could not read band description.");
        }
        catch ( const KEAIOException &e)
        {
            throw e;
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return description;
        */
    }
    
    
    
    void KEAImageIO::setNoDataValue(uint32_t band, const void *data, KEADataType inDataType)
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
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
            catch (const H5::Exception &e)
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
            catch (const H5::Exception &e)
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
        catch ( const H5::Exception &e) 
        {
            throw KEAIOException("The image data type was not specified.");
        }
        catch ( const KEAIOException &e)
        {
            throw e;
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        */
    }
    
    void KEAImageIO::getNoDataValue(uint32_t band, void *data, KEADataType inDataType)
    {
        /*
        kealib::kea_lock lock(*this->m_mutex);
        KEAStackPrintState printState;
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
            catch ( const H5::Exception &e)
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
        catch ( const H5::Exception &e) 
        {
            throw KEAIOException("The image band no data value was not specified.");
        }
        catch ( const KEAIOException &e)
        {
            throw e;
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        */
    }
    
    void KEAImageIO::undefineNoDataValue(uint32_t band)
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
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
            catch ( const H5::Exception &e)
            {
                H5::DataSpace attr_dataspace = H5::DataSpace(H5S_SCALAR);
                H5::Attribute noDataDefAttribute = datasetImgNDV.createAttribute(KEA_NODATA_DEFINED, H5::PredType::STD_I8LE, attr_dataspace);
                int val = 0;
                noDataDefAttribute.write(H5::PredType::NATIVE_INT, &val);
                noDataDefAttribute.close();
            }
            
            datasetImgNDV.close();
        }
        catch ( const H5::Exception &e)
        {
            throw KEAIOException("The image band no data value had not been created.");
        }
        catch ( const KEAIOException &e)
        {
            throw e;
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        */
    }
    
    std::vector<KEAImageGCP*>* KEAImageIO::getGCPs()
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
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
            catch (const H5::Exception &e)
            {
                throw KEAIOException("Could not read the number of GCPs.");
            }
            catch ( const KEAIOException &e)
            {
                throw e;
            }
            catch ( const std::exception &e)
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
                H5::DSetMemXferPropList xfer;
                // Ensures that malloc()/free() are from the same C runtime
                xfer.setVlenMemManager(kealibmalloc, nullptr, kealibfree, nullptr);
                gcpsDataset.read(gcpsHDF, *fieldDtMem, gcpsMemspace, gcpsDataspace, xfer);
                
                gcpsDataset.close();
                gcpsDataspace.close();
                gcpsMemspace.close();
                
                delete fieldDtMem;
            }
            catch( const H5::Exception &e )
            {
                throw KEAIOException(e.getDetailMsg());
            }
            
            KEAImageGCP *tmpGCP = nullptr;
            for(uint32_t i = 0; i < numGCPs; ++i)
            {
                tmpGCP = new KEAImageGCP();
                tmpGCP->pszId = std::string(gcpsHDF[i].pszId);
                free(gcpsHDF[i].pszId);
                tmpGCP->pszInfo = std::string(gcpsHDF[i].pszInfo);
                free(gcpsHDF[i].pszInfo);
                tmpGCP->dfGCPLine = gcpsHDF[i].dfGCPLine;
                tmpGCP->dfGCPPixel = gcpsHDF[i].dfGCPPixel;
                tmpGCP->dfGCPX = gcpsHDF[i].dfGCPX;
                tmpGCP->dfGCPY = gcpsHDF[i].dfGCPY;
                tmpGCP->dfGCPZ = gcpsHDF[i].dfGCPZ;
                gcps->push_back(tmpGCP);
            }
            
            delete[] gcpsHDF;
        }
        catch (const H5::Exception &e)
        {
            throw KEAIOException(e.getCDetailMsg());
        }
        catch ( const KEAIOException &e)
        {
            throw e;
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return gcps;
        */
    }
    
    void KEAImageIO::setGCPs(std::vector<KEAImageGCP*> *gcps, const std::string &projWKT)
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        uint32_t numGCPs = gcps->size();
        
        KEAImageGCP_HDF5 *gcpsHDF = new KEAImageGCP_HDF5[numGCPs];
        
        uint32_t i = 0;
        for(auto iterGCP = gcps->begin(); iterGCP != gcps->end(); ++iterGCP)
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
                
                delete fieldDtMem;
            }
            catch ( const H5::Exception &e)
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
                
                delete fieldDtDisk;
                delete fieldDtMem;
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
                catch (const H5::Exception &e)
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
            catch (const H5::Exception &e)
            {
                throw KEAIOException("Could not write the number of GCPs.");
            }
            catch ( const KEAIOException &e)
            {
                throw e;
            }
            catch ( const std::exception &e)
            {
                throw KEAIOException(e.what());
            }
        }
        catch (const H5::Exception &e)
        {
            throw KEAIOException(e.getCDetailMsg());
        }
        catch ( const KEAIOException &e)
        {
            throw e;
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        for(uint32_t j = 0; j < i; j++)
        {
            delete[] gcpsHDF[j].pszId;
            delete[] gcpsHDF[j].pszInfo;
        }
        
        delete[] gcpsHDF;
        
        
        // Set the projection string
        try
        {
            this->setGCPProjection(projWKT);
        }
        catch ( const KEAIOException &e)
        {
            throw e;
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        */
    }
    
    uint32_t KEAImageIO::getGCPCount()
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
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
        catch ( const H5::Exception &e)
        {
            throw KEAIOException("The number of image bands was not specified.");
        }
        
        return numGCPs;
        */
    }
    
    std::string KEAImageIO::getGCPProjection()
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        // READ WKT STRING
        std::string gcpProj = "";
        try
        {
            H5::DataSet datasetGCPSpatialReference = this->keaImgFile->openDataSet( KEA_GCPS_PROJ );
            H5::DataType strDataType = datasetGCPSpatialReference.getDataType();
            gcpProj = readString(datasetGCPSpatialReference, strDataType);
            datasetGCPSpatialReference.close();
        }
        catch ( const H5::Exception &e)
        {
            throw KEAIOException("The spatial reference was not specified.");
        }
        
        return gcpProj;
        */
    }
    
    void KEAImageIO::setGCPProjection(const std::string &projWKT)
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        // SET THE WKT STRING SPATAIL REFERENCE IN GLOBAL HEADER
        try
        {
            const char *wStrdata[1];
            H5::DataSet datasetSpatialReference = this->keaImgFile->openDataSet(KEA_GCPS_PROJ);
            H5::DataType strDataType = datasetSpatialReference.getDataType();
            wStrdata[0] = projWKT.c_str();
            datasetSpatialReference.write((void*)wStrdata, strDataType);
            datasetSpatialReference.close();
            this->keaImgFile->flush(H5F_SCOPE_GLOBAL);
        }
        catch (const H5::Exception &e)
        {
            const char *wStrdata[1];
			hsize_t	dimsForStr[1];
			dimsForStr[0] = 1; // number of lines;
            H5::DataSpace dataspaceStrAll(1, dimsForStr);
            H5::StrType strTypeAll(0, H5T_VARIABLE);
            H5::DataSet datasetSpatialReference = this->keaImgFile->createDataSet(KEA_GCPS_PROJ, strTypeAll, dataspaceStrAll);
			wStrdata[0] = projWKT.c_str();
			datasetSpatialReference.write((void*)wStrdata, strTypeAll);
			datasetSpatialReference.close();
        }
        catch ( const KEAIOException &e)
        {
            throw e;
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        */
    }
    
    void KEAImageIO::setSpatialInfo(KEAImageSpatialInfo *inSpatialInfo)
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        try 
        {
            // SET X AND Y TL IN GLOBAL HEADER
            double doubleVals[2];
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
            
            // SET THE WKT STRING SPATAIL REFERENCE IN GLOBAL HEADER
			const char *wStrdata[1];
            H5::DataSet datasetSpatialReference = this->keaImgFile->openDataSet(KEA_DATASETNAME_HEADER_WKT);
            H5::DataType strDataType = datasetSpatialReference.getDataType();
			wStrdata[0] = inSpatialInfo->wktString.c_str();			
			datasetSpatialReference.write((void*)wStrdata, strDataType);
			datasetSpatialReference.close();
            
            this->keaImgFile->flush(H5F_SCOPE_GLOBAL);
        } 
        catch (const H5::Exception &e)
        {
            throw KEAIOException(e.getCDetailMsg());
        }
        catch ( const KEAIOException &e)
        {
            throw e;
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        */
    }
    
    KEAImageSpatialInfo* KEAImageIO::getSpatialInfo()
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        return spatialInfoFile;
    }
    
    uint32_t KEAImageIO::getNumOfImageBands()
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        return this->numImgBands;
    }
    
    uint32_t KEAImageIO::getImageBlockSize(uint32_t band)
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
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
            catch ( const H5::Exception &e) 
            {
                throw KEAIOException("Could not get image block size.");
            }            
        }
        catch(const KEAIOException &e)
        {
            throw e;
        }
        catch( const H5::FileIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( const H5::DataSetIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( const H5::DataSpaceIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( const H5::DataTypeIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return imgBlockSize;
        */
    }

    uint32_t KEAImageIO::getAttributeTableChunkSize(uint32_t band)
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        uint32_t attChunkSize = 0;
        
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
                hsize_t dimsValue[1];
                dimsValue[0] = 1;
                H5::DataSpace valueDataSpace(1, dimsValue);
                std::string imageBandPath = KEA_DATASETNAME_BAND + uint2Str(band);
                H5::DataSet datasetAttSize = this->keaImgFile->openDataSet( imageBandPath + KEA_ATT_CHUNKSIZE_HEADER);
                datasetAttSize.read(&attChunkSize, H5::PredType::NATIVE_UINT32, valueDataSpace);
                datasetAttSize.close();
                valueDataSpace.close();
            } 
            catch ( const H5::Exception &e) 
            {
                throw KEAIOException("Could not get image block size.");
            }            
        }
        catch(const KEAIOException &e)
        {
            throw e;
        }
        catch( const H5::FileIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( const H5::DataSetIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( const H5::DataSpaceIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( const H5::DataTypeIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return attChunkSize;
        */
    }
    
    KEADataType KEAImageIO::getImageBandDataType(uint32_t band)
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
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
        catch ( const H5::Exception &e) 
        {
            throw KEAIOException("The image band data type was not specified.");
        }
        catch ( const KEAIOException &e)
        {
            throw e;
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return imgDataType;
        */
    }
    
    std::string KEAImageIO::getKEAImageVersion() 
    {
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        return this->keaVersion;
    }
    
    void KEAImageIO::setImageBandLayerType(uint32_t band, KEALayerType imgLayerType)
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
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
        catch ( const H5::Exception &e) 
        {
            throw KEAIOException("The image band data type was not specified.");
        }
        catch ( const KEAIOException &e)
        {
            throw e;
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        */
    }
    
    KEALayerType KEAImageIO::getImageBandLayerType(uint32_t band)
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
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
        catch ( const H5::Exception &e) 
        {
            throw KEAIOException("The image band data type was not specified.");
        }
        catch ( const KEAIOException &e)
        {
            throw e;
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return imgLayerType;
        */
    }
    
    void KEAImageIO::setImageBandClrInterp(uint32_t band, KEABandClrInterp imgLayerClrInterp)
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
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
        catch ( const H5::Exception &e) 
        {
            hsize_t dimsUsage[1];
            dimsUsage[0] = 1;
            H5::DataSpace usageDataSpace(1, dimsUsage);
            H5::DataSet usageDataset = this->keaImgFile->createDataSet((KEA_DATASETNAME_BAND + uint2Str(band) + KEA_BANDNAME_USAGE), H5::PredType::STD_U8LE, usageDataSpace);
            usageDataset.write( &value, H5::PredType::NATIVE_UINT32 );
            usageDataset.close();
        }
        catch ( const KEAIOException &e)
        {
            throw e;
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        */
    }
    
    KEABandClrInterp KEAImageIO::getImageBandClrInterp(uint32_t band)
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
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
        catch ( const H5::Exception &e) 
        {
            //throw KEAIOException("The image band data type was not specified.");
            imgLayerClrInterp = kea_generic; // Field was not present within the file.
        }
        catch ( const KEAIOException &e)
        {
            throw e;
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return imgLayerClrInterp;
        */
    }
    
    void KEAImageIO::createOverview(uint32_t band, uint32_t overview, uint64_t xSize, uint64_t ySize)
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
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
        catch (const H5::Exception &e)
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
        catch (const H5::Exception &e)
        {
            throw KEAIOException(e.getCDetailMsg());
        }
        catch ( const KEAIOException &e)
        {
            throw e;
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        */
    }
    
    void KEAImageIO::removeOverview(uint32_t band, uint32_t overview)
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
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
        catch (const H5::Exception &e)
        {
            // Do nothing as dataset does not exist.
        }
        catch ( const KEAIOException &e)
        {
            throw e;
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        */
    }
    
    uint32_t KEAImageIO::getOverviewBlockSize(uint32_t band, uint32_t overview)
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
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
            catch ( const H5::Exception &e) 
            {
                throw KEAIOException("Could not retrieve the overview block size.");
            }            
        }
        catch(const KEAIOException &e)
        {
            throw e;
        }
        catch( const H5::FileIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( const H5::DataSetIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( const H5::DataSpaceIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( const H5::DataTypeIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return ovBlockSize;
        */
    }
    
    void KEAImageIO::writeToOverview(uint32_t band, uint32_t overview, void *data, uint64_t xPxlOff, uint64_t yPxlOff, uint64_t xSizeOut, uint64_t ySizeOut, uint64_t xSizeBuf, uint64_t ySizeBuf, KEADataType inDataType)
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
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
            catch ( const H5::Exception &e) 
            {
                throw KEAIOException("Could not write image data.");
            }
            
            this->keaImgFile->flush(H5F_SCOPE_GLOBAL);
        }
        catch(const KEAIOException &e)
        {
            throw e;
        }
        catch( const H5::FileIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( const H5::DataSetIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( const H5::DataSpaceIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( const H5::DataTypeIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        */
    }
    
    void KEAImageIO::readFromOverview(uint32_t band, uint32_t overview, void *data, uint64_t xPxlOff, uint64_t yPxlOff, uint64_t xSizeIn, uint64_t ySizeIn, uint64_t xSizeBuf, uint64_t ySizeBuf, KEADataType inDataType)
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
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
            catch ( const H5::Exception &e) 
            {
                throw KEAIOException("Could not read from image overview.");
            }            
        }
        catch(const KEAIOException &e)
        {
            throw e;
        }
        catch( const H5::FileIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( const H5::DataSetIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( const H5::DataSpaceIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
		catch( const H5::DataTypeIException &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        */
    }
    
    uint32_t KEAImageIO::getNumOfOverviews(uint32_t band)
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
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
        catch (const H5::Exception &e)
        {
            throw KEAIOException("Could not retrieve the number of image band overviews.");
        }
        catch ( const KEAIOException &e)
        {
            throw e;
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return numOverviews;
        */
    }
    
    void KEAImageIO::getOverviewSize(uint32_t band, uint32_t overview, uint64_t *xSize, uint64_t *ySize)
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
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
                hsize_t dims[2];
                imgBandDataspace.getSimpleExtentDims(dims);
                
                *xSize = dims[1];
                *ySize = dims[0];

                imgBandDataset.close();
            } 
            catch(const KEAIOException &e)
            {
                throw e;
            }
            catch ( const H5::Exception &e) 
            {
                throw KEAIOException("Could not read from image overview.");
            }            
        }
        catch( const H5::Exception &e )
		{
			throw KEAIOException("Could not get the overview size.");
		}
        catch ( const KEAIOException &e)
        {
            throw e;
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        */
    }
    
    KEAAttributeTable* KEAImageIO::getAttributeTable(KEAATTType type, uint32_t band)
    {
        /*
        KEAAttributeTable *att = nullptr;
        try 
        {
            if(type == kea_att_mem)
            {
                att = kealib::KEAAttributeTableInMem::createKeaAtt(this->keaImgFile, this->m_mutex, band);
            }
            else if(type == kea_att_file)
            {
                att = kealib::KEAAttributeTableFile::createKeaAtt(this->keaImgFile, this->m_mutex, band);
            }
            else
            {
                throw KEAATTException("The attribute table type was not recognised.");
            }
        }
        catch(const KEAATTException &e)
        {
            throw e;
        }
        catch ( const KEAIOException &e)
        {
            throw e;
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return att;
        */
    }
    
    void KEAImageIO::setAttributeTable(KEAAttributeTable* att, uint32_t band, uint32_t chunkSize, uint32_t deflate)
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        try 
        {
            att->exportToKeaFile(this->keaImgFile, band, chunkSize, deflate);
            this->keaImgFile->flush(H5F_SCOPE_GLOBAL);
        }
        catch(const KEAATTException &e)
        {
            throw e;
        }
        catch ( const KEAIOException &e)
        {
            throw e;
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        */
    }
    
    bool KEAImageIO::attributeTablePresent(uint32_t band)
    {
        /*
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        bool attPresent = false;
        try 
        {
            std::string bandPathBase = KEA_DATASETNAME_BAND + uint2Str(band);
            hsize_t attSize[5];
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
            catch (const H5::Exception &e) 
            {
                throw KEAIOException("The attribute table size field is not present.");
            }
            
            if(attSize[0] > 0)
            {
                attPresent = true;
            }
        }
        catch(const KEAIOException &e)
        {
            throw e;
        }
        catch(const KEAATTException &e)
        {
            throw e;
        }
        catch( const H5::Exception &e )
		{
			throw KEAIOException(e.getCDetailMsg());
		}
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        return attPresent;
        */
    }
    
    void KEAImageIO::close()
    {
        kealib::kea_lock lock(*this->m_mutex); 
        if (this->fileOpen)
        {
            try
            {
                delete this->spatialInfoFile;
                this->keaImgFile->flush();
                delete this->keaImgFile;
                this->keaImgFile = nullptr;
                this->fileOpen = false;
            }
            catch(const KEAIOException &e)
            {
                throw e;
            }
            catch( const HighFive::Exception &e )
            {
                throw KEAIOException(e.what());
            }
            catch ( const std::exception &e)
            {
                throw KEAIOException(e.what());
            }
        }
    }

    HighFive::File *KEAImageIO::createKEAImage(
        const std::string &fileName, KEADataType dataType, uint32_t xSize,
        uint32_t ySize, uint32_t numImgBands, std::vector<std::string> *bandDescrips,
        KEAImageSpatialInfo *spatialInfo, uint32_t imageBlockSize,
        uint32_t attBlockSize, int mdcElmts, hsize_t rdccNElmts, hsize_t rdccNBytes,
        double rdccW0, hsize_t sieveBuf, hsize_t metaBlockSize, uint32_t deflate
    )
    {
        //KEAStackPrintState printState;

        HighFive::File *keaImgH5File = nullptr;

        // Define dataspaces for writing string data
        auto scalar_dataspace = HighFive::DataSpace(HighFive::DataSpace::dataspace_scalar);
        auto var_stringtype = HighFive::VariableLengthStringType();

        try
        {
            auto keaFileAccessProps = HighFive::FileAccessProps::Default();
            //keaAccessPlist.setCache(mdcElmts, rdccNElmts, rdccNBytes, rdccW0);
            //keaAccessPlist.setSieveBufSize(sieveBuf);
            //keaAccessPlist.setMetaBlockSize(metaBlockSize);

            keaImgH5File = new HighFive::File( fileName, HighFive::File::Truncate | HighFive::File::Create | HighFive::File::ReadWrite, keaFileAccessProps);

            //////////// CREATE GLOBAL HEADER ////////////////
            keaImgH5File->createGroup( KEA_DATASETNAME_HEADER );

            bool deleteSpatialInfo = false;
            if(spatialInfo == nullptr)
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
                std::cout << "Creating new spatial info object." << std::endl;
            }

            spatialInfo->xSize = xSize;
            spatialInfo->ySize = ySize;

            // SET NUMBER OF IMAGE BANDS IN GLOBAL HEADER
            KEAImageIO::setNumImgBandsInFileMetadata(keaImgH5File, numImgBands);

            // SET X AND Y TL IN GLOBAL HEADER
            std::vector<double> tlCoordsSpatial(2);
            tlCoordsSpatial[0] = double(spatialInfo->tlX);
            tlCoordsSpatial[1] = double(spatialInfo->tlY);
            auto spatialTLDataset = keaImgH5File->createDataSet<double>(KEA_DATASETNAME_HEADER_TL, HighFive::DataSpace::From(tlCoordsSpatial)); // Explicit datatype: IEEE_F64LE
            spatialTLDataset.write(tlCoordsSpatial);

            // SET X AND Y RESOLUTION IN GLOBAL HEADER
            std::vector<float> pxlResSpatial(2);
            pxlResSpatial[0] = float(spatialInfo->xRes);
            pxlResSpatial[1] = float(spatialInfo->yRes);
            auto spatialResDataset = keaImgH5File->createDataSet<float>(KEA_DATASETNAME_HEADER_RES, HighFive::DataSpace::From(pxlResSpatial)); // Explicit datatype: IEEE_F32LE
            spatialResDataset.write(pxlResSpatial);

            // SET X AND Y ROTATION IN GLOBAL HEADER
            std::vector<float> pxlRotSpatial(2);
            pxlRotSpatial[0] = float(spatialInfo->xRot);
            pxlRotSpatial[1] = float(spatialInfo->yRot);
            auto spatialRotDataset = keaImgH5File->createDataSet<float>(KEA_DATASETNAME_HEADER_ROT, HighFive::DataSpace::From(pxlRotSpatial)); // Explicit datatype: IEEE_F32LE
            spatialRotDataset.write(pxlRotSpatial);

            // SET NUMBER OF X AND Y PIXELS
            std::vector<uint64_t> imgSizeSpatial(2);
            imgSizeSpatial[0] = uint64_t(spatialInfo->xSize);
            imgSizeSpatial[1] = uint64_t(spatialInfo->ySize);
            auto spatialSizeDataset = keaImgH5File->createDataSet<uint64_t>(KEA_DATASETNAME_HEADER_SIZE, HighFive::DataSpace::From(imgSizeSpatial)); // Explicit datatype: UINT64
            spatialSizeDataset.write(imgSizeSpatial);

            // SET THE WKT STRING SPATIAL REFERENCE IN GLOBAL HEADER
            auto datasetSpatialReference = keaImgH5File->createDataSet(KEA_DATASETNAME_HEADER_WKT, scalar_dataspace, var_stringtype);
            datasetSpatialReference.write(spatialInfo->wktString);

            // SET THE FILE TYPE IN GLOBAL HEADER
            auto datasetFileType = keaImgH5File->createDataSet(KEA_DATASETNAME_HEADER_FILETYPE, scalar_dataspace, var_stringtype);
            datasetFileType.write(KEA_FILE_TYPE);

            // SET THE FILE GENARATOR IN GLOBAL HEADER
            auto datasetGenarator = keaImgH5File->createDataSet(KEA_DATASETNAME_HEADER_GENERATOR, scalar_dataspace, var_stringtype);
            datasetGenarator.write(KEA_SOFTWARE);

            // SET THE FILE VERSION IN GLOBAL HEADER
            auto datasetVersion = keaImgH5File->createDataSet(KEA_DATASETNAME_HEADER_VERSION, scalar_dataspace, var_stringtype);
            datasetVersion.write(KEA_VERSION);

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
                if (bandDescrips != nullptr && i < bandDescrips->size()) {
                    bandDescription = bandDescrips->at(i);
                }

                addImageBandToFile(keaImgH5File, dataType, xSize, ySize,
                        i+1, bandDescription, imageBlockSize, attBlockSize,
                        deflate);
            }
            //////////// CREATED IMAGE BANDS ////////////////

            keaImgH5File->flush();
        }
        catch (HighFive::Exception &e) {
            throw KEAIOException(e.what());
        }
        catch (const KEAIOException &e)
        {
            throw e;
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }

        return keaImgH5File;
    }
    
    HighFive::File* KEAImageIO::openKeaH5RW(const std::string &fileName, int mdcElmts, hsize_t rdccNElmts, hsize_t rdccNBytes, double rdccW0, hsize_t sieveBuf, hsize_t metaBlockSize)
    {
        //KEAStackPrintState printState;
        
        HighFive::File *keaImgH5File = nullptr;
        try 
        {
            auto keaFileAccessProps = HighFive::FileAccessProps::Default();
            //keaAccessPlist.setCache(mdcElmts, rdccNElmts, rdccNBytes, rdccW0);
            //keaAccessPlist.setSieveBufSize(sieveBuf);
            //keaAccessPlist.setMetaBlockSize(metaBlockSize);

            keaImgH5File = new HighFive::File( fileName, HighFive::File::ReadWrite, keaFileAccessProps);

        } 
        catch (const KEAIOException &e) 
        {
            throw e;
        }
        catch (HighFive::Exception &e) {
            throw KEAIOException(e.what());
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }

        return keaImgH5File;
    }
    
    HighFive::File* KEAImageIO::openKeaH5RDOnly(const std::string &fileName, int mdcElmts, hsize_t rdccNElmts, hsize_t rdccNBytes, double rdccW0, hsize_t sieveBuf, hsize_t metaBlockSize)
    {
        //KEAStackPrintState printState;
        
        HighFive::File *keaImgH5File = nullptr;

        try 
        {
            auto keaFileAccessProps = HighFive::FileAccessProps::Default();
            //keaAccessPlist.setCache(mdcElmts, rdccNElmts, rdccNBytes, rdccW0);
            //keaAccessPlist.setSieveBufSize(sieveBuf);
            //keaAccessPlist.setMetaBlockSize(metaBlockSize);

            keaImgH5File = new HighFive::File( fileName, HighFive::File::ReadOnly, keaFileAccessProps);

        }
        catch (const KEAIOException &e) 
        {
            throw e;
        }
        catch (HighFive::Exception &e) {
            throw KEAIOException(e.what());
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }

        return keaImgH5File;
    }
        
    bool KEAImageIO::isKEAImage(const std::string &fileName)
    {
        bool keaImageFound = false;
        //KEAStackPrintState printState;
        try 
        {
            auto keaImgH5File = HighFive::File( fileName, HighFive::File::ReadOnly);

            try 
            {
                std::string fileType = "";
                if (keaImgH5File.exist(KEA_DATASETNAME_HEADER_FILETYPE))
                {
                    auto datasetFileType = keaImgH5File.getDataSet(KEA_DATASETNAME_HEADER_FILETYPE);
                    datasetFileType.read(fileType);
                }
                                
                try 
                {
                    if(fileType == "KEA")
                    {
                        std::string fileVersion = "";
                        if (keaImgH5File.exist(KEA_DATASETNAME_HEADER_VERSION))
                        {
                            auto datasetFileVersion = keaImgH5File.getDataSet(KEA_DATASETNAME_HEADER_VERSION);
                            datasetFileVersion.read(fileVersion);
                        }
                        
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
                catch (HighFive::Exception &e)
                {
                    keaImageFound = false;
                }
                
            } 
            catch (HighFive::Exception &e)
            {
                keaImageFound = false;
            }
        }
        catch (HighFive::Exception &e)
        {
            keaImageFound = false;
        }
        catch ( const KEAIOException &e)
        {
            throw e;
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }

        return keaImageFound;
    }

    KEAImageIO::~KEAImageIO()
    {
        
    }

    void KEAImageIO::addImageBand(const KEADataType dataType, const std::string &bandDescrip, const uint32_t imageBlockSize, const uint32_t attBlockSize, const uint32_t deflate)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        //KEAStackPrintState printState;
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

        this->keaImgFile->flush();
    }
    
    void KEAImageIO::removeImageBand(const uint32_t bandIndex)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        //KEAStackPrintState printState;
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        KEAImageIO::removeImageBandFromFile(this->keaImgFile, bandIndex, this->numImgBands);
    
        --this->numImgBands;

        // update the band counter in the file metadata
        KEAImageIO::setNumImgBandsInFileMetadata(this->keaImgFile, this->numImgBands);

        this->keaImgFile->flush();
    }

    HighFive::DataType KEAImageIO::convertDatatypeKeaToH5STD(const KEADataType dataType)
    {
        switch (dataType)
        {
            case kea_8int:
                return HighFive::AtomicType<int8_t>();
            case kea_16int:
                return HighFive::AtomicType<int16_t>();
            case kea_32int:
                return HighFive::AtomicType<int32_t>();
            case kea_64int:
                return HighFive::AtomicType<int64_t>();
            case kea_8uint:
                return HighFive::AtomicType<uint8_t>();
            case kea_16uint:
                return HighFive::AtomicType<uint16_t>();
            case kea_32uint:
                return HighFive::AtomicType<uint32_t>();
            case kea_64uint:
                return HighFive::AtomicType<uint64_t>();
            case kea_32float:
                return HighFive::AtomicType<float>();
            case kea_64float:
                return HighFive::AtomicType<double>();
            default:
                throw KEAIOException("The specified data type was not recognised.");
        }
    }

    HighFive::DataType KEAImageIO::convertDatatypeKeaToH5Native(
        const KEADataType dataType
    )
    {
        switch (dataType)
        {
            case kea_8int:
                return HighFive::AtomicType<int8_t>();
            case kea_16int:
                return HighFive::AtomicType<int16_t>();
            case kea_32int:
                return HighFive::AtomicType<int32_t>();
            case kea_64int:
                return HighFive::AtomicType<int64_t>();
            case kea_8uint:
                return HighFive::AtomicType<uint8_t>();
            case kea_16uint:
                return HighFive::AtomicType<uint16_t>();
            case kea_32uint:
                return HighFive::AtomicType<uint32_t>();
            case kea_64uint:
                return HighFive::AtomicType<uint64_t>();
            case kea_32float:
                return HighFive::AtomicType<float>();
            case kea_64float:
                return HighFive::AtomicType<double>();
            default:
                throw KEAIOException("The specified data type was not recognised.");
        }
    }

    const std::string KEAImageIO::convertDatatypeKeaToCStdStr(const KEADataType dataType)
    {
        switch (dataType)
        {
            case kea_8int:
                return "int8_t";
            case kea_16int:
                return "int16_t";
            case kea_32int:
                return "int32_t";
            case kea_64int:
                return "int64_t";
            case kea_8uint:
                return "uint8_t";
            case kea_16uint:
                return "uint16_t";
            case kea_32uint:
                return "uint32_t";
            case kea_64uint:
                return "uint64_t";
            case kea_32float:
                return "float";
            case kea_64float:
                return "double";
            default:
                throw KEAIOException("The specified data type was not recognised.");
        }
    }

    void KEAImageIO::addImageBandToFile(HighFive::File *keaImgH5File, const KEADataType dataType, const uint32_t xSize,   const uint32_t ySize, const uint32_t bandIndex, const std::string &bandDescripIn, const uint32_t imageBlockSize, const uint32_t attBlockSize,  const uint32_t deflate)
    {
        // Define dataspaces for writing string data
        auto scalar_dataspace = HighFive::DataSpace(HighFive::DataSpace::dataspace_scalar);
        auto var_stringtype = HighFive::VariableLengthStringType();
        auto stringType = HighFive::AtomicType<std::string>();

        //int initFillVal = 0;
        std::string bandDescrip = bandDescripIn; // may be updated below

        // Find the smallest axis of the image.
        uint64_t minImgDim = xSize < ySize ? xSize : ySize; 
        uint32_t blockSize2Use = imageBlockSize > minImgDim ? minImgDim : imageBlockSize;

        try
        {
            HighFive::DataSpace dataSpace = HighFive::DataSpace({ ySize, xSize });
            HighFive::DataType dataTypeH5 = convertDatatypeKeaToH5STD(dataType);

            HighFive::DataSetCreateProps imgBandDataSetProps;
            imgBandDataSetProps.add(HighFive::Chunking(blockSize2Use, blockSize2Use));
            imgBandDataSetProps.add(HighFive::Deflate(deflate));
            imgBandDataSetProps.add(HighFive::Shuffle());
            // HOW TO Add a FILL VALUE?

            HighFive::DataSetAccessProps imgBandAccessProps = HighFive::DataSetAccessProps::Default();

            uint32_t bandType = kea_continuous;
            uint32_t bandUsage = kea_generic;

            // CREATE IMAGE BAND HDF5 GROUP
            std::string bandName = KEA_DATASETNAME_BAND + uint2Str(bandIndex);
            keaImgH5File->createGroup( bandName );

            // CREATE THE IMAGE DATA ARRAY
            HighFive::DataSet imgBandDataSet = keaImgH5File->createDataSet((bandName+KEA_BANDNAME_DATA), dataSpace, dataTypeH5, imgBandDataSetProps, imgBandAccessProps);
            imgBandDataSet.createAttribute<char[6]>(KEA_ATTRIBUTENAME_CLASS, "IMAGE");
            imgBandDataSet.createAttribute<char[4]>(KEA_ATTRIBUTENAME_IMAGE_VERSION, "1.2");
            imgBandDataSet.createAttribute<uint16_t>(KEA_ATTRIBUTENAME_BLOCK_SIZE, blockSize2Use);

            // SET BAND NAME / DESCRIPTION
            if (bandDescrip == "")
            {
                bandDescrip = "Band " + uint2Str(bandIndex);
            }
            auto datasetBandDescript = keaImgH5File->createDataSet((bandName+KEA_BANDNAME_DESCRIP), scalar_dataspace, var_stringtype);
            datasetBandDescript.write(bandDescrip);

            // SET IMAGE BAND DATA TYPE IN IMAGE BAND
            auto dtDataset = keaImgH5File->createDataSet<uint16_t>((bandName+KEA_BANDNAME_DT), dataType);
            dtDataset.write(bandType);

            // SET IMAGE BAND TYPE IN IMAGE BAND (I.E., CONTINUOUS (0) OR
            // THEMATIC (1))
            auto typeDataset = keaImgH5File->createDataSet<uint8_t>((bandName+KEA_BANDNAME_TYPE), bandType);
            typeDataset.write(bandType);

            // SET IMAGE BAND USAGE IN IMAGE BAND
            auto usageDataset = keaImgH5File->createDataSet<uint8_t>((bandName+KEA_BANDNAME_USAGE), bandUsage);
            usageDataset.write(bandUsage);

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
            auto attChunkSizeDataset = keaImgH5File->createDataSet<uint64_t>((bandName+KEA_ATT_CHUNKSIZE_HEADER), attBlockSize);
            attChunkSizeDataset.write(attBlockSize);

            // SET ATTRIBUTE TABLE SIZE
            std::vector<uint64_t> attSize = {0, 0, 0, 0, 0};
            auto spatialTLDataset = keaImgH5File->createDataSet<uint64_t>((bandName+KEA_ATT_SIZE_HEADER), HighFive::DataSpace::From(attSize));
            spatialTLDataset.write(attSize);

        }
        catch (const HighFive::DataSetException &e)
        {
            throw KEAIOException(e.what());
        }
        catch (const HighFive::GroupException &e)
        {
            throw KEAIOException(e.what());
        }
        catch (const HighFive::DataSpaceException &e)
        {
            throw KEAIOException(e.what());
        }
        catch (const HighFive::DataTypeException &e)
        {
            throw KEAIOException(e.what());
        }
        catch (const HighFive::Exception &e)
        {
            throw KEAIOException(e.what());
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }
    
    void KEAImageIO::removeImageBandFromFile(HighFive::File *keaImgH5File, const uint32_t bandIndex, const uint32_t numImgBands)
    {
        if( ( bandIndex < 1) || ( bandIndex > numImgBands ) )
        {
            throw KEAIOException("Invalid band index");
        }
    
        try
        {
            std::string bandName = KEA_DATASETNAME_BAND + uint2Str(bandIndex);
            keaImgH5File->unlink(bandName);
            
            // now rename all the ones with band > bandIndex
            for( uint32_t sourceIndex = bandIndex + 1; sourceIndex <= numImgBands; sourceIndex++ )
            {
                std::string srcName = KEA_DATASETNAME_BAND + uint2Str(sourceIndex);
                std::string dstName = KEA_DATASETNAME_BAND + uint2Str(sourceIndex - 1);
                keaImgH5File->rename(srcName, dstName);
            }
        }
        catch (const HighFive::Exception &e)
        {
            throw KEAIOException(e.what());
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }

    void KEAImageIO::setNumImgBandsInFileMetadata(HighFive::File *keaImgH5File, const uint32_t numImgBands)
    {
        try
        {
            if (keaImgH5File->exist(KEA_DATASETNAME_HEADER_NUMBANDS))
            {
                auto numBandsDataset = keaImgH5File->getDataSet(KEA_DATASETNAME_HEADER_NUMBANDS);
                numBandsDataset.write(numImgBands);
            }
            else
            {
                keaImgH5File->createDataSet<uint16_t>(KEA_DATASETNAME_HEADER_NUMBANDS, numImgBands);
            }
        }
        catch (const HighFive::Exception &e)
        {
            throw kealib::KEAIOException(e.what());
        }
        catch ( const KEAIOException &e)
        {
            throw e;
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }
    
    /*
    H5::CompType* KEAImageIO::createGCPCompTypeDisk()
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
        catch( const H5::FileIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
        catch( const H5::DataSetIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
        catch( const H5::DataSpaceIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
        catch( const H5::DataTypeIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }

    }
    
    H5::CompType* KEAImageIO::createGCPCompTypeMem()
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
        catch( const H5::FileIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
        catch( const H5::DataSetIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
        catch( const H5::DataSpaceIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
        catch( const H5::DataTypeIException &e )
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
    return double(LIBKEA_VERSION_MAJOR) + double(LIBKEA_VERSION_MINOR) / 100.0 + 
        double(LIBKEA_VERSION_PATCH) / 10000.0;
}



