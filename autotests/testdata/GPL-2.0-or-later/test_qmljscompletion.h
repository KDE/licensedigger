/*****************************************************************************
 *   Copyright 2011-2012 Sven Brauch <svenbrauch@googlemail.com>             *
 *                                                                           *
 *   This program is free software: you can redistribute it and/or modify    *
 *   it under the terms of the GNU General Public License as published by    *
 *   the Free Software Foundation, either version 2 of the License, or       *
 *   (at your option) any later version.                                     *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *   GNU General Public License for more details.                            *
 *                                                                           *
 *   You should have received a copy of the GNU General Public License       *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>    *
 *****************************************************************************/

#ifndef QMLCOMPLETIONTEST_H
#define QMLCOMPLETIONTEST_H

#include <QObject>

namespace QmlJS {

class QmlCompletionTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testContainsDeclaration();
    void testContainsDeclaration_data();
    void testDoesNotContainDeclaration();
    void testDoesNotContainDeclaration_data();
    void testContainsText();
    void testContainsText_data();
};

}
#endif
