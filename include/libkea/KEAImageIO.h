/*
 *  KEAImageIO.h
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
 
/*! @mainpage KeaLib Developer Documentation
 *
 * Welcome to the KeaLib Developer Documentation
 *
 * The main class to start  with is \ref kealib::KEAImageIO "KEAImageIO".
 */
 
#ifndef KEAImageIO_H
#define KEAImageIO_H

#include <iostream>
#include <string>
#include <vector>

#include <highfive/highfive.hpp>

#include "libkea/KEACommon.h"
#include "libkea/KEAException.h"
#include "libkea/KEAAttributeTable.h"
#include "libkea/KEAAttributeTableInMem.h"
#include "libkea/KEAAttributeTableFile.h"

namespace kealib{
        
    class KEA_EXPORT KEAImageIO : public KEABase
    {
    public:
        KEAImageIO();
                
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
        void openKEAImageHeader(HighFive::File *keaImgH5File);
        
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
        void writeImageBlock2Band(uint32_t band, void *data, uint64_t xPxlOff, uint64_t yPxlOff, uint64_t xSizeOut, uint64_t ySizeOut, uint64_t xSizeBuf, uint64_t ySizeBuf, KEADataType inDataType);
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
        void readImageBlock2Band(uint32_t band, void *data, uint64_t xPxlOff, uint64_t yPxlOff, uint64_t xSizeIn, uint64_t ySizeIn, uint64_t xSizeBuf, uint64_t ySizeBuf, KEADataType inDataType);
        
        /**
         * Creates a mask band
         * 
         * This method creates a mask band for the specified band
         *
         * @param band    The band to create a mask band for. 1-based.
         * @param deflate The level of compression to use
         * 
         * @throws KEAIOException If there is a problem creating the mask band
         */
        void createMask(uint32_t band, uint32_t deflate=KEA_DEFLATE);
        
        /**
         * Writes data to the mask band
         *
         * Once a mask band has been created with createMask(), use this method to write data to it.
         * 
         * @param band The one-based index of the band with the mask band to which the data will be written. Band 1 is the first band.
         * @param data A pointer to the memory containing the image data to be written.
         * @param xPxlOff The horizontal pixel offset in the image where the data block starts.
         * @param yPxlOff The vertical pixel offset in the image where the data block starts.
         * @param xSizeOut The horizontal size of the output image data block to be written.
         * @param ySizeOut The vertical size of the output image data block to be written.
         * @param xSizeBuf The horizontal size of the provided data buffer.
         * @param ySizeBuf The vertical size of the provided data buffer.
         * @param inDataType The data type of the input image data, specified using KEADataType.
         * 
         * @throws KEAIOException 
         */ 
        void writeImageBlock2BandMask(uint32_t band, void *data, uint64_t xPxlOff, uint64_t yPxlOff, uint64_t xSizeOut, uint64_t ySizeOut, uint64_t xSizeBuf, uint64_t ySizeBuf, KEADataType inDataType);
        
        /**
         * Reads data from the mask band
         *
         * Once a mask band has been created with createMask(), use this method to read data from it.
         *
         * @param band       The band number with the mask band to read from. Band numbers start at 1 and must not exceed
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
         * @throws KEAIOException 
         */
        void readImageBlock2BandMask(uint32_t band, void *data, uint64_t xPxlOff, uint64_t yPxlOff, uint64_t xSizeIn, uint64_t ySizeIn, uint64_t xSizeBuf, uint64_t ySizeBuf, KEADataType inDataType);
        /**
         * Determines whether a mask band has been created for the specified band
         *
         * @param band  The 1-based band number to check the existance of a mask band for
         * @throws KEAIOException 
         */
        bool maskCreated(uint32_t band);

