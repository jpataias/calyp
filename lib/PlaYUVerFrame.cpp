/*    This file is a part of PlaYUVer project
 *    Copyright (C) 2014-2018  by Joao Carreira   (jfmcarreira@gmail.com)
 *                                Luis Lucas      (luisfrlucas@gmail.com)
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/**
 * \file     PlaYUVerFrame.cpp
 * \brief    Video Frame handling
 */

#include "PlaYUVerFrame.h"

#include "LibMemory.h"
#include "PlaYUVerFramePixelFormats.h"
#include "config.h"

#include <cmath>
#ifdef USE_OPENCV
#include <opencv2/core/core.hpp>
#endif

#define MAX_NUMBER_COMPONENTS 4

std::vector<String> PlaYUVerFrame::supportedColorSpacesListNames()
{
  return std::vector<String>{
      "YUV",
      "RGB",
      "GRAY",
      "ARGB",
  };
}

std::vector<String> PlaYUVerFrame::supportedPixelFormatListNames()
{
  std::vector<String> formatsList;
  for( Int i = 0; i < numberOfFormats(); i++ )
  {
    formatsList.push_back( g_PlaYUVerPixFmtDescriptorsMap.at( i ).name );
  }
  return formatsList;
}

std::vector<String> PlaYUVerFrame::supportedPixelFormatListNames( Int colorSpace )
{
  std::vector<String> formatsList;
  for( Int i = 0; i < numberOfFormats(); i++ )
  {
    if( g_PlaYUVerPixFmtDescriptorsMap.at( i ).colorSpace == colorSpace )
      formatsList.push_back( g_PlaYUVerPixFmtDescriptorsMap.at( i ).name );
  }
  return formatsList;
}

Int PlaYUVerFrame::numberOfFormats()
{
  return g_PlaYUVerPixFmtDescriptorsMap.size();
}

Int PlaYUVerFrame::findPixelFormat( const String& name )
{
  for( Int i = 0; i < numberOfFormats(); i++ )
  {
    if( g_PlaYUVerPixFmtDescriptorsMap.at( i ).name == name )
      return i;
  }
  return -1;
}

struct PlaYUVerPixelPrivate
{
  PlaYUVerPixelPrivate( const Int& ColorSpace )
  {
    iColorSpace = ColorSpace == PlaYUVerPixel::COLOR_GRAY ?
                      PlaYUVerPixel::COLOR_YUV :
                      ColorSpace;
    for( Int i = 0; i < MAX_NUMBER_COMPONENTS; i++ )
    {
      PixelComponents[i] = 0;
    }
  }

  Int iColorSpace;
  Pel PixelComponents[MAX_NUMBER_COMPONENTS];
};

static inline Void yuvToRgb( const Int& iY, const Int& iU, const Int& iV, Int& iR, Int& iG, Int& iB )
{
  iR = iY + ( ( 1436 * ( iV - 128 ) ) >> 10 );
  iG = iY - ( ( 352 * ( iU - 128 ) + 731 * ( iV - 128 ) ) >> 10 );
  iB = iY + ( ( 1812 * ( iU - 128 ) ) >> 10 );
  iR = iR < 0 ? 0 : iR > 255 ? 255 : iR;
  iG = iG < 0 ? 0 : iG > 255 ? 255 : iG;
  iB = iB < 0 ? 0 : iB > 255 ? 255 : iB;
}

static inline Void rgbToYuv( const Int& iR, const Int& iG, const Int& iB, Int& iY, Int& iU, Int& iV )
{
  iY = ( 299 * iR + 587 * iG + 114 * iB + 500 ) / 1000;
  iU = ( 1000 * ( iB - iY ) + 226816 ) / 1772;
  iV = ( 1000 * ( iR - iY ) + 179456 ) / 1402;
}

Int PlaYUVerPixel::getMaxNumberOfComponents()
{
  return MAX_NUMBER_COMPONENTS;
}

PlaYUVerPixel::PlaYUVerPixel( const Int& ColorSpace )
    : d( new PlaYUVerPixelPrivate( ColorSpace ) )
{
}

PlaYUVerPixel::PlaYUVerPixel( const Int& ColorSpace, const Pel& c0 )
    : d( new PlaYUVerPixelPrivate( ColorSpace ) )
{
  d->PixelComponents[0] = c0;
}

PlaYUVerPixel::PlaYUVerPixel( const Int& ColorSpace, const Pel& c0, const Pel& c1, const Pel& c2 )
    : d( new PlaYUVerPixelPrivate( ColorSpace ) )
{
  d->PixelComponents[0] = c0;
  d->PixelComponents[1] = c1;
  d->PixelComponents[2] = c2;
}

PlaYUVerPixel::PlaYUVerPixel( const Int& ColorSpace, const Pel& c0, const Pel& c1, const Pel& c2, const Pel& c3 )
    : d( new PlaYUVerPixelPrivate( ColorSpace ) )
{
  d->PixelComponents[0] = c0;
  d->PixelComponents[1] = c1;
  d->PixelComponents[2] = c2;
  d->PixelComponents[3] = c3;
}

PlaYUVerPixel::PlaYUVerPixel( const PlaYUVerPixel& other )
    : d( new PlaYUVerPixelPrivate( other.colorSpace() ) )
{
  for( Int i = 0; i < MAX_NUMBER_COMPONENTS; i++ )
  {
    d->PixelComponents[i] = other[i];
  }
}

PlaYUVerPixel::~PlaYUVerPixel()
{
  delete d;
}

Int PlaYUVerPixel::colorSpace() const
{
  return d->iColorSpace;
}

Pel PlaYUVerPixel::Y() const
{
  return d->PixelComponents[0];
}
Pel PlaYUVerPixel::Cb() const
{
  return d->PixelComponents[1];
}
Pel PlaYUVerPixel::Cr() const
{
  return d->PixelComponents[2];
}
Pel PlaYUVerPixel::R() const
{
  return d->PixelComponents[0];
}
Pel PlaYUVerPixel::G() const
{
  return d->PixelComponents[1];
}
Pel PlaYUVerPixel::B() const
{
  return d->PixelComponents[2];
}
Pel PlaYUVerPixel::A() const
{
  return d->PixelComponents[3];
}

Pel& PlaYUVerPixel::Y()
{
  return d->PixelComponents[0];
}
Pel& PlaYUVerPixel::Cb()
{
  return d->PixelComponents[1];
}
Pel& PlaYUVerPixel::Cr()
{
  return d->PixelComponents[2];
}
Pel& PlaYUVerPixel::R()
{
  return d->PixelComponents[0];
}
Pel& PlaYUVerPixel::G()
{
  return d->PixelComponents[1];
}
Pel& PlaYUVerPixel::B()
{
  return d->PixelComponents[2];
}
Pel& PlaYUVerPixel::A()
{
  return d->PixelComponents[3];
}

Pel PlaYUVerPixel::operator[]( const Int& channel ) const
{
  return d->PixelComponents[channel];
}
Pel& PlaYUVerPixel::operator[]( const Int& channel )
{
  return d->PixelComponents[channel];
}

