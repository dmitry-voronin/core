/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: string.c,v $
 * $Revision: 1.19 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/
#ifdef AVS
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
#pragma warning(disable:4738) // storing 32-bit float result in memory, possible loss of performance
#endif

#include <rtl/memory.h>
#include <osl/interlck.h>
#include <rtl/alloc.h>
#include <osl/diagnose.h>
#include <rtl/tencinfo.h>

#include "strimp.h"
#include "surrogates.h"
#include <rtl/string.h>

#include "rtl/math.h"
#include "rtl/tencinfo.h"
#endif
#include "rtl/string.h"
#include "rtl/strimp.h"

using namespace SVMCore;

/* ======================================================================= */

/* static data to be referenced by all empty strings
 * the refCount is predefined to 1 and must never become 0 !
 */
static rtl_String const aImplEmpty_rtl_String =
{
    SAL_STRING_STATIC_FLAG|1,
            /* sal_Int32    refCount;   */
    0,      /* sal_Int32    length;     */
    { 0 }   /* sal_Char     buffer[1];  */
};

/* ======================================================================= */

#define IMPL_RTL_STRCODE            sal_Char
#define IMPL_RTL_USTRCODE( c )      ((unsigned char)c)
#define IMPL_RTL_STRNAME( n )       rtl_str_ ## n

#define IMPL_RTL_STRINGNAME( n )    rtl_string_ ## n
#define IMPL_RTL_STRINGDATA         rtl_String
#define IMPL_RTL_EMPTYSTRING        aImplEmpty_rtl_String

/* ======================================================================= */

/* Include String/UString template code */

#include "strtmpl.cpp"

sal_Int32  rtl_str_valueOfFloat(sal_Char * pStr, float f)
{
    rtl_String * pResult = NULL;
    sal_Int32 nLen;
    rtl_math_doubleToString(
        &pResult, 0, 0, f, rtl_math_StringFormat_G,
        RTL_STR_MAX_VALUEOFFLOAT - RTL_CONSTASCII_LENGTH("-x.E-xxx"), '.', 0, 0,
        sal_True);
    nLen = pResult->length;
    //OSL_ASSERT(nLen < RTL_STR_MAX_VALUEOFFLOAT);
    rtl_copyMemory(pStr, pResult->buffer, (nLen + 1) * sizeof(sal_Char));
    rtl_string_release(pResult);
    return nLen;
}

sal_Int32  rtl_str_valueOfDouble(sal_Char * pStr, double d)
{
    rtl_String * pResult = NULL;
    sal_Int32 nLen;
    rtl_math_doubleToString(
        &pResult, 0, 0, d, rtl_math_StringFormat_G,
        RTL_STR_MAX_VALUEOFDOUBLE - RTL_CONSTASCII_LENGTH("-x.E-xxx"), '.', 0,
        0, sal_True);
    nLen = pResult->length;
    //OSL_ASSERT(nLen < RTL_STR_MAX_VALUEOFDOUBLE);
    rtl_copyMemory(pStr, pResult->buffer, (nLen + 1) * sizeof(sal_Char));
    rtl_string_release(pResult);
    return nLen;
}

float  rtl_str_toFloat(sal_Char const * pStr)
{
    return (float) rtl_math_stringToDouble(pStr, pStr + rtl_str_getLength(pStr),
                                           '.', 0, 0, 0);
}

double  rtl_str_toDouble(sal_Char const * pStr)
{
    return rtl_math_stringToDouble(pStr, pStr + rtl_str_getLength(pStr), '.', 0,
                                   0, 0);
}

/* ======================================================================= */

static int rtl_ImplGetFastUTF8ByteLen( const sal_Unicode* pStr, sal_Int32 nLen )
{
    int                 n;
    sal_Unicode         c;
    sal_uInt32          nUCS4Char;
    const sal_Unicode*  pEndStr;

    n = 0;
    pEndStr  = pStr+nLen;
    while ( pStr < pEndStr )
    {
        c = *pStr;

        if ( c < 0x80 )
            n++;
        else if ( c < 0x800 )
            n += 2;
        else
        {
            if ( !SAL_RTL_IS_HIGH_SURROGATE(c) )
                n += 3;
            else
            {
                nUCS4Char = c;

                if ( pStr+1 < pEndStr )
                {
                    c = *(pStr+1);
                    if ( SAL_RTL_IS_LOW_SURROGATE(c) )
                    {
                        nUCS4Char = SAL_RTL_COMBINE_SURROGATES(nUCS4Char, c);
                        pStr++;
                    }
                }

                if ( nUCS4Char < 0x10000 )
                    n += 3;
                else if ( nUCS4Char < 0x200000 )
                    n += 4;
                else if ( nUCS4Char < 0x4000000 )
                    n += 5;
                else
                    n += 6;
            }
        }

        pStr++;
    }

    return n;
}

/* ----------------------------------------------------------------------- */

