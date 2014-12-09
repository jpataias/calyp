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
 * \file     SubWindowHandle.h
 * \brief    Sub windows handling
 */

#ifndef __SUBWINDOWHANDLE_H__
#define __SUBWINDOWHANDLE_H__

#include "config.h"
#include "lib/PlaYUVerDefs.h"
#if( QT_VERSION_PLAYUVER == 5 )
#include <QtWidgets>
#elif( QT_VERSION_PLAYUVER == 4 )
#include <QtGui>
#endif
#include "ModulesHandle.h"

namespace plaYUVer
{

class SubWindowHandle: public QMdiSubWindow
{
Q_OBJECT

private:
  UInt m_uiCategory;

public:
  enum SubWindowCategories
  {
    VIDEO_SUBWINDOW = 0,
    MODULE_SUBWINDOW,
    PLOT_SUBWINDOW,
  }
  SubWindowHandle( QWidget * parent, UInt category );
  ~SubWindowHandle();
};

}  // NAMESPACE

#endif // __SUBWINDOWHANDLE_H__