PlaYUVerPixel PlaYUVerPixel::operator=( const PlaYUVerPixel& other )
{
  for( Int i = 0; i < MAX_NUMBER_COMPONENTS; i++ )
  {
    d->PixelComponents[i] = other[i];
  }
  return *this;
}

PlaYUVerPixel PlaYUVerPixel::operator+( const PlaYUVerPixel& in )
{
  PlaYUVerPixel result;
  for( Int i = 0; i < MAX_NUMBER_COMPONENTS; i++ )
  {
    result[i] = d->PixelComponents[i] + in[i];
  }
  return result;
}

PlaYUVerPixel PlaYUVerPixel::operator+=( const PlaYUVerPixel& in )
{
  for( Int i = 0; i < MAX_NUMBER_COMPONENTS; i++ )
  {
    d->PixelComponents[i] += in[i];
  }
  return *this;
}

PlaYUVerPixel PlaYUVerPixel::operator-( const PlaYUVerPixel& in )
{
  PlaYUVerPixel result;
  for( Int i = 0; i < MAX_NUMBER_COMPONENTS; i++ )
  {
    result[i] = d->PixelComponents[i] - in[i];
  }
  return result;
}

PlaYUVerPixel PlaYUVerPixel::operator-=( const PlaYUVerPixel& in )
{
  for( Int i = 0; i < MAX_NUMBER_COMPONENTS; i++ )
  {
    d->PixelComponents[i] -= in[i];
  }
  return *this;
}

PlaYUVerPixel PlaYUVerPixel::operator*( const Double& op )
{
  PlaYUVerPixel result;
  for( Int i = 0; i < MAX_NUMBER_COMPONENTS; i++ )
  {
    result[i] = d->PixelComponents[i] * op;
  }
  return result;
}

PlaYUVerPixel PlaYUVerPixel::ConvertPixel( ColorSpace eOutputSpace )
{
  Int outA, outB, outC;
  PlaYUVerPixel outPixel( COLOR_INVALID, 0, 0, 0 );

  if( ColorSpace() == eOutputSpace || eOutputSpace == COLOR_RGBA || eOutputSpace == COLOR_GRAY )
    return *this;

  if( eOutputSpace == COLOR_RGB )
  {
    yuvToRgb( Y(), Cb(), Cr(), outA, outB, outC );
    outPixel.R() = outA;
    outPixel.G() = outB;
    outPixel.B() = outC;
  }
  if( eOutputSpace == COLOR_YUV )
  {
    rgbToYuv( R(), G(), B(), outA, outB, outC );
    outPixel.Y() = outA;
    outPixel.Cb() = outB;
    outPixel.Cr() = outC;
  }
  return outPixel;
}

struct PlaYUVerFramePrivate
{
  Bool m_bInit;

  //! Struct with the pixel format description.
  const PlaYUVerPixFmtDescriptor* m_pcPelFormat;
  String m_cPelFmtName;

  UInt m_uiWidth;         //!< Width of the frame
  UInt m_uiHeight;        //!< Height of the frame
  Int m_iPixelFormat;     //!< Pixel format number (it follows the list of supported pixel formats)
  UInt m_uiBitsPel;       //!< Bits per pixel/channel
  UInt m_uiHalfPelValue;  //!< Bits per pixel/channel

  Pel*** m_pppcInputPel;

  Bool m_bHasRGBPel;  //!< Flag indicating that the ARGB buffer was computed
  UChar* m_pcARGB32;  //!< Buffer with the ARGB pixels used in Qt libs

  /** Histogram control variables **/
  Bool m_bHasHistogram;
  Bool m_bHistogramRunning;
  /** The histogram data.*/
  UInt* m_puiHistogram;
  /** If the image is RGB and calcLuma is true, we have 1 more channel */
  UInt m_uiHistoChannels;
  /** Numbers of histogram segments depending of image bytes depth*/
  UInt m_uiHistoSegments;

  /**
	 * Common constructor function of a frame
	 *
	 * @param width width of the frame
	 * @param height height of the frame
	 * @param pel_format pixel format index (always use PixelFormats enum)
	 *
	 */
  Void init( UInt width, UInt height, Int pel_format, Int bitsPixel )
  {
    m_bInit = false;
    m_bHasRGBPel = false;
    m_pppcInputPel = NULL;
    m_pcARGB32 = NULL;
    m_uiWidth = width;
    m_uiHeight = height;
    m_iPixelFormat = pel_format;
    m_uiBitsPel = bitsPixel < 8 ? 8 : bitsPixel;
    m_uiHalfPelValue = 1 << ( m_uiBitsPel - 1 );

    if( m_uiWidth == 0 || m_uiHeight == 0 || m_iPixelFormat == -1 || bitsPixel > 16 )
    {
      throw PlaYUVerFailure( "PlaYUVerFrame", "Cannot create a PlYUVerFrame of this type" );
    }

    m_pcPelFormat = &( g_PlaYUVerPixFmtDescriptorsMap.at( pel_format ) );
    Int iNumberChannels = m_pcPelFormat->numberChannels;

    std::size_t mem_size = 0;
    std::size_t num_of_ptrs = 0;
    for( Int ch = 0; ch < iNumberChannels; ch++ )
    {
      Int ratioW = ch > 0 ? m_pcPelFormat->log2ChromaWidth : 0;
      Int ratioH = ch > 0 ? m_pcPelFormat->log2ChromaHeight : 0;
      // Add pointers mem
      num_of_ptrs += CHROMASHIFT( m_uiHeight, ratioH );
      // Add pixel mem
      mem_size += CHROMASHIFT( m_uiHeight, ratioH ) * CHROMASHIFT( m_uiWidth, ratioW ) * sizeof( Pel );
    }
    mem_size += num_of_ptrs * sizeof( Pel* ) + sizeof( Pel** ) * iNumberChannels;

    m_pppcInputPel = (Pel***)xMallocMem( mem_size );

    Pel** pelPtrMem = (Pel**)( m_pppcInputPel + iNumberChannels );
    Pel* pelMem = (Pel*)( pelPtrMem + num_of_ptrs );
    for( Int ch = 0; ch < iNumberChannels; ch++ )
    {
      Int ratioW = ch > 0 ? m_pcPelFormat->log2ChromaWidth : 0;
      Int ratioH = ch > 0 ? m_pcPelFormat->log2ChromaHeight : 0;
      m_pppcInputPel[ch] = pelPtrMem;
      for( UInt h = 0; h < CHROMASHIFT( m_uiHeight, ratioH ); h++ )
      {
        *pelPtrMem = pelMem;
        pelPtrMem++;
        pelMem += CHROMASHIFT( m_uiWidth, ratioW );
      }
    }

    /* Alloc ARGB memory */
    getMem1D( &m_pcARGB32, m_uiHeight * m_uiWidth * 4 );

    m_puiHistogram = NULL;
    m_bHasHistogram = false;
    m_bHistogramRunning = false;

    m_uiHistoSegments = 1 << m_uiBitsPel;

    if( m_pcPelFormat->colorSpace == PlaYUVerPixel::COLOR_RGB ||
        m_pcPelFormat->colorSpace == PlaYUVerPixel::COLOR_RGBA )
      m_uiHistoChannels = m_pcPelFormat->numberChannels + 1;
    else
      m_uiHistoChannels = m_pcPelFormat->numberChannels;

    getMem1D<UInt>( &( m_puiHistogram ), m_uiHistoSegments * m_uiHistoChannels );

    m_cPelFmtName = PlaYUVerFrame::supportedPixelFormatListNames()[m_iPixelFormat].c_str();

    m_bInit = true;
  }

