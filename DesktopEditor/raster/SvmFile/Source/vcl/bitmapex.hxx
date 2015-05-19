/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: bitmapex.hxx,v $
 * $Revision: 1.4 $
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

#ifndef _SV_BITMAPEX_HXX
#define _SV_BITMAPEX_HXX
#ifdef AVS
#include <vcl/dllapi.h>
#include <vcl/bitmap.hxx>
#include <vcl/alpha.hxx>
#include <tools/color.hxx>
#endif
#include "../vcl/bitmap.hxx"
#include "../tools/color.hxx"

namespace SVMCore{
// -------------------
// - TransparentType -
// -------------------

enum TransparentType
{
	TRANSPARENT_NONE, 
	TRANSPARENT_COLOR,
	TRANSPARENT_BITMAP
};

// ------------
// - BitmapEx -
// ------------

class  BitmapEx
{
	friend class ImpGraphic;

private:

	Bitmap				aBitmap;
	Bitmap				aMask;
	Size				aBitmapSize;
	Color				aTransparentColor;
	TransparentType		eTransparent;
	BOOL				bAlpha;

public:

//#if 0 // _SOLAR__PRIVATE

      ImpBitmap*  ImplGetBitmapImpBitmap() const { return aBitmap.ImplGetImpBitmap(); }
      ImpBitmap*  ImplGetMaskImpBitmap() const { return aMask.ImplGetImpBitmap(); }

//#endif // PRIVATE

public:

						BitmapEx();
#ifdef AVS
						BitmapEx( const ResId& rResId );
#endif
						BitmapEx( const BitmapEx& rBitmapEx );
#ifdef AVS
						BitmapEx( const BitmapEx& rBitmapEx, Point aSrc, Size aSize );
#endif
						BitmapEx( const Bitmap& rBmp );
						BitmapEx( const Bitmap& rBmp, const Bitmap& rMask );
						BitmapEx( const Bitmap& rBmp, const AlphaMask& rAlphaMask );
						BitmapEx( const Bitmap& rBmp, const Color& rTransparentColor );

						~BitmapEx();

	BitmapEx&			operator=( const BitmapEx& rBitmapEx );
#ifdef AVS
	BOOL				operator==( const BitmapEx& rBitmapEx ) const;
	BOOL				operator!=( const BitmapEx& rBitmapEx ) const { return !(*this==rBitmapEx); }
#endif
	BOOL				operator!() const { return !aBitmap; }
#ifdef AVS
	BOOL				IsEqual( const BitmapEx& rBmpEx ) const;
#endif
	BOOL				IsEmpty() const;
	void				SetEmpty();
#ifdef AVS
	void				Clear();

	void				Draw( OutputDevice* pOutDev, 
							  const Point& rDestPt ) const;
	void				Draw( OutputDevice* pOutDev, 
							  const Point& rDestPt, const Size& rDestSize ) const;
	void				Draw( OutputDevice* pOutDev, 
							  const Point& rDestPt, const Size& rDestSize,
							  const Point& rSrcPtPixel, const Size& rSrcSizePixel ) const;
#endif
	BOOL				IsTransparent() const;
	TransparentType		GetTransparentType() const { return eTransparent; }

	Bitmap				GetBitmap( const Color* pTransReplaceColor = NULL ) const;

	Bitmap				GetMask() const;
#ifdef AVS
    BitmapEx            GetColorTransformedBitmapEx( BmpColorMode eColorMode ) const;
#endif
	BOOL				IsAlpha() const;
	AlphaMask			GetAlpha() const;

    const Size&         GetSizePixel() const { return aBitmapSize; }
#ifdef AVS
	void				SetSizePixel( const Size& rNewSize );

	const Size&			GetPrefSize() const { return aBitmap.GetPrefSize(); }
	void				SetPrefSize( const Size& rPrefSize ) { aBitmap.SetPrefSize( rPrefSize ); }

	const MapMode&		GetPrefMapMode() const { return aBitmap.GetPrefMapMode(); }
	void				SetPrefMapMode( const MapMode& rPrefMapMode ) { aBitmap.SetPrefMapMode( rPrefMapMode ); }

	const Color&		GetTransparentColor() const { return aTransparentColor; }
	void				SetTransparentColor( const Color& rColor ) { aTransparentColor = rColor; }

