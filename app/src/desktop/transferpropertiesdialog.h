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

#ifndef TRANSFERPROPERTIESDIALOG_H
#define TRANSFERPROPERTIESDIALOG_H

#include "transferitem.h"
#include <QDialog>
#include <QPointer>

class TransferItemPriorityModel;
class QComboBox;
class QDialogButtonBox;
class QFormLayout;
class QLabel;
class QProgressBar;

class TransferPropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TransferPropertiesDialog(TransferItem *transfer, QWidget *parent = 0);

private Q_SLOTS:
    void setPriority(int index);
    
    void onDataChanged(TransferItem *transfer, int role);

private:
    void updateIcon(TransferItem *transfer);
    void updateName(TransferItem *transfer);
    void updatePluginName(TransferItem *transfer);
    void updatePriority(TransferItem *transfer);
    void updateProgress(TransferItem *transfer);
    void updateSpeed(TransferItem *transfer);
    void updateStatus(TransferItem *transfer);
    void updateUrl(TransferItem *transfer);
    
    QPointer<TransferItem> m_transfer;

    TransferItemPriorityModel *m_priorityModel;

    QLabel *m_iconLabel;
    QLabel *m_pluginLabel;
    QLabel *m_nameLabel;
    QLabel *m_urlLabel;
    QLabel *m_speedLabel;
    QLabel *m_statusLabel;

    QComboBox *m_prioritySelector;

    QProgressBar *m_progressBar;

    QDialogButtonBox *m_buttonBox;

    QFormLayout *m_layout;
};
    
#endif // TRANSFERPROPERTIESDIALOG_H
