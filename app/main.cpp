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
 * \file     main.cpp
 * \brief    main file
 */

#include "config.h"
#include "lib/PlaYUVerDefs.h"
#include <QApplication>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusReply>
#include "PlaYUVerAppAdaptor.h"
#include "RunningInstanceInfo.h"
#include "plaYUVerApp.h"
#include "SubWindowHandle.h"
#ifdef USE_FERVOR
#include "fvupdater.h"
#endif

using namespace plaYUVer;

#if _WIN32
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

int main( int argc, char *argv[] )
{
  qRegisterMetaType<PlaYUVerStreamInfoVector>( "PlaYUVerStreamInfoVector" );
  qRegisterMetaTypeStreamOperators<PlaYUVerStreamInfoVector>( "PlaYUVerStreamInfoVector" );

  QApplication application( argc, argv );
  QApplication::setApplicationName( "PlaYUVer" );
  QApplication::setApplicationVersion( PLAYUVER_VERSION_STRING );
  QApplication::setOrganizationName( "pixlra" );
  QApplication::setOrganizationDomain( "playuver.pixlra" );

  /**
   * use dbus, if available
   * allows for resuse of running Kate instances
   */
  QDBusConnectionInterface * const sessionBusInterface = QDBusConnection::sessionBus().interface();
  if( sessionBusInterface )
  {
    RunningInstanceInfoMap mapSessionRii;
    if( !fillinRunningAppInstances( &mapSessionRii ) )
    {
      return 1;
    }
    QStringList playuverServices;
    for( RunningInstanceInfoMap::const_iterator it = mapSessionRii.constBegin(); it != mapSessionRii.constEnd(); ++it )
    {
      playuverServices << ( *it )->serviceName;
    }
    QString serviceName;

    //const QStringList urls = parser.positionalArguments();

    Bool force_new = false;

    //cleanup map
    cleanupRunningAppInstanceMap( &mapSessionRii );

    //if no new instance is forced and no already opened session is requested,
    //check if a pid is given, which should be reused.
    // two possibilities: pid given or not...
//    if( ( !force_new ) && serviceName.isEmpty() )
//    {
//      if( /*( parser.isSet( usePidOption ) )*/|| ( !qgetenv( "PLAYUVER_PID" ).isEmpty() ) )
//      {
//        QString usePid = ( parser.isSet( usePidOption ) ) ? parser.value( usePidOption ) : QString::fromLocal8Bit( qgetenv( "KATE_PID" ) );
//
//        serviceName = QStringLiteral("org.kde.kate-")+ usePid;
//        if( !playuverServices.contains( serviceName ) )
//        {
//          serviceName.clear();
//        }
//      }
//    }

    QStringList filenameList;
    for( Int i = 1; i < argc; i++ )
    {
      filenameList.append( QString( argv[i] ) );
    }
    if( filenameList.isEmpty() )
    {
      force_new = true;
    }

    if( ( !force_new ) && ( serviceName.isEmpty() ) )
    {
      if( playuverServices.count() > 0 )
      {
        serviceName = playuverServices[0];
      }
    }

    //check again if service is still running
    bool foundRunningService = false;
    if( !serviceName.isEmpty() )
    {
      QDBusReply<bool> there = sessionBusInterface->isServiceRegistered( serviceName );
      foundRunningService = there.isValid() && there.value();
    }

    if( foundRunningService )
    {
      // open given session
      QDBusMessage m = QDBusMessage::createMethodCall( serviceName, QStringLiteral( PLAYUVER_DBUS_PATH ), QStringLiteral( PLAYUVER_DBUS_SESSION_NAME ),
          QStringLiteral( "activate" ) );

      QDBusConnection::sessionBus().call( m );

      // only block, if files to open there....
      bool needToBlock = false;

      QStringList tokens;

      // open given files...
      foreach(const QString & file, filenameList){
      QDBusMessage m = QDBusMessage::createMethodCall(serviceName,
          QStringLiteral(PLAYUVER_DBUS_PATH), QStringLiteral(PLAYUVER_DBUS_SESSION_NAME), QStringLiteral("loadFile"));

      QList<QVariant> dbusargs;

      dbusargs.append( file );

      m.setArguments(dbusargs);

      QDBusMessage res = QDBusConnection::sessionBus().call(m);
      if (res.type() == QDBusMessage::ReplyMessage)
      {
        if (res.arguments().count() == 1)
        {
          QVariant v = res.arguments()[0];
          if (v.isValid())
          {
            QString s = v.toString();
            if ((!s.isEmpty()) && (s != QStringLiteral("ERROR")))
            {
              tokens << s;
            }
          }
        }
      }
    }

    // this will wait until exiting is emitted by the used instance, if wanted...
      return needToBlock ? application.exec() : 0;
    }
  }

  /**
   * if we arrive here, we need to start a new playuver instance!
   */
  plaYUVerApp mainwindow;
  mainwindow.show();
  mainwindow.parseArgs( argc, argv );
#ifdef USE_FERVOR
  FvUpdater::sharedUpdater()->SetFeedURL("http://192.168.96.201/share/pixLRA/plaYUVer/PlaYUVerUpdate.xml");
  FvUpdater::sharedUpdater()->SetDependencies(DEPENDENCIES_STRING);
#endif

  QDBusConnection::sessionBus().registerService( PLAYUVER_DBUS_SESSION_NAME );

  return application.exec();
}