	USHORT				GetBitCount() const { return aBitmap.GetBitCount(); }
	ULONG				GetSizeBytes() const;
	ULONG				GetChecksum() const;

public:

    /** Convert bitmap format

    	@param eConversion
        The format this bitmap should be converted to.

        @return TRUE, if the conversion was completed successfully.
     */
    BOOL				Convert( BmpConversion eConversion );

    /** Reduce number of colors for the bitmap

    	@param nNewColorCount
        Maximal number of bitmap colors after the reduce operation

        @param eReduce
        Algorithm to use for color reduction

        @return TRUE, if the color reduction operation was completed successfully.
     */
	BOOL				ReduceColors( USHORT nNewColorCount, 
									  BmpReduce eReduce = BMP_REDUCE_SIMPLE );

    /** Apply a dither algorithm to the bitmap

    	This method dithers the bitmap inplace, i.e. a true color
    	bitmap is converted to a paletted bitmap, reducing the color
    	deviation by error diffusion.

    	@param nDitherFlags
        The algorithm to be used for dithering
     */
	BOOL				Dither( ULONG nDitherFlags = BMP_DITHER_MATRIX );

    /** Crop the bitmap

    	@param rRectPixel
        A rectangle specifying the crop amounts on all four sides of
        the bitmap. If the upper left corner of the bitmap is assigned
        (0,0), then this method cuts out the given rectangle from the
        bitmap. Note that the rectangle is clipped to the bitmap's
        dimension, i.e. negative left,top rectangle coordinates or
        exceeding width or height is ignored.

        @return TRUE, if cropping was performed successfully. If
        nothing had to be cropped, because e.g. the crop rectangle
        included the bitmap, FALSE is returned, too!
     */
    BOOL				Crop( const Rectangle& rRectPixel );

    /** Expand the bitmap by pixel padding

    	@param nDX 
        Number of pixel to pad at the right border of the bitmap

    	@param nDY
        Number of scanlines to pad at the bottom border of the bitmap

        @param pInitColor
        Color to use for padded pixel

        @return TRUE, if padding was performed successfully. FALSE is
        not only returned when the operation failed, but also if
        nothing had to be done, e.g. because nDX and nDY were zero.
     */
    BOOL				Expand( ULONG nDX, ULONG nDY, 
								const Color* pInitColor = NULL, 
								BOOL bExpandTransparent = FALSE );

    /** Copy a rectangular area from another bitmap

    	@param rRectDst
        Destination rectangle in this bitmap. This is clipped to the
        bitmap dimensions.

        @param rRectSrc
        Source rectangle in pBmpSrc. This is clipped to the source
        bitmap dimensions. Note further that no scaling takes place
        during this copy operation, i.e. only the minimum of source
        and destination rectangle's width and height are used.

        @param pBmpSrc
        The source bitmap to copy from. If this argument is NULL, or
        equal to the object this method is called on, copying takes
        place within the same bitmap.

        @return TRUE, if the operation completed successfully. FALSE
        is not only returned when the operation failed, but also if
        nothing had to be done, e.g. because one of the rectangles are
        empty.
     */
    BOOL				CopyPixel( const Rectangle& rRectDst,
								   const Rectangle& rRectSrc,
								   const BitmapEx* pBmpExSrc = NULL );
#endif
    /** Fill the entire bitmap with the given color

    	@param rFillColor
        Color value to use for filling. Set the transparency part of
        the color to fill the mask.

        @return TRUE, if the operation was completed successfully.
     */
	BOOL				Erase( const Color& rFillColor );
#ifdef AVS
    /** Perform the Invert operation on every pixel

        @return TRUE, if the operation was completed successfully.
     */
	BOOL				Invert();
#endif
    /** Mirror the bitmap

    	@param nMirrorFlags
        About which axis (horizontal, vertical, or both) to mirror

        @return TRUE, if the operation was completed successfully.        
     */
    BOOL				Mirror( ULONG nMirrorFlags );

    /** Scale the bitmap

    	@param rNewSize
        The resulting size of the scaled bitmap

        @param nScaleFlag
        The algorithm to be used for scaling

        @return TRUE, if the operation was completed successfully.        
     */
	BOOL				Scale( const Size& rNewSize, ULONG nScaleFlag = BMP_SCALE_FAST );

