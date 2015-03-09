/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014-2015  by Luis Lucas      (luisfrlucas@gmail.com)
 *                                Joao Carreira   (jfmcarreira@gmail.com)
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
 * \file     PlaYUVerFrame.h
 * \brief    Video Frame handling
 */

#ifndef __PLAYUVERFRAME_H__
#define __PLAYUVERFRAME_H__

#include <iostream>
#include <cstdio>
#include <cassert>
#include <vector>
#include "PlaYUVerDefs.h"
#include "PlaYUVerFrameStats.h"

class PixFcSSE;

namespace plaYUVer
{

class PlaYUVerFrame : public PlaYUVerFrameStats
{
public:
  class Pixel
  {
  private:
    Int m_iColorSpace;
    Pel PixelComponents[3];
  public:
    Pixel()
    {
      m_iColorSpace = PlaYUVerFrame::COLOR_INVALID;
      PixelComponents[0] = 0;
      PixelComponents[1] = 0;
      PixelComponents[2] = 0;
    }
    Pixel( Int ColorSpace, Pel c0, Pel c1, Pel c2 )
    {
      m_iColorSpace = ColorSpace == PlaYUVerFrame::COLOR_GRAY ? PlaYUVerFrame::COLOR_YUV : ColorSpace;
      PixelComponents[0] = c0;
      PixelComponents[1] = c1;
      PixelComponents[2] = c2;
    }
    Int ColorSpace() { return m_iColorSpace; }
    Pel* Components()  { return PixelComponents; }
    Pel& Y()  { return PixelComponents[0]; }
    Pel& Cb() { return PixelComponents[1]; }
    Pel& Cr() { return PixelComponents[2]; }
    Pel& R()  { return PixelComponents[0]; }
    Pel& G()  { return PixelComponents[1]; }
    Pel& B()  { return PixelComponents[2]; }
  };

  /** ColorSpace Enum
    * List of supported color spaces
    */
  enum ColorSpace
  {
    COLOR_INVALID = -1,
    COLOR_YUV = 0,
    COLOR_RGB,
    COLOR_ARGB,
    COLOR_GRAY,
  };

  /** ColorSpace Enum
    * List of supported pixel formats (old)
    */
  enum PixelFormats
  {
    NO_FMT = -1,
    YUV420p = 0,
    YUV444p,
    YUV422p,
    YUYV422,
    GRAY,
    RGB24,
    BRG24,
    NUMBER_FORMATS
  };

  /**
    * Function that handles the supported pixel formats
    * of PlaYUVerFrame
    * @@return vector of strings with pixel formats names
    *
    */
  static std::vector<std::string> supportedPixelFormatListNames();

  /**
    * Creates a new frame using the following configuration
    *
    * @param width width of the frame
    * @param height height of the frame
    * @param pel_format pixel format index (always use PixelFormats enum)
    *
    * @note this function might misbehave if the pixel format enum is not correct
    *
    */
  PlaYUVerFrame( UInt width = 0, UInt height = 0, Int pel_format = 0 );
  
  /**
    * Creates and new frame with the configuration of an
    * existing one and copy its contents
    *
    * @param other existing frame to copy from
    *
    */
  PlaYUVerFrame( PlaYUVerFrame * );
  
  /**
    * Creates and new frame with the configuration of an
    * existing one and copy its contents. 
    * This function only copies a specific region from the existing frame
    *
    * @param other existing frame to copy from
    * @param posX position X to crop from
    * @param posY position Y to crop from
    * @param areaWidth crop width
    * @param areaHeight crop height
    *
    */
  PlaYUVerFrame( PlaYUVerFrame *other, UInt posX, UInt posY, UInt areaWidth, UInt areaHeight );
  ~PlaYUVerFrame();

  UInt64 getBytesPerFrame();
  static UInt64 getBytesPerFrame( UInt, UInt, Int );

  Int getColorSpace() const;
  UInt getNumberChannels() const;
  UInt getChromaWidth() const;
  UInt getChromaHeight() const;
  UInt getChromaLength() const;

  Void frameFromBuffer( Pel*, UInt64 );
  Void frameToBuffer( Pel* );

  Void fillRGBBuffer();

  Void copyFrom( PlaYUVerFrame* );

  static Void adjustSelectedAreaDims( UInt& posX, UInt& posY, UInt& areaWidth, UInt& areaHeight, Int pelFormat );
  Void copyFrom( PlaYUVerFrame*, UInt, UInt );

  PlaYUVerFrame::Pixel getPixelValue( Int, Int, Int );
  static PlaYUVerFrame::Pixel ConvertPixel( Pixel, Int );

  UInt getWidth() const
  {
    return m_uiWidth;
  }
  UInt getHeight() const
  {
    return m_uiHeight;
  }
  Int getPelFormat() const
  {
    return m_iPixelFormat;
  }
  Int getBitsChannel() const
  {
    return m_iBitsChannels;
  }
  Bool isValid() const
  {
    return ( m_uiWidth > 0 ) && ( m_uiHeight > 0 ) && ( m_iPixelFormat >= 0 );
  }
  Bool haveSameFmt( PlaYUVerFrame* other ) const
  {
    if( other )
    {
      return ( m_uiWidth == other->getWidth() ) && ( m_uiHeight == other->getHeight() ) && ( m_iPixelFormat == other->getPelFormat() );
    }
    else
    {
      return false;
    }
  }
  Pel*** getPelBufferYUV() const
  {
    return m_pppcInputPel;
  }
  Pel*** getPelBufferYUV()
  {
    m_bHasHistogram = false;
    m_bHasRGBPel = false;
    return m_pppcInputPel;
  }
  UChar* getRGBBuffer() const
  {
    return m_pcRGB32;
  }

  /**
   * Interface to other libs
   */
  Void getCvMat( Void** );
  Void fromCvMat( Void* );

  /**
   * Quality metrics interface
   */

  enum QualityMetrics
  {
    NO_METRIC = -1,
    PSNR_METRIC = 0,
    MSE_METRIC,
    SSIM_METRIC,
    NUMBER_METRICS,
  };

  static std::vector<std::string>  supportedQualityMetricsList()
  {
    std::vector<std::string> metrics;
    metrics.push_back( "PSNR" );
    metrics.push_back( "MSE" );
    metrics.push_back( "SSIM" );
    return metrics;
  }

  Double getQuality( Int Metric, PlaYUVerFrame* Org, Int component );
  Double getMSE( PlaYUVerFrame* Org, Int component );
  Double getPSNR( PlaYUVerFrame* Org, Int component );
  Double getSSIM( PlaYUVerFrame* Org, Int component );

private:

  struct structPlaYUVerFramePelFormat* m_pcPelFormat;

  UInt m_uiWidth; //!< Width of the frame
  UInt m_uiHeight; //!< Height of the frame
  Int m_iPixelFormat; //!< Pixel format number (it follows the list of supported pixel formats)
  Int m_iNumberChannels;
  Int m_iBitsChannels;

  Pel*** m_pppcInputPel; 
  
  Bool m_bHasRGBPel; //!< Flag indicating that the ARGB buffer was computed
  UChar* m_pcRGB32; //!< Buffer with the ARGB pixels used in Qt libs

  Void init( UInt width, UInt height, Int pel_format );
  Void openPixfc();
  Void closePixfc();
  PixFcSSE* m_pcPixfc;
  Void FrametoRGB8Pixfc();
};

}  // NAMESPACE

#endif // __PLAYUVERFRAME_H__
