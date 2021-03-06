/*
 * Copyright (C) 2016 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef QDL_H
#define QDL_H

#include "transferitem.h"
#include <QPointer>
#include <QStringList>

class MainWindow;

class Qdl : public QObject
{
    Q_OBJECT

    Q_ENUMS(Action)

    Q_CLASSINFO("D-Bus Interface", "org.marxoft.qdl2")

public:
    enum Action {
        Continue = 0,
        Pause,
        Quit
    };
    
    ~Qdl();

    static Qdl* instance();

public Q_SLOTS:
    Q_SCRIPTABLE static void quit();
    Q_SCRIPTABLE static void showWindow();
    Q_SCRIPTABLE static void closeWindow();

    Q_SCRIPTABLE static QVariantMap addTransfer(const QString &url, const QString &requestMethod = QString("GET"),
            const QVariantMap &requestHeaders = QVariantMap(), const QString &postData = QString(),
            const QString &category = QString(), bool createSubfolder = false,
            int priority = TransferItem::NormalPriority, const QString &customCommand = QString(),
            bool overrideGlobalCommand = false, bool startAutomatically = false);
    Q_SCRIPTABLE static QVariantList addTransfers(const QStringList &urls, const QString &requestMethod = QString("GET"),
            const QVariantMap &requestHeaders = QVariantMap(), const QString &postData = QString(),
            const QString &category = QString(), bool createSubfolder = false,
            int priority = TransferItem::NormalPriority, const QString &customCommand = QString(),
            bool overrideGlobalCommand = false, bool startAutomatically = false);
    Q_SCRIPTABLE static QVariantList getTransfers(int offset = 0, int limit = -1);
    Q_SCRIPTABLE static QVariantMap getTransfersStatus();
    Q_SCRIPTABLE static QVariantMap getTransfer(const QString &id);
    Q_SCRIPTABLE static QVariantList searchTransfers(const QString &property, const QVariant &value, int offset = 0,
            int limit = -1);
    Q_SCRIPTABLE static bool setTransferProperty(const QString &id, const QString &property, const QVariant &value);
    Q_SCRIPTABLE static bool setTransferProperties(const QString &id, const QVariantMap &properties);
    Q_SCRIPTABLE static bool startTransfer(const QString &id);
    Q_SCRIPTABLE static bool pauseTransfer(const QString &id);
    Q_SCRIPTABLE static bool removeTransfer(const QString &id, bool deleteFiles = false);
    Q_SCRIPTABLE static bool moveTransfers(const QString &sourceParentId, int sourceRow, int count,
            const QString &destinationParentId, int destinationRow);
    Q_SCRIPTABLE static void startTransfers();
    Q_SCRIPTABLE static void pauseTransfers();

    Q_SCRIPTABLE static QVariantMap addCategory(const QString &name, const QString &path);
    Q_SCRIPTABLE static QVariantList getCategories();
    Q_SCRIPTABLE static QVariantMap getCategory(const QString &name);
    Q_SCRIPTABLE static bool removeCategory(const QString &name);

    Q_SCRIPTABLE static void clearClipboardUrls();
    Q_SCRIPTABLE static QStringList getClipboardUrls();
    Q_SCRIPTABLE static bool removeClipboardUrl(const QString &url);

    Q_SCRIPTABLE static QVariantList getDecaptchaPlugins();
    Q_SCRIPTABLE static QVariantMap getDecaptchaPlugin(const QString &id);
    Q_SCRIPTABLE static QVariantList getDecaptchaPluginSettings(const QString &id);
    Q_SCRIPTABLE static bool setDecaptchaPluginSettings(const QString &id, const QVariantMap &properties);

    Q_SCRIPTABLE static QVariantList getRecaptchaPlugins();
    Q_SCRIPTABLE static QVariantMap getRecaptchaPlugin(const QString &id);
    Q_SCRIPTABLE static QVariantList getRecaptchaPluginSettings(const QString &id);
    Q_SCRIPTABLE static bool setRecaptchaPluginSettings(const QString &id, const QVariantMap &properties);

    Q_SCRIPTABLE static QVariantList getSearchPlugins();
    Q_SCRIPTABLE static QVariantMap getSearchPlugin(const QString &id);
    Q_SCRIPTABLE static QVariantList getSearchPluginSettings(const QString &id);
    Q_SCRIPTABLE static bool setSearchPluginSettings(const QString &id, const QVariantMap &properties);

    Q_SCRIPTABLE static QVariantList getServicePlugins();
    Q_SCRIPTABLE static QVariantMap getServicePlugin(const QString &id);
    Q_SCRIPTABLE static QVariantList getServicePluginSettings(const QString &id);
    Q_SCRIPTABLE static bool setServicePluginSettings(const QString &id, const QVariantMap &properties);

    Q_SCRIPTABLE static QVariantMap getSettings();
    Q_SCRIPTABLE static bool setSettings(const QVariantMap &settings);

    Q_SCRIPTABLE static QVariantList addUrlChecks(const QStringList &urls, const QString &category = QString(),
            bool createSubfolder = false, int priority = TransferItem::NormalPriority,
            const QString &customCommand = QString(), bool overrideGlobalCommand = false,
            bool startAutomatically = false);
    Q_SCRIPTABLE static void clearUrlChecks();
    Q_SCRIPTABLE static QVariantList getUrlChecks();
    Q_SCRIPTABLE static QVariantMap getUrlChecksStatus();
    Q_SCRIPTABLE static bool submitUrlCheckCaptchaResponse(const QString &response);
    Q_SCRIPTABLE static bool submitUrlCheckSettingsResponse(const QVariantMap &settings);

    Q_SCRIPTABLE static QVariantList addUrlRetrievals(const QStringList &urls, const QString &pluginId = QString());
    Q_SCRIPTABLE static void clearUrlRetrievals();
    Q_SCRIPTABLE static QVariantList getUrlRetrievals();
    Q_SCRIPTABLE static QVariantMap getUrlRetrievalsStatus();

    Q_SCRIPTABLE static QVariantList addDownloadRequests(const QStringList &urls);
    Q_SCRIPTABLE static void clearDownloadRequests();
    Q_SCRIPTABLE static QVariantList getDownloadRequests();
    Q_SCRIPTABLE static QVariantMap getDownloadRequestsStatus();
    Q_SCRIPTABLE static bool submitDownloadRequestCaptchaResponse(const QString &response);
    Q_SCRIPTABLE static bool submitDownloadRequestSettingsResponse(const QVariantMap &settings);

private:
    Qdl();

    static Qdl *self;

    static QPointer<MainWindow> window;
};

#endif // QDL_H