    /** Scale the bitmap

    	@param rScaleX
        The scale factor in x direction.

    	@param rScaleY
        The scale factor in y direction.

        @return TRUE, if the operation was completed successfully.        
     */
    BOOL				Scale( const double& rScaleX, const double& rScaleY, ULONG nScaleFlag = BMP_SCALE_FAST );
#ifdef AVS
    /** Rotate bitmap by the specified angle

    	@param nAngle10
        The rotation angle in tenth of a degree. The bitmap is always rotated around its center.

        @param rFillColor
        The color to use for filling blank areas. During rotation, the
        bitmap is enlarged such that the whole rotation result fits
        in. The empty spaces around that rotated original bitmap are
        then filled with this color.

        @return TRUE, if the operation was completed successfully.        
     */
    BOOL				Rotate( long nAngle10, const Color& rFillColor );
#endif
    /** Replace all pixel having the search color with the specified color

    	@param rSearchColor
        Color specifying which pixel should be replaced
        
        @param rReplaceColor
        Color to be placed in all changed pixel

        @param nTol
        Tolerance value. Specifies the maximal difference between
        rSearchColor and the individual pixel values, such that the
        corresponding pixel is still regarded a match.

        @return TRUE, if the operation was completed successfully.        
     */
    BOOL				Replace( const Color& rSearchColor, const Color& rReplaceColor, ULONG nTol = 0 );
#ifdef AVS
    /** Replace all pixel having one the search colors with the corresponding replace color

    	@param pSearchColor
        Array of colors specifying which pixel should be replaced
        
        @param pReplaceColor
        Array of colors to be placed in all changed pixel

        @param nColorCount
        Size of the aforementioned color arrays

        @param nTol
        Tolerance value. Specifies the maximal difference between
        pSearchColor colors and the individual pixel values, such that
        the corresponding pixel is still regarded a match.

        @return TRUE, if the operation was completed successfully.        
     */
    BOOL				Replace( const Color* pSearchColors, const Color* pReplaceColors, 
								 ULONG nColorCount, const ULONG* pTols = NULL );

    /** Change various global color characteristics

    	@param nLuminancePercent
        Percent of luminance change, valid range [-100,100]. Values outside this range are clipped to the valid range.

    	@param nContrastPercent
        Percent of contrast change, valid range [-100,100]. Values outside this range are clipped to the valid range.

    	@param nChannelRPercent
        Percent of red channel change, valid range [-100,100]. Values outside this range are clipped to the valid range.

    	@param nChannelGPercent
        Percent of green channel change, valid range [-100,100]. Values outside this range are clipped to the valid range.

    	@param nChannelBPercent
        Percent of blue channel change, valid range [-100,100]. Values outside this range are clipped to the valid range.

        @param fGamma
        Exponent of the gamma function applied to the bitmap. The
        value 1.0 results in no change, the valid range is
        (0.0,10.0]. Values outside this range are regarded as 1.0.

        @param bInvert
        If TRUE, invert the channel values with the logical 'not' operator

        @return TRUE, if the operation was completed successfully.        
     */
	BOOL				Adjust( short nLuminancePercent = 0,
								short nContrastPercent = 0,
								short nChannelRPercent = 0, 
								short nChannelGPercent = 0, 
								short nChannelBPercent = 0,
								double fGamma = 1.0,
								BOOL bInvert = FALSE );

    /** Apply specified filter to the bitmap

    	@param eFilter
        The filter algorithm to apply

        @param pFilterParam
        Various parameter for the different bitmap filter algorithms

        @param pProgress
        A callback for showing the progress of the vectorization
        
        @return TRUE, if the operation was completed successfully.        
     */
	BOOL				Filter( BmpFilter eFilter, 
								const BmpFilterParam* pFilterParam = NULL,
								const Link* pProgress = NULL );
#endif
public:
#ifdef AVS
	friend  SvStream&	operator<<( SvStream& rOStm, const BitmapEx& rBitmapEx );
#endif
	friend  SvStream&	operator>>( SvStream& rIStm, BitmapEx& rBitmapEx );

};

}//SVMCore

#endif // _SV_BITMAPEX_HXX