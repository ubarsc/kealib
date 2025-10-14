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

HIGHFIVE_REGISTER_TYPE(kealib::KEAImageGCP_HDF5, kealib::KEAImageIO::createGCPCompType)

namespace kealib{

    KEAImageIO::KEAImageIO()
    {
        this->fileOpen = false;
    }

    /**
     * Opens the header of the KEA image file and initializes relevant spatial and metadata attributes.
     *
     * This method takes a pointer to a KEA HDF5 image file and reads the file's header information in
     * order to initialize spatial metadata, such as the number of image bands, top-left coordinates,
     * pixel resolution, image rotation, image size, and the spatial reference system in WKT format.
     * The method also verifies the file type and format version to ensure compatibility.
     *
     * @param keaImgH5File A pointer to the HighFive::File object representing the KEA image file.
     *                     This file must be an open HDF5 file containing a valid KEA image structure.
     *
     * @throws KEAIOException If the file is not a valid KEA file, required datasets are missing,
     *                        or errors occur while reading any of the metadata attributes.
     */
    void KEAImageIO::openKEAImageHeader(HighFive::File *keaImgH5File)
    {
        KEAStackPrintState printState;
        try 
        {
            kealib::kea_lock lock(*this->m_mutex); 

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
            //std::cout << "TL: " << this->spatialInfoFile->tlX << ", " << this->spatialInfoFile->tlY << std::endl;

            
            // READ RESOLUTION
            if (keaImgH5File->exist(KEA_DATASETNAME_HEADER_RES))
            {
                std::vector<double> pxlResSpatial(2);
                auto spatialResDataset = keaImgH5File->getDataSet(KEA_DATASETNAME_HEADER_RES);
                spatialResDataset.read(pxlResSpatial);
                this->spatialInfoFile->xRes = pxlResSpatial[0];
                this->spatialInfoFile->yRes = pxlResSpatial[1];
            }
            else
            {
                throw KEAIOException("The pixel resolution was not specified.");
            }
            //std::cout << "Res: " << this->spatialInfoFile->xRes << ", " << this->spatialInfoFile->yRes << std::endl;

            
            // READ ROTATION
            if (keaImgH5File->exist(KEA_DATASETNAME_HEADER_ROT))
            {
                std::vector<double> pxlRotSpatial(2);
                auto spatialRotDataset = keaImgH5File->getDataSet(KEA_DATASETNAME_HEADER_ROT);
                spatialRotDataset.read(pxlRotSpatial);
                this->spatialInfoFile->xRot = pxlRotSpatial[0];
                this->spatialInfoFile->yRot = pxlRotSpatial[1];
            }
            else
            {
                throw KEAIOException("The image rotation was not specified.");
            }
            //std::cout << "Rot: " << this->spatialInfoFile->xRot << ", " << this->spatialInfoFile->yRot << std::endl;

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
            //std::cout << "WKT: " << this->spatialInfoFile->wktString << std::endl;
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

    void KEAImageIO::writeImageToDataset(HighFive::DataSet &dataset, uint32_t band, 
        void *data, uint64_t xPxlOff, uint64_t yPxlOff, uint64_t xSizeOut,
        uint64_t ySizeOut, uint64_t xSizeBuf, uint64_t ySizeBuf, KEADataType inDataType)
    {
        uint64_t endXPxl = xPxlOff + xSizeOut;
        uint64_t endYPxl = yPxlOff + ySizeOut;
        auto dims = dataset.getDimensions();

        if (xPxlOff > dims[1])
        {
            throw KEAIOException("Start X Pixel is not within image.");
        }

        if (endXPxl > dims[1])
        {
            throw KEAIOException("End X Pixel is not within image.");
        }

        if (yPxlOff > dims[0])
        {
            throw KEAIOException("Start Y Pixel is not within image.");
        }

        if (endYPxl > dims[0])
        {
            throw KEAIOException("End Y Pixel is not within image.");
        }

        // GET NATIVE DATASET
        auto imgBandDT = convertDatatypeKeaToH5Native(inDataType);
        
        // what type of write?
        if((dims[0] != ySizeOut) || (dims[1] != xSizeOut))
        {
            // partial
            /*std::cout << "writing subset of band " << band << " from image." <<
                    std::endl;
            std::cout << "xSizeBuf: " << xSizeBuf << ", ySizeBuf: " << ySizeBuf
                    << std::endl;
            std::cout << "xSizeOut: " << xSizeOut << ", ySizeOut: " << ySizeOut <<
                    std::endl;
            std::cout << "xPxlOff: " << xPxlOff << ", yPxlOff: " << yPxlOff <<
                    std::endl;*/

            // Is this a write off the edge?
			if((ySizeBuf != ySizeOut) || (xSizeBuf != xSizeOut))
			{
			 std::cout << "write off edge" << std::endl;
                // so we need to write a smaller part of image data from a larger buffer
                // This goes beyond what HighFive can currently do so we must resort
                // to the C API. This is a rough port of what happens in the old Kealib.
				HighFive::DataSpace dataSpace = HighFive::DataSpace({ySizeBuf, xSizeBuf});
				
				// So the main trick here is that you can "select" on a dataspace (the buffer)
				// with HDF5, but not HighFive (yet).
				// The below code selects the hyperslab on the dataspace
				hsize_t dataSelectMemDims[2]; // "count"
				dataSelectMemDims[0] = ySizeOut; // all the pixels in the y dimension 
				dataSelectMemDims[1] = 1;  // only 1 "element" in the "x" dimension - see dataSelectBlockSizeDims where the definition of a block is set

				hsize_t dataOffDims[2]; // start
				dataOffDims[0] = 0; // this is 0,0 because it is 
				dataOffDims[1] = 0;

				hsize_t dataSelectStrideDims[2];  // the stride.
				dataSelectStrideDims[0] = 1;      // 1 in the y dimension
				if(xSizeBuf == xSizeOut)
				{
					dataSelectStrideDims[1] = 1;  // we are writing all the way accross, so 1 in the x dimension
				}
				else
				{
					dataSelectStrideDims[1] = xSizeBuf - xSizeOut; // writing to the end of the image, so set the stride for
					                   // each read from the dataspace - just the number of pixels we are writing. 
				}

				hsize_t dataSelectBlockSizeDims[2]; // the definition of what a block is
				dataSelectBlockSizeDims[0] = 1;     // y dimension - 1 element
				dataSelectBlockSizeDims[1] = xSizeOut;  // x dimension - one block is one row of image data - see dataSelectMemDims above
				// now select the hyperslab for the dataspace
				if( H5Sselect_hyperslab(dataSpace.getId(), H5S_SELECT_SET, dataOffDims, 
				        dataSelectStrideDims, dataSelectMemDims, dataSelectBlockSizeDims) < 0 )
				{
					H5Eprint(H5E_DEFAULT, stderr);
					throw KEAIOException("Error in H5Sselect_hyperslab 1");
				}
				
				// now set the hyperslab for the dataset 
				auto imgBandDataspace = dataset.getSpace();
				hsize_t dataOffset[2];  // the offset into the dataset
                dataOffset[0] = yPxlOff;
                dataOffset[1] = xPxlOff;
                hsize_t dataInDims[2];
                dataInDims[0] = ySizeOut;  // the size
                dataInDims[1] = xSizeOut;
                // set the hyperslab
                if( H5Sselect_hyperslab(imgBandDataspace.getId(), H5S_SELECT_SET, dataOffset, NULL, dataInDims, NULL) < 0 )
                {
					H5Eprint(H5E_DEFAULT, stderr);
					throw KEAIOException("Error in H5Sselect_hyperslab 2");
                }
				
				// now do the actual write
                if( H5Dwrite(dataset.getId(), imgBandDT.getId(), dataSpace.getId(), imgBandDataspace.getId(), H5P_DEFAULT, data) < 0 )
                {
					H5Eprint(H5E_DEFAULT, stderr);
					throw KEAIOException("Error in H5Dwrite");
                }
			}
			else
			{
			    // we are doing a partial write but not over the edge of the image
			    // HighFive can handle this no problems
				std::vector<size_t> startOffset = {yPxlOff, xPxlOff};
				std::vector<size_t> bufSize = {ySizeBuf, xSizeBuf};
				dataset.select(startOffset, bufSize).write_raw(
					data,
					imgBandDT
				);

			}
		}
        else
        {
            /*std::cout << "writing band " << band << " from image." << std::endl;
            std::cout << "xSizeBuf: " << xSizeBuf << ", ySizeBuf: " << ySizeBuf
                    << std::endl;
            std::cout << "xSizeOut: " << xSizeOut << ", ySizeOut: " << ySizeOut <<
                    std::endl;
            std::cout << "xPxlOff: " << xPxlOff << ", yPxlOff: " << yPxlOff <<
                    std::endl;*/
            // we are writing the whole image
            dataset.write_raw(data, imgBandDT);
        }
        
    }

    /**
     * Writes a block of image data to a specified band in the KEA image file.
     *
     * This method handles the writing of image data to a specific band while ensuring
     * that the specified pixel offsets, dimensions, and buffer sizes fit within the
     * bounds of the image dimensions. It supports writing subsets or entire blocks of
     * data depending on the provided parameters. Metadata integrity and dataset
     * existence are also verified within this function.
     *
     * @param band The one-based index of the band to which the data will be written. Band 1 is the first band.
     * @param data A pointer to the memory containing the image data to be written.
     * @param xPxlOff The horizontal pixel offset in the image where the data block starts.
     * @param yPxlOff The vertical pixel offset in the image where the data block starts.
     * @param xSizeOut The horizontal size of the output image data block to be written.
     * @param ySizeOut The vertical size of the output image data block to be written.
     * @param xSizeBuf The horizontal size of the provided data buffer.
     * @param ySizeBuf The vertical size of the provided data buffer.
     * @param inDataType The data type of the input image data, specified using KEADataType.
     *
     * @throws KEAIOException If the file is not open, the band index is invalid, pixel offsets are outside the image bounds,
     *                        or the band dataset does not exist. Other errors related to I/O operations or metadata issues
     *                        are also reported via this exception.
     */
    void KEAImageIO::writeImageBlock2Band(
        uint32_t band, void *data, uint64_t xPxlOff, uint64_t yPxlOff,
        uint64_t xSizeOut, uint64_t ySizeOut, uint64_t xSizeBuf, uint64_t ySizeBuf,
        KEADataType inDataType
    )
    {
        kealib::kea_lock lock(*this->m_mutex);
        KEAStackPrintState printState;

        if (!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }

        try
        {
            // CHECK PARAMETERS PROVIDED FIT WITHIN IMAGE
            if (band == 0)
            {
                throw KEAIOException("KEA Image Bands start at 1.");
            }
            else if (band > this->numImgBands)
            {
                throw KEAIOException("Band is not present within image.");
            }


            // OPEN BAND DATASET AND WRITE IMAGE DATA
            std::string imageBandPath = KEA_DATASETNAME_BAND + uint2Str(band) + KEA_BANDNAME_DATA;
            if (this->keaImgFile->exist(imageBandPath))
            {
                auto imgBandDataset = this->keaImgFile->getDataSet(imageBandPath);
                
                writeImageToDataset(imgBandDataset, band, data, xPxlOff, yPxlOff, xSizeOut,
                    ySizeOut, xSizeBuf, ySizeBuf, inDataType);
                // Flushing the dataset
                this->keaImgFile->flush();
            }
            else
            {
                throw KEAIOException("Band image dataset does not exist.");
            }
        }
        catch (const KEAIOException &e)
        {
            throw e;
        }
        catch (const HighFive::Exception &e)
        {
            throw KEAIOException(e.what());
        }
        catch (const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }
    
    void KEAImageIO::readImageFromDataset(const HighFive::DataSet &dataset, uint32_t band, 
        void *data, uint64_t xPxlOff, uint64_t yPxlOff, uint64_t xSizeIn,
        uint64_t ySizeIn, uint64_t xSizeBuf, uint64_t ySizeBuf, KEADataType inDataType, 
        bool ismask)
    {
        uint64_t endXPxl = xPxlOff + xSizeIn;
        uint64_t endYPxl = yPxlOff + ySizeIn;
        auto dims = dataset.getDimensions();

        if (xPxlOff > dims[1])
        {
            throw KEAIOException("Start X Pixel is not within image.");
        }

        if (endXPxl > dims[1])
        {
            throw KEAIOException("End X Pixel is not within image.");
        }

        if (yPxlOff > dims[0])
        {
            throw KEAIOException("Start Y Pixel is not within image.");
        }

        if (endYPxl > dims[0])
        {
            throw KEAIOException("End Y Pixel is not within image.");
        }

        // GET NATIVE DATASET
        auto imgBandDT = convertDatatypeKeaToH5Native(inDataType);
        
        // what type of read?
        if((dims[0] != ySizeBuf) || (dims[1] != xSizeBuf))
        {
            // partial
            /*std::cout << "reading subset of band " << band << " from image." <<
                    std::endl;
            std::cout << "xSizeBuf: " << xSizeBuf << ", ySizeBuf: " << ySizeBuf
                    << std::endl;
            std::cout << "xSizeIn: " << xSizeIn << ", ySizeIn: " << ySizeIn <<
                    std::endl;
            std::cout << "xPxlOff: " << xPxlOff << ", yPxlOff: " << yPxlOff <<
                    std::endl;*/

            // Is this a read off the edge?
			if((ySizeBuf != ySizeIn) || (xSizeBuf != xSizeIn))
			{
                // so we need to read just image data but put it in a larger buffer.
                // This goes beyond what HighFive can currently do so we must resort
                // to the C API. This is a rough port of what happens in the old Kealib.
				HighFive::DataSpace dataSpace = HighFive::DataSpace({ySizeBuf, xSizeBuf});
				
				if(!ismask)
				{
    				// First, work out the ignore value (I don't think old kealib does this)
    				void *pNoData = malloc(imgBandDT.getSize());
    				try
    				{
    					this->getNoDataValue(band, pNoData, inDataType);
    				}
    				catch(const KEAIOException &e)
    				{
    				    // no nodata set
    					free(pNoData);
    					pNoData = NULL;  // H5Dfill defaults to 0 in this case which is the default fill value
    					// for an image dataset
    				}
    				
    				// fill the whole buffer with the no data value so parts that are not read in stay
    				// set to this value.
    				if( H5Dfill(pNoData, imgBandDT.getId(), data, imgBandDT.getId(), dataSpace.getId()) < 0 )
    				{
    					H5Eprint(H5E_DEFAULT, stderr);
    					throw KEAIOException("Error in H5Dfill");
    				}
    				free(pNoData);
                }
                else
                {
                    // is a mask. Fill with 255
                    int fill = FILL_MASK_DATA;
    				if( H5Dfill(&fill, H5T_NATIVE_INT, data, imgBandDT.getId(), dataSpace.getId()) < 0 )
    				{
    					H5Eprint(H5E_DEFAULT, stderr);
    					throw KEAIOException("Error in H5Dfill");
    				}
                }
					
				// So the main trick here is that you can "select" on a dataspace (the buffer)
				// with HDF5, but not HighFive (yet).
				// The below code selects the hyperslab on the dataspace
				hsize_t dataSelectMemDims[2]; // "count"
				dataSelectMemDims[0] = ySizeIn; // all the pixels in the y dimension 
				dataSelectMemDims[1] = 1;  // only 1 "element" in the "x" dimension - see dataSelectBlockSizeDims where the definition of a block is set

				hsize_t dataOffDims[2]; // start
				dataOffDims[0] = 0; // this is 0,0 because it is 
				dataOffDims[1] = 0;

				hsize_t dataSelectStrideDims[2];  // the stride.
				dataSelectStrideDims[0] = 1;      // 1 in the y dimension
				if(xSizeBuf == xSizeIn)
				{
					dataSelectStrideDims[1] = 1;  // we are reading all the way accross, so 1 in the x dimension
				}
				else
				{
					dataSelectStrideDims[1] = xSizeBuf - xSizeIn; // reading to the end of the image, so set the stride for
					                   // each write into the dataspace - just the number of pixels we are reading. The remainder
					                   // will stay the ignore
				}

				hsize_t dataSelectBlockSizeDims[2]; // the definition of what a block is
				dataSelectBlockSizeDims[0] = 1;     // y dimension - 1 element
				dataSelectBlockSizeDims[1] = xSizeIn;  // x dimension - one block is one row of image data - see dataSelectMemDims above
				// now select the hyperslab for the dataspace
				if( H5Sselect_hyperslab(dataSpace.getId(), H5S_SELECT_SET, dataOffDims, 
				        dataSelectStrideDims, dataSelectMemDims, dataSelectBlockSizeDims) < 0 )
				{
					H5Eprint(H5E_DEFAULT, stderr);
					throw KEAIOException("Error in H5Sselect_hyperslab 1");
				}
				
				// now set the hyperslab for the dataset 
				auto imgBandDataspace = dataset.getSpace();
				hsize_t dataOffset[2];  // the offset into the dataset
                dataOffset[0] = yPxlOff;
                dataOffset[1] = xPxlOff;
                hsize_t dataInDims[2];
                dataInDims[0] = ySizeIn;  // the size
                dataInDims[1] = xSizeIn;
                // set the hyperslab
                if( H5Sselect_hyperslab(imgBandDataspace.getId(), H5S_SELECT_SET, dataOffset, NULL, dataInDims, NULL) < 0 )
                {
					H5Eprint(H5E_DEFAULT, stderr);
					throw KEAIOException("Error in H5Sselect_hyperslab 2");
                }
				
				// now do the actual read
                if( H5Dread(dataset.getId(), imgBandDT.getId(), dataSpace.getId(), imgBandDataspace.getId(), H5P_DEFAULT, data) < 0 )
                {
					H5Eprint(H5E_DEFAULT, stderr);
					throw KEAIOException("Error in H5Dread");
                }
			}
			else
			{
			    // we are doing a partial read but not over the edge of the image
			    // HighFive can handle this no problems
				std::vector<size_t> startOffset = {yPxlOff, xPxlOff};
				std::vector<size_t> bufSize = {ySizeBuf, xSizeBuf};
				dataset.select(startOffset, bufSize).read_raw(
					data,
					imgBandDT
				);

			}
		}
        else
        {
            /*std::cout << "reading band " << band << " from image." << std::endl;
            std::cout << "xSizeBuf: " << xSizeBuf << ", ySizeBuf: " << ySizeBuf
                    << std::endl;
            std::cout << "xSizeIn: " << xSizeIn << ", ySizeIn: " << ySizeIn <<
                    std::endl;
            std::cout << "xPxlOff: " << xPxlOff << ", yPxlOff: " << yPxlOff <<
                    std::endl;*/
            // we are reading the whole image
            dataset.read_raw(data, imgBandDT);
        }
        
    }

    /**
     * Reads a block of image data for a specified band from the KEA image file.
     *
     * This method fetches a specified subset of image pixels for a particular band from the
     * associated KEA image dataset and writes it to the provided buffer. The subset is defined
     * by the spatial offsets and sizes. The method ensures the provided input parameters are valid
     * and fall within the image dimensions. If the buffer size does not match the subset size, only
     * the specified region within the image dimensions is processed.
     *
     * @param band       The band number to read from. Band numbers start at 1 and must not exceed
     *                   the total number of bands in the image.
     * @param data       A pointer to the buffer where the image data will be stored after reading.
     *                   The buffer should have enough memory based on the subset size.
     * @param xPxlOff    The horizontal pixel offset in the image from which the subset begins.
     * @param yPxlOff    The vertical pixel offset in the image from which the subset begins.
     * @param xSizeIn    The width of the subset to read, starting from xPxlOff.
     * @param ySizeIn    The height of the subset to read, starting from yPxlOff.
     * @param xSizeBuf   The width of the provided buffer where the data will be stored.
     * @param ySizeBuf   The height of the provided buffer where the data will be stored.
     * @param inDataType The data type of the pixel values specified as a KEADataType.
     *                   This determines how to interpret the data while reading.
     *
     * @throws KEAIOException If the file is not open, if the band does not exist,
     *                        if spatial offsets/sizes are out of bounds,
     *                        or if the dataset does not exist.
     * @throws HighFive::Exception If there are any HDF5-related errors during the dataset operations.
     * @throws std::exception If any other unexpected errors are encountered.
     */
    void KEAImageIO::readImageBlock2Band(
        uint32_t band, void *data, uint64_t xPxlOff, uint64_t yPxlOff, uint64_t xSizeIn,
        uint64_t ySizeIn, uint64_t xSizeBuf, uint64_t ySizeBuf, KEADataType inDataType
    )
    {
        kealib::kea_lock lock(*this->m_mutex);
        KEAStackPrintState printState;
        
        if (!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }

        try
        {
            // CHECK PARAMETERS PROVIDED FIT WITHIN IMAGE
            if (band == 0)
            {
                throw KEAIOException("KEA Image Bands start at 1.");
            }
            else if (band > this->numImgBands)
            {
                throw KEAIOException("Band is not present within image.");
            }

            // OPEN BAND DATASET AND READ IMAGE DATA
            std::string imageBandPath = KEA_DATASETNAME_BAND + uint2Str(band) + KEA_BANDNAME_DATA;
            if (this->keaImgFile->exist(imageBandPath))
            {
                auto imgBandDataset = this->keaImgFile->getDataSet(imageBandPath);
                readImageFromDataset(imgBandDataset, band, data, xPxlOff, yPxlOff, xSizeIn,
                    ySizeIn, xSizeBuf, ySizeBuf, inDataType);            
            }
            else
            {
                throw KEAIOException("Band image dataset does not exist.");
            }
        }
        catch (const KEAIOException &e)
        {
            throw e;
        }
        catch (const HighFive::Exception &e)
        {
            throw KEAIOException(e.what());
        }
        catch (const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }
  
    
    
    void KEAImageIO::createMask(uint32_t band, uint32_t deflate)
    {
        kealib::kea_lock lock(*this->m_mutex);
        KEAStackPrintState printState;
         
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        if(!this->maskCreated(band))
        {
            uint32_t blockSize2Use = getImageBlockSize(band);
            try
            {
                HighFive::DataSpace dataSpace = HighFive::DataSpace({spatialInfoFile->ySize, spatialInfoFile->xSize});
    
                HighFive::DataSetCreateProps imgBandDataSetProps;
                imgBandDataSetProps.add(HighFive::Chunking(blockSize2Use, blockSize2Use));
                imgBandDataSetProps.add(HighFive::Deflate(deflate));
                imgBandDataSetProps.add(HighFive::Shuffle());
                int initFillVal = FILL_MASK_DATA;
                // HighFive doesn't appear to support this (yet)
                if( H5Pset_fill_value(imgBandDataSetProps.getId(), H5T_NATIVE_INT, &initFillVal) < 0 )
                {
    				H5Eprint(H5E_DEFAULT, stderr);
    				throw KEAIOException("Error in H5Pset_fill_value");
                }
            
                HighFive::DataSetAccessProps imgBandAccessProps =
                        HighFive::DataSetAccessProps::Default();
                // CREATE THE IMAGE DATA ARRAY
                std::string imageBandPath = KEA_DATASETNAME_BAND + uint2Str(band);
                HighFive::DataSet imgBandDataSet = keaImgFile->createDataSet(
                    (imageBandPath + KEA_BANDNAME_MASK),
                    dataSpace,
                    HighFive::AtomicType<uint8_t>(),
                    imgBandDataSetProps,
                    imgBandAccessProps
                );
                auto scalar_dataspace = HighFive::DataSpace(
					HighFive::DataSpace::dataspace_scalar
				);
				imgBandDataSet.createAttribute(KEA_ATTRIBUTENAME_CLASS, 
					scalar_dataspace,
					HighFive::FixedLengthStringType(6, HighFive::StringPadding::NullTerminated)).write("IMAGE");
            
				imgBandDataSet.createAttribute(KEA_ATTRIBUTENAME_IMAGE_VERSION,
					scalar_dataspace,
					HighFive::FixedLengthStringType(4, HighFive::StringPadding::NullTerminated)).write("1.2");
                
                this->keaImgFile->flush();
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
            catch (const std::exception &e)
            {
                throw KEAIOException(e.what());
            }
        }
    }
    
    void KEAImageIO::writeImageBlock2BandMask(uint32_t band, void *data, uint64_t xPxlOff, uint64_t yPxlOff, uint64_t xSizeOut, uint64_t ySizeOut, uint64_t xSizeBuf, uint64_t ySizeBuf, KEADataType inDataType)
    {
        kealib::kea_lock lock(*this->m_mutex);
        KEAStackPrintState printState;

        if (!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }

        try
        {
            // CHECK PARAMETERS PROVIDED FIT WITHIN IMAGE
            if (band == 0)
            {
                throw KEAIOException("KEA Image Bands start at 1.");
            }
            else if (band > this->numImgBands)
            {
                throw KEAIOException("Band is not present within image.");
            }


            // OPEN BAND DATASET AND WRITE IMAGE DATA
            std::string imageBandPath = KEA_DATASETNAME_BAND + uint2Str(band) + KEA_BANDNAME_MASK;
            if (this->keaImgFile->exist(imageBandPath))
            {
                auto imgBandDataset = this->keaImgFile->getDataSet(imageBandPath);

                writeImageToDataset(imgBandDataset, band, data, xPxlOff, yPxlOff, xSizeOut,
                    ySizeOut, xSizeBuf, ySizeBuf, inDataType);
                // Flushing the dataset
                this->keaImgFile->flush();
            }
            else
            {
                throw KEAIOException("Band image dataset does not exist.");
            }
        }
        catch (const KEAIOException &e)
        {
            throw e;
        }
        catch (const HighFive::Exception &e)
        {
            throw KEAIOException(e.what());
        }
        catch (const std::exception &e)
        {
            throw KEAIOException(e.what());
        }

    }
    
    void KEAImageIO::readImageBlock2BandMask(uint32_t band, void *data, uint64_t xPxlOff, uint64_t yPxlOff, uint64_t xSizeIn, uint64_t ySizeIn, uint64_t xSizeBuf, uint64_t ySizeBuf, KEADataType inDataType)
    {
        kealib::kea_lock lock(*this->m_mutex);
        KEAStackPrintState printState;
        
        if (!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }

        try
        {
            // CHECK PARAMETERS PROVIDED FIT WITHIN IMAGE
            if (band == 0)
            {
                throw KEAIOException("KEA Image Bands start at 1.");
            }
            else if (band > this->numImgBands)
            {
                throw KEAIOException("Band is not present within image.");
            }

            // OPEN BAND DATASET AND READ IMAGE DATA
            std::string imageMaskBandPath = KEA_DATASETNAME_BAND + uint2Str(band) + KEA_BANDNAME_MASK;
            if (this->keaImgFile->exist(imageMaskBandPath))
            {
                auto imgBandDataset = this->keaImgFile->getDataSet(imageMaskBandPath);
                readImageFromDataset(imgBandDataset, band, data, xPxlOff, yPxlOff, xSizeIn,
                    ySizeIn, xSizeBuf, ySizeBuf, inDataType, true);            
            }
            else
            {
                throw KEAIOException("Band image mask dataset does not exist.");
            }
        }
        catch (const KEAIOException &e)
        {
            throw e;
        }
        catch (const HighFive::Exception &e)
        {
            throw KEAIOException(e.what());
        }
        catch (const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }
    
    bool KEAImageIO::maskCreated(uint32_t band)
    {
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
        try
        {
            std::string imageBandPath = KEA_DATASETNAME_BAND + uint2Str(band);
            if (this->keaImgFile->exist(imageBandPath + KEA_BANDNAME_MASK))
            {
                maskPresent = true;
            }
                
        }
        catch (HighFive::Exception &e)
        {
            maskPresent = false;
        }
        
        return maskPresent;
    }
    
    
    void KEAImageIO::setImageMetaData(const std::string &name, const std::string &value)
    {
        
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
            if(!this->keaImgFile->exist(metaDataH5Path))
            {
          		// TODO: dataspace instead of scalar to be compatible with old KEA
                HighFive::DataSpace dataSpace = HighFive::DataSpace({1});
                keaImgFile->createDataSet(metaDataH5Path, dataSpace, HighFive::VariableLengthStringType());
                std::cout << "created dataset " << metaDataH5Path << std::endl;
            }
            auto dataset = this->keaImgFile->getDataSet(metaDataH5Path);
            dataset.write(value);
        
            // Flushing the dataset
            this->keaImgFile->flush();
        }
        catch (const HighFive::Exception &e) 
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
    }
    
    std::string KEAImageIO::getImageMetaData(const std::string &name)
    {
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
            auto dataset = this->keaImgFile->getDataSet(metaDataH5Path);
            dataset.read(value);
        } 
        catch ( const HighFive::Exception &e) 
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
    }
    
    std::vector<std::string> KEAImageIO::getImageMetaDataNames()
    {
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        try 
        {
            // Try to open dataset with matadata name
            auto group = this->keaImgFile->getGroup(KEA_DATASETNAME_METADATA);
            return group.listObjectNames();
        }
        catch (const HighFive::Exception &e)
        {
            throw KEAIOException("Could not retrieve image meta data.");
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }
    
    std::vector< std::pair<std::string, std::string> > KEAImageIO::getImageMetaData()
    {
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        std::vector< std::pair<std::string, std::string> > metaData;
        
        try 
        {
            auto group = this->keaImgFile->getGroup(KEA_DATASETNAME_METADATA);
            for( size_t i = 0; i < group.getNumberObjects(); i++ )
            {
                std::string name = group.getObjectName(i);
                std::string value = this->getImageMetaData(name);
                metaData.push_back(std::pair<std::string, std::string>(name,value));
            }
        }
        catch (const HighFive::Exception &e)
        {
            throw KEAIOException("Could not retrieve image meta data.");
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return metaData;
    }
    
    void KEAImageIO::setImageMetaData(const std::vector< std::pair<std::string, std::string> > &data)
    {
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
        }
        catch (const HighFive::Exception &e)
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
    }
    
    void KEAImageIO::setImageBandMetaData(uint32_t band, const std::string &name, const std::string &value)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        // FORM META-DATA PATH WITHIN THE H5 FILE 
        std::string metaDataH5Path = KEA_DATASETNAME_BAND + uint2Str(band) + KEA_BANDNAME_METADATA + std::string("/") + name;

        if(!this->keaImgFile->exist(metaDataH5Path))
        {
            HighFive::DataSpace dataSpace = HighFive::DataSpace(1);
            keaImgFile->createDataSet(metaDataH5Path, dataSpace, HighFive::VariableLengthStringType());
            std::cout << "created dataset " << metaDataH5Path << std::endl;
        }
        // WRITE IMAGE META DATA
        try 
        {
            auto dataset = this->keaImgFile->getDataSet(metaDataH5Path);
            dataset.write(value);
        
            // Flushing the dataset
            this->keaImgFile->flush();
        }
        catch (const HighFive::Exception &e) 
        {
            throw KEAIOException("Could not set band meta-data.");
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
    
    std::string KEAImageIO::getImageBandMetaData(uint32_t band, const std::string &name)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        std::string metaDataH5Path = KEA_DATASETNAME_BAND + uint2Str(band) + KEA_BANDNAME_METADATA + std::string("/") + name;
        std::string value = "";
        // READ IMAGE META-DATA
        try 
        {
            auto dataset = this->keaImgFile->getDataSet(metaDataH5Path);
            dataset.read(value);
        } 
        catch ( const HighFive::Exception &e) 
        {
            throw KEAIOException("Meta-data band variable was not accessable.");
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
    }
    
    std::vector<std::string> KEAImageIO::getImageBandMetaDataNames(uint32_t band)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        std::string metaDataGroupName = KEA_DATASETNAME_BAND + uint2Str(band) + KEA_BANDNAME_METADATA;
        
        try 
        {
            // Try to open the group
            auto group = this->keaImgFile->getGroup(metaDataGroupName);
            return group.listObjectNames();
        }
        catch (const HighFive::Exception &e)
        {
            throw KEAIOException("Could not retrieve band meta data.");
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }
    
    std::vector< std::pair<std::string, std::string> > KEAImageIO::getImageBandMetaData(uint32_t band)
    {
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
            auto group = this->keaImgFile->getGroup(metaDataGroupName);
            for( size_t i = 0; i < group.getNumberObjects(); i++ )
            {
                std::string name = group.getObjectName(i);
                std::string value = this->getImageBandMetaData(band, name);
                metaData.push_back(std::pair<std::string, std::string>(name,value));
            }
        }
        catch (const HighFive::Exception &e)
        {
            throw KEAIOException("Could not retrieve image meta data.");
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return metaData;
    }
    
    void KEAImageIO::setImageBandMetaData(uint32_t band, const std::vector< std::pair<std::string, std::string> > &data)
    {
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
        }
        catch (const HighFive::Exception &e)
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
    }
    
    void KEAImageIO::setImageBandDescription(uint32_t band, const std::string &description)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        std::string descDataH5Path = KEA_DATASETNAME_BAND + uint2Str(band) + KEA_BANDNAME_DESCRIP;

        // WRITE IMAGE BAND DESCRIPTION
        try 
        {
            if(!this->keaImgFile->exist(descDataH5Path))
            {
                HighFive::DataSpace dataSpace = HighFive::DataSpace(1, 1);
                keaImgFile->createDataSet(descDataH5Path, dataSpace, HighFive::AtomicType<std::string>());
            }
            auto dataset = this->keaImgFile->getDataSet(descDataH5Path);
            dataset.write(description);
        
            // Flushing the dataset
            this->keaImgFile->flush();
        }
        catch ( const HighFive::Exception &e) 
        {
            throw KEAIOException("Description was not accessable.");
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
    
    std::string KEAImageIO::getImageBandDescription(uint32_t band)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        std::string descDataH5Path = KEA_DATASETNAME_BAND + uint2Str(band) + KEA_BANDNAME_DESCRIP;
        std::string description = "";
        // READ IMAGE BAND DESCRIPTION
        
        try 
        {
            auto dataset = this->keaImgFile->getDataSet(descDataH5Path);
            dataset.read(description);
        } 
        catch ( const HighFive::Exception &e) 
        {
            throw KEAIOException("Meta-data band variable was not accessable.");
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
    }
    
    
    
    void KEAImageIO::setNoDataValue(uint32_t band, const void *data, KEADataType inDataType)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        std::string noDataValPath = KEA_DATASETNAME_BAND + uint2Str(band) + KEA_BANDNAME_NO_DATA_VAL;
        // SET NODATA
        try 
        {    
            auto hdfDataType = convertDatatypeKeaToH5Native(inDataType);
            KEADataType imgDataType = this->getImageBandDataType(band);
            auto imgBandDT = convertDatatypeKeaToH5STD(imgDataType);
            if(!this->keaImgFile->exist(noDataValPath))
            {
        		// TODO: dataspace instead of scalar to be compatible with old KEA
                HighFive::DataSpace dataSpace = HighFive::DataSpace({1});
                keaImgFile->createDataSet(noDataValPath, dataSpace, imgBandDT);
                std::cout << "created dataset " << noDataValPath << std::endl;
            }
            auto dataset = this->keaImgFile->getDataSet( noDataValPath );
            dataset.write_raw(data, hdfDataType);
            std::cout << "wrote value" << std::endl;
            // now set flag that says whether nodata set or not
            int8_t val = 1;
            dataset.createAttribute(KEA_NODATA_DEFINED, val);
            std::cout << "wrote attr" << std::endl;
            // Flushing the dataset
            this->keaImgFile->flush();
        }
        catch ( const HighFive::Exception &e) 
        {
            throw KEAIOException("Could not set nodata value");
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
    
    void KEAImageIO::getNoDataValue(uint32_t band, void *data, KEADataType inDataType)
    {
        kealib::kea_lock lock(*this->m_mutex);
        KEAStackPrintState printState;
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        // READ IMAGE BAND NO DATA VALUE
        std::string noDataValPath = KEA_DATASETNAME_BAND + uint2Str(band) + KEA_BANDNAME_NO_DATA_VAL;
        
        try
        {
            auto hdfDataType = convertDatatypeKeaToH5Native(inDataType);
            if (keaImgFile->exist(noDataValPath))
            {
                auto datasetBandDataType = keaImgFile->getDataSet(noDataValPath);
                // check set/not set flag
                auto setattrib = datasetBandDataType.getAttribute(KEA_NODATA_DEFINED);
                int8_t val = setattrib.read<int8_t>();
                if( val == 1 )
                {
                    datasetBandDataType.read_raw(data, hdfDataType);
                }
                else
                {
                    throw KEAIOException("The image band no data value was not defined.");
                }
            }
            else
            {
                throw KEAIOException("The image band no data value was not set.");
            }
        } 
        catch ( const HighFive::Exception &e) 
        {
            throw KEAIOException("The image band no data value was not found.");
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
    
    void KEAImageIO::undefineNoDataValue(uint32_t band)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        std::string noDataValPath = KEA_DATASETNAME_BAND + uint2Str(band) + KEA_BANDNAME_NO_DATA_VAL;
        
        // UNDEFINE THE NO DATA VALUE
        try
        {
            auto datasetBandDataType = keaImgFile->getDataSet(noDataValPath);
            int8_t val = 0;
            if( datasetBandDataType.hasAttribute(KEA_NODATA_DEFINED))
            {
                auto attribset = datasetBandDataType.getAttribute(KEA_NODATA_DEFINED);
                attribset.write(val);
            }
            else
            {
                datasetBandDataType.createAttribute(KEA_NODATA_DEFINED, &val);
            }
            // Flushing the dataset
            this->keaImgFile->flush();
        }
        catch ( const HighFive::Exception &e)
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
    }
    
    std::vector<KEAImageGCP*>* KEAImageIO::getGCPs()
    {
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
                auto numGCPsDataset = this->keaImgFile->getDataSet(KEA_GCPS_NUM);
                numGCPsDataset.read(numGCPs);
            }
            catch (const HighFive::Exception &e)
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
                // Open the GCPs dataset 
                auto fieldDtMem = this->createGCPCompType();
                if( this->keaImgFile->exist(KEA_GCPS_DATA))
                {
                    auto gcpsDataset = this->keaImgFile->getDataSet(KEA_GCPS_DATA);
                    gcpsDataset.read_raw(gcpsHDF);
                }
                else
                {
                    throw KEAIOException("Unable to read GCPs");
                }
            }
            catch( const HighFive::Exception &e )
            {
                throw KEAIOException(e.what());
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
        catch (const HighFive::Exception &e)
        {
            throw KEAIOException(e.what());
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
    }
    
    void KEAImageIO::setGCPs(std::vector<KEAImageGCP*> *gcps, const std::string &projWKT)
    {
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
            auto fieldDtMem = this->createGCPCompType();
            if( this->keaImgFile->exist(KEA_GCPS_DATA))
            {
                auto gcpsDataset = this->keaImgFile->getDataSet(KEA_GCPS_DATA);
                auto gcpsDataDims = gcpsDataset.getDimensions();
                
                if(numGCPs > gcpsDataDims[0])
                {
                    std::vector<size_t> dims;
                    dims.push_back(numGCPs);
                    gcpsDataset.resize(dims);
                }
                
                gcpsDataset.write(gcpsHDF);
            }
            else
            {
                // https://github.com/highfive-devs/highfive/blob/main/src/examples/create_datatype.cpp
                // allow the number of GCPs to grow
                std::vector<size_t> dims;
                dims.push_back(numGCPs);
                std::vector<size_t> maxdims;
                maxdims.push_back(HighFive::DataSpace::UNLIMITED);
                HighFive::DataSpace gcpsDataSpace = HighFive::DataSpace(dims, maxdims);
                
                HighFive::DataSetCreateProps creationGCPsDSPList;
                creationGCPsDSPList.add(HighFive::Chunking(numGCPs));
                creationGCPsDSPList.add(HighFive::Deflate(KEA_DEFLATE));
                creationGCPsDSPList.add(HighFive::Shuffle());
                
                // don't commit data type - HDF5 will but data type in file right here

                auto gcpsDataset = this->keaImgFile->createDataSet(KEA_GCPS_DATA, gcpsDataSpace, fieldDtMem, creationGCPsDSPList);
                gcpsDataset.write(gcpsHDF);
            }
            
            
            if( !this->keaImgFile->exist(KEA_GCPS_NUM))
            {
        		// TODO: dataspace instead of scalar to be compatible with old KEA
				HighFive::DataSpace dataSpace = HighFive::DataSpace({1});
                this->keaImgFile->createDataSet<uint32_t>(KEA_GCPS_NUM, dataSpace).write(numGCPs);
            }
            else
            {
                auto numGCPsDataset = this->keaImgFile->getDataSet(KEA_GCPS_NUM);
                numGCPsDataset.write(numGCPs);
            }
            
            // Flushing the dataset
            this->keaImgFile->flush();
        }
        catch (const HighFive::Exception &e)
        {
            throw KEAIOException(e.what());
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
    }
    
    uint32_t KEAImageIO::getGCPCount()
    {
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
            auto numGCPsDataset = this->keaImgFile->getDataSet(KEA_GCPS_NUM);
            numGCPsDataset.read(numGCPs);
        }
        catch ( const HighFive::Exception &e)
        {
            throw KEAIOException("Unable to read number of GCPs");
        }
        
        return numGCPs;
    }
    
    std::string KEAImageIO::getGCPProjection()
    {
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
            auto datasetGCPSpatialReference = this->keaImgFile->getDataSet( KEA_GCPS_PROJ );
            datasetGCPSpatialReference.read(gcpProj);
        }
        catch ( const HighFive::Exception &e)
        {
            throw KEAIOException("The spatial reference was not specified.");
        }
        
        return gcpProj;
    }
    
    void KEAImageIO::setGCPProjection(const std::string &projWKT)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        // SET THE WKT STRING SPATIAL REFERENCE IN GLOBAL HEADER
        try
        {
            if( this->keaImgFile->exist(KEA_GCPS_PROJ))
            {
                auto datasetSpatialReference = this->keaImgFile->getDataSet(KEA_GCPS_PROJ);
                datasetSpatialReference.write(projWKT);
            }
            else
            {
        		// TODO: dataspace instead of scalar to be compatible with old KEA
                HighFive::DataSpace dataSpace = HighFive::DataSpace({1});
                auto datasetSpatialReference = this->keaImgFile->createDataSet(KEA_GCPS_PROJ, dataSpace, HighFive::VariableLengthStringType());
                datasetSpatialReference.write(projWKT);
            }
            this->keaImgFile->flush();
        }
        catch (const HighFive::Exception &e)
        {
            throw KEAIOException(e.what());
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
    
    void KEAImageIO::setSpatialInfo(KEAImageSpatialInfo *inSpatialInfo)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        try 
        {
            // SET X AND Y TL IN GLOBAL HEADER
            std::vector<double> tlCoordsSpatial(2);
            tlCoordsSpatial[0] = double(inSpatialInfo->tlX);
            tlCoordsSpatial[1] = double(inSpatialInfo->tlY);
            auto spatialTLDataset = keaImgFile->getDataSet(
                KEA_DATASETNAME_HEADER_TL
            ); // Explicit datatype: IEEE_F64LE
            spatialTLDataset.write(tlCoordsSpatial);

            // SET X AND Y RESOLUTION IN GLOBAL HEADER
            std::vector<float> pxlResSpatial(2);
            pxlResSpatial[0] = float(inSpatialInfo->xRes);
            pxlResSpatial[1] = float(inSpatialInfo->yRes);
            auto spatialResDataset = keaImgFile->getDataSet(
                KEA_DATASETNAME_HEADER_RES
            ); 
            spatialResDataset.write(pxlResSpatial);

            // SET X AND Y ROTATION IN GLOBAL HEADER
            std::vector<float> pxlRotSpatial(2);
            pxlRotSpatial[0] = float(inSpatialInfo->xRot);
            pxlRotSpatial[1] = float(inSpatialInfo->yRot);
            auto spatialRotDataset = keaImgFile->getDataSet(
                KEA_DATASETNAME_HEADER_ROT
            ); 
            spatialRotDataset.write(pxlRotSpatial);

            // SET THE WKT STRING SPATIAL REFERENCE IN GLOBAL HEADER
            auto datasetSpatialReference = keaImgFile->getDataSet(
                KEA_DATASETNAME_HEADER_WKT
            );
            datasetSpatialReference.write(inSpatialInfo->wktString);
            this->keaImgFile->flush();
        } 
        catch (const HighFive::Exception &e)
        {
            throw KEAIOException(e.what());
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

    /**
     * Retrieves the block size of an image band.
     *
     * This method returns the block size for a specific band in the KEA image.
     * The block size refers to the dimensions used for storing blocks of image data
     * within a band.
     *
     * @param band The band number for which the block size is to be retrieved.
     *             Band indexing starts at 1. Providing a value of 0 or greater than
     *             the total number of bands in the image will throw an exception.
     * @return The block size of the specified band as an unsigned 32-bit integer.
     * @throws KEAIOException If the image file is not open, the band index is invalid,
     *                        the specific band dataset or block size attribute is missing,
     *                        or if an error occurs while reading the necessary data.
     */
    uint32_t KEAImageIO::getImageBlockSize(uint32_t band)
    {
        kealib::kea_lock lock(*this->m_mutex);
        if (!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        // CHECK PARAMETERS PROVIDED FIT WITHIN IMAGE
        if (band == 0)
        {
            throw KEAIOException("KEA Image Bands start at 1.");
        }
        else if (band > this->numImgBands)
        {
            throw KEAIOException("Band is not present within image.");
        }

        uint32_t imgBlockSize = 0;

        // READ IMAGE BLOCK SIZE
        std::string imageBandPath = KEA_DATASETNAME_BAND + uint2Str(band);
        if (keaImgFile->exist(imageBandPath + KEA_BANDNAME_DATA))
        {
            try
            {
                auto imgBandDataset = keaImgFile->getDataSet(imageBandPath + KEA_BANDNAME_DATA);

                if (imgBandDataset.hasAttribute(KEA_ATTRIBUTENAME_BLOCK_SIZE))
                {
                    auto imgBandBlockSizeAttribute = imgBandDataset.getAttribute(
                        KEA_ATTRIBUTENAME_BLOCK_SIZE
                    );
                    imgBandBlockSizeAttribute.read(imgBlockSize);
                }
                else
                {
                    throw KEAIOException(
                        "The attribute 'BLOCK_SIZE' does not exist for the specified band."
                    );
                }
            }
            catch (const KEAIOException &e)
            {
                throw e;
            }
            catch (const HighFive::Exception &e)
            {
                throw KEAIOException(e.what());
            }
            catch (const std::exception &e)
            {
                throw KEAIOException(e.what());
            }
        }
        else
        {
            throw KEAIOException("The number of image bands was not specified.");
        }

        return imgBlockSize;
    }

    uint32_t KEAImageIO::getAttributeTableChunkSize(uint32_t band)
    {
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
            std::string imageBandPath = KEA_DATASETNAME_BAND + uint2Str(band);
            try 
            {
                auto datasetAttSize = this->keaImgFile->getDataSet( imageBandPath + KEA_ATT_CHUNKSIZE_HEADER);
                datasetAttSize.read(attChunkSize);
            } 
            catch ( const H5::Exception &e) 
            {
                throw KEAIOException("Could not get attribute table block size.");
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
        
        return attChunkSize;
    }

    KEADataType KEAImageIO::getImageBandDataType(uint32_t band)
    {
        kealib::kea_lock lock(*this->m_mutex);
        if (!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        // CHECK PARAMETERS PROVIDED FIT WITHIN IMAGE
        if (band == 0)
        {
            throw KEAIOException("KEA Image Bands start at 1.");
        }
        else if (band > this->numImgBands)
        {
            throw KEAIOException("Band is not present within image.");
        }

        KEADataType imgDataType = kealib::kea_undefined;

        // READ IMAGE DATA TYPE
        if (keaImgFile->exist(KEA_DATASETNAME_BAND + uint2Str(band) + KEA_BANDNAME_DT))
        {
            try
            {
                auto datasetBandDataType = keaImgFile->getDataSet(
                    KEA_DATASETNAME_BAND + uint2Str(band) + KEA_BANDNAME_DT
                );
                uint32_t dtValue;
                datasetBandDataType.read(dtValue);
                imgDataType = (KEADataType) dtValue;
            }
            catch (const KEAIOException &e)
            {
                throw e;
            }
            catch (const HighFive::Exception &e)
            {
                throw KEAIOException(e.what());
            }
            catch (const std::exception &e)
            {
                throw KEAIOException(e.what());
            }
        }
        else
        {
            throw KEAIOException("The number of image bands was not specified.");
        }

        return imgDataType;
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
            auto datasetImgLT = this->keaImgFile->getDataSet( KEA_DATASETNAME_BAND + uint2Str(band) + KEA_BANDNAME_TYPE );
            datasetImgLT.write(value);
            this->keaImgFile->flush();
        } 
        catch ( const HighFive::Exception &e) 
        {
            throw KEAIOException(e.what());
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
    
    KEALayerType KEAImageIO::getImageBandLayerType(uint32_t band)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        // READ IMAGE LAYER TYPE
        uint32_t value;
        try 
        {
            auto datasetImgLT = this->keaImgFile->getDataSet( KEA_DATASETNAME_BAND + uint2Str(band) + KEA_BANDNAME_TYPE );
            datasetImgLT.read(value);
        } 
        catch ( const HighFive::Exception &e) 
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
        
        return (KEALayerType)value;
    }
    
    void KEAImageIO::setImageBandClrInterp(uint32_t band, KEABandClrInterp imgLayerClrInterp)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        // WRITE IMAGE LAYER USAGE
        uint32_t value = (uint32_t) imgLayerClrInterp;
        std::string datasetName = KEA_DATASETNAME_BAND + uint2Str(band) + KEA_BANDNAME_USAGE;
        try 
        {
            if( !this->keaImgFile->exist(datasetName) )
            {
                keaImgFile->createDataSet<uint32_t>(datasetName, value);
            }
            else
            {
                auto dataset = keaImgFile->getDataSet(datasetName);
                dataset.write(value);
            }
        } 
        catch ( const HighFive::Exception &e) 
        {
            throw KEAIOException(e.what());
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
    
    KEABandClrInterp KEAImageIO::getImageBandClrInterp(uint32_t band)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        KEABandClrInterp imgLayerClrInterp = kea_generic;
        
        // READ IMAGE LAYER USAGE
        std::string datasetName = KEA_DATASETNAME_BAND + uint2Str(band) + KEA_BANDNAME_USAGE;
        try 
        {
            auto dataset = keaImgFile->getDataSet(datasetName);
            uint32_t value;
            dataset.read(value);
            imgLayerClrInterp = (KEABandClrInterp)value;
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
    }
    
    void KEAImageIO::createOverview(uint32_t band, uint32_t overview, uint64_t xSize, uint64_t ySize)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        if(!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }
        
        std::string overviewName = KEA_DATASETNAME_BAND + uint2Str(band) + KEA_OVERVIEWSNAME_OVERVIEW + uint2Str(overview);
                
        if( this->keaImgFile->exist(overviewName))
        {
            this->keaImgFile->unlink(overviewName);
        }

        // Find the smallest axis of the image.
        uint32_t imageBlockSize = this->getImageBlockSize(band);
        uint64_t minImgDim = xSize < ySize ? xSize : ySize;
        uint32_t blockSize2Use =
                imageBlockSize > minImgDim ? minImgDim : imageBlockSize;
        
        try 
        {
            KEADataType imgDataType = this->getImageBandDataType(band);

            HighFive::DataSpace dataSpace = HighFive::DataSpace({ySize, xSize});
            HighFive::DataType dataTypeH5 = convertDatatypeKeaToH5STD(imgDataType);

            HighFive::DataSetCreateProps imgBandDataSetProps;
            imgBandDataSetProps.add(HighFive::Chunking(blockSize2Use, blockSize2Use));
            imgBandDataSetProps.add(HighFive::Shuffle());
            int initFillVal = FILL_IMAGE_DATA;
            // HighFive doesn't appear to support this (yet)
            if( H5Pset_fill_value(imgBandDataSetProps.getId(), H5T_NATIVE_INT, &initFillVal) < 0 )
            {
				H5Eprint(H5E_DEFAULT, stderr);
				throw KEAIOException("Error in H5Pset_fill_value");
            }

            HighFive::DataSetAccessProps imgBandAccessProps =
                    HighFive::DataSetAccessProps::Default();

            // CREATE THE IMAGE DATA ARRAY
            HighFive::DataSet imgBandDataSet = this->keaImgFile->createDataSet(
                overviewName,
                dataSpace,
                dataTypeH5,
                imgBandDataSetProps,
                imgBandAccessProps
            );

            auto scalar_dataspace = HighFive::DataSpace(
				HighFive::DataSpace::dataspace_scalar
			);
			imgBandDataSet.createAttribute(KEA_ATTRIBUTENAME_CLASS, 	
				scalar_dataspace,
				HighFive::FixedLengthStringType(6, HighFive::StringPadding::NullTerminated)).write("IMAGE");
            
			imgBandDataSet.createAttribute(KEA_ATTRIBUTENAME_IMAGE_VERSION,
				scalar_dataspace,
				HighFive::FixedLengthStringType(4, HighFive::StringPadding::NullTerminated)).write("1.2");
            imgBandDataSet.createAttribute<uint16_t>(
                KEA_ATTRIBUTENAME_BLOCK_SIZE,
                blockSize2Use
            );
            
            this->keaImgFile->flush();
        }
        catch (const HighFive::Exception &e)
        {
            throw KEAIOException(e.what());
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
    
    void KEAImageIO::removeOverview(uint32_t band, uint32_t overview)
    {
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
            auto imgBandDataset = this->keaImgFile->getDataSet( overviewName );
            this->keaImgFile->unlink(overviewName);
            this->keaImgFile->flush();
        }
        catch (const HighFive::Exception &e)
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
    }
    
    uint32_t KEAImageIO::getOverviewBlockSize(uint32_t band, uint32_t overview)
    {
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
                auto imgBandDataset = this->keaImgFile->getDataSet( overviewName );
                
                auto imgBandBlockSizeAttribute = imgBandDataset.getAttribute(
                      KEA_ATTRIBUTENAME_BLOCK_SIZE
                );
                imgBandBlockSizeAttribute.read(ovBlockSize);
            } 
            catch ( const HighFive::Exception &e) 
            {
                throw KEAIOException("Could not retrieve the overview block size.");
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
        
        return ovBlockSize;
    }
    
    void KEAImageIO::writeToOverview(uint32_t band, uint32_t overview, void *data, uint64_t xPxlOff, uint64_t yPxlOff, uint64_t xSizeOut, uint64_t ySizeOut, uint64_t xSizeBuf, uint64_t ySizeBuf, KEADataType inDataType)
    {
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
            std::string overviewName = KEA_DATASETNAME_BAND + uint2Str(band) + KEA_OVERVIEWSNAME_OVERVIEW + uint2Str(overview);

            // OPEN BAND DATASET AND WRITE IMAGE DATA
            if (this->keaImgFile->exist(overviewName))
            {
                auto imgBandDataset = this->keaImgFile->getDataSet(overviewName);
                writeImageToDataset(imgBandDataset, band, data, xPxlOff, yPxlOff, xSizeOut,
                    ySizeOut, xSizeBuf, ySizeBuf, inDataType);
                
                // Flushing the dataset
                this->keaImgFile->flush();
            }
            else
            { 
                throw KEAIOException("Band overview dataset does not exist.");
            }
        }
        catch(const KEAIOException &e)
        {
            throw e;
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
    
    void KEAImageIO::readFromOverview(uint32_t band, uint32_t overview, void *data, uint64_t xPxlOff, uint64_t yPxlOff, uint64_t xSizeIn, uint64_t ySizeIn, uint64_t xSizeBuf, uint64_t ySizeBuf, KEADataType inDataType)
    {
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
            auto imgBandDT = convertDatatypeKeaToH5Native(inDataType);
            std::string overviewName = KEA_DATASETNAME_BAND + uint2Str(band) + KEA_OVERVIEWSNAME_OVERVIEW + uint2Str(overview);
            
            if (this->keaImgFile->exist(overviewName))
            {
                auto imgBandDataset = this->keaImgFile->getDataSet(overviewName);
                readImageFromDataset(imgBandDataset, band, data, xPxlOff, yPxlOff, xSizeIn,
                    ySizeIn, xSizeBuf, ySizeBuf, inDataType);            
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
        catch (const HighFive::Exception &e)
        {
            throw KEAIOException(e.what());
        }
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }
    
    uint32_t KEAImageIO::getNumOfOverviews(uint32_t band)
    {
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
            auto group = this->keaImgFile->getGroup(overviewGroupName);
            numOverviews = group.getNumberObjects();
        }
        catch (const HighFive::Exception &e)
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
    }
    
    void KEAImageIO::getOverviewSize(uint32_t band, uint32_t overview, uint64_t *xSize, uint64_t *ySize)
    {
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
                auto imgBandDataset = this->keaImgFile->getDataSet( overviewName );
                
                auto dims = imgBandDataset.getDimensions();
                
                if(dims.size() != 2)
                {
                    throw KEAIOException("The number of dimensions for the overview must be 2.");
                }
                
                *xSize = dims[1];
                *ySize = dims[0];
            } 
            catch(const KEAIOException &e)
            {
                throw e;
            }
            catch ( const HighFive::Exception &e) 
            {
                throw KEAIOException("Could not read from image overview.");
            }            
        }
        catch( const HighFive::Exception &e )
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
    }
    
    KEAAttributeTable* KEAImageIO::getAttributeTable(KEAATTType type, uint32_t band)
    {
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
            
            // Read header size.
            std::vector<uint64_t> attSize;
            try
            {
                auto datasetAttSize = this->keaImgFile->getDataSet( bandPathBase + KEA_ATT_SIZE_HEADER );
                datasetAttSize.read(attSize);
            }
            catch (const HighFive::Exception &e)
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
        catch ( const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        return attPresent;
    }

    /**
     * Closes the KEA image file and releases associated resources.
     *
     * This method ensures that any open KEA image file is properly closed,
     * releasing memory and file locks, and committing any unsaved changes
     * to the file. If the file is not open, no action is performed.
     *
     * The method handles exceptions from the KEA library, HighFive, or
     * standard exceptions, wrapping and rethrowing them as KEAIOException
     * to provide uniform error reporting.
     *
     * This function operates under a thread-safe lock to prevent concurrent
     * access issues when modifying shared resources.
     *
     * @throws KEAIOException If an error occurs while flushing the file,
     *                        deleting resources, or any internal operation.
     */
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

    /**
     * Creates a new KEA image file and initializes its spatial metadata, global headers,
     * global metadata, ground control points (GCPs), and specified image bands.
     *
     * This method constructs a HighFive::File object for the KEA image file, populates its
     * headers with spatial information, file metadata, and other required attributes. It also creates
     * the requested number of image bands and their associated datasets. Spatial information and metadata
     * such as top-left coordinates, pixel resolution, rotation, and coordinate reference system can be
     * specified or initialized with default values.
     *
     * @param fileName The name of the KEA image file to be created.
     * @param dataType Data type of the image bands (e.g., integer, float).
     * @param xSize The number of pixels along the X (horizontal) dimension of the image.
     * @param ySize The number of pixels along the Y (vertical) dimension of the image.
     * @param numImgBands The number of image bands to create in the KEA file.
     * @param bandDescrips Optional vector containing descriptions for each band. If null, bands will have empty descriptions.
     * @param spatialInfo Optional pointer to KEAImageSpatialInfo containing spatial metadata to be written to the file.
     *                    If null, default values will be used.
     * @param imageBlockSize The block size for dividing image band data into tiles.
     * @param attBlockSize The block size for attribute data in the image bands.
     * @param mdcElmts The number of metadata cache elements.
     * @param rdccNElmts The number of elements in the raw data chunk cache.
     * @param rdccNBytes The total size (in bytes) of the raw data chunk cache.
     * @param rdccW0 The policy for evicting raw data chunks from the cache.
     * @param sieveBuf The size of the sieve buffer (in bytes).
     * @param metaBlockSize The size (in bytes) of blocks allocated for metadata.
     * @param deflate The compression level to use (0 = no compression, 9 = maximum compression).
     *
     * @return A pointer to the created HighFive::File object representing the KEA image file.
     *
     * @throws KEAIOException If an error occurs during file creation, writing metadata, or initializing content.
     * @throws HighFive::Exception If there is a failure within the HighFive library while creating or modifying the file.
     * @throws std::exception For any other uncaught errors that occur during processing.
     */
    HighFive::File *KEAImageIO::createKEAImage(
        const std::string &fileName, KEADataType dataType, uint32_t xSize,
        uint32_t ySize, uint32_t numImgBands, std::vector<std::string> *bandDescrips,
        KEAImageSpatialInfo *spatialInfo, uint32_t imageBlockSize,
        uint32_t attBlockSize, int mdcElmts, hsize_t rdccNElmts, hsize_t rdccNBytes,
        double rdccW0, hsize_t sieveBuf, hsize_t metaBlockSize, uint32_t deflate
    )
    {
        HighFive::File *keaImgH5File = nullptr;

        // Define dataspaces for writing string data
        auto scalar_dataspace = HighFive::DataSpace(
            HighFive::DataSpace::dataspace_scalar
        );
        auto var_stringtype = HighFive::VariableLengthStringType();
		// TODO: dataspace instead of scalar to be compatible with old KEA
        auto oldKeaDataSpace = HighFive::DataSpace({1});
        try
        {
            auto keaFileAccessProps = HighFive::FileAccessProps::Default();
            keaFileAccessProps.add(HighFive::MetadataBlockSize(metaBlockSize));
            // HighFive doesn't seem to support these ones now
            if( H5Pset_sieve_buf_size(keaFileAccessProps.getId(), sieveBuf) < 0 )
            {
                H5Eprint(H5E_DEFAULT, stderr);
				throw KEAIOException("Error in H5Pset_sieve_buf_size");
            }
            if( H5Pset_cache(keaFileAccessProps.getId(), mdcElmts, rdccNElmts, rdccNBytes, rdccW0) < 0 )
            {
                H5Eprint(H5E_DEFAULT, stderr);
				throw KEAIOException("Error in H5Pset_cache");
            }

            keaImgH5File = new HighFive::File(
                fileName,
                HighFive::File::Truncate | HighFive::File::Create |
                HighFive::File::ReadWrite,
                keaFileAccessProps
            );

            //////////// CREATE GLOBAL HEADER ////////////////
            keaImgH5File->createGroup(KEA_DATASETNAME_HEADER);

            bool deleteSpatialInfo = false;
            if (spatialInfo == nullptr)
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
            auto spatialTLDataset = keaImgH5File->createDataSet<double>(
                KEA_DATASETNAME_HEADER_TL,
                HighFive::DataSpace::From(tlCoordsSpatial)
            ); // Explicit datatype: IEEE_F64LE
            spatialTLDataset.write(tlCoordsSpatial);

            // SET X AND Y RESOLUTION IN GLOBAL HEADER
            std::vector<double> pxlResSpatial(2);
            pxlResSpatial[0] = spatialInfo->xRes;
            pxlResSpatial[1] = spatialInfo->yRes;
            auto spatialResDataset = keaImgH5File->createDataSet<double>(
                KEA_DATASETNAME_HEADER_RES,
                HighFive::DataSpace::From(pxlResSpatial)
            ); 
            spatialResDataset.write(pxlResSpatial);

            // SET X AND Y ROTATION IN GLOBAL HEADER
            std::vector<double> pxlRotSpatial(2);
            pxlRotSpatial[0] = spatialInfo->xRot;
            pxlRotSpatial[1] = spatialInfo->yRot;
            auto spatialRotDataset = keaImgH5File->createDataSet<double>(
                KEA_DATASETNAME_HEADER_ROT,
                HighFive::DataSpace::From(pxlRotSpatial)
            ); 
            spatialRotDataset.write(pxlRotSpatial);

            // SET NUMBER OF X AND Y PIXELS
            std::vector<uint64_t> imgSizeSpatial(2);
            imgSizeSpatial[0] = uint64_t(spatialInfo->xSize);
            imgSizeSpatial[1] = uint64_t(spatialInfo->ySize);
            auto spatialSizeDataset = keaImgH5File->createDataSet<uint64_t>(
                KEA_DATASETNAME_HEADER_SIZE,
                HighFive::DataSpace::From(imgSizeSpatial)
            ); // Explicit datatype: UINT64
            spatialSizeDataset.write(imgSizeSpatial);

            // SET THE WKT STRING SPATIAL REFERENCE IN GLOBAL HEADER
            auto datasetSpatialReference = keaImgH5File->createDataSet(
                KEA_DATASETNAME_HEADER_WKT,
                oldKeaDataSpace,
                var_stringtype
            );
            datasetSpatialReference.write(spatialInfo->wktString);

            // SET THE FILE TYPE IN GLOBAL HEADER
            auto datasetFileType = keaImgH5File->createDataSet(
                KEA_DATASETNAME_HEADER_FILETYPE,
                oldKeaDataSpace,
                var_stringtype
            );
            datasetFileType.write(KEA_FILE_TYPE);

            // SET THE FILE GENARATOR IN GLOBAL HEADER
            auto datasetGenarator = keaImgH5File->createDataSet(
                KEA_DATASETNAME_HEADER_GENERATOR,
                oldKeaDataSpace,
                var_stringtype
            );
            datasetGenarator.write(KEA_SOFTWARE);

            // SET THE FILE VERSION IN GLOBAL HEADER
            auto datasetVersion = keaImgH5File->createDataSet(
                KEA_DATASETNAME_HEADER_VERSION,
                oldKeaDataSpace,
                var_stringtype
            );
            datasetVersion.write(KEA_VERSION);

            if (deleteSpatialInfo)
            {
                delete spatialInfo;
            }
            //////////// CREATED GLOBAL HEADER ////////////////

            //////////// CREATE GLOBAL META-DATA ////////////////
            keaImgH5File->createGroup(KEA_DATASETNAME_METADATA);
            //////////// CREATED GLOBAL META-DATA ////////////////

            //////////// CREATE GCPS ////////////////
            keaImgH5File->createGroup(KEA_GCPS);
            //////////// CREATED GCPS ////////////////

            //////////// CREATE IMAGE BANDS ////////////////
            for (uint32_t i = 0; i < numImgBands; ++i)
            {
                std::string bandDescription = "";
                if (bandDescrips != nullptr && i < bandDescrips->size())
                {
                    bandDescription = bandDescrips->at(i);
                }

                addImageBandToFile(
                    keaImgH5File,
                    dataType,
                    xSize,
                    ySize,
                    i + 1,
                    bandDescription,
                    imageBlockSize,
                    attBlockSize,
                    deflate
                );
            }
            //////////// CREATED IMAGE BANDS ////////////////

            keaImgH5File->flush();
        }
        catch (HighFive::Exception &e)
        {
            throw KEAIOException(e.what());
        }
        catch (const KEAIOException &e)
        {
            throw e;
        }
        catch (const std::exception &e)
        {
            throw KEAIOException(e.what());
        }

        return keaImgH5File;
    }

    /**
     * Opens a KEA HDF5 file for read-write access and returns a pointer to the file object.
     *
     * This method creates and initializes an instance of the HighFive::File class with read-write
     * access mode. It optionally allows configuration of HDF5 file access properties, such as
     * metadata cache settings, raw data chunk cache, and I/O performance parameters. If the file
     * cannot be opened or any error occurs during initialization, this method throws an appropriate
     * exception.
     *
     * @param fileName The path to the KEA HDF5 file to be opened.
     * @param mdcElmts The number of elements in the metadata cache.
     * @param rdccNElmts The number of elements in the raw data chunk cache.
     * @param rdccNBytes The total size, in bytes, of the raw data chunk cache.
     * @param rdccW0 The write mode fraction for the raw data chunk cache.
     * @param sieveBuf The minimum size, in bytes, of the sieve buffer for data I/O.
     * @param metaBlockSize The size, in bytes, of the metadata aggregation block.
     *
     * @return A pointer to the HighFive::File object representing the opened KEA file with read-write access.
     *
     * @throws KEAIOException If the file cannot be opened, does not exist, or any error occurs
     *                        during the file initialization or access configuration.
     */
    HighFive::File *KEAImageIO::openKeaH5RW(
        const std::string &fileName, int mdcElmts, hsize_t rdccNElmts,
        hsize_t rdccNBytes, double rdccW0, hsize_t sieveBuf, hsize_t metaBlockSize
    )
    {
        HighFive::File *keaImgH5File = nullptr;
        try
        {
            auto keaFileAccessProps = HighFive::FileAccessProps::Default();
            //keaAccessPlist.setCache(mdcElmts, rdccNElmts, rdccNBytes, rdccW0);
            //keaAccessPlist.setSieveBufSize(sieveBuf);
            //keaAccessPlist.setMetaBlockSize(metaBlockSize);

            keaImgH5File = new HighFive::File(
                fileName,
                HighFive::File::ReadWrite,
                keaFileAccessProps
            );
        }
        catch (const KEAIOException &e)
        {
            throw e;
        }
        catch (HighFive::Exception &e)
        {
            throw KEAIOException(e.what());
        }
        catch (const std::exception &e)
        {
            throw KEAIOException(e.what());
        }

        return keaImgH5File;
    }

    /**
     * Opens a KEA HDF5 image file in read-only mode and returns a pointer to the file object.
     *
     * This method attempts to open the specified KEA image file in a read-only mode using the HighFive
     * library. It initializes default file access properties and sets up the file structure while
     * handling potential exceptions. File caching and property optimizations, such as metadata cache
     * configuration or sieve buffer size, may be configured but are currently commented out.
     *
     * @param fileName The path to the KEA HDF5 image file that needs to be opened.
     * @param mdcElmts Number of elements in the metadata cache (currently unused).
     * @param rdccNElmts Number of elements in the raw data chunk cache (currently unused).
     * @param rdccNBytes Total size of the raw data chunk cache in bytes (currently unused).
     * @param rdccW0 The preemption policy value for raw data chunk cache (currently unused).
     * @param sieveBuf Size of the data sieve buffer in bytes (currently unused).
     * @param metaBlockSize Size of the metadata block allocation in bytes (currently unused).
     *
     * @return A pointer to a HighFive::File object representing the opened KEA HDF5 image file.
     *         The file is opened in a read-only mode.
     *
     * @throws KEAIOException If the file is not a valid KEA file, the file does not exist,
     *                        or any error occurs while opening the file or initializing the file properties.
     */
    HighFive::File *KEAImageIO::openKeaH5RDOnly(
        const std::string &fileName, int mdcElmts, hsize_t rdccNElmts,
        hsize_t rdccNBytes, double rdccW0, hsize_t sieveBuf, hsize_t metaBlockSize,
       	hid_t driver_id, const void* driver_info
    )
    {
        HighFive::File *keaImgH5File = nullptr;

        try
        {
            auto keaFileAccessProps = HighFive::FileAccessProps::Default();
            keaFileAccessProps.add(HighFive::MetadataBlockSize(metaBlockSize));
            // HighFive doesn't seem to support these ones now
            if( H5Pset_sieve_buf_size(keaFileAccessProps.getId(), sieveBuf) < 0 )
            {
                H5Eprint(H5E_DEFAULT, stderr);
				throw KEAIOException("Error in H5Pset_sieve_buf_size");
            }
            if( H5Pset_cache(keaFileAccessProps.getId(), mdcElmts, rdccNElmts, rdccNBytes, rdccW0) < 0 )
            {
                H5Eprint(H5E_DEFAULT, stderr);
				throw KEAIOException("Error in H5Pset_cache");
            }
            if( driver_id != 0 )
            {
                if( H5Pset_driver(keaFileAccessProps.getId(), driver_id, driver_info) < 0 )
                {
                    H5Eprint(H5E_DEFAULT, stderr);
    				throw kealib::KEAIOException("Error in H5Pset_driver");
                }
            }

            keaImgH5File = new HighFive::File(
                fileName,
                HighFive::File::ReadOnly,
                keaFileAccessProps
            );
        }
        catch (const KEAIOException &e)
        {
            throw e;
        }
        catch (HighFive::Exception &e)
        {
            throw KEAIOException(e.what());
        }
        catch (const std::exception &e)
        {
            throw KEAIOException(e.what());
        }

        return keaImgH5File;
    }

    /**
     * Determines whether the specified file is a valid KEA image file.
     *
     * This method checks the provided file to verify if it complies with the KEA file format
     * by inspecting key datasets in the HDF5 structure, including the file type and version metadata.
     * The file is validated as a KEA image if the file type dataset contains "KEA" and the version is
     * one of the supported versions (e.g., "1.0" or "1.1").
     *
     * @param fileName A string representing the path to the file being checked.
     *                 The file must be accessible and of HDF5 format.
     *
     * @return True if the file is verified as a KEA image, otherwise False.
     *
     * @throws KEAIOException If the file cannot be opened, an error occurs during metadata reading,
     *                        or a non-KEA related exception occurs.
     */
    bool KEAImageIO::isKEAImage(const std::string &fileName)
    {
        bool keaImageFound = false;

        try
        {
            auto keaImgH5File = HighFive::File(fileName, HighFive::File::ReadOnly);

            try
            {
                std::string fileType = "";
                if (keaImgH5File.exist(KEA_DATASETNAME_HEADER_FILETYPE))
                {
                    auto datasetFileType = keaImgH5File.getDataSet(
                        KEA_DATASETNAME_HEADER_FILETYPE
                    );
                    datasetFileType.read(fileType);
                }

                try
                {
                    if (fileType == "KEA")
                    {
                        std::string fileVersion = "";
                        if (keaImgH5File.exist(KEA_DATASETNAME_HEADER_VERSION))
                        {
                            auto datasetFileVersion = keaImgH5File.getDataSet(
                                KEA_DATASETNAME_HEADER_VERSION
                            );
                            datasetFileVersion.read(fileVersion);
                        }

                        if ((fileVersion == "1.0") || (fileVersion == "1.1") || (fileVersion == "2.0"))
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
        catch (const KEAIOException &e)
        {
            throw e;
        }
        catch (const std::exception &e)
        {
            throw KEAIOException(e.what());
        }

        return keaImageFound;
    }

    KEAImageIO::~KEAImageIO()
    {
        
    }

    /**
     * Adds a new image band to the KEA image file.
     *
     * This method creates a new image band with the specified data type, description, and block sizes,
     * and appends it to the KEA file. It updates the number of image bands in the file's metadata and
     * ensures the new band is properly added and initialized.
     *
     * @param dataType The data type of the new image band (e.g., integer, float, etc.).
     * @param bandDescrip A description of the new image band, typically used as metadata.
     * @param imageBlockSize The block size used for storing the image data in the file.
     * @param attBlockSize The block size used for storing the attribute table data in the file.
     * @param deflate The compression level (0 for no compression, higher values for increasing compression).
     *
     * @throws KEAIOException If the image file is not open or issues occur during the band addition process.
     */
    void KEAImageIO::addImageBand(
        const KEADataType dataType, const std::string &bandDescrip,
        const uint32_t imageBlockSize, const uint32_t attBlockSize,
        const uint32_t deflate
    )
    {
        kealib::kea_lock lock(*this->m_mutex);

        if (!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }

        const uint32_t xSize = this->spatialInfoFile->xSize;
        const uint32_t ySize = this->spatialInfoFile->ySize;

        // add a new image band to the file
        KEAImageIO::addImageBandToFile(
            this->keaImgFile,
            dataType,
            xSize,
            ySize,
            this->numImgBands + 1,
            bandDescrip,
            imageBlockSize,
            attBlockSize,
            deflate
        );
        ++this->numImgBands;

        // update the band counter in the file metadata
        KEAImageIO::setNumImgBandsInFileMetadata(this->keaImgFile, this->numImgBands);

        this->keaImgFile->flush();
    }

    /**
     * Removes an image band from the KEA image file at the specified band index.
     *
     * This method deletes a specific image band from the KEA file, decrements the total
     * number of image bands, updates the band count in the file metadata, and flushes
     * the changes to the file. If the image file is not open, an exception is thrown.
     *
     * @param bandIndex The index of the image band to be removed. The index is 1-based.
     *
     * @throws KEAIOException If the image file is not open, or if errors occur while
     *                         removing the image band or updating the metadata.
     */
    void KEAImageIO::removeImageBand(const uint32_t bandIndex)
    {
        kealib::kea_lock lock(*this->m_mutex);

        if (!this->fileOpen)
        {
            throw KEAIOException("Image was not open.");
        }

        KEAImageIO::removeImageBandFromFile(
            this->keaImgFile,
            bandIndex,
            this->numImgBands
        );

        --this->numImgBands;

        // update the band counter in the file metadata
        KEAImageIO::setNumImgBandsInFileMetadata(this->keaImgFile, this->numImgBands);

        this->keaImgFile->flush();
    }

    /**
     * Converts a KEA data type to the corresponding HDF5 standard data type.
     *
     * This method maps KEA-specific data types, represented by the KEADataType
     * enumeration, to their equivalent HDF5 standard data types for usage within
     * HDF5 data structures. Supported KEA data types include various integer,
     * unsigned integer, and floating-point types. If an unrecognized data type
     * is provided as input, an exception is thrown.
     *
     * @param dataType The KEADataType value representing the KEA data type
     *                 to be converted.
     *
     * @return A HighFive::DataType object corresponding to the equivalent
     *         HDF5 standard data type for the given KEA data type.
     *
     * @throws KEAIOException If the specified data type is not recognized among
     *                        the defined KEA data types.
     */
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

    /**
     * Converts a KEA data type to the corresponding HDF5 native data type.
     *
     * This method translates the specified KEADataType to its equivalent HighFive::DataType,
     * enabling compatibility with HDF5 storage requirements. An exception is thrown if the
     * provided KEA data type is not recognized or supported.
     *
     * @param dataType The KEADataType to be converted. This should correspond to one of the
     *                 predefined KEA data types, such as kea_8int, kea_16uint, kea_32float, etc.
     *
     * @return The HighFive::DataType corresponding to the input KEADataType.
     *
     * @throws KEAIOException If the specified data type is not recognized as a valid KEA data type.
     */
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

    /**
     * Converts a KEA image data type to its equivalent C++ standard string.
     *
     * This method maps KEA-specific data types to their corresponding standardized
     * C++ string representation of primitive or numerical data types.
     *
     * @param dataType The KEADataType enumeration value representing the data type
     *                 in the KEA image format that needs to be converted.
     *
     * @return A string representation of the C++ data type corresponding to the
     *         provided KEADataType value.
     *
     * @throws KEAIOException If the provided KEADataType is not recognized or
     *                        cannot be mapped to a compatible C++ data type.
     */
    const std::string KEAImageIO::convertDatatypeKeaToCStdStr(
        const KEADataType dataType
    )
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

    /**
     * Adds a new image band to the specified KEA image file with the given parameters.
     *
     * This method creates a new HDF5 group and datasets within the KEA image file to represent an image band.
     * Metadata and attributes such as the band description, block size, data type, band type, band usage,
     * and other properties are also initialized. The method ensures proper dataset and attribute creation
     * while supporting compression and chunking for efficient storage.
     *
     * @param keaImgH5File A pointer to the HighFive::File object representing the open KEA image file.
     * @param dataType The data type for the image data within the band, specified as a KEADataType.
     * @param xSize The width of the image band in pixels.
     * @param ySize The height of the image band in pixels.
     * @param bandIndex The 1-based index of the band being added to the file.
     * @param bandDescripIn An optional description of the band. If empty, a default name is used.
     * @param imageBlockSize The block size to use for chunking the image data. Adjusted if exceeding minimum dimension.
     * @param attBlockSize The block size to use for attribute table chunking.
     * @param deflate The compression level to use for deflating data (0-9, where higher values indicate stronger compression).
     *
     * @throws KEAIOException If an error occurs while creating groups/datasets, writing attributes, or accessing metadata.
     */
    void KEAImageIO::addImageBandToFile(
        HighFive::File *keaImgH5File, const KEADataType dataType, const uint32_t xSize,
        const uint32_t ySize, const uint32_t bandIndex,
        const std::string &bandDescripIn, const uint32_t imageBlockSize,
        const uint32_t attBlockSize, const uint32_t deflate
    )
    {
        // Define dataspaces for writing string data
        auto scalar_dataspace = HighFive::DataSpace(
            HighFive::DataSpace::dataspace_scalar
        );
        auto var_stringtype = HighFive::VariableLengthStringType();
        //auto stringType = HighFive::AtomicType<std::string>();
        // TODO: dataspace instead of scalar to be compatible with old KEA
        auto oldKeaDataSpace = HighFive::DataSpace({1});

        int initFillVal = FILL_IMAGE_DATA;
        std::string bandDescrip = bandDescripIn; // may be updated below

        // Find the smallest axis of the image.
        uint64_t minImgDim = xSize < ySize ? xSize : ySize;
        uint32_t blockSize2Use =
                imageBlockSize > minImgDim ? minImgDim : imageBlockSize;

        try
        {
            HighFive::DataSpace dataSpace = HighFive::DataSpace({ySize, xSize});
            HighFive::DataType dataTypeH5 = convertDatatypeKeaToH5STD(dataType);

            HighFive::DataSetCreateProps imgBandDataSetProps;
            imgBandDataSetProps.add(HighFive::Chunking(blockSize2Use, blockSize2Use));
            imgBandDataSetProps.add(HighFive::Deflate(deflate));
            imgBandDataSetProps.add(HighFive::Shuffle());
            // HighFive doesn't appear to support this (yet)
            if( H5Pset_fill_value(imgBandDataSetProps.getId(), H5T_NATIVE_INT, &initFillVal) < 0 )
            {
				H5Eprint(H5E_DEFAULT, stderr);
				throw KEAIOException("Error in H5Pset_fill_value");
            }

            HighFive::DataSetAccessProps imgBandAccessProps =
                    HighFive::DataSetAccessProps::Default();

            uint8_t bandType = kea_continuous;
            uint8_t bandUsage = kea_generic;

            // CREATE IMAGE BAND HDF5 GROUP
            std::string bandName = KEA_DATASETNAME_BAND + uint2Str(bandIndex);
            keaImgH5File->createGroup(bandName);

            // CREATE THE IMAGE DATA ARRAY
            HighFive::DataSet imgBandDataSet = keaImgH5File->createDataSet(
                (bandName + KEA_BANDNAME_DATA),
                dataSpace,
                dataTypeH5,
                imgBandDataSetProps,
                imgBandAccessProps
            );
            imgBandDataSet.createAttribute(KEA_ATTRIBUTENAME_CLASS, 
				scalar_dataspace,
				HighFive::FixedLengthStringType(6, HighFive::StringPadding::NullTerminated)).write("IMAGE");
            
            imgBandDataSet.createAttribute(KEA_ATTRIBUTENAME_IMAGE_VERSION,
            	scalar_dataspace,
				HighFive::FixedLengthStringType(4, HighFive::StringPadding::NullTerminated)).write("1.2");
            
            imgBandDataSet.createAttribute<uint16_t>(
                KEA_ATTRIBUTENAME_BLOCK_SIZE,
                blockSize2Use
            );

            // SET BAND NAME / DESCRIPTION
            if (bandDescrip.empty())
            {
                bandDescrip = "Band " + uint2Str(bandIndex);
            }
            auto datasetBandDescript = keaImgH5File->createDataSet(
                (bandName + KEA_BANDNAME_DESCRIP),
                oldKeaDataSpace,
                var_stringtype
            );
            datasetBandDescript.write(bandDescrip);

            // SET IMAGE BAND DATA TYPE IN IMAGE BAND
            // TODO: dataspace instead of scalar to be compatible with old KEA
            auto dtDataset = keaImgH5File->createDataSet<uint16_t>(
                (bandName + KEA_BANDNAME_DT),
                oldKeaDataSpace
            );
            dtDataset.write(uint16_t(dataType));

            // SET IMAGE BAND TYPE IN IMAGE BAND (I.E., CONTINUOUS (0) OR
            // THEMATIC (1))
            // TODO: dataspace instead of scalar to be compatible with old KEA
            auto typeDataset = keaImgH5File->createDataSet<uint8_t>(
                (bandName + KEA_BANDNAME_TYPE),
                oldKeaDataSpace
            );
            typeDataset.write(bandType);

            // SET IMAGE BAND USAGE IN IMAGE BAND
            // TODO: dataspace instead of scalar to be compatible with old KEA
            auto usageDataset = keaImgH5File->createDataSet<uint8_t>(
                (bandName + KEA_BANDNAME_USAGE),
                oldKeaDataSpace
            );
            usageDataset.write(bandUsage);

            // CREATE META-DATA
            keaImgH5File->createGroup(bandName + KEA_BANDNAME_METADATA);

            // CREATE OVERVIEWS GROUP
            keaImgH5File->createGroup(bandName + KEA_BANDNAME_OVERVIEWS);

            // CREATE ATTRIBUTE TABLE GROUP
            keaImgH5File->createGroup(bandName + KEA_BANDNAME_ATT);
            keaImgH5File->createGroup(bandName + KEA_ATT_GROUPNAME_DATA);
            keaImgH5File->createGroup(bandName + KEA_ATT_GROUPNAME_NEIGHBOURS);
            keaImgH5File->createGroup(bandName + KEA_ATT_GROUPNAME_HEADER);


            // SET ATTRIBUTE TABLE CHUNK SIZE
            // TODO: dataspace instead of scalar to be compatible with old KEA
            auto attChunkSizeDataSpace = HighFive::DataSpace({1});
            auto attChunkSizeDataset = keaImgH5File->createDataSet<uint64_t>(
                (bandName + KEA_ATT_CHUNKSIZE_HEADER),
                attChunkSizeDataSpace
            );
            attChunkSizeDataset.write(attBlockSize);


            // SET ATTRIBUTE TABLE SIZE
            std::vector<uint64_t> attSize = {0, 0, 0, 0, 0};
            auto attSizeDataset = keaImgH5File->createDataSet<uint64_t>(
                (bandName + KEA_ATT_SIZE_HEADER),
                HighFive::DataSpace::From(attSize)
            );
            attSizeDataset.write(attSize);
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
        catch (const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }

    /**
     * Removes a specified image band from the KEA image file and renames the remaining bands.
     *
     * This method deletes the dataset corresponding to the specified band index from the HDF5 file
     * and shifts the remaining bands to close the gap in the sequential numbering. If the provided
     * band index is invalid (i.e., less than 1 or greater than the total number of bands), an exception
     * is thrown. It also handles potential exceptions from the HDF5 operations.
     *
     * @param keaImgH5File A pointer to the HighFive::File object representing the KEA image file.
     *                     This file must have datasets corresponding to the image bands.
     * @param bandIndex    The index of the image band to be removed. The band index must be greater
     *                     than or equal to 1 and less than or equal to the total number of image bands.
     * @param numImgBands  The total number of image bands currently present in the file.
     *
     * @throws KEAIOException If the band index is invalid or any errors occur during the HDF5 file operations,
     *                        such as unlinking or renaming datasets.
     */
    void KEAImageIO::removeImageBandFromFile(
        HighFive::File *keaImgH5File, const uint32_t bandIndex,
        const uint32_t numImgBands
    )
    {
        if ((bandIndex < 1) || (bandIndex > numImgBands))
        {
            throw KEAIOException("Invalid band index");
        }

        try
        {
            std::string bandName = KEA_DATASETNAME_BAND + uint2Str(bandIndex);
            keaImgH5File->unlink(bandName);

            // now rename all the ones with band > bandIndex
            for (uint32_t sourceIndex = bandIndex + 1; sourceIndex <= numImgBands;
                 sourceIndex++)
            {
                std::string srcName = KEA_DATASETNAME_BAND + uint2Str(sourceIndex);
                std::string dstName = KEA_DATASETNAME_BAND + uint2Str(sourceIndex - 1);
                if (!keaImgH5File->rename(srcName, dstName))
                {
                    throw KEAIOException("Failed to rename dataset");
                }
            }
        }
        catch (const HighFive::Exception &e)
        {
            throw KEAIOException(e.what());
        }
        catch (const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }

    /**
     * Updates or creates the metadata entry for the number of image bands in the KEA image file.
     *
     * This method determines if the dataset corresponding to the number of image bands
     * exists in the provided KEA HDF5 file. If it exists, the dataset is updated with the
     * new value. Otherwise, a new dataset is created with the specified value. The method
     * ensures the metadata is consistent and throws appropriate exceptions upon error.
     *
     * @param keaImgH5File A pointer to the HighFive::File object representing the KEA image file.
     *                     This file must be an open HDF5 file capable of being read or written to.
     * @param numImgBands The number of image bands to be set in the metadata. This value
     *                    represents the updated or initial band count for the KEA file.
     *
     * @throws KEAIOException If there is an issue reading or writing the dataset, the file
     *                        structure is invalid, or any other lower-level library exception occurs.
     */
    void KEAImageIO::setNumImgBandsInFileMetadata(
        HighFive::File *keaImgH5File, const uint32_t numImgBands
    )
    {
        try
        {
            if (keaImgH5File->exist(KEA_DATASETNAME_HEADER_NUMBANDS))
            {
                auto numBandsDataset = keaImgH5File->getDataSet(
                    KEA_DATASETNAME_HEADER_NUMBANDS
                );
                numBandsDataset.write(numImgBands);
            }
            else
            {
          		// TODO: dataspace instead of scalar to be compatible with old KEA
                keaImgH5File->createDataSet<uint16_t>(
                    KEA_DATASETNAME_HEADER_NUMBANDS,
                    HighFive::DataSpace({1})
                ).write(numImgBands);
            }
        }
        catch (const HighFive::Exception &e)
        {
            throw kealib::KEAIOException(e.what());
        }
        catch (const KEAIOException &e)
        {
            throw e;
        }
        catch (const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }
   
    
    HighFive::CompoundType KEAImageIO::createGCPCompType()
    {
        try
        {
            std::vector<HighFive::CompoundType::member_def> members;
            members.push_back(HighFive::CompoundType::member_def(KEA_GCPS_PSZID, HighFive::VariableLengthStringType()));
            members.push_back(HighFive::CompoundType::member_def(KEA_GCPS_PSZINFO, HighFive::VariableLengthStringType()));
            members.push_back(HighFive::CompoundType::member_def(KEA_GCPS_DFPIXEL, HighFive::AtomicType<double>()));
            members.push_back(HighFive::CompoundType::member_def(KEA_GCPS_DFLINE, HighFive::AtomicType<double>()));
            members.push_back(HighFive::CompoundType::member_def(KEA_GCPS_DFX, HighFive::AtomicType<double>()));
            members.push_back(HighFive::CompoundType::member_def(KEA_GCPS_DFY, HighFive::AtomicType<double>()));
            members.push_back(HighFive::CompoundType::member_def(KEA_GCPS_DFZ, HighFive::AtomicType<double>()));
            return HighFive::CompoundType(members);
        }
        catch( const HighFive::Exception &e)
        {
            throw kealib::KEAIOException(e.what());
        }
    }
} // namespace libkea

#include "libkea/kea-config.h"
/**
 * Retrieves the current version of the KEA library in numerical format.
 *
 * This method calculates the version of the KEA library by combining the major, minor,
 * and patch version components into a single double. The major version contributes
 * to the integer part, while the minor and patch versions are added as fractions
 * to differentiate patch levels and ensure version precision.
 *
 * @return A double representing the KEA library version, formatted as major.minor.patch.
 */
double get_kealibversion()
{
    return double(LIBKEA_VERSION_MAJOR) + double(LIBKEA_VERSION_MINOR) / 100.0 + 
        double(LIBKEA_VERSION_PATCH) / 10000.0;
}