  ~PlaYUVerFramePrivate()
  {
    while( m_bHistogramRunning )
      ;
    if( m_puiHistogram )
      freeMem1D( m_puiHistogram );
    m_bHasHistogram = false;

    if( m_pppcInputPel )
      xFreeMem( m_pppcInputPel );

    if( m_pcARGB32 )
      freeMem1D( m_pcARGB32 );
  }
};

/*!
 * \brief Constructors
 */

PlaYUVerFrame::PlaYUVerFrame( UInt width, UInt height, Int pelFormat, Int bitsPixel )
    : d( new PlaYUVerFramePrivate )
{
  d->init( width, height, pelFormat, bitsPixel );
}

PlaYUVerFrame::PlaYUVerFrame( const PlaYUVerFrame& other )
    : d( new PlaYUVerFramePrivate )
{
  d->init( other.getWidth(), other.getHeight(), other.getPelFormat(), other.getBitsPel() );
  copyFrom( &other );
}

PlaYUVerFrame::PlaYUVerFrame( const PlaYUVerFrame* other )
    : d( new PlaYUVerFramePrivate )
{
  if( other )
  {
    d->init( other->getWidth(), other->getHeight(), other->getPelFormat(), other->getBitsPel() );
    copyFrom( other );
  }
}

PlaYUVerFrame::PlaYUVerFrame( const PlaYUVerFrame& other, UInt x, UInt y, UInt width, UInt height )
    : d( new PlaYUVerFramePrivate )
{
  const PlaYUVerPixFmtDescriptor* pcPelFormat = &( g_PlaYUVerPixFmtDescriptorsMap.at( other.getPelFormat() ) );
  if( pcPelFormat->log2ChromaWidth )
  {
    if( x % ( 1 << pcPelFormat->log2ChromaWidth ) )
      x--;
    if( ( x + width ) % ( 1 << pcPelFormat->log2ChromaWidth ) )
      width++;
  }

  if( pcPelFormat->log2ChromaHeight )
  {
    if( y % ( 1 << pcPelFormat->log2ChromaHeight ) )
      y--;

    if( ( y + height ) % ( 1 << pcPelFormat->log2ChromaHeight ) )
      height++;
  }

  d->init( width, height, other.getPelFormat(), other.getBitsPel() );
  copyFrom( other, x, y );
}

PlaYUVerFrame::PlaYUVerFrame( const PlaYUVerFrame* other, UInt posX, UInt posY, UInt areaWidth, UInt areaHeight )
    : d( new PlaYUVerFramePrivate )
{
  if( !other )
    return;

  const PlaYUVerPixFmtDescriptor* pcPelFormat = &( g_PlaYUVerPixFmtDescriptorsMap.at( other->getPelFormat() ) );
  if( pcPelFormat->log2ChromaWidth )
  {
    if( posX % ( 1 << pcPelFormat->log2ChromaWidth ) )
      posX--;
    if( ( posX + areaWidth ) % ( 1 << pcPelFormat->log2ChromaWidth ) )
      areaWidth++;
  }

  if( pcPelFormat->log2ChromaHeight )
  {
    if( posY % ( 1 << pcPelFormat->log2ChromaHeight ) )
      posY--;

    if( ( posY + areaHeight ) % ( 1 << pcPelFormat->log2ChromaHeight ) )
      areaHeight++;
  }

  d->init( areaWidth, areaHeight, other->getPelFormat(), other->getBitsPel() );
  copyFrom( other, posX, posY );
}

PlaYUVerFrame::~PlaYUVerFrame()
{
  delete d;
}

Bool PlaYUVerFrame::haveSameFmt( const PlaYUVerFrame& other, UInt match ) const
{
  Bool bRet = true;
  if( match & MATCH_COLOR_SPACE )
    bRet &= ( getColorSpace() == other.getColorSpace() );
  if( match & MATCH_RESOLUTION )
    bRet &= ( getWidth() == other.getWidth() ) && ( getHeight() == other.getHeight() );
  if( match & MATCH_PEL_FMT )
    bRet &= ( getPelFormat() == other.getPelFormat() );
  if( match & MATCH_BITS )
    bRet &= ( getBitsPel() == other.getBitsPel() );
  if( match & MATCH_COLOR_SPACE_IGNORE_GRAY )
    bRet &= ( getColorSpace() == PlaYUVerPixel::COLOR_GRAY || getColorSpace() == other.getColorSpace() );
  return bRet;
}

Bool PlaYUVerFrame::haveSameFmt( const PlaYUVerFrame* other, UInt match ) const
{
  Bool bRet = false;
  if( other )
  {
    bRet = haveSameFmt( *other, match );
  }
  return bRet;
}

Int PlaYUVerFrame::getPelFormat() const
{
  return d->m_iPixelFormat;
}

String PlaYUVerFrame::getPelFmtName()
{
  return d->m_pcPelFormat->name;
}

Int PlaYUVerFrame::getColorSpace() const
{
  return d->m_pcPelFormat->colorSpace;
}

UInt PlaYUVerFrame::getNumberChannels() const
{
  return d->m_pcPelFormat->numberChannels;
}

UInt PlaYUVerFrame::getWidth( Int channel ) const
{
  return CHROMASHIFT( d->m_uiWidth, channel > 0 ? d->m_pcPelFormat->log2ChromaWidth : 0 );
}
UInt PlaYUVerFrame::getHeight( Int channel ) const
{
  return CHROMASHIFT( d->m_uiHeight, channel > 0 ? d->m_pcPelFormat->log2ChromaHeight : 0 );
}

UInt PlaYUVerFrame::getPixels( Int channel ) const
{
  return getWidth( channel ) * getWidth( channel );
}

UInt8 PlaYUVerFrame::getChromaWidthRatio() const
{
  return d->m_pcPelFormat->log2ChromaWidth;
}

UInt8 PlaYUVerFrame::getChromaHeightRatio() const
{
  return d->m_pcPelFormat->log2ChromaHeight;
}

UInt PlaYUVerFrame::getChromaWidth() const
{
  if( d->m_pcPelFormat->colorSpace == GRAY )
    return 0;
  return CHROMASHIFT( d->m_uiWidth, d->m_pcPelFormat->log2ChromaWidth );
}

UInt PlaYUVerFrame::getChromaHeight() const
{
  if( d->m_pcPelFormat->colorSpace == GRAY )
    return 0;
  return CHROMASHIFT( d->m_uiHeight, d->m_pcPelFormat->log2ChromaHeight );
}

UInt PlaYUVerFrame::getChromaLength() const
{
  return getChromaWidth() * getChromaHeight();
}