        /**
         * Set an image metadata value
         *
         * @param name  The name of the metadata item to set
         * @param value The value to set the metadata item to
         * @throws KEAIOException
         */        
        void setImageMetaData(const std::string &name, const std::string &value);
        /**
         * Get an image metadata value
         *
         * @param name The name of the metadata item to retrieve
         * @throws KEAIOException
         */        
        std::string getImageMetaData(const std::string &name);
        /**
         * Get all the names of the image metadata items
         * @throws KEAIOException
         */
        std::vector<std::string> getImageMetaDataNames();
        /**
         * Get all the image metadata names and values as a vector of pairs
         * @throws KEAIOException
         */
        std::vector< std::pair<std::string, std::string> > getImageMetaData();
        /**
         * Set all the image metadata names and values at once
         * @param data a vector of pairs describing the metadata
         * @throws KEAIOException
         */
        void setImageMetaData(const std::vector< std::pair<std::string, std::string> > &data);

        /**
         * Set a band metadata value
         *
         * @param band  1-based index of the band to set metadata
         * @param name  The name of the metadata item to set
         * @param value The value to set the metadata item to
         * @throws KEAIOException
         */        
        void setImageBandMetaData(uint32_t band, const std::string &name, const std::string &value);
        /**
         * Get an band metadata value
         *
         * @param band  1-based index of the band to get metadata
         * @param name The name of the metadata item to retrieve
         * @throws KEAIOException
         */        
        std::string getImageBandMetaData(uint32_t band, const std::string &name);
        /**
         * Get all the names of the band metadata items
         *
         * @param band  1-based index of the band to get metadata
         * @throws KEAIOException
         */
        std::vector<std::string> getImageBandMetaDataNames(uint32_t band);
        /**
         * Get all the band metadata names and values as a vector of pairs
         * @param band  1-based index of the band to set metadata
         * @throws KEAIOException
         */
        std::vector< std::pair<std::string, std::string> > getImageBandMetaData(uint32_t band);
        /**
         * Set all the band metadata names and values at once
         * @param band  1-based index of the band to set metadata
         * @param data a vector of pairs describing the metadata
         * @throws KEAIOException
         */
        void setImageBandMetaData(uint32_t band, const std::vector< std::pair<std::string, std::string> > &data);
        
        /**
         * Set the description of an image band to a string
         *
         * @param band          1-based index of the band to set description
         * @param description   string to set as the description
         * @throws KEAIOException
         */
        void setImageBandDescription(uint32_t band, const std::string &description);
        /**
         * Get the description of an image band as a string
         *
         * @param band          1-based index of the band to set description
         * @throws KEAIOException
         */
        std::string getImageBandDescription(uint32_t band);

        /**
         * Set the no data value for a band
         * 
         * @param band          1-based index of the band to set the no data
         * @param data          pointer to the nodata value
         * @param inDataType    data type of the no data value passed
         * @throws KEAIOException
         */         
        void setNoDataValue(uint32_t band, const void *data, KEADataType inDataType);
        /**
         * Get the no data value for a band
         * 
         * @param band          1-based index of the band to get the no data
         * @param data          pointer that receives the no data
         * @param inDataType    data type to read the no data as
         * @throws KEAIOException if the nodata has not been set for this band
         */         
        void getNoDataValue(uint32_t band, void *data, KEADataType inDataType);
        /**
         * Unset the no data for a band
         *
         * @param band          1-based index of the band to unset the nodata
         * @throws KEAIOException if the nodata has not been set for this band
         */
        void undefineNoDataValue(uint32_t band);
        
        /**
         * Get the GCPs for the image 
         *
         * @throws KEAIOException 
         */
        std::vector<KEAImageGCP*>* getGCPs();
        /** 
         * Set the GCPs for the image
         *
         * @throws KEAIOException 
         */
        void setGCPs(std::vector<KEAImageGCP*> *gcps, const std::string &projWKT);
        /**
         * Get the count of the GCPs saved for this image
         * 
         * @throws KEAIOException 
         */
        uint32_t getGCPCount();
        /**
         * Get the projection of the saved CGPs as a WKT string
         *
         * @throws KEAIOException 
         */
        std::string getGCPProjection();
        /**
         * Set the projection of the saved CGPs as a WKT string
         *
         * @param projWKT a WKT string defining the projection
         * @throws KEAIOException 
         */
        void setGCPProjection(const std::string &projWKT);
        
