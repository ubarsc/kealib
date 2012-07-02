/*
 *  KEAWriter.cpp
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

#include "libkea/KEAWriter.h"

namespace libkea{
    
    KEAWriter::KEAWriter()
    {
        
    }
    
    H5::H5File* KEAWriter::createKEAImage(std::string fileName, KEADataType dataType, unsigned int xSize, unsigned int ySize, unsigned int numImgBands, std::vector<std::string> *bandDescrips, KEAImageSpatialInfo * spatialInfo, unsigned int blockSize)throw(KEAIOException)
    {
        //H5::Exception::dontPrint();
        
        H5::H5File *keaImgFile = NULL;
        
        try 
        {
            // CREATE HDF FILE ACCESS PROPERTIES - VALUES CAN BE TUNED FROM KEACommon.h
            H5::FileAccPropList keaAccessPlist = H5::FileAccPropList(H5::FileAccPropList::DEFAULT);
            keaAccessPlist.setCache(KEA_WRITE_MDC_NELMTS, KEA_WRITE_RDCC_NELMTS, KEA_WRITE_RDCC_NBYTES, KEA_WRITE_RDCC_W0);
            keaAccessPlist.setSieveBufSize(KEA_WRITE_SIEVE_BUF);
            hsize_t metaBlockSize = KEA_WRITE_META_BLOCKSIZE;
            keaAccessPlist.setMetaBlockSize(metaBlockSize);
            
            // CREATE THE HDF FILE - EXISTING FILE WILL BE TRUNCATED
            keaImgFile = new H5::H5File( fileName, H5F_ACC_TRUNC, H5::FileCreatPropList::DEFAULT, keaAccessPlist);
            
            //////////// CREATE GLOBAL HEADER ////////////////
            keaImgFile->createGroup( KEA_DATASETNAME_HEADER );
            
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
            
            // SET COMPRESSION BLOCK SIZE IN GLOBAL HEADER
            hsize_t dimsBlockSize[1];
			dimsBlockSize[0] = 1;
            H5::DataSpace blockSizeDataSpace(1, dimsBlockSize);
            H5::DataSet blockSizeDataset = keaImgFile->createDataSet(KEA_DATASETNAME_HEADER_BLOCKSIZE, H5::PredType::STD_U16LE, blockSizeDataSpace);
			blockSizeDataset.write( &blockSize, H5::PredType::NATIVE_UINT );
            blockSizeDataset.close();
            blockSizeDataSpace.close();
            
            // SET NUMBER OF IMAGE BANDS IN GLOBAL HEADER
            hsize_t dimsNumBands[1];
			dimsNumBands[0] = 1;
            H5::DataSpace numBandsDataSpace(1, dimsNumBands);
            H5::DataSet numBandsDataset = keaImgFile->createDataSet(KEA_DATASETNAME_HEADER_NUMBANDS, H5::PredType::STD_U16LE, numBandsDataSpace);
			numBandsDataset.write( &numImgBands, H5::PredType::NATIVE_UINT );
            numBandsDataset.close();
            numBandsDataSpace.close();
                        
            // SET X AND Y TL IN GLOBAL HEADER
            double *doubleVals = new double[2];
            doubleVals[0] = spatialInfo->tlX;
            doubleVals[1] = spatialInfo->tlY;
            hsize_t dimsSpatialTL[1];
			dimsSpatialTL[0] = 2;
            H5::DataSpace spatialTLDataSpace(1, dimsSpatialTL);
            H5::DataSet spatialTLDataset = keaImgFile->createDataSet(KEA_DATASETNAME_HEADER_TL, H5::PredType::IEEE_F64LE, spatialTLDataSpace);
			spatialTLDataset.write( doubleVals, H5::PredType::IEEE_F64LE );
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
            H5::DataSet spatialResDataset = keaImgFile->createDataSet(KEA_DATASETNAME_HEADER_RES, H5::PredType::IEEE_F32LE, spatialResDataSpace);
			spatialResDataset.write( floatVals, H5::PredType::IEEE_F32LE );
            spatialResDataset.close();
            spatialResDataSpace.close();
            
            // SET X AND Y ROTATION IN GLOBAL HEADER
            floatVals[0] = spatialInfo->xRot;
            floatVals[1] = spatialInfo->yRot;
            hsize_t dimsSpatialRot[1];
			dimsSpatialRot[0] = 2;
            H5::DataSpace spatialRotDataSpace(1, dimsSpatialRot);
            H5::DataSet spatialRotDataset = keaImgFile->createDataSet(KEA_DATASETNAME_HEADER_ROT, H5::PredType::IEEE_F32LE, spatialRotDataSpace);
			spatialRotDataset.write( floatVals, H5::PredType::IEEE_F32LE );
            spatialRotDataset.close();
            spatialRotDataSpace.close();
            delete[] floatVals;
            
            // SET THE WKT STRING SPATAIL REFERENCE IN GLOBAL HEADER
			const char **wStrdata = NULL;
			hsize_t	dimsForStr[1];
			dimsForStr[0] = 1; // number of lines;
            H5::DataSpace dataspaceStrAll(1, dimsForStr);
            H5::StrType strTypeAll(0, H5T_VARIABLE);
            H5::DataSet datasetSpatialReference = keaImgFile->createDataSet(KEA_DATASETNAME_HEADER_WKT, strTypeAll, dataspaceStrAll);
			wStrdata = new const char*[1];
			wStrdata[0] = spatialInfo->wktString.c_str();			
			datasetSpatialReference.write((void*)wStrdata, strTypeAll);
			datasetSpatialReference.close();
			delete[] wStrdata;
            
            if(deleteSpatialInfo)
            {
                delete spatialInfo;
            }
            //////////// CREATED GLOBAL HEADER ////////////////
            
            //////////// CREATE GLOBAL META-DATA ////////////////
			keaImgFile->createGroup( KEA_DATASETNAME_METADATA );
            //////////// CREATED GLOBAL META-DATA ////////////////
            
            
            //////////// CREATE IMAGE BANDS ////////////////
            bool bandDescriptsDefined = true;
            if(bandDescrips == NULL)
            {
                bandDescriptsDefined = false;
            }
            
            H5::DataType imgBandDT = H5::PredType::IEEE_F32LE;
            if(dataType == kea_8int)
            {
                imgBandDT = H5::PredType::STD_I8LE;
            }
            else if(dataType == kea_16int)
            {
                imgBandDT = H5::PredType::STD_I16LE;
            }
            else if(dataType == kea_32int)
            {
                imgBandDT = H5::PredType::STD_I32LE;
            }
            else if(dataType == kea_64int)
            {
                imgBandDT = H5::PredType::STD_I64LE;
            }
            else if(dataType == kea_8uint)
            {
                imgBandDT = H5::PredType::STD_U8LE;
            }
            else if(dataType == kea_16uint)
            {
                imgBandDT = H5::PredType::STD_U16LE;
            }
            else if(dataType == kea_32uint)
            {
                imgBandDT = H5::PredType::STD_U32LE;
            }
            else if(dataType == kea_64uint)
            {
                imgBandDT = H5::PredType::STD_U64LE;
            }
            else if(dataType == kea_32float)
            {
                imgBandDT = H5::PredType::IEEE_F32LE;
            }
            else if(dataType == kea_64float)
            {
                imgBandDT = H5::PredType::IEEE_F64LE;
            }
            else
            {
                throw KEAIOException("The specified data type was not recognised.");
            }
            
            int initFillVal = 0;
            
            hsize_t imageBandDims[2];
            imageBandDims[0] = ySize;
            imageBandDims[1] = xSize;
            H5::DataSpace imgBandDataSpace(2, imageBandDims);
            
            hsize_t dimsImageBandChunk[2];
			dimsImageBandChunk[0] = blockSize;
			dimsImageBandChunk[1] = blockSize;
            
            H5::DSetCreatPropList initParamsImgBand;
			initParamsImgBand.setChunk(2, dimsImageBandChunk);			
			initParamsImgBand.setShuffle();
            initParamsImgBand.setDeflate(KEA_DEFLATE);
			initParamsImgBand.setFillValue( H5::PredType::NATIVE_INT, &initFillVal);
            
            std::string zeroChar = "0";
            std::string bandName = "";
            std::string bandDescrip = "";
            for(unsigned int i = 0; i < numImgBands; ++i)
            {
                bandName = KEA_DATASETNAME_BAND + uint2Str(i+1);
                
                // CREATE IMAGE BAND HDF5 GROUP
                keaImgFile->createGroup( bandName );
                
                
                // CREATE THE IMAGE DATA ARRAY
                H5::DataSet imgBandDataSet = keaImgFile->createDataSet((bandName+KEA_BANDNAME_DATA), imgBandDT, imgBandDataSpace, initParamsImgBand);
                imgBandDataSet.close();
                
                // SET BAND NAME / DESCRIPTION
                if(bandDescriptsDefined && (i < bandDescrips->size()))
                {
                    bandDescrip = bandDescrips->at(i);
                }
                else
                {
                    bandDescriptsDefined = false;
                }
                
                if(!bandDescriptsDefined)
                {
                    bandDescrip = "Band " + uint2Str(i+1);
                }
                
                H5::DataSet datasetBandDescription = keaImgFile->createDataSet((bandName+KEA_BANDNAME_DESCRIP), strTypeAll, dataspaceStrAll);
                wStrdata = new const char*[1];
                wStrdata[0] = bandDescrip.c_str();			
                datasetBandDescription.write((void*)wStrdata, strTypeAll);
                datasetBandDescription.close();
                delete[] wStrdata;
                
                // CREATE META-DATA
                keaImgFile->createGroup( bandName+KEA_BANDNAME_METADATA );
                
                H5::DataSet datasetBandMetaMin = keaImgFile->createDataSet((bandName+KEA_BANDNAME_METADATA_MIN), strTypeAll, dataspaceStrAll);
                wStrdata = new const char*[1];
                wStrdata[0] = zeroChar.c_str();			
                datasetBandMetaMin.write((void*)wStrdata, strTypeAll);
                datasetBandMetaMin.close();
                delete[] wStrdata;
                
                H5::DataSet datasetBandMetaMax = keaImgFile->createDataSet((bandName+KEA_BANDNAME_METADATA_MAX), strTypeAll, dataspaceStrAll);
                wStrdata = new const char*[1];
                wStrdata[0] = zeroChar.c_str();			
                datasetBandMetaMax.write((void*)wStrdata, strTypeAll);
                datasetBandMetaMax.close();
                delete[] wStrdata;
                
                H5::DataSet datasetBandMetaMean = keaImgFile->createDataSet((bandName+KEA_BANDNAME_METADATA_MEAN), strTypeAll, dataspaceStrAll);
                wStrdata = new const char*[1];
                wStrdata[0] = zeroChar.c_str();			
                datasetBandMetaMean.write((void*)wStrdata, strTypeAll);
                datasetBandMetaMean.close();
                delete[] wStrdata;
                
                H5::DataSet datasetBandMetaStdDev = keaImgFile->createDataSet((bandName+KEA_BANDNAME_METADATA_STDDEV), strTypeAll, dataspaceStrAll);
                wStrdata = new const char*[1];
                wStrdata[0] = zeroChar.c_str();			
                datasetBandMetaStdDev.write((void*)wStrdata, strTypeAll);
                datasetBandMetaStdDev.close();
                delete[] wStrdata;
                
                H5::DataSet datasetBandMetaWavelength = keaImgFile->createDataSet((bandName+KEA_BANDNAME_METADATA_WAVELENGTH), strTypeAll, dataspaceStrAll);
                wStrdata = new const char*[1];
                wStrdata[0] = zeroChar.c_str();			
                datasetBandMetaWavelength.write((void*)wStrdata, strTypeAll);
                datasetBandMetaWavelength.close();
                delete[] wStrdata;
                
                H5::DataSet datasetBandMetaFWHM = keaImgFile->createDataSet((bandName+KEA_BANDNAME_METADATA_FWHM), strTypeAll, dataspaceStrAll);
                wStrdata = new const char*[1];
                wStrdata[0] = zeroChar.c_str();			
                datasetBandMetaFWHM.write((void*)wStrdata, strTypeAll);
                datasetBandMetaFWHM.close();
                delete[] wStrdata;
                
                
                // CREATE OVERVIEWS GROUP
                keaImgFile->createGroup( bandName+KEA_BANDNAME_OVERVIEWS );
                
                // CREATE ATTRIBUTE TABLE GROUP
                keaImgFile->createGroup( bandName+KEA_BANDNAME_ATT );
                keaImgFile->createGroup( bandName+KEA_ATT_GROUPNAME_DATA );
                keaImgFile->createGroup( bandName+KEA_ATT_GROUPNAME_NEIGHBOURS );
                keaImgFile->createGroup( bandName+KEA_ATT_GROUPNAME_HEADER );
                
                // SET ATTRIBUTE TABLE SIZE
                int attSize = 0;
                hsize_t dimsAttSize[1];
                dimsAttSize[0] = 1;
                H5::DataSpace attSizeDataSpace(1, dimsAttSize);
                H5::DataSet attSizeDataset = keaImgFile->createDataSet((bandName+KEA_ATT_SIZE_HEADER), H5::PredType::STD_U64LE, attSizeDataSpace);
                attSizeDataset.write( &attSize, H5::PredType::NATIVE_INT );
                attSizeDataset.close();
                attSizeDataSpace.close();
            }
            dataspaceStrAll.close();
            //////////// CREATED IMAGE BANDS ////////////////
        
            
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
        
        return keaImgFile;
    }

    KEAWriter::~KEAWriter()
    {
        
    }
}