UInt PlaYUVerFrame::getBitsPel() const
{
  return d->m_uiBitsPel;
}

UInt64 PlaYUVerFrame::getBytesPerFrame()
{
  return getBytesPerFrame( d->m_uiWidth, d->m_uiHeight, d->m_iPixelFormat, d->m_uiBitsPel );
}

UInt64 PlaYUVerFrame::getBytesPerFrame( UInt uiWidth, UInt uiHeight, Int iPixelFormat, UInt bitsPixel )
{
  const PlaYUVerPixFmtDescriptor* pcPelFormat = &( g_PlaYUVerPixFmtDescriptorsMap.at( iPixelFormat ) );
  UInt bytesPerPixel = ( bitsPixel - 1 ) / 8 + 1;
  UInt64 numberBytes = uiWidth * uiHeight;
  if( pcPelFormat->numberChannels > 1 )
  {
    UInt64 numberBytesChroma =
        CHROMASHIFT( uiWidth, pcPelFormat->log2ChromaWidth ) * CHROMASHIFT( uiHeight, pcPelFormat->log2ChromaHeight );
    numberBytes += ( pcPelFormat->numberChannels - 1 ) * numberBytesChroma;
  }
  return numberBytes * bytesPerPixel;
}

Void PlaYUVerFrame::clear()
{
  Pel* pPel;
  Pel uiValue = 128;
  Int ratioH, ratioW;
  UInt i;
  for( UInt ch = 0; ch < d->m_pcPelFormat->numberChannels; ch++ )
  {
    ratioW = ch > 0 ? d->m_pcPelFormat->log2ChromaWidth : 0;
    ratioH = ch > 0 ? d->m_pcPelFormat->log2ChromaHeight : 0;
    pPel = d->m_pppcInputPel[ch][0];
    for( i = 0; i < CHROMASHIFT( d->m_uiHeight, ratioH ) * CHROMASHIFT( d->m_uiWidth, ratioW ); i++ )
    {
      *pPel++ = uiValue;
    }
  }
}

Pel*** PlaYUVerFrame::getPelBufferYUV() const
{
  return d->m_pppcInputPel;
}
Pel*** PlaYUVerFrame::getPelBufferYUV()
{
  d->m_bHasHistogram = false;
  d->m_bHasRGBPel = false;
  return d->m_pppcInputPel;
}

UChar* PlaYUVerFrame::getRGBBuffer() const
{
  if( d->m_bHasRGBPel )
  {
    return d->m_pcARGB32;
  }
  return NULL;
}
UChar* PlaYUVerFrame::getRGBBuffer()
{
  if( !d->m_bHasRGBPel )
  {
    fillRGBBuffer();
  }
  return d->m_pcARGB32;
}

PlaYUVerPixel PlaYUVerFrame::getPixelValue( Int xPos, Int yPos )
{
  PlaYUVerPixel PixelValue( d->m_pcPelFormat->colorSpace );
  for( UInt ch = 0; ch < d->m_pcPelFormat->numberChannels; ch++ )
  {
    Int ratioH = ch > 0 ? d->m_pcPelFormat->log2ChromaWidth : 0;
    Int ratioW = ch > 0 ? d->m_pcPelFormat->log2ChromaHeight : 0;
    PixelValue[ch] = d->m_pppcInputPel[ch][( yPos >> ratioH )][( xPos >> ratioW )];
  }
  return PixelValue;
}

PlaYUVerPixel PlaYUVerFrame::getPixelValue( Int xPos, Int yPos, PlaYUVerPixel::ColorSpace eColorSpace )
{
  PlaYUVerPixel PixelValue( d->m_pcPelFormat->colorSpace );
  for( UInt ch = 0; ch < d->m_pcPelFormat->numberChannels; ch++ )
  {
    Int ratioH = ch > 0 ? d->m_pcPelFormat->log2ChromaWidth : 0;
    Int ratioW = ch > 0 ? d->m_pcPelFormat->log2ChromaHeight : 0;
    PixelValue[ch] = d->m_pppcInputPel[ch][( yPos >> ratioH )][( xPos >> ratioW )];
  }
  PixelValue = PixelValue.ConvertPixel( eColorSpace );
  return PixelValue;
}

Void PlaYUVerFrame::setPixelValue( Int xPos, Int yPos, PlaYUVerPixel pixel )
{
  for( UInt ch = 0; ch < d->m_pcPelFormat->numberChannels; ch++ )
  {
    Int ratioH = ch > 0 ? d->m_pcPelFormat->log2ChromaWidth : 0;
    Int ratioW = ch > 0 ? d->m_pcPelFormat->log2ChromaHeight : 0;
    d->m_pppcInputPel[ch][( yPos >> ratioH )][( xPos >> ratioW )] = pixel[ch];
  }
  d->m_bHasHistogram = false;
  d->m_bHasRGBPel = false;
}

Void PlaYUVerFrame::copyFrom( const PlaYUVerFrame& other )
{
  if( haveSameFmt( other, MATCH_ALL ) )
    return;
  d->m_bHasRGBPel = false;
  d->m_bHasHistogram = false;
  memcpy( *d->m_pppcInputPel[LUMA], other.getPelBufferYUV()[LUMA][0], getBytesPerFrame() * sizeof( Pel ) );
}

Void PlaYUVerFrame::copyFrom( const PlaYUVerFrame* other )
{
  if( other )
    copyFrom( *other );
}

Void PlaYUVerFrame::copyFrom( const PlaYUVerFrame& other, UInt xPos, UInt yPos )
{
  if( haveSameFmt( other, MATCH_ALL ) )
    return;
  Pel*** pInput = other.getPelBufferYUV();
  for( UInt ch = 0; ch < d->m_pcPelFormat->numberChannels; ch++ )
  {
    Int ratioH = ch > 0 ? d->m_pcPelFormat->log2ChromaWidth : 0;
    Int ratioW = ch > 0 ? d->m_pcPelFormat->log2ChromaHeight : 0;
    for( UInt i = 0; i < CHROMASHIFT( d->m_uiHeight, ratioH ); i++ )
    {
      memcpy( d->m_pppcInputPel[ch][i], &( pInput[ch][( yPos >> ratioH ) + i][( xPos >> ratioW )] ),
              ( d->m_uiWidth >> ratioW ) * sizeof( Pel ) );
    }
  }
  d->m_bHasRGBPel = false;
  d->m_bHasHistogram = false;
}

Void PlaYUVerFrame::copyFrom( const PlaYUVerFrame* other, UInt x, UInt y )
{
  if( other )
    copyFrom( *other, x, y );
}

Void PlaYUVerFrame::frameFromBuffer( Byte* Buff, Int iEndianness, UInt64 uiBuffSize )
{
  if( uiBuffSize != getBytesPerFrame() )
    return;

  frameFromBuffer( Buff, iEndianness );
}