        /**
         * Set the spatial info for the dataset
         *
         * @param spatialInfo a pointer to the spatial info struct to set for this dataset
         * @throws KEAIOException 
         */
        void setSpatialInfo(KEAImageSpatialInfo *spatialInfo);
        /**
         * Get the spatial info for the dataset
         */
        KEAImageSpatialInfo* getSpatialInfo();
           
        /**
         * Get the number of bands of this image
         */     
        uint32_t getNumOfImageBands();
        
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
        uint32_t getImageBlockSize(uint32_t band);
        
        /**
         * Get the KEA data type for the specified band
         *
         * @param band 1-based index of the band
         * @throws KEAIOException
         */
        KEADataType getImageBandDataType(uint32_t band);
        
        /**
         * Get the version of this KEA file
         *
         * @throws KEAIOException if file not open
         */
        std::string getKEAImageVersion();
        
        /**
         * Set the layer type for an image band (thematic or continuous)
         * 
         * @param band    1-based index of the band to set the layer type
         * @param imgLayerType the layer type to set
         * @throws KEAIOException
         */
        void setImageBandLayerType(uint32_t band, KEALayerType imgLayerType);
        /**
         * Get the layer type (thematic or continuous) for the image band
         * @param band    1-based index of the band to get the layer type for
         * @throws KEAIOException
         */ 
        KEALayerType getImageBandLayerType(uint32_t band);

        /**
         * Set the colour interpretation for the image band
         *
         * @param band    1-based index of the band to set the colour interpretation
         * @param imgLayerClrInterp the colur interpretation to set
         * @throws KEAIOException
         */        
        void setImageBandClrInterp(uint32_t band, KEABandClrInterp imgLayerClrInterp);
        /**
         * Get the colour interpretation for the image band
         *
         * @param band    1-based index of the band to get the colour interpretation
         * @throws KEAIOException
         */        
        KEABandClrInterp getImageBandClrInterp(uint32_t band);

        /**
         * Create an overview for an image band
         *
         * @param band      1-based index of the image band
         * @param overview  0-based overview level to create
         * @param xSize     The X size of the overview in pixels        
         * @param ySize     The X size of the overview in pixels
         * @throws KEAIOException
         */        
        void createOverview(uint32_t band, uint32_t overview, uint64_t xSize, uint64_t ySize);
        /**
         * Remove an overview for an image band
         *
         * @param band      1-based index of the image band
         * @param overview  0-based overview level to remove
         * @throws KEAIOException
         */        
        void removeOverview(uint32_t band, uint32_t overview);
        /**
         * Get the block size used for an overview
         *
         * @param band  1-based index of image band
         * @param overview  0-based overview level
         * @throws KEAIOException
         */
        uint32_t getOverviewBlockSize(uint32_t band, uint32_t overview);
        /**
         * Write data to an overview
         *
         * @param band  1-based index of image band
         * @param overview  0-based overview level
         * @param data A pointer to the memory containing the image data to be written.
         * @param xPxlOff The horizontal pixel offset in the image where the data block starts.
         * @param yPxlOff The vertical pixel offset in the image where the data block starts.
         * @param xSizeOut The horizontal size of the output image data block to be written.
         * @param ySizeOut The vertical size of the output image data block to be written.
         * @param xSizeBuf The horizontal size of the provided data buffer.
         * @param ySizeBuf The vertical size of the provided data buffer.
         * @param inDataType The data type of the input image data, specified using KEADataType.
         * @throws KEAIOException
         */
        void writeToOverview(uint32_t band, uint32_t overview, void *data, uint64_t xPxlOff, uint64_t yPxlOff, uint64_t xSizeOut, uint64_t ySizeOut, uint64_t xSizeBuf, uint64_t ySizeBuf, KEADataType inDataType);
        /**
         * Read data from an overview
         *
         * @param band  1-based index of image band
         * @param overview  0-based overview level
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
         * @throws KEAIOException
         */
        void readFromOverview(uint32_t band, uint32_t overview, void *data, uint64_t xPxlOff, uint64_t yPxlOff, uint64_t xSizeIn, uint64_t ySizeIn, uint64_t xSizeBuf, uint64_t ySizeBuf, KEADataType inDataType);
        /**
         * Get the number of overviews for an image band
         *
         * @param band 1-based index of image band
         * @throws KEAIOException
         */
        uint32_t getNumOfOverviews(uint32_t band);
        /**
         * Get the size of an overview
         *
         * @param band  1-based index of image band
         * @param overview  0-based overview level
         * @param xSize pointer to where the X size of the overview will be stored
         * @param ySize pointer to where the Y size of the overview will be stored
         * @throws KEAIOException
         */
        void getOverviewSize(uint32_t band, uint32_t overview, uint64_t *xSize, uint64_t *ySize);

