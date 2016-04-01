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
 * \file     PlaYUVerStream.h
 * \ingroup  PlaYUVerLib
 * \brief    Input stream handling
 */

#ifndef __PLAYUVERSTREAM_H__
#define __PLAYUVERSTREAM_H__

#include "PlaYUVerDefs.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include "PlaYUVerFrame.h"

namespace plaYUVer
{

class LibAvContextHandle;
class LibOpenCVHandler;

typedef struct
{
  std::string shortName;
  UInt uiWidth;
  UInt uiHeight;
} PlaYUVerStdResolution;

typedef struct
{
  std::string formatName;
  std::string formatExt;
} PlaYUVerSupportedFormat;

#define INI_REGIST_PLAYUVER_SUPPORTED_FMT \
  std::vector<PlaYUVerSupportedFormat> formatsList; \
  PlaYUVerSupportedFormat formatElem;

#define END_REGIST_PLAYUVER_SUPPORTED_FMT \
        return formatsList;

#define REGIST_PLAYUVER_SUPPORTED_FMT( name, ext ) \
        formatElem.formatName = name; \
        formatElem.formatExt = lowercase( ext ); \
        formatsList.push_back( formatElem );

#define APPEND_PLAYUVER_SUPPORTED_FMT( class_name, fct ) \
  { \
    std::vector<PlaYUVerSupportedFormat> new_fmts = class_name::supported##fct##Formats(); \
    formatsList.insert( formatsList.end(), new_fmts.begin(), new_fmts.end() ); \
  }

/**
 * \class PlaYUVerStream
 * \ingroup  PlaYUVerLib PlaYUVerLib_Stream
 */
class PlaYUVerStream
{
public:

  static std::vector<PlaYUVerSupportedFormat> supportedReadFormats();
  static std::vector<PlaYUVerSupportedFormat> supportedWriteFormats();
  static std::vector<PlaYUVerSupportedFormat> supportedSaveFormats();

  static std::vector<PlaYUVerStdResolution> stdResolutionSizes();

  PlaYUVerStream();
  ~PlaYUVerStream();

  enum PlaYUVerStreamErrors
  {
    NO_STREAM_ERROR = 0,
    READING,
    WRITING,
    LAST_FRAME,
    END_OF_SEQ,
  };

  Bool open( std::string filename, std::string resolution, std::string input_format, UInt bitsPel, Int endianness, UInt frame_rate,  Bool bInput = true );
  Bool open( std::string filename, UInt width, UInt height, Int input_format, UInt bitsPel, Int endianness, UInt frame_rate, Bool bInput = true );

  Bool reload();

  Void close();

  Bool openFile();
  Void closeFile();

  Void getFormat( UInt& rWidth, UInt& rHeight, Int& rInputFormat, UInt& rBitsPerPel, Int& rEndianness, UInt& rFrameRate );

  Void loadAll();

  Void readFrame();
  Void readFrameFillRGBBuffer();
  Void writeFrame();
  Void writeFrame( PlaYUVerFrame *pcFrame );

  Bool saveFrame( const std::string& filename );
  static Bool saveFrame( const std::string& filename, PlaYUVerFrame *saveFrame );

  Bool setNextFrame();
  PlaYUVerFrame* getCurrFrame();
  PlaYUVerFrame* getCurrFrame( PlaYUVerFrame * );
  PlaYUVerFrame* getNextFrame();

  Bool seekInputRelative( Bool bIsFoward );
  Bool seekInput( UInt64 new_frame_num );

  Bool isInit()
  {
    return m_bInit;
  }

  std::string getFileName()
  {
    return m_cFilename;
  }
  std::string getFormatName()
  {
    return m_cFormatName;
  }
  std::string getCodecName()
  {
    return m_cCodedName;
  }

  UInt getFrameNum()
  {
    return m_uiTotalFrameNum;
  }
  UInt getWidth() const
  {
    return m_uiWidth;
  }
  UInt getHeight() const
  {
    return m_uiHeight;
  }
  Int getCurrFrameNum()
  {
    return m_iCurrFrameNum;
  }
  Double getFrameRate()
  {
    return m_dFrameRate;
  }
  Void getDuration( Int* duration_array );

private:

  Bool m_bInit;
  Bool m_bIsInput;
  Bool m_bIsOpened;

  Bool m_bLoadAll;

  enum PlaYUVerStreamHandlers
  {
    INVALID_HANDLER = -1,
    YUV_IO,
    OPENCV_HANDLER,
    FFMPEG,
    TOTAL_HANDLERR
  };
  Int m_iStreamHandler;
  LibAvContextHandle* m_cLibAvContext;

  std::string m_cFilename;
  Char* m_pchFilename;

  std::string m_cFormatName;
  std::string m_cCodedName;

  UInt m_uiWidth;
  UInt m_uiHeight;
  Int m_iPixelFormat;
  UInt m_uiBitsPerPixel;
  Int m_iEndianness;
  Double m_dFrameRate;
  UInt64 m_uiTotalFrameNum;
  Int64 m_iCurrFrameNum;

  FILE* m_pFile; /**< The input file pointer >*/
  std::fstream m_fsIOStream; /**< The input file pointer >*/
  Byte* m_pStreamBuffer;

  UInt m_uiFrameBufferSize;
  PlaYUVerFrame **m_ppcFrameBuffer;
  PlaYUVerFrame *m_pcCurrFrame;
  PlaYUVerFrame *m_pcNextFrame;
  UInt m_uiFrameBufferIndex;
  UInt64 m_uiCurrFrameFileIdx;

  Void findHandler();

};

}  // NAMESPACE

#endif // __PLAYUVERSTREAM_H__
