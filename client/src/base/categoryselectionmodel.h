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

#ifndef CATEGORYSELECTIONMODEL_H
#define CATEGORYSELECTIONMODEL_H

#include "selectionmodel.h"
#include "categories.h"

class CategorySelectionModel : public SelectionModel
{
    Q_OBJECT

public:
    explicit CategorySelectionModel(QObject *parent = 0) :
        SelectionModel(parent)
    {
        reload();
    }

public Q_SLOTS:
    void reload() {
        clear();
        append(tr("Default"), QString());
        const CategoryList categories = Categories::instance()->get();

        for (int i = 0; i < categories.size(); i++) {
            append(categories.at(i).name, categories.at(i).name);
        }
    }
};
        
#endif // CATEGORYSELECTIONMODEL_H