        /**
         * Get the attribute table for an image band
         * 
         * @param type Type of attribute table object to return - in mem or file
         * @param band  1-based index of image band
         * @throws KEAIOException
         */
        KEAAttributeTable* getAttributeTable(KEAATTType type, uint32_t band);
        /**
         * Set an attribute table for an image band
         *
         * @param att pointer to the attribute table object
         * @param band  1-based index of image band
         * @param chunkSize checksize to use when writing the table
         * @param deflate deflate level to use
         * @throws KEAIOException
         */
        void setAttributeTable(KEAAttributeTable* att, uint32_t band, uint32_t chunkSize=KEA_ATT_CHUNK_SIZE, uint32_t deflate=KEA_DEFLATE);
        /**
         * Check whether an attriute table is present for an image band
         *
         * @param band  1-based index of image band
         * @throws KEAIOException
         */
        bool attributeTablePresent(uint32_t band);
        /**
         * Get the chunk size for the attribute table
         * @param band  1-based index of image band
         * @throws KEAIOException
         */
        uint32_t getAttributeTableChunkSize(uint32_t band);

        /**
         * Close file and flush all buffers
         * @throws KEAIOException
         */
        void close();

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
         * @param deflate ThebandDescripIn compression level (0 for no compression, higher values for increasing compression).
         *
         * @throws KEAIOException If the image file is not open or issues occur during the band addition process.
         */
        virtual void addImageBand(const KEADataType dataType, const std::string &bandDescrip, const uint32_t imageBlockSize = KEA_IMAGE_CHUNK_SIZE, const uint32_t attBlockSize = KEA_ATT_CHUNK_SIZE, const uint32_t deflate = KEA_DEFLATE);
        
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
        virtual void removeImageBand(const uint32_t bandIndex);

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
        static HighFive::File* createKEAImage(const std::string &fileName, KEADataType dataType, uint32_t xSize, uint32_t ySize, uint32_t numImgBands, std::vector<std::string> *bandDescrips=NULL, KEAImageSpatialInfo *spatialInfo=NULL, uint32_t imageBlockSize=KEA_IMAGE_CHUNK_SIZE, uint32_t attBlockSize=KEA_ATT_CHUNK_SIZE, int mdcElmts=KEA_MDC_NELMTS, hsize_t rdccNElmts=KEA_RDCC_NELMTS, hsize_t rdccNBytes=KEA_RDCC_NBYTES, double rdccW0=KEA_RDCC_W0, hsize_t sieveBuf=KEA_SIEVE_BUF, hsize_t metaBlockSize=KEA_META_BLOCKSIZE, uint32_t deflate=KEA_DEFLATE);
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
        static bool isKEAImage(const std::string &fileName);
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
        static HighFive::File* openKeaH5RW(const std::string &fileName, int mdcElmts=KEA_MDC_NELMTS, hsize_t rdccNElmts=KEA_RDCC_NELMTS, hsize_t rdccNBytes=KEA_RDCC_NBYTES, double rdccW0=KEA_RDCC_W0, hsize_t sieveBuf=KEA_SIEVE_BUF, hsize_t metaBlockSize=KEA_META_BLOCKSIZE);
        /**
         * Opens a KEA HDF5 image file in read-only mode and returns a pointer to the file object.
         *
         * This method attempts to open the specified KEA image file in a read-only mode using the HighFive
         * library. It initializes default file access properties and sets up the file structure while
         * handling potential exceptions. File caching and property optimizations, such as metadata cache
         * configuration or sieve buffer size, may be configured but are currently commented out.
         *
         * @param fileName The path to the KEA HDF5 image file that needs to be opened.
         * @param mdcElmts Number of elements in the metadata cache.
         * @param rdccNElmts Number of elements in the raw data chunk cache.
         * @param rdccNBytes Total size of the raw data chunk cache in bytes.
         * @param rdccW0 The preemption policy value for raw data chunk cache.
         * @param sieveBuf Size of the data sieve buffer in bytes.
         * @param metaBlockSize Size of the metadata block allocation in bytes.
         * @param driver_id ID of HDF5 virtual filesystem driver
         * @param driver_info a pointer to be passed to the virtual filesystem driver
         *
         * @return A pointer to a HighFive::File object representing the opened KEA HDF5 image file.
         *         The file is opened in a read-only mode.
         *
         * @throws KEAIOException If the file is not a valid KEA file, the file does not exist,
         *                        or any error occurs while opening the file or initializing the file properties.
         */
        static HighFive::File* openKeaH5RDOnly(const std::string &fileName, int mdcElmts=KEA_MDC_NELMTS, hsize_t rdccNElmts=KEA_RDCC_NELMTS, 
            hsize_t rdccNBytes=KEA_RDCC_NBYTES, double rdccW0=KEA_RDCC_W0, hsize_t sieveBuf=KEA_SIEVE_BUF, 
            hsize_t metaBlockSize=KEA_META_BLOCKSIZE, hid_t driver_id=0, const void* driver_info=nullptr);
        virtual ~KEAImageIO();
        
