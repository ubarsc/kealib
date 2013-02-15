/*
 *  KEAMaskBand.h
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

#ifndef KEAMASKBAND_H
#define KEAMASKBAND_H

#include "KEABand.h"

// mask class. Derives from our band class
// and just override the read/write block functions
class KEAMaskBand : public KEARasterBand
{
public:
    KEAMaskBand(KEADataset *pDataset, int nSrcBand, GDALAccess eAccess, 
                libkea::KEAImageIO *pImageIO, int *pRefCount );
    ~KEAMaskBand();

    // virtual methods for RATs - not implemented for masks
    const GDALRasterAttributeTable *GetDefaultRAT();
    CPLErr SetDefaultRAT(const GDALRasterAttributeTable *poRAT);

protected:
    // we just override these functions from KEARasterBand
    virtual CPLErr IReadBlock( int, int, void * );
    virtual CPLErr IWriteBlock( int, int, void * );

};

#endif //KEAMASKBAND_H
