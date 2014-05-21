/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014  by Luis Lucas      (luisfrlucas@gmail.com)
 *                           Joao Carreira   (jfmcarreira@gmail.com)
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
 * \file     PlaYUVerModuleIf.h
 * \brief    PlaYUVer modules interface
 */

#ifndef __PLAYUVERMODULESIF_H__
#define __PLAYUVERMODULESIF_H__

#include "config.h"
#include "PlaYUVerDefs.h"
#include <iostream>
#include <cstdio>
#include "PlaYUVerFrame.h"

class QImage;
class QAction;

namespace plaYUVer
{

enum __PlaYUVerModuleTypes
{
  FRAME_LEVEL_MODULE,
  VIDEO_LEVEL_MODULE,
};

#define APPLY_WHILE_PLAYING true
#define MAX_NUMBER_FRAMES 3

enum __PlaYUVerModuleNumberOfFrames
{
  // Number of frames
  MODULE_REQUIRES_ONE_FRAME = 1,
  MODULE_REQUIRES_TWO_FRAMES = 2,
  MODULE_REQUIRES_THREE_FRAMES = 3,
};

enum __PlaYUVerModuleRequirements
{
  // Requirements
  MODULE_REQUIRES_NOTHING = 0,
  MODULE_REQUIRES_NEW_WINDOW = 2,
  MODULE_REQUIRES_SIDEBAR = 4,
  MODULES_REQUIREMENTS_TOTAL,
};

#define REGISTER_MODULE(X)                      \
    {                                           \
      if( USE_##X )                             \
      {                                         \
        X *pMod = new X();                      \
        appendModule( pMod );                   \
      }                                         \
    }

typedef struct __PlaYUVerModuleDefinition
{
  Int m_iModuleType;
  const Char* m_pchModuleCategory;
  const Char* m_pchModuleName;
  const Char* m_pchModuleTooltip;
  UInt m_uiNumberOfFrames;
  UInt m_uiModuleRequirements;
  Bool m_bApplyWhilePlaying;
} PlaYUVerModuleDefinition;

class SubWindowHandle;

class PlaYUVerModuleIf
{
  friend class ModulesHandle;
private:
  QAction* m_pcAction;
  SubWindowHandle* m_pcSubWindow[MAX_NUMBER_FRAMES];
  SubWindowHandle* m_pcDisplaySubWindow;
  PlaYUVerModuleDefinition m_cModuleDef;
public:
  PlaYUVerModuleIf() :
          m_pcAction( NULL ),
          m_pcDisplaySubWindow( NULL )
  {
    for( Int i = 0; i < MAX_NUMBER_FRAMES; i++ )
    {
      m_pcSubWindow[i] = NULL;
    }
  }
  virtual ~PlaYUVerModuleIf() { }

  virtual Void create()                 { }
  virtual Void create( PlaYUVerFrame* ) { }

  virtual Void              process()                                                 {               }
  virtual PlaYUVerFrame*    process( PlaYUVerFrame* )                                 { return NULL;  }
  virtual PlaYUVerFrame*    process( PlaYUVerFrame*, PlaYUVerFrame* )                 { return NULL;  }
  virtual PlaYUVerFrame*    process( PlaYUVerFrame*, PlaYUVerFrame*, PlaYUVerFrame* ) { return NULL;  }

  virtual Void destroy(){ }

  Void setModuleDefinition( PlaYUVerModuleDefinition def )  { m_cModuleDef = def; }
};

}  // NAMESPACE

#endif // __PLAYUVERMODULESIF_H__