        /**
         * Helper method to get a HighFive::CompoundType for the GCP data type
         * @throws KEAIOException
         **/
        static HighFive::CompoundType createGCPCompType();

    protected:
        /********** STATIC PROTECTED **********/
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
        static HighFive::DataType convertDatatypeKeaToH5STD(const KEADataType dataType);

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
        static HighFive::DataType convertDatatypeKeaToH5Native( const KEADataType dataType);

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
        static const std::string convertDatatypeKeaToCStdStr(const KEADataType dataType);

        /**
         * Adds a new image band to the specified KEA image file with the given parameters.
         *
         * This method creates a new HDF5 group and datasets within the KEA image file to represent an image band.
         * Metadata and attributes such as the band description, block size, data type, band type, band usage,
         * and other properties are also initialized. The method ensures proper dataset and attribute creation
         * while supporting compression and chunking for efficient storage.
         * Does NOT flush the buffer
         *
         * @param keaImgH5File A pointer to the HighFive::File object representing the open KEA image file.
         * @param dataType The data type for the image data within the band, specified as a KEADataType.
         * @param xSize The width of the image band in pixels.
         * @param ySize The height of the image band in pixels.
         * @param bandIndex The 1-based index of the band being added to the file.
         * @param bandDescrip An optional description of the band. If empty, a default name is used.
         * @param imageBlockSize The block size to use for chunking the image data. Adjusted if exceeding minimum dimension.
         * @param attBlockSize The block size to use for attribute table chunking.
         * @param deflate The compression level to use for deflating data (0-9, where higher values indicate stronger compression).
         *
         * @throws KEAIOException If an error occurs while creating groups/datasets, writing attributes, or accessing metadata.
         */
        static void addImageBandToFile(HighFive::File *keaImgH5File, const KEADataType dataType, const uint32_t xSize, const uint32_t ySize, const uint32_t bandIndex, const std::string &bandDescrip, const uint32_t imageBlockSize, const uint32_t attBlockSize, const uint32_t deflate);
        
