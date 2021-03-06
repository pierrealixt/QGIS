/***************************************************************************
    qgsmdaldataitems.h
    ------------------
    begin                : April 2018
    copyright            : (C) 2018 by Peter Petrik
    email                : zilolv at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef QGSMDALDATAITEMS_H
#define QGSMDALDATAITEMS_H

#include "qgsdataitem.h"

#include <QString>

class QgsMdalLayerItem : public QgsLayerItem
{
    Q_OBJECT
  public:
    QgsMdalLayerItem( QgsDataItem *parent, const QString &name, const QString &path, const QString &uri );

    QString layerName() const override;
};

#endif // QGSMDALDATAITEMS_H