Void PlaYUVerFrame::frameFromBuffer( Byte* Buff, Int iEndianness )
{
  Byte* ppBuff[MAX_NUMBER_PLANES];
  Byte* pTmpBuff;
  Pel* pPel;
  UInt bytesPixel = ( d->m_uiBitsPel - 1 ) / 8 + 1;
  Int ratioH, ratioW, step;
  UInt i, ch;
  Int startByte = 0;
  Int endByte = bytesPixel;
  Int incByte = 1;
  Int b;
  Int maxval = pow( 2, d->m_uiBitsPel ) - 1;

  if( iEndianness == 0 )
  {
    startByte = bytesPixel - 1;
    endByte = -1;
    incByte = -1;
  }

  ppBuff[0] = Buff;
  for( i = 1; i < MAX_NUMBER_PLANES; i++ )
  {
    ratioW = i > 1 ? d->m_pcPelFormat->log2ChromaWidth : 0;
    ratioH = i > 1 ? d->m_pcPelFormat->log2ChromaHeight : 0;
    ppBuff[i] = ppBuff[i - 1] + CHROMASHIFT( d->m_uiHeight, ratioH ) * CHROMASHIFT( d->m_uiWidth, ratioW ) * bytesPixel;
  }

  for( ch = 0; ch < d->m_pcPelFormat->numberChannels; ch++ )
  {
    ratioW = ch > 0 ? d->m_pcPelFormat->log2ChromaWidth : 0;
    ratioH = ch > 0 ? d->m_pcPelFormat->log2ChromaHeight : 0;
    step = d->m_pcPelFormat->comp[ch].step_minus1;

    pPel = d->m_pppcInputPel[ch][0];
    pTmpBuff = ppBuff[d->m_pcPelFormat->comp[ch].plane] + ( d->m_pcPelFormat->comp[ch].offset_plus1 - 1 );

    for( i = 0; i < CHROMASHIFT( d->m_uiHeight, ratioH ) * CHROMASHIFT( d->m_uiWidth, ratioW ); i++ )
    {
      *pPel = 0;
      for( b = startByte; b != endByte; b += incByte )
      {
        *pPel += *pTmpBuff << ( b * 8 );
        pTmpBuff++;
        // Check max value and bound it to "maxval" to prevent segfault when
        // calculating histogram
        if( *pPel > maxval )
          *pPel = 0;
        // -----
      }
      pPel++;
      pTmpBuff += step;
    }
  }
  d->m_bHasRGBPel = false;
  d->m_bHasHistogram = false;
}

Void PlaYUVerFrame::frameToBuffer( Byte* output_buffer, Int iEndianness )
{
  UInt bytesPixel = ( d->m_uiBitsPel - 1 ) / 8 + 1;
  Byte* ppBuff[MAX_NUMBER_PLANES];
  Byte* pTmpBuff;
  Pel* pTmpPel;
  Int ratioH, ratioW, step;
  UInt i, ch;
  Int startByte = 0;
  Int endByte = bytesPixel;
  Int incByte = 1;
  Int b;

  if( iEndianness == 0 )
  {
    startByte = bytesPixel - 1;
    endByte = -1;
    incByte = -1;
  }

  ppBuff[0] = output_buffer;
  for( Int i = 1; i < MAX_NUMBER_PLANES; i++ )
  {
    ratioW = i > 1 ? d->m_pcPelFormat->log2ChromaWidth : 0;
    ratioH = i > 1 ? d->m_pcPelFormat->log2ChromaHeight : 0;
    ppBuff[i] = ppBuff[i - 1] + CHROMASHIFT( d->m_uiHeight, ratioH ) * CHROMASHIFT( d->m_uiWidth, ratioW ) * bytesPixel;
  }

  for( ch = 0; ch < d->m_pcPelFormat->numberChannels; ch++ )
  {
    ratioW = ch > 0 ? d->m_pcPelFormat->log2ChromaWidth : 0;
    ratioH = ch > 0 ? d->m_pcPelFormat->log2ChromaHeight : 0;
    step = d->m_pcPelFormat->comp[ch].step_minus1;

    pTmpPel = d->m_pppcInputPel[ch][0];
    pTmpBuff = ppBuff[d->m_pcPelFormat->comp[ch].plane];

    for( i = 0; i < CHROMASHIFT( d->m_uiHeight, ratioH ) * CHROMASHIFT( d->m_uiWidth, ratioW ); i++ )
    {
      for( b = startByte; b != endByte; b += incByte )
      {
        *pTmpBuff = *pTmpPel >> ( 8 * b );
        pTmpBuff++;
      }
      pTmpPel++;
      pTmpBuff += step;
    }
  }
}

Void PlaYUVerFrame::fillRGBBuffer()
{
#define PEL_ARGB( a, r, g, b ) ( ( a & 0xff ) << 24 ) | ( ( r & 0xff ) << 16 ) | ( ( g & 0xff ) << 8 ) | ( b & 0xff )
#define PEL_RGB( r, g, b ) PEL_ARGB( 0xffu, r, g, b )
#define CLAMP_YUV2RGB( X ) X = X < 0 ? 0 : X > 255 ? 255 : X;
#define YUV2RGB( iY, iU, iV, iR, iG, iB )                          \
  iR = iY + ( ( 1436 * ( iV - 128 ) ) >> 10 );                     \
  iG = iY - ( ( 352 * ( iU - 128 ) + 731 * ( iV - 128 ) ) >> 10 ); \
  iB = iY + ( ( 1812 * ( iU - 128 ) ) >> 10 );                     \
  CLAMP_YUV2RGB( iR )                                              \
  CLAMP_YUV2RGB( iG )                                              \
  CLAMP_YUV2RGB( iB )

  if( d->m_bHasRGBPel )
    return;
  Int shiftBits = d->m_uiBitsPel - 8;
  UInt* pARGB = (UInt*)d->m_pcARGB32;
  if( d->m_pcPelFormat->colorSpace == PlaYUVerPixel::COLOR_GRAY )
  {
    Pel* pY = d->m_pppcInputPel[LUMA][0];
    for( UInt i = 0; i < d->m_uiHeight * d->m_uiWidth; i++ )
    {
      *pARGB++ = PEL_RGB( *pY, *pY, *pY );
      pY++;
    }
  }
  else if( d->m_pcPelFormat->colorSpace == PlaYUVerPixel::COLOR_RGB )
  {
    Pel* pR = d->m_pppcInputPel[COLOR_R][0];
    Pel* pG = d->m_pppcInputPel[COLOR_G][0];
    Pel* pB = d->m_pppcInputPel[COLOR_B][0];
    for( UInt i = 0; i < d->m_uiHeight * d->m_uiWidth; i++ )
      *pARGB++ = PEL_RGB( ( *pR++ ) >> shiftBits, ( *pG++ ) >> shiftBits, ( *pB++ ) >> shiftBits );
  }
  else if( d->m_pcPelFormat->colorSpace == PlaYUVerPixel::COLOR_RGBA )
  {
    Pel* pR = d->m_pppcInputPel[COLOR_R][0];
    Pel* pG = d->m_pppcInputPel[COLOR_G][0];
    Pel* pB = d->m_pppcInputPel[COLOR_B][0];
    Pel* pA = d->m_pppcInputPel[COLOR_A][0];
    for( UInt i = 0; i < d->m_uiHeight * d->m_uiWidth; i++ )
      *pARGB++ = PEL_ARGB( ( *pA++ ) >> shiftBits, ( *pR++ ) >> shiftBits, ( *pG++ ) >> shiftBits, ( *pB++ ) >> shiftBits );
  }
  else if( d->m_pcPelFormat->colorSpace == PlaYUVerPixel::COLOR_YUV )
  {
    Pel* pLineY = d->m_pppcInputPel[LUMA][0];
    Pel* pLineU = d->m_pppcInputPel[CHROMA_U][0];
    Pel* pLineV = d->m_pppcInputPel[CHROMA_V][0];
    UInt uiChromaStride = CHROMASHIFT( d->m_uiWidth, d->m_pcPelFormat->log2ChromaWidth );
    Pel* pY;
    Pel* pU;
    Pel* pV;
    Int iY, iU, iV, iR, iG, iB;
    UInt* pARGBLine = pARGB;
    UInt* pARGB;

    UInt y, x;
    Int i, j;
    for( y = 0; y < CHROMASHIFT( d->m_uiHeight, d->m_pcPelFormat->log2ChromaHeight ); y++ )
    {
      for( i = 0; i < 1 << d->m_pcPelFormat->log2ChromaHeight; i++ )
      {
        pY = pLineY;
        pU = pLineU;
        pV = pLineV;
        pARGB = pARGBLine;
        for( x = 0; x < CHROMASHIFT( d->m_uiWidth, d->m_pcPelFormat->log2ChromaWidth ); x++ )
        {
          iU = *pU++;
          iU >>= shiftBits;
          iV = *pV++;
          iV >>= shiftBits;
          for( j = 0; j < ( 1 << d->m_pcPelFormat->log2ChromaWidth ); j++ )
          {
            iY = *pY++;
            iY >>= shiftBits;
            YUV2RGB( iY, iU, iV, iR, iG, iB );
            *pARGB++ = PEL_RGB( iR, iG, iB );
          }
        }
        pLineY += d->m_uiWidth;
        pARGBLine += d->m_uiWidth;
      }
      pLineU += uiChromaStride;
      pLineV += uiChromaStride;
    }
  }
  d->m_bHasRGBPel = true;
}

