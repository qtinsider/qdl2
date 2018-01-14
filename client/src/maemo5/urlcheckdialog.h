/*
 * Copyright (C) 2018 Stuart Howarth <showarth@marxoft.co.uk>
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

#ifndef URLCHECKDIALOG_H
#define URLCHECKDIALOG_H

#include "urlcheckmodel.h"
#include <QDialog>

class QDialogButtonBox;
class QGridLayout;
class QLabel;
class QProgressBar;
class QTreeView;

class UrlCheckDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UrlCheckDialog(QWidget *parent = 0);

public Q_SLOTS:
    void addUrl(const QString &url, const QString &category = QString(), bool createSubfolder = false,
            int priority = TransferItem::NormalPriority, const QString &customCommand = QString(),
            bool overrideGlobalCommand = false);
    void addUrls(const QStringList &urls, const QString &category = QString(), bool createSubfolder = false,
            int priority = TransferItem::NormalPriority, const QString &customCommand = QString(),
            bool overrideGlobalCommand = false);
    
    virtual void accept();
    virtual void reject();
    
    void clear();

private Q_SLOTS:
    void showCaptchaDialog(int captchaType, const QByteArray &captchaData);
    void showPluginSettingsDialog(const QString &title, const QVariantList &settings);

    void updateStatusLabel();
    void onStatusChanged(UrlCheckModel::Status status);
    
private:
    QTreeView *m_view;
    
    QProgressBar *m_progressBar;

    QLabel *m_statusLabel;

    QDialogButtonBox *m_buttonBox;

    QGridLayout *m_layout;
};

#endif // URLCHECKDIALOG_H
