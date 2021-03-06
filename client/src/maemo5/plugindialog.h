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

#ifndef PLUGINDIALOG_H
#define PLUGINDIALOG_H

#include <QDialog>
#include <QVariantList>

class QDialogButtonBox;
class QHBoxLayout;
class QScrollArea;
class QVBoxLayout;

class PluginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PluginDialog(const QString &pluginId, QWidget *parent = 0);

public Q_SLOTS:
    virtual void accept();

private Q_SLOTS:
    void setBooleanValue(bool value);
    void setIntegerValue(int value);
    void setListValue(const QVariant &value);
    void setMultiListValue(const QVariantList &values);
    void setTextValue(const QString &value);

    void onSettingsReady(const QString &pluginId, const QVariantList &settings);

private:
    void addCheckBox(const QString &label, const QString &key, bool value);
    void addGroup(const QString &label, const QString &key, const QVariantList &settings);
    void addLineEdit(const QString &label, const QString &key, const QString &value, bool isPassword = false);
    void addMultiValueSelector(const QString &label, const QString &key, const QVariantList &options,
                               const QVariantList &values);
    void addSpinBox(const QString &label, const QString &key, int minimum, int maximum, int step, int value);
    void addValueSelector(const QString &label, const QString &key, const QVariantList &options, const QVariant &value);
    void addWidget(const QVariantMap &setting, const QString &group = QString());

    QScrollArea *m_scrollArea;

    QWidget *m_container;

    QDialogButtonBox *m_buttonBox;

    QVBoxLayout *m_vbox;
    QHBoxLayout *m_layout;

    QString m_pluginId;

    QVariantMap m_settings;
};

#endif // PLUGINDIALOG_H