/**
 * Histogram
 */

Void PlaYUVerFrame::calcHistogram()
{
  if( d->m_bHasHistogram || !d->m_puiHistogram )
    return;

  d->m_bHistogramRunning = true;

  xMemSet( UInt, d->m_uiHistoSegments * d->m_uiHistoChannels, d->m_puiHistogram );

  UInt i, j;
  UInt numberChannels = d->m_pcPelFormat->numberChannels;
  Int colorSpace = d->m_pcPelFormat->colorSpace;
  if( colorSpace == PlaYUVerPixel::COLOR_YUV )
  {
    const Pel* data[3];
    data[LUMA] = d->m_pppcInputPel[LUMA][0];
    data[CHROMA_U] = d->m_pppcInputPel[CHROMA_U][0];
    data[CHROMA_V] = d->m_pppcInputPel[CHROMA_V][0];
    for( i = 0; i < d->m_uiHeight * d->m_uiWidth; i++ )
    {
      d->m_puiHistogram[*( data[LUMA] ) + LUMA * d->m_uiHistoSegments]++;
      data[LUMA] += 1;
    }
    for( i = 0; ( i < getChromaLength() ); i++ )
    {
      for( j = 1; j < numberChannels; j++ )
      {
        d->m_puiHistogram[*( data[j] ) + j * d->m_uiHistoSegments]++;
        data[j] += 1;
      }
    }
  }
  else
  {
    const Pel* data[3];
    Pel luma;
    data[COLOR_R] = d->m_pppcInputPel[COLOR_R][0];
    data[COLOR_G] = d->m_pppcInputPel[COLOR_G][0];
    data[COLOR_B] = d->m_pppcInputPel[COLOR_B][0];

    for( i = 0; ( i < d->m_uiHeight * d->m_uiWidth ); i++ )
    {
      for( j = 0; j < numberChannels; j++ )
      {
        d->m_puiHistogram[*( data[j] ) + j * d->m_uiHistoSegments]++;
      }
      if( ( colorSpace == PlaYUVerPixel::COLOR_RGB || colorSpace == PlaYUVerPixel::COLOR_RGBA ) )
      {
        PlaYUVerPixel currPixel( colorSpace, *( data[COLOR_R] ), *( data[COLOR_G] ), *( data[COLOR_B] ) );
        luma = currPixel.ConvertPixel( PlaYUVerPixel::COLOR_YUV ).Y();
        d->m_puiHistogram[luma + j * d->m_uiHistoSegments]++;
      }
      for( j = 0; j < numberChannels; j++ )
        data[j] += 1;
    }
  }
  d->m_bHasHistogram = true;
  d->m_bHistogramRunning = false;
}

Int PlaYUVerFrame::getHistogramSegment()
{
  return d->m_uiHistoSegments;
}

Int getRealHistoChannel( Int colorSpace, Int channel )
{
  Int histoChannel;

  switch( colorSpace )
  {
  case PlaYUVerPixel::COLOR_GRAY:
    if( channel != LUMA )
    {
      return -1;
    }
    histoChannel = 0;
    break;
  case PlaYUVerPixel::COLOR_RGB:
    if( channel != COLOR_R && channel != COLOR_G && channel != COLOR_B && channel != LUMA )
    {
      return -1;
    }
    else
    {
      //       if( channel == LUMA )
      //       {
      //         histoChannel = 3;
      //       }
      //       else
      {
        histoChannel = channel;
      }
    }
    break;
  case PlaYUVerPixel::COLOR_RGBA:
    if( channel != COLOR_R && channel != COLOR_G && channel != COLOR_B && channel != COLOR_A && channel != LUMA )
    {
      return -1;
    }
    else
    {
      if( channel == LUMA )
      {
        histoChannel = 4;
      }
      else
      {
        histoChannel = channel;
      }
    }
    break;
  case PlaYUVerPixel::COLOR_YUV:
    if( channel != LUMA && channel != CHROMA_U && channel != CHROMA_V )
    {
      return -1;
    }
    else
    {
      histoChannel = channel;
    }
    break;
  default:
    histoChannel = -1;
  }
  return histoChannel;
}

UInt PlaYUVerFrame::getMin( Int channel )
{
  UInt value;
  Int histoChannel;
  Int indexStart;

  if( !d->m_bHasHistogram )
  {
    return 0;
  }

  histoChannel = getRealHistoChannel( getColorSpace(), channel );

  if( histoChannel == -1 )
  {
    return 0;
  }

  indexStart = histoChannel * d->m_uiHistoSegments;

  for( value = 0; value <= d->m_uiHistoSegments; value++ )
  {
    if( d->m_puiHistogram[indexStart + value] > 0 )
    {
      break;
    }
  }
  return value;
}

