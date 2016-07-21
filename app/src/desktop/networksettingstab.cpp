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

#include "networksettingstab.h"
#include "networkproxytypemodel.h"
#include "settings.h"
#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>

NetworkSettingsTab::NetworkSettingsTab(QWidget *parent) :
    SettingsTab(parent),
    m_proxyTypeModel(new NetworkProxyTypeModel(this)),
    m_proxyCheckBox(new QCheckBox(tr("&Enable network proxy"), this)),
    m_authCheckBox(new QCheckBox(tr("Enable &authentication"), this)),
    m_proxyTypeSelector(new QComboBox(this)),
    m_hostEdit(new QLineEdit(this)),
    m_usernameEdit(new QLineEdit(this)),
    m_passwordEdit(new QLineEdit(this)),
    m_portSpinBox(new QSpinBox(this)),
    m_layout(new QFormLayout(this))
{
    setWindowTitle(tr("Network"));

    m_proxyTypeSelector->setModel(m_proxyTypeModel);

    m_passwordEdit->setEchoMode(QLineEdit::Password);

    m_portSpinBox->setMaximum(100000);

    m_layout->addRow(m_proxyCheckBox);
    m_layout->addRow(m_proxyTypeSelector);
    m_layout->addRow(tr("&Host:"), m_hostEdit);
    m_layout->addRow(tr("&Port:"), m_portSpinBox);
    m_layout->addRow(m_authCheckBox);
    m_layout->addRow(tr("&Username:"), m_usernameEdit);
    m_layout->addRow(tr("&Password:"), m_passwordEdit);

    restore();
}

void NetworkSettingsTab::restore() {
    m_proxyCheckBox->setChecked(Settings::networkProxyEnabled());
    m_authCheckBox->setChecked(Settings::networkProxyAuthenticationEnabled());
    m_proxyTypeSelector->setCurrentIndex(qMax(0, m_proxyTypeSelector->findData(Settings::networkProxyType())));
    m_hostEdit->setText(Settings::networkProxyHost());
    m_usernameEdit->setText(Settings::networkProxyUsername());
    m_passwordEdit->setText(Settings::networkProxyPassword());
    m_portSpinBox->setValue(Settings::networkProxyPort());
}

void NetworkSettingsTab::save() {
    Settings::setNetworkProxyEnabled(m_proxyCheckBox->isChecked());
    Settings::setNetworkProxyAuthenticationEnabled(m_authCheckBox->isChecked());
    Settings::setNetworkProxyType(m_proxyTypeSelector->itemData(m_proxyTypeSelector->currentIndex()).toInt());
    Settings::setNetworkProxyHost(m_hostEdit->text());
    Settings::setNetworkProxyUsername(m_usernameEdit->text());
    Settings::setNetworkProxyPassword(m_passwordEdit->text());
    Settings::setNetworkProxyPort(m_portSpinBox->value());
    Settings::setNetworkProxy();
}