        /**
         * Removes a specified image band from the KEA image file and renames the remaining bands.
         *
         * This method deletes the dataset corresponding to the specified band index from the HDF5 file
         * and shifts the remaining bands to close the gap in the sequential numbering. If the provided
         * band index is invalid (i.e., less than 1 or greater than the total number of bands), an exception
         * is thrown. It also handles potential exceptions from the HDF5 operations.
         * Does NOT flush the buffer
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
        static void removeImageBandFromFile(HighFive::File *keaImgH5File, const uint32_t bandIndex, const uint32_t numImgBands);

        /**
         * Updates or creates the metadata entry for the number of image bands in the KEA image file.
         *
         * This method determines if the dataset corresponding to the number of image bands
         * exists in the provided KEA HDF5 file. If it exists, the dataset is updated with the
         * new value. Otherwise, a new dataset is created with the specified value. The method
         * ensures the metadata is consistent and throws appropriate exceptions upon error.
         * Does NOT flush the buffer
         *
         * @param keaImgH5File A pointer to the HighFive::File object representing the KEA image file.
         *                     This file must be an open HDF5 file capable of being read or written to.
         * @param numImgBands The number of image bands to be set in the metadata. This value
         *                    represents the updated or initial band count for the KEA file.
         *
         * @throws KEAIOException If there is an issue reading or writing the dataset, the file
         *                        structure is invalid, or any other lower-level library exception occurs.
         */
        static void setNumImgBandsInFileMetadata(HighFive::File *keaImgH5File, const uint32_t numImgBands);

        /**
          * helper to read part of an image from a HDF5 dataset
          *
          * This is an internal method designed to read imagery, overviews and masks
          * out of an HighFive::DataSet object,
          *
          * @param dataset A reference to a HighFive::DataSet containing imagery to read
          * @param band 1-based index of the image band to read
          * @param data A pointer to the memory to read the data into
          * @param xPxlOff The horizontal pixel offset in the image where the data block starts.
          * @param yPxlOff The vertical pixel offset in the image where the data block starts.
          * @param xSizeIn The horizontal size of the output image data block to be written.
          * @param ySizeIn The vertical size of the output image data block to be written.
          * @param xSizeBuf The horizontal size of the provided data buffer.
          * @param ySizeBuf The vertical size of the provided data buffer.
          * @param inDataType The data type of the input image data, specified using KEADataType.
          * @param ismask A flag determining whether kealib should look for an ignore value - only done if !ismask
          *
          * @throws KEAIOException If there is a problem reading from the dataset
          */        
        void readImageFromDataset(const HighFive::DataSet &dataset, uint32_t band,
            void *data, uint64_t xPxlOff, uint64_t yPxlOff, uint64_t xSizeIn,
            uint64_t ySizeIn, uint64_t xSizeBuf, uint64_t ySizeBuf, KEADataType inDataType,
            bool ismask=false);
            
        /**
          * helper to write part of an image form a HDF5 dataset
          *
          * This is an internal method designed to read imagery, overviews and masks
          * out of an HighFive::DataSet object,
          *
          * @param dataset A reference to a HighFive::DataSet containing imagery to read
          * @param data A pointer to the memory containing the image data to be written.
          * @param xPxlOff The horizontal pixel offset in the image where the data block starts.
          * @param yPxlOff The vertical pixel offset in the image where the data block starts.
          * @param xSizeOut The horizontal size of the output image data block to be written.
          * @param ySizeOut The vertical size of the output image data block to be written.
          * @param xSizeBuf The horizontal size of the provided data buffer.
          * @param ySizeBuf The vertical size of the provided data buffer.
          * @param inDataType The data type of the input image data, specified using KEADataType.
          *
          * @throws KEAIOException If there is a problem writing to the dataset
          */        
        void writeImageToDataset(HighFive::DataSet &dataset, 
            void *data, uint64_t xPxlOff, uint64_t yPxlOff, uint64_t xSizeOut,
            uint64_t ySizeOut, uint64_t xSizeBuf, uint64_t ySizeBuf, KEADataType inDataType);


        
        //static std::string readString(H5::DataSet& dataset, H5::DataType strDataType);
        
        /********** PROTECTED MEMBERS **********/
        bool fileOpen;
        HighFive::File *keaImgFile;
        KEAImageSpatialInfo *spatialInfoFile;
        uint32_t numImgBands;
        std::string keaVersion;
    };
    
}

// returns the current KEA version as a double
extern "C" KEA_EXPORT double get_kealibversion();

#endif




