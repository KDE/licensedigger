/*
 *   Copyright 2015 Marco Martin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.0
import QtQuick.Controls 2.0 as Controls
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.8

ScrollablePage {
    id: page
    Layout.fillWidth: true
    implicitWidth: Units.gridUnit * (Math.floor(Math.random() * 35) + 8)

    title: i18n("Simple Scrollable Page")

    actions {
        contextualActions: [
            Action {
                text:"Action for buttons"
                iconName: "bookmarks"
                onTriggered: print("Action 1 clicked")
            },
            Action {
                text:"Action 2"
                iconName: "folder"
                enabled: false
            }
        ]
    }

    ColumnLayout {
        width: page.width
        spacing: Units.smallSpacing

        Controls.Label {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed a sem venenatis, dictum odio vitae, tincidunt sapien. Proin a suscipit ligula, id interdum leo. Donec sed dolor sed lacus dignissim tempor a a lorem. In ullamcorper varius vestibulum. Sed nec arcu semper, varius velit ut, pharetra est. Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Integer odio nibh, tincidunt quis condimentum quis, consequat id lacus. Nulla quis mauris erat. Suspendisse rhoncus suscipit massa, at suscipit lorem rhoncus et. "
        }
    }
}
