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

#ifndef RECAPTCHADIALOG_H
#define RECAPTCHADIALOG_H

#include <QDialog>

class RecaptchaPluginConfigModel;
class QHBoxLayout;
class QListView;
class QModelIndex;

class RecaptchaDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RecaptchaDialog(QWidget *parent = 0);

private Q_SLOTS:
    void showPluginDialog(const QModelIndex &index);

private:
    RecaptchaPluginConfigModel *m_model;

    QListView *m_view;

    QHBoxLayout *m_layout;
};

#endif // RECAPTCHADIALOG_H
