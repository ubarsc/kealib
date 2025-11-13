/*
 *  kearat.h
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

#ifndef KEARAT_H
#define KEARAT_H

#include "gdal_priv.h"
#include "gdal_rat.h"
#include "keaband.h"

class KEARasterAttributeTable : public GDALRasterAttributeTable
{
private:
    kealib::KEAAttributeTable *m_poKEATable;
    std::vector<kealib::KEAATTField> m_aoFields;
    CPLString osWorkingResult;
    KEARasterBand *m_poBand;
    CPLMutex            *m_hMutex;
#ifdef HAVE_SETVALUE_CPLERR 
    mutable std::vector<GByte> m_abyCachedWKB{};
#endif

public:
    KEARasterAttributeTable(kealib::KEAAttributeTable *poKEATable, KEARasterBand *poBand);
    ~KEARasterAttributeTable();

    GDALDefaultRasterAttributeTable *Clone() const;

    virtual int           GetColumnCount() const;

    virtual const char   *GetNameOfCol( int ) const;
    virtual GDALRATFieldUsage GetUsageOfCol( int ) const;
    virtual GDALRATFieldType GetTypeOfCol( int ) const;
    
    virtual int           GetColOfUsage( GDALRATFieldUsage ) const;

    virtual int           GetRowCount() const;

    virtual const char   *GetValueAsString( int iRow, int iField ) const;
    virtual int           GetValueAsInt( int iRow, int iField ) const;
    virtual double        GetValueAsDouble( int iRow, int iField ) const;

#ifdef HAVE_SETVALUE_CPLERR 
    bool GetValueAsBoolean(int iRow, int iField) const override;
    GDALRATDateTime GetValueAsDateTime(int iRow, int iField) const override;
    const GByte *GetValueAsWKBGeometry(int iRow, int iField,
                                       size_t &nWKBSize) const override;
                                       
    virtual CPLErr SetValue(int iRow, int iField,
                            const char *pszValue) override;
    virtual CPLErr SetValue(int iRow, int iField, double dfValue) override;
    virtual CPLErr SetValue(int iRow, int iField, int nValue) override;
    CPLErr SetValue(int iRow, int iField,
                    const GDALRATDateTime &sDateTime) override;
    CPLErr SetValue(int iRow, int iField, const void *pabyWKB,
                    size_t nWKBSize) override;
    CPLErr SetValue(int iRow, int iField, bool nValue) override;
#else
    virtual void          SetValue( int iRow, int iField, const char *pszValue ) override;
    virtual void          SetValue( int iRow, int iField, double dfValue) override;
    virtual void          SetValue( int iRow, int iField, int nValue ) override;
#endif
    virtual CPLErr        ValuesIO(GDALRWFlag eRWFlag, int iField, int iStartRow, int iLength, double *pdfData);
    virtual CPLErr        ValuesIO(GDALRWFlag eRWFlag, int iField, int iStartRow, int iLength, int *pnData);
    virtual CPLErr        ValuesIO(GDALRWFlag eRWFlag, int iField, int iStartRow, int iLength, char **papszStrList);
#ifdef HAVE_SETVALUE_CPLERR
    CPLErr ValuesIO(GDALRWFlag eRWFlag, int iField, int iStartRow, int iLength,
                    bool *pbData) override;
    CPLErr ValuesIO(GDALRWFlag eRWFlag, int iField, int iStartRow, int iLength,
                    GDALRATDateTime *pasDateTime) override;
    CPLErr ValuesIO(GDALRWFlag eRWFlag, int iField, int iStartRow, int iLength,
                    GByte **ppabyWKB, size_t *pnWKBSize) override;
#endif
    virtual int           ChangesAreWrittenToFile();
    virtual void          SetRowCount( int iCount );

    virtual CPLErr        CreateColumn( const char *pszFieldName, 
                                GDALRATFieldType eFieldType, 
                                GDALRATFieldUsage eFieldUsage );

    virtual CPLErr        SetLinearBinning( double dfRow0Min,
                                            double dfBinSize );
    virtual int           GetLinearBinning( double *pdfRow0Min,
                                            double *pdfBinSize ) const;

    virtual CPLXMLNode   *Serialize() const;

    virtual CPLErr        SetTableType(const GDALRATTableType eInTableType);
    virtual GDALRATTableType GetTableType() const;
    virtual void          RemoveStatistics();
};

#endif //KEARAT_H
