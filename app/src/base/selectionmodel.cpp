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

#include "selectionmodel.h"

SelectionModel::SelectionModel(QObject *parent) :
    QAbstractListModel(parent),
    m_alignment(Qt::AlignLeft | Qt::AlignVCenter)
{
    m_roles[NameRole] = "name";
    m_roles[ValueRole] = "value";
#if QT_VERSION < 0x050000
    setRoleNames(m_roles);
#endif
}

#if QT_VERSION >= 0x050000
QHash<int, QByteArray> SelectionModel::roleNames() const {
    return m_roles;
}
#endif

int SelectionModel::rowCount(const QModelIndex &) const {
    return m_items.size();
}

int SelectionModel::columnCount(const QModelIndex &) const {
    return 2;
}

Qt::Alignment SelectionModel::textAlignment() const {
    return m_alignment;
}

void SelectionModel::setTextAlignment(Qt::Alignment align) {
    if (align != textAlignment()) {
        m_alignment = align;
        emit textAlignmentChanged();
        
        if (!m_items.isEmpty()) {
            emit dataChanged(index(0), index(m_items.size() - 1));
        }
    }
}

QVariant SelectionModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if ((orientation != Qt::Horizontal) || (role != Qt::DisplayRole)) {
        return QVariant();
    }
    
    switch (section) {
    case 0:
        return tr("Name");
    case 1:
        return tr("Value");
    default:
        return QVariant();
    }
}

QVariant SelectionModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }
    
    switch (role) {
    case Qt::DisplayRole:
        switch (index.column()) {
        case 0:
            return m_items.at(index.row()).first;
        case 1:
            return m_items.at(index.row()).second;
        default:
            break;
        }
        
        return QVariant();
    case NameRole:
        return m_items.at(index.row()).first;
    case ValueRole:
        return m_items.at(index.row()).second;
    case Qt::TextAlignmentRole:
        return QVariant(textAlignment());
    default:
        return QVariant();
    }
}

QMap<int, QVariant> SelectionModel::itemData(const QModelIndex &index) const {
    QMap<int, QVariant> map;
    map[NameRole] = data(index, NameRole);
    map[ValueRole] = data(index, ValueRole);
    
    return map;
}

bool SelectionModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!index.isValid()) {
        return false;
    }
    
    switch (role) {
    case NameRole:
        m_items[index.row()].first = value.toString();
        break;
    case ValueRole:
        m_items[index.row()].second = value;
        break;
    default:
        return false;
    }
    
    emit dataChanged(index, index);
    return true;
}

bool SelectionModel::setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles) {
    if (roles.isEmpty()) {
        return false;
    }
    
    QMapIterator<int, QVariant> iterator(roles);
    
    while (iterator.hasNext()) {
        iterator.next();
        
        if (!setData(index, iterator.value(), iterator.key())) {
            return false;
        }
    }
    
    return true;
}

QVariant SelectionModel::data(int row, const QByteArray &role) const {
    return data(index(row), m_roles.key(role));
}

QVariantMap SelectionModel::itemData(int row) const {
    QVariantMap map;
    map["name"] = data(row, "name");
    map["value"] = data(row, "value");
    
    return map;
}

bool SelectionModel::setData(int row, const QVariant &value, const QByteArray &role) {
    return setData(index(row), value, m_roles.key(role));
}

bool SelectionModel::setItemData(int row, const QVariantMap &roles) {
    if (roles.isEmpty()) {
        return false;
    }
    
    QMapIterator<QString, QVariant> iterator(roles);
    
    while (iterator.hasNext()) {
        iterator.next();
        
        if (!setData(row, iterator.value(), iterator.key().toUtf8())) {
            return false;
        }
    }
    
    return true;
}

QModelIndexList SelectionModel::match(const QModelIndex &start, int role, const QVariant &value, int hits,
                                      Qt::MatchFlags flags) const {
    return QAbstractListModel::match(start, role, value, hits, flags);
}

int SelectionModel::match(int start, const QByteArray &role, const QVariant &value, int flags) const {
    const QModelIndexList idxs = match(index(start), m_roles.key(role), value, 1, Qt::MatchFlags(flags));
    return idxs.isEmpty() ? -1 : idxs.first().row();
}

void SelectionModel::append(const QString &name, const QVariant &value) {
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    m_items << QPair<QString, QVariant>(name, value);
    endInsertRows();
    emit countChanged(rowCount());
}

void SelectionModel::insert(int row, const QString &name, const QVariant &value) {
    if ((row < 0) || (row >= m_items.size())) {
        append(name, value);
    }
    else {
        beginInsertRows(QModelIndex(), row, row);
        m_items.insert(row, QPair<QString, QVariant>(name, value));
        endInsertRows();
        emit countChanged(rowCount());
    }
}

bool SelectionModel::remove(int row) {
    if ((row >= 0) && (row < m_items.size())) {
        beginRemoveRows(QModelIndex(), row, row);
        m_items.removeAt(row);
        endRemoveRows();
        emit countChanged(rowCount());
        
        return true;
    }
    
    return false;
}

void SelectionModel::clear() {
    if (!m_items.isEmpty()) {
        beginResetModel();
        m_items.clear();
        endResetModel();
        emit countChanged(0);
    }
}
