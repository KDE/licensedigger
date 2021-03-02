/*
   SPDX-FileCopyrightText: 2020 Felix Ernst <fe.a.ernst@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick 2.1
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.7 as QtControls
import QtQuick.Dialogs 1.2 as QtDialogs
import org.kde.kirigami 2.3 as Kirigami
import org.kde.kcm 1.0


QtControls.Button {
    icon.name: "help-contextual"
    flat: true
    property alias toolTipText: toolTip.text
    property var toolTipVisible: false
    onReleased: {
        toolTipVisible ? toolTip.delay = Kirigami.Units.toolTipDelay : toolTip.delay = 0;
        toolTipVisible = !toolTipVisible;
    }
    onActiveFocusChanged: {
        toolTip.delay = Kirigami.Units.toolTipDelay;
        toolTipVisible = false;
    }
    Layout.maximumHeight: parent.height
    QtControls.ToolTip {
        id: toolTip
        implicitWidth: Math.min(21 * Kirigami.Units.gridUnit, root.width) // Wikipedia says anything between 45 and 75 characters per line is acceptable. 21 * Kirigami.Units.gridUnit feels right.
        visible: parent.hovered || parent.toolTipVisible
        onVisibleChanged: {
            if (!visible && parent.toolTipVisible) {
                parent.toolTipVisible = false;
                delay = Kirigami.Units.toolTipDelay;
            }
        }
        timeout: -1
    }
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.WhatsThisCursor
        acceptedButtons: Qt.NoButton
    }
    Accessible.name: i18nc("@action:button", "Show Contextual Help")
}