UInt PlaYUVerFrame::getMax( Int channel )
{
  Int value;
  Int histoChannel;
  Int indexStart;

  if( !d->m_bHasHistogram )
  {
    return 0;
  }

  histoChannel = getRealHistoChannel( getColorSpace(), channel );

  if( histoChannel == -1 )
  {
    return 0;
  }

  indexStart = histoChannel * d->m_uiHistoSegments;

  for( value = d->m_uiHistoSegments - 1; value >= 0; value-- )
  {
    if( d->m_puiHistogram[indexStart + value] > 0 )
    {
      break;
    }
  }
  return value;
}

UInt PlaYUVerFrame::getCount( Int channel, UInt start, UInt end )
{
  UInt i;
  Int histoChannel;
  Int indexStart;
  UInt count = 0;

  if( !d->m_bHasHistogram || start < 0 || end > d->m_uiHistoSegments - 1 || start > end )
  {
    return 0.0;
  }

  histoChannel = getRealHistoChannel( getColorSpace(), channel );

  if( histoChannel == -1 )
  {
    return 0;
  }

  indexStart = histoChannel * d->m_uiHistoSegments;

  for( i = start; i <= end; i++ )
  {
    count += d->m_puiHistogram[indexStart + i];
  }
  return count;
}

Double PlaYUVerFrame::getMean( Int channel, UInt start, UInt end )
{
  Int indexStart;
  Int histoChannel;
  Double mean = 0.0;
  Double count;

  if( !d->m_bHasHistogram || start < 0 || end > d->m_uiHistoSegments - 1 || start > end )
  {
    return 0.0;
  }

  histoChannel = getRealHistoChannel( getColorSpace(), channel );

  if( histoChannel == -1 )
  {
    return 0.0;
  }

  indexStart = histoChannel * d->m_uiHistoSegments;

  for( UInt i = start; i <= end; i++ )
  {
    mean += i * d->m_puiHistogram[indexStart + i];
  }

  count = getCount( channel, start, end );

  if( count > 0.0 )
  {
    return mean / count;
  }

  return mean;
}

Int PlaYUVerFrame::getMedian( Int channel, UInt start, UInt end )
{
  Int histoChannel;
  Int indexStart;
  Double sum = 0.0;
  Double count;

  if( !d->m_bHasHistogram || start < 0 || end > d->m_uiHistoSegments - 1 || start > end )
  {
    return 0;
  }

  histoChannel = getRealHistoChannel( getColorSpace(), channel );

  if( histoChannel == -1 )
  {
    return 0;
  }

  indexStart = histoChannel * d->m_uiHistoSegments;

  count = getCount( channel, start, end );

  for( UInt i = start; i <= end; i++ )
  {
    sum += d->m_puiHistogram[indexStart + i];
    if( sum * 2 > count )
      return i;
  }

  return 0;
}

Double PlaYUVerFrame::getStdDev( Int channel, UInt start, UInt end )
{
  Int histoChannel;
  Int indexStart;
  Double dev = 0.0;
  Double count;
  Double mean;

  if( !d->m_bHasHistogram || start < 0 || end > d->m_uiHistoSegments - 1 || start > end )
  {
    return 0.0;
  }

  histoChannel = getRealHistoChannel( getColorSpace(), channel );

  if( histoChannel == -1 )
  {
    return 0.0;
  }

  indexStart = histoChannel * d->m_uiHistoSegments;
  mean = getMean( channel, start, end );
  count = getCount( channel, start, end );

  if( count == 0.0 )
    count = 1.0;

  /*------------ original

	 for ( i = start ; i <= end ; i++ )
	 {
	 dev += ( i - mean ) * ( i - mean ) * d->m_puiHistogram[indexStart +i];
	 }

	 return sqrt( dev / count );

	 -----------------------*/

  for( UInt i = start; i <= end; i++ )
  {
    dev += ( i * i ) * d->m_puiHistogram[indexStart + i];
  }

  return sqrt( ( dev - count * mean * mean ) / ( count - 1 ) );
}

Double PlaYUVerFrame::getHistogramValue( Int channel, UInt bin )
{
  Double value;
  Int histoChannel;
  Int indexStart;

  if( !d->m_bHasHistogram || bin < 0 || bin > d->m_uiHistoSegments - 1 )
    return 0.0;

  histoChannel = getRealHistoChannel( getColorSpace(), channel );

  if( histoChannel == -1 )
  {
    return 0.0;
  }

  indexStart = histoChannel * d->m_uiHistoSegments;

  value = d->m_puiHistogram[indexStart + bin];

  return value;
}

Double PlaYUVerFrame::getMaximum( Int channel )
{
  Double max = 0.0;

  Int histoChannel;
  Int indexStart;

  if( !d->m_bHasHistogram )
    return 0.0;

  histoChannel = getRealHistoChannel( getColorSpace(), channel );

  if( histoChannel == -1 )
  {
    return 0.0;
  }

  indexStart = histoChannel * d->m_uiHistoSegments;

  for( UInt x = 0; x < d->m_uiHistoSegments; x++ )
  {
    if( d->m_puiHistogram[indexStart + x] > max )
    {
      max = d->m_puiHistogram[indexStart + x];
    }
  }

  return max;
}

/*
 **************************************************************
 * Interface to other libs
 **************************************************************
 */

Bool PlaYUVerFrame::toMat( cv::Mat& cvMat, Bool convertToGray )
{
  Bool bRet = false;
#ifdef USE_OPENCV
  if( convertToGray && !( d->m_pcPelFormat->colorSpace == PlaYUVerPixel::COLOR_YUV ||
                          d->m_pcPelFormat->colorSpace == PlaYUVerPixel::COLOR_GRAY ) )
  {
    return bRet;
  }
  Int cvType = CV_MAKETYPE( CV_8U, convertToGray ? 1 : d->m_pcPelFormat->numberChannels );
  cvMat.create( d->m_uiHeight, d->m_uiWidth, cvType );
  UChar* pCvPel = cvMat.data;
  if( !convertToGray && ( d->m_pcPelFormat->colorSpace == PlaYUVerPixel::COLOR_YUV ||
                          d->m_pcPelFormat->colorSpace == PlaYUVerPixel::COLOR_RGB ) )
  {
    fillRGBBuffer();
    UChar* pARGB = d->m_pcARGB32;
    for( UInt i = 0; i < d->m_uiWidth * d->m_uiHeight; i++ )
    {
      *pCvPel++ = *pARGB++;
      *pCvPel++ = *pARGB++;
      *pCvPel++ = *pARGB++;
      pARGB++;
    }
    bRet = true;
  }
  else if( convertToGray || d->m_pcPelFormat->colorSpace == PlaYUVerPixel::COLOR_GRAY )
  {
    Pel* pPel = d->m_pppcInputPel[LUMA][0];
    for( UInt i = 0; i < d->m_uiWidth * d->m_uiHeight; i++ )
    {
      *pCvPel++ = *pPel++;
    }
    bRet = true;
  }
  else
  {
    return false;
  }
#endif
  return bRet;
}

