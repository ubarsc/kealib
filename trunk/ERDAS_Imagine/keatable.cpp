/*
 *  keatable.cpp
 *  LibKEA
 *
 *  Created by Sam Gillingham on 26/07/2013.
 *  Copyright 2013 LibKEA. All rights reserved.
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

#include "kea.h"

//#define KEADEBUG 1

// these are the special column names that Imagine uses
#define COLUMN_RED "Red"
#define COLUMN_GREEN "Green"
#define COLUMN_BLUE "Blue"
#define COLUMN_OPACITY "Opacity"
#define COLUMN_ALPHA "Alpha" // Imagine prefers Opacity
#define COLUMN_CLASSNAMES "Class_Names"
#define COLUMN_HISTOGRAM "Histogram"

#define MAXSIZE_BLOCK 1000

long
keaTableOpen(void *fileHandle, char *tableName, unsigned long *numRows, void **tableHandle)
{
#ifdef KEADEBUG
    fprintf(stderr, "%s %p %s\n", __FUNCTION__, fileHandle, tableName );
#endif
    // tableName will be in the form:
    // :LayerName:Descriptor_Table
    // or:
    // :LayerName:OverviewName:Descriptor_Table

    // we will assume fileHandle is always a KEA_File - seems to be the case
    KEA_File *pKEAFile = (KEA_File*)fileHandle;
    kealib::KEAAttributeTable *pKEATable = NULL;
    char *pszLastColon = strrchr(tableName, ':');
    if( pszLastColon != NULL )
    {
        //fprintf( stderr, "Table name: %s\n", pszLastColon+1);
        // Imagine always asks for this one, so perhaps don't need to check
        if( strcmp(pszLastColon+1, "Descriptor_Table") == 0 )
        {
            int nNameLen = pszLastColon - tableName;
            char *pszLayerName = (char*)malloc((nNameLen + 1) * sizeof(char));
            strncpy(pszLayerName, &tableName[1], nNameLen - 1);
            pszLayerName[nNameLen - 1] = '\0';

            unsigned long dtype, width, height, bWidth, bHeight, compression;
            KEA_Layer *pKEALayer;
            if( keaLayerOpen(fileHandle, pszLayerName, &dtype, &width, &height, 
                                &compression, &bWidth, &bHeight, (void**)&pKEALayer) == 0 )
            {
                if( pKEALayer->pImageIO->attributeTablePresent(pKEALayer->nBand) )
                {
                    try
                    {
                        pKEATable = pKEALayer->pImageIO->getAttributeTable(kealib::kea_att_file, pKEALayer->nBand);
                        *numRows = pKEATable->getSize();
                        //fprintf( stderr, "numrows = %ld\n", *numRows);
                    }
                    catch(kealib::KEAException &e)
                    {
                        fprintf(stderr, "Error in %s: %s\n", __FUNCTION__, e.what());
                        delete pKEATable;
                        pKEATable = NULL;
                    }
                }
                else
                {
                    //fprintf( stderr, "No RAT present\n" );
                }
                // does nothing, but for completeness
                // Note we keep the RAT around so not sure this is right anyway
                keaLayerClose(pKEALayer);
            }
            else
            {
                fprintf( stderr, "Unable to find layer %s when looking for table\b", pszLayerName);
            }
            free(pszLayerName);
        }
    }
#ifdef KEADEBUG
    fprintf( stderr, "%s returning %p\n", __FUNCTION__, pKEATable);
#endif
    *tableHandle = pKEATable;
    return 0;
}

long
keaTableClose(void *tableHandle)
{
#ifdef KEADEBUG
    fprintf(stderr, "%s %p\n", __FUNCTION__, tableHandle );
#endif
    kealib::KEAAttributeTable *pKEATable = (kealib::KEAAttributeTable*)tableHandle;
    delete pKEATable;

    return 0;
}

long
keaTableColumnNamesGet(void *tableHandle, unsigned long *count, char ***columnNames)
{
#ifdef KEADEBUG
    fprintf(stderr, "%s %p\n", __FUNCTION__, tableHandle );
#endif
    kealib::KEAAttributeTable *pKEATable = (kealib::KEAAttributeTable*)tableHandle;

    std::vector<std::string> colNames = pKEATable->getFieldNames();
    *count = colNames.size();
    *columnNames = emsc_New(*count, char *);

    // Do it in the Imagine preferred order
    unsigned long nIdx = 0;
    std::vector<std::string>::iterator itr;

    itr = std::find(colNames.begin(), colNames.end(), COLUMN_HISTOGRAM);
    if( itr != colNames.end() )
    {
        (*columnNames)[nIdx] = estr_Duplicate((char*)COLUMN_HISTOGRAM);
        colNames.erase(itr);
        nIdx++;
#ifdef KEADEBUG
        fprintf( stderr, "Returning column (%s)\n", COLUMN_HISTOGRAM);
#endif
    }
    itr = std::find(colNames.begin(), colNames.end(), COLUMN_RED);
    if( itr != colNames.end() )
    {
        (*columnNames)[nIdx] = estr_Duplicate((char*)COLUMN_RED);
        colNames.erase(itr);
        nIdx++;
#ifdef KEADEBUG
        fprintf( stderr, "Returning column (%s)\n", COLUMN_RED);
#endif
    }
    itr = std::find(colNames.begin(), colNames.end(), COLUMN_GREEN);
    if( itr != colNames.end() )
    {
        (*columnNames)[nIdx] = estr_Duplicate((char*)COLUMN_GREEN);
        colNames.erase(itr);
        nIdx++;
#ifdef KEADEBUG
        fprintf( stderr, "Returning column (%s)\n", COLUMN_GREEN);
#endif
    }
    itr = std::find(colNames.begin(), colNames.end(), COLUMN_BLUE);
    if( itr != colNames.end() )
    {
        (*columnNames)[nIdx] = estr_Duplicate((char*)COLUMN_BLUE);
        colNames.erase(itr);
        nIdx++;
#ifdef KEADEBUG
        fprintf( stderr, "Returning column (%s)\n", COLUMN_BLUE);
#endif
    }
    itr = std::find(colNames.begin(), colNames.end(), COLUMN_ALPHA);
    if( itr != colNames.end() )
    {
        // call it Opacity for Imagine
        (*columnNames)[nIdx] = estr_Duplicate((char*)COLUMN_OPACITY);
        colNames.erase(itr);
        nIdx++;
#ifdef KEADEBUG
        fprintf( stderr, "Returning column (%s)\n", COLUMN_OPACITY);
#endif
    }
    itr = std::find(colNames.begin(), colNames.end(), COLUMN_CLASSNAMES);
    if( itr != colNames.end() )
    {
        (*columnNames)[nIdx] = estr_Duplicate((char*)COLUMN_CLASSNAMES);
        colNames.erase(itr);
        nIdx++;
#ifdef KEADEBUG
        fprintf( stderr, "Returning column (%s)\n", COLUMN_CLASSNAMES);
#endif
    }

    // do the rest of the attributes
    for( std::vector<std::string>::iterator itr = colNames.begin(); itr != colNames.end(); itr++)
    {
        std::string sVal = (*itr);
        (*columnNames)[nIdx] = estr_Duplicate((char*)sVal.c_str());
#ifdef KEADEBUG
        fprintf( stderr, "Returning column (%s)\n", sVal.c_str());
#endif
        nIdx++;
    }

    return 0;
}

long 
keaTableRowCountGet(void *tableHandle, unsigned long *rowCount)
{
#ifdef KEADEBUG
    fprintf(stderr, "%s %p\n", __FUNCTION__, tableHandle);
#endif
    kealib::KEAAttributeTable *pKEATable = (kealib::KEAAttributeTable*)tableHandle;
    *rowCount = pKEATable->getSize();

    return 0;
}

long 
keaColumnOpen(void *tableHandle, char *columnName, unsigned long *dataType, 
                        unsigned long *maxStringLength, void **columnHandle)
{
#ifdef KEADEBUG
    fprintf(stderr, "%s %p %s\n", __FUNCTION__, tableHandle, columnName );
#endif
    kealib::KEAAttributeTable *pKEATable = (kealib::KEAAttributeTable*)tableHandle;
    long rCode = -1;

    try
    {
        std::string sColumnName = columnName;
        if( sColumnName == COLUMN_OPACITY )
            sColumnName = COLUMN_ALPHA; // KEA uses Alpha

        kealib::KEAFieldDataType ktype = pKEATable->getDataFieldType(sColumnName);
        size_t nColIdx = pKEATable->getFieldIndex(sColumnName);

        KEA_Column *pKEAColumn = new KEA_Column();
        pKEAColumn->pKEATable = pKEATable;
        pKEAColumn->nColIdx = nColIdx;
        pKEAColumn->eType = ktype;
        //fprintf( stderr, "col type = %d\n", ktype);

        if( ( sColumnName == COLUMN_RED ) || ( sColumnName == COLUMN_GREEN ) || 
            ( sColumnName == COLUMN_BLUE ) || ( sColumnName == COLUMN_ALPHA ) )
        {
            //fprintf( stderr, "bTreatIntAsFloat = true\n" );
            pKEAColumn->bTreatIntAsFloat = true;
            *dataType = (unsigned long)kealib::kea_att_float;
        }
        else
        {
            //fprintf( stderr, "bTreatIntAsFloat = false\n" );
            pKEAColumn->bTreatIntAsFloat = false;
            *dataType = (unsigned long)ktype; // see keaInstanceColumnTypesGet
        }

        if( ktype == kealib::kea_att_string )
        {
            // need to work out max length - should really do this in 'blocks'
            unsigned long nMaxString = 0;
            size_t numRows = pKEATable->getSize();
			std::vector<std::string> aStrings;

            for( size_t nRowIndex = 0; nRowIndex < numRows; nRowIndex += MAXSIZE_BLOCK)
            {
                size_t len = MAXSIZE_BLOCK;
                if( ( nRowIndex + len ) > numRows )
				{
                    len = numRows - nRowIndex;
				}

                pKEATable->getStringFields(nRowIndex, len, nColIdx, &aStrings);

                for( size_t n = 0; n < len; n++ )
                {
                    unsigned int nNewLen = aStrings[n].length() + 1;
                    if( nNewLen > nMaxString)
                        nMaxString = nNewLen;
                }
            }
            //fprintf(stderr, "Max string size = %ld\n", nMaxString);
            *maxStringLength = nMaxString;
        }

        *columnHandle = pKEAColumn;
        rCode = 0;
    }
    catch(kealib::KEAException &e)
    {
        fprintf(stderr, "Exception raised in %s: %s\n", __FUNCTION__, e.what());
    }

    return rCode;
}

long 
keaColumnClose(void *columnHandle)
{
#ifdef KEADEBUG
    fprintf(stderr, "%s %p\n", __FUNCTION__, columnHandle);
#endif
    KEA_Column *pKEAColumn = (KEA_Column*)columnHandle;
    delete pKEAColumn;
    return 0;
}

long
keaColumnModTimeGet(void *columnHandle, time_t *modTime)
{
#ifdef KEADEBUG
    fprintf(stderr, "%s %p\n", __FUNCTION__, columnHandle);
#endif
    // dunno...
    *modTime = 0;
    return 0;
}

long
keaColumnDataRead(void *columnHandle, unsigned long startRow, unsigned long numRows, 
                    unsigned char *data)
{
#ifdef KEADEBUG
    fprintf(stderr, "%s %p %ld %ld\n", __FUNCTION__, columnHandle, startRow, numRows);
#endif
    KEA_Column *pKEAColumn = (KEA_Column*)columnHandle;
    kealib::KEAAttributeTable *pKEATable = pKEAColumn->pKEATable;
    //fprintf( stderr, "Column name = %s\n", pKEAColumn->sName.c_str());
    long rCode = -1;

    int32_t *pIntData = (int32_t*)data;
    double *pDoubleData = (double*)data;
    char **ppszStringData = (char**)data;

    try
    {
        switch(pKEAColumn->eType)
        {
            case kealib::kea_att_bool:
            {
                // need to read in as bools - alloc mem
                bool *pBoolData = (bool*)malloc(numRows * sizeof(bool));
                if( pBoolData == NULL )
                    return -1;

                pKEATable->getBoolFields(startRow, numRows, pKEAColumn->nColIdx, pBoolData);
                for( unsigned long n = 0; n < numRows; n++ )
                    pIntData[n] = pBoolData[n]? 1 : 0;

                free(pBoolData);
            }
            break;
            case kealib::kea_att_int:
            {
                // read as int64_t so we need buffer
                int64_t *pInt64Data = (int64_t*)malloc(numRows * sizeof(int64_t));
                if( pInt64Data == NULL )
                    return -1;

                pKEATable->getIntFields(startRow, numRows, pKEAColumn->nColIdx, pInt64Data);
                for( unsigned long n = 0; n < numRows; n++ )
                {
                    if( pKEAColumn->bTreatIntAsFloat )
                    {
                        pDoubleData[n] = (double)pInt64Data[n] / 255.0;
                        if( pDoubleData[n] > 1.0 )
                            pDoubleData[n] = 1.0;
                    }
                    else
                    {
                        pIntData[n] = pInt64Data[n];
                    }
                }
                free(pInt64Data);
            }
            break;
            case kealib::kea_att_float:
                pKEATable->getFloatFields(startRow, numRows, pKEAColumn->nColIdx, pDoubleData);
                break;

            case kealib::kea_att_string:
                // alloc strings in Imagine memory via estr_Duplicate
				{
					std::vector<std::string> aStrings;
					pKEATable->getStringFields(startRow, numRows, pKEAColumn->nColIdx, &aStrings);
					// now go through and duplicate strings using the Imagine routine
					for( unsigned long i = 0; i < numRows; i++ )
					{
						ppszStringData[i] = estr_Duplicate((char*)aStrings[i].c_str());
					}
				}
                break;

            default:
                fprintf( stderr, "Unknown column type: %d\n", pKEAColumn->eType);
                break;
        }

        rCode = 0;
    }
    catch(kealib::KEAException &e)
    {
        fprintf(stderr, "Exception raised in %s: %s\n", __FUNCTION__, e.what());
    }
    return rCode;
}

// the following just need to exist before it will look at the RAT for some reason...
long
keaColumnDataWrite(void *columnHandle, unsigned long startRow, unsigned long numRows, unsigned char *data)
{
#ifdef KEADEBUG
    fprintf(stderr, "%s %p %ld %ld\n", __FUNCTION__, columnHandle, startRow, numRows);
#endif
    return -1;
}

long
keaColumnDestroy(void *tableHandle, char *columnName)
{
#ifdef KEADEBUG
    fprintf(stderr, "%s %p %s\n", __FUNCTION__, tableHandle, columnName);
#endif
    return -1;
}
