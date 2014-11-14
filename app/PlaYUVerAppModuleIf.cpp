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
 * \file     PlaYUVerAppModuleIf.cpp
 * \brief    PlaYUVer modules interface
 */

#include "PlaYUVerAppModuleIf.h"
#include "SubWindowHandle.h"

namespace plaYUVer
{

PlaYUVerAppModuleIf::PlaYUVerAppModuleIf() :
        m_pcAction( NULL ),
        m_pcDisplaySubWindow( NULL ),
        m_pcDockWidget( NULL ),
        m_pcModuleDock( NULL ),
        m_pcModuleStream( NULL ),
        m_pcProcessedFrame( NULL ),
        m_dMeasurementResult( 0 )
{
  for( Int i = 0; i < MAX_NUMBER_FRAMES; i++ )
  {
    m_pcSubWindow[i] = NULL;
  }
}

void PlaYUVerAppModuleIf::run()
{
  m_pcProcessedFrame = NULL;
  m_dMeasurementResult = 0;

  if( m_pcModule->m_cModuleDef.m_iModuleType == FRAME_PROCESSING_MODULE )
  {
    switch( m_pcModule->m_cModuleDef.m_uiNumberOfFrames )
    {
    case MODULE_REQUIRES_ONE_FRAME:
      m_pcProcessedFrame = m_pcModule->process( m_pcSubWindow[0]->getCurrFrame() );
      break;
    case MODULE_REQUIRES_TWO_FRAMES:
      m_pcProcessedFrame = m_pcModule->process( m_pcSubWindow[0]->getCurrFrame(), m_pcSubWindow[1]->getCurrFrame() );
      break;
    case MODULE_REQUIRES_THREE_FRAMES:
      m_pcProcessedFrame = m_pcModule->process( m_pcSubWindow[0]->getCurrFrame(), m_pcSubWindow[1]->getCurrFrame(), m_pcSubWindow[2]->getCurrFrame() );
      break;
    }
  }
  else if( m_pcModule->m_cModuleDef.m_iModuleType == FRAME_MEASUREMENT_MODULE )
  {
    switch( m_pcModule->m_cModuleDef.m_uiNumberOfFrames )
    {
    case MODULE_REQUIRES_ONE_FRAME:
      m_dMeasurementResult = m_pcModule->measure( m_pcSubWindow[0]->getCurrFrame() );
      break;
    case MODULE_REQUIRES_TWO_FRAMES:
      m_dMeasurementResult = m_pcModule->measure( m_pcSubWindow[0]->getCurrFrame(), m_pcSubWindow[1]->getCurrFrame() );
      break;
    case MODULE_REQUIRES_THREE_FRAMES:
      m_dMeasurementResult = m_pcModule->measure( m_pcSubWindow[0]->getCurrFrame(), m_pcSubWindow[1]->getCurrFrame(), m_pcSubWindow[2]->getCurrFrame() );
      break;
    }
  }
  else
  {
    return;
  }
  postProgress( true );
  return;
}

Void PlaYUVerAppModuleIf::postProgress( Bool success )
{
  EventData *eventData = new EventData( success, this );
  if( parent() )
    QCoreApplication::postEvent( parent(), eventData );
}

}  // NAMESPACE