Bool PlaYUVerFrame::fromMat( cv::Mat& cvMat )
{
  Bool bRet = false;
#ifdef USE_OPENCV
  if( !d->m_bInit )
  {
    if( d->m_iPixelFormat == NO_FMT )
    {
      switch( cvMat.channels() )
      {
      case 1:
        d->m_iPixelFormat = findPixelFormat( "GRAY" );
        break;
      case 3:
        d->m_iPixelFormat = findPixelFormat( "BGR24" );
        break;
      default:
        return false;
      }
    }
    d->init( cvMat.cols, cvMat.rows, d->m_iPixelFormat, 8 );
  }

  d->m_bHasRGBPel = false;
  d->m_bHasHistogram = false;

  UChar* pCvPel = cvMat.data;
  if( d->m_iPixelFormat != GRAY )
  {
    Pel* pInputPelR = d->m_pppcInputPel[COLOR_R][0];
    Pel* pInputPelG = d->m_pppcInputPel[COLOR_G][0];
    Pel* pInputPelB = d->m_pppcInputPel[COLOR_B][0];
    for( UInt i = 0; i < d->m_uiHeight * d->m_uiWidth; i++ )
    {
      *pInputPelB++ = *pCvPel++;
      *pInputPelG++ = *pCvPel++;
      *pInputPelR++ = *pCvPel++;
    }
    bRet = true;
  }
  else
  {
    Pel* pPel = d->m_pppcInputPel[LUMA][0];
    for( UInt i = 0; i < d->m_uiWidth * d->m_uiHeight; i++ )
    {
      *pPel++ = *pCvPel++;
    }
    bRet = true;
  }
#endif
  return bRet;
}

/*
 **************************************************************
 * Quality Related Function API
 **************************************************************
 */

std::vector<String> PlaYUVerFrame::supportedQualityMetricsList()
{
  return std::vector<String>{
      "PSNR",
      "MSE",
      "SSIM",
  };
}

Double PlaYUVerFrame::getQuality( Int Metric, PlaYUVerFrame* Org, Int component )
{
  switch( Metric )
  {
  case PSNR_METRIC:
    return getPSNR( Org, component );
    break;
  case MSE_METRIC:
    return getMSE( Org, component );
    break;
  case SSIM_METRIC:
    return getSSIM( Org, component );
    break;
  default:
    assert( 0 );
  };
  return 0;
}

Double PlaYUVerFrame::getMSE( PlaYUVerFrame* Org, Int component )
{
  Pel* pPelYUV = getPelBufferYUV()[component][0];
  Pel* pOrgPelYUV = Org->getPelBufferYUV()[component][0];

  Int aux_pel_1, aux_pel_2;
  Int diff = 0;
  Double ssd = 0;

  UInt numberOfPixels = 0;
  if( component == LUMA )
  {
    numberOfPixels = Org->getHeight() * Org->getWidth();
  }
  else
  {
    numberOfPixels = getChromaLength();
  }

  for( UInt i = 0; i < numberOfPixels; i++ )
  {
    aux_pel_1 = *pPelYUV++;
    aux_pel_2 = *pOrgPelYUV++;
    diff = aux_pel_1 - aux_pel_2;
    ssd += ( Double )( diff * diff );
  }
  if( ssd == 0.0 )
  {
    return 0.0;
  }
  return ssd / Double( numberOfPixels );
}

Double PlaYUVerFrame::getPSNR( PlaYUVerFrame* Org, Int component )
{
  UInt uiMaxValue = ( 1 << Org->getBitsPel() ) - 1;
  Double uiMaxValueSquare = uiMaxValue * uiMaxValue;
  Double dPSNR = 100;
  Double dMSE = getMSE( Org, component );
  if( dMSE != 0 )
    dPSNR = 10 * log10( uiMaxValueSquare / dMSE );
  return dPSNR;
}

float compute_ssim( Pel** refImg, Pel** encImg, Int width, Int height, Int win_width, Int win_height,
                    Int max_pel_value_comp, Int overlapSize )
{
  static const float K1 = 0.01f, K2 = 0.03f;
  float max_pix_value_sqd;
  float C1, C2;
  float win_pixels = (float)( win_width * win_height );
#ifdef UNBIASED_VARIANCE
  float win_pixels_bias = win_pixels - 1;
#else
  float win_pixels_bias = win_pixels;
#endif
  float mb_ssim, meanOrg, meanEnc;
  float varOrg, varEnc, covOrgEnc;
  int imeanOrg, imeanEnc, ivarOrg, ivarEnc, icovOrgEnc;
  float cur_distortion = 0.0;
  int i, j, n, m, win_cnt = 0;

  max_pix_value_sqd = (float)( max_pel_value_comp * max_pel_value_comp );
  C1 = K1 * K1 * max_pix_value_sqd;
  C2 = K2 * K2 * max_pix_value_sqd;

  for( j = 0; j <= height - win_height; j += overlapSize )
  {
    for( i = 0; i <= width - win_width; i += overlapSize )
    {
      imeanOrg = 0;
      imeanEnc = 0;
      ivarOrg = 0;
      ivarEnc = 0;
      icovOrgEnc = 0;

      for( n = j; n < j + win_height; n++ )
      {
        for( m = i; m < i + win_width; m++ )
        {
          imeanOrg += refImg[n][m];
          imeanEnc += encImg[n][m];
          ivarOrg += refImg[n][m] * refImg[n][m];
          ivarEnc += encImg[n][m] * encImg[n][m];
          icovOrgEnc += refImg[n][m] * encImg[n][m];
        }
      }

      meanOrg = (float)imeanOrg / win_pixels;
      meanEnc = (float)imeanEnc / win_pixels;

      varOrg = ( (float)ivarOrg - ( (float)imeanOrg ) * meanOrg ) / win_pixels_bias;
      varEnc = ( (float)ivarEnc - ( (float)imeanEnc ) * meanEnc ) / win_pixels_bias;
      covOrgEnc = ( (float)icovOrgEnc - ( (float)imeanOrg ) * meanEnc ) / win_pixels_bias;

      mb_ssim = (float)( ( 2.0 * meanOrg * meanEnc + C1 ) * ( 2.0 * covOrgEnc + C2 ) );
      mb_ssim /= (float)( meanOrg * meanOrg + meanEnc * meanEnc + C1 ) * ( varOrg + varEnc + C2 );

      cur_distortion += mb_ssim;
      win_cnt++;
    }
  }

  cur_distortion /= (float)win_cnt;

  if( cur_distortion >= 1.0 && cur_distortion < 1.01 )  // avoid float accuracy problem at very low QP(e.g.2)
    cur_distortion = 1.0;

  return cur_distortion;
}

Double PlaYUVerFrame::getSSIM( PlaYUVerFrame* Org, Int component )
{
  Double dSSIM = 1;
  if( component == LUMA )
  {
    dSSIM = compute_ssim( d->m_pppcInputPel[component], Org->getPelBufferYUV()[component], d->m_uiWidth, d->m_uiHeight,
                          8, 8, 255, 8 );
  }
  else
  {
    dSSIM = compute_ssim( d->m_pppcInputPel[component], Org->getPelBufferYUV()[component], getChromaWidth(),
                          getChromaHeight(), 4, 4, 255, 4 );
  }
  return dSSIM;
}
