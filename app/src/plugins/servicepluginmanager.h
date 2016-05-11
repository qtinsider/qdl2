/*
 * Copyright (C) 2016 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 3, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef SERVICEPLUGINMANAGER_H
#define SERVICEPLUGINMANAGER_H

#include "serviceplugin.h"
#include "servicepluginconfig.h"
#include <QDateTime>
#include <QPointer>

struct ServicePluginPair
{
    ServicePluginPair(ServicePluginConfig* c, ServicePlugin* p) :
        config(c),
        plugin(p)
    {
    }

    QPointer<ServicePluginConfig> config;
    QPointer<ServicePlugin> plugin;
};

typedef QList<ServicePluginPair> ServicePluginList;

class ServicePluginManager : public QObject
{
    Q_OBJECT

public:
    ~ServicePluginManager();

    static ServicePluginManager* instance();

    ServicePluginList plugins() const;

public Q_SLOTS:
    ServicePluginConfig* getConfigById(const QString &id) const;
    ServicePluginConfig* getConfigByUrl(const QString &url) const;

    ServicePlugin* getPluginById(const QString &id) const;
    ServicePlugin* getPluginByUrl(const QString &url) const;

    ServicePlugin* createPluginById(const QString &id, QObject *parent = 0) const;
    ServicePlugin* createPluginByUrl(const QString &url, QObject *parent = 0) const;

    bool urlIsSupported(const QString &url) const;

    int load();

private:
    ServicePluginManager();

    ServicePluginConfig* getConfigByFilePath(const QString &filePath) const;

    QNetworkAccessManager* networkAccessManager();

    static ServicePluginManager *self;

    QNetworkAccessManager *m_nam;

    QDateTime m_lastLoaded;

    ServicePluginList m_plugins;
};

#endif // SERVICEPLUGINMANAGER_H