sal_Bool  rtl_impl_convertUStringToString(rtl_String ** pTarget,
                                                  sal_Unicode const * pSource,
                                                  sal_Int32 nLength,
                                                  rtl_TextEncoding nEncoding,
                                                  sal_uInt32 nFlags,
                                                  sal_Bool bCheckErrors)
{
    //OSL_ASSERT(pTarget != NULL
               && (pSource != NULL || nLength == 0)
               && nLength >= 0
               && rtl_isOctetTextEncoding(nEncoding));

    if ( !nLength )
        rtl_string_new( pTarget );
    else
    {
        rtl_String*                 pTemp;
        rtl_UnicodeToTextConverter  hConverter;
        sal_uInt32                  nInfo;
        sal_Size                    nSrcChars;
        sal_Size                    nDestBytes;
        sal_Size                    nNewLen;
        sal_Size                    nNotConvertedChars;
        sal_Size                    nMaxCharLen;

        /* Optimization for UTF-8 - we try to calculate the exact length */
        /* For all other encoding we try an good estimation */
        if ( nEncoding == RTL_TEXTENCODING_UTF8 )
        {
            nNewLen = rtl_ImplGetFastUTF8ByteLen( pSource, nLength );
            /* Includes the string only ASCII, then we could copy
               the buffer faster */
            if ( nNewLen == (sal_Size)nLength )
            {
                IMPL_RTL_STRCODE* pBuffer;
                if ( *pTarget )
                    IMPL_RTL_STRINGNAME( release )( *pTarget );
                *pTarget = IMPL_RTL_STRINGNAME( ImplAlloc )( nLength );
                //OSL_ASSERT(*pTarget != NULL);
                pBuffer = (*pTarget)->buffer;
                do
                {
                    /* Check ASCII range */
                    OSL_ENSURE( *pSource <= 127,
                                "rtl_uString2String() - UTF8 test is encoding is wrong" );

                    *pBuffer = (IMPL_RTL_STRCODE)(unsigned char)*pSource;
                    pBuffer++;
                    pSource++;
                    nLength--;
                }
                while ( nLength );
                return sal_True;
            }

            nMaxCharLen = 4;
        }
        else
        {
            rtl_TextEncodingInfo aTextEncInfo;
            aTextEncInfo.StructSize = sizeof( aTextEncInfo );
            if ( !rtl_getTextEncodingInfo( nEncoding, &aTextEncInfo ) )
            {
                aTextEncInfo.AverageCharSize    = 1;
                aTextEncInfo.MaximumCharSize    = 8;
            }

            nNewLen = nLength*aTextEncInfo.AverageCharSize;
            nMaxCharLen = aTextEncInfo.MaximumCharSize;
        }

        nFlags |= RTL_UNICODETOTEXT_FLAGS_FLUSH;
        hConverter = rtl_createUnicodeToTextConverter( nEncoding );

        for (;;)
        {
            pTemp = IMPL_RTL_STRINGNAME( ImplAlloc )( nNewLen );
            //OSL_ASSERT(pTemp != NULL);
            nDestBytes = rtl_convertUnicodeToText( hConverter, 0,
                                                   pSource, nLength,
                                                   pTemp->buffer, nNewLen,
                                                   nFlags,
                                                   &nInfo, &nSrcChars );
            if (bCheckErrors && (nInfo & RTL_UNICODETOTEXT_INFO_ERROR) != 0)
            {
                rtl_freeMemory(pTemp);
                rtl_destroyUnicodeToTextConverter(hConverter);
                return sal_False;
            }

            if ((nInfo & RTL_UNICODETOTEXT_INFO_DESTBUFFERTOSMALL) == 0)
                break;

            /* Buffer not big enough, try again with enough space */
            rtl_freeMemory( pTemp );

            /* Try with the max. count of characters with
               additional overhead for replacing functionality */
            nNotConvertedChars = nLength-nSrcChars;
            nNewLen = nDestBytes+(nNotConvertedChars*nMaxCharLen)+nNotConvertedChars+4;
        }

        /* Set the buffer to the correct size or is there to
           much overhead, reallocate to the correct size */
        if ( nNewLen > nDestBytes+8 )
        {
            rtl_String* pTemp2 = IMPL_RTL_STRINGNAME( ImplAlloc )( nDestBytes );
            //OSL_ASSERT(pTemp2 != NULL);
            rtl_str_ImplCopy( pTemp2->buffer, pTemp->buffer, nDestBytes );
            rtl_freeMemory( pTemp );
            pTemp = pTemp2;
        }
        else
        {
            pTemp->length = nDestBytes;
            pTemp->buffer[nDestBytes] = 0;
        }

        rtl_destroyUnicodeToTextConverter( hConverter );
        if ( *pTarget )
            IMPL_RTL_STRINGNAME( release )( *pTarget );
        *pTarget = pTemp;

        /* Results the conversion in an empty buffer -
           create an empty string */
        if ( pTemp && !nDestBytes )
            rtl_string_new( pTarget );
    }
    return sal_True;
}

void  rtl_uString2String( rtl_String** ppThis,
                                  const sal_Unicode* pUStr,
                                  sal_Int32 nULen,
                                  rtl_TextEncoding eTextEncoding,
                                  sal_uInt32 nCvtFlags )
{
    rtl_impl_convertUStringToString(ppThis, pUStr, nULen, eTextEncoding,
                                    nCvtFlags, sal_False);
}

sal_Bool  rtl_convertUStringToString(rtl_String ** pTarget,
                                             sal_Unicode const * pSource,
                                             sal_Int32 nLength,
                                             rtl_TextEncoding nEncoding,
                                             sal_uInt32 nFlags)
{
    return rtl_impl_convertUStringToString(pTarget, pSource, nLength, nEncoding,
                                           nFlags, sal_True);
}