/*  This file is part of the KDE libraries
 *  Copyright 2021 Carson Black <uhhadd@gmail.com>
 *
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License or ( at
 *  your option ) version 3 or, at the discretion of KDE e.V. ( which shall
 *  act as a proxy as in section 14 of the GPLv3 ), any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

import QtQuick 2.15
import QtQuick.Layouts 1.10
import QtQuick.Controls 2.10
import org.kde.kirigami 2.13 as Kirigami

Item {
    id: root

    property color currentColor: "cyan"
    onCurrentColorChanged: {
        helper.changed(currentColor)
    }

    width: Kirigami.Units.gridUnit * 25
    height: Kirigami.Units.gridUnit * 30

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Kirigami.SwipeNavigator {
            id: sn

            onCurrentIndexChanged: {
                switch (currentIndex) {
                case 0:
                    break
                default:
                    this.pages[currentIndex].updateColours()
                }
            }

            header: RowLayout {
                Item { implicitWidth: Kirigami.Units.largeSpacing }
                Label {
                    text: String(root.currentColor)
                }
                ToolButton {
                    icon.name: "edit-copy"
                    onClicked: helper.copy()
                    ToolTip { visible: parent.hovered; text: i18nd("plasma-integration-color-dialog", "Copy color to clipboard") }
                }
                ToolButton {
                    icon.name: "edit-paste"
                    onClicked: helper.paste()
                    ToolTip { visible: parent.hovered; text: i18nd("plasma-integration-color-dialog", "Paste color from clipboard") }
                }
            }
            footer: RowLayout {
                spacing: Kirigami.Units.smallSpacing
                ToolButton {
                    icon.name: "color-picker"
                    onClicked: helper.pick()
                    ToolTip { visible: parent.hovered; text: i18nd("plasma-integration-color-dialog", "Pick Colour From Screen") }
                }
                Item { implicitWidth: Kirigami.Units.smallSpacing }
            }

            Layout.fillWidth: true
            Layout.fillHeight: true

            PencilBox { id: pencilBox }
            HSV { id: hsv }
            RGB { id: rgb }
            SavedColors { id: savedColors }
        }
        ToolBar {
            id: tb

            Layout.fillWidth: true
            position: ToolBar.Footer

            RowLayout {
                anchors.fill: parent

                Rectangle {
                    color: root.currentColor
                    radius: 4

                    Layout.preferredWidth: height
                    Layout.fillHeight: true
                }

                Kirigami.Separator {
                    Layout.fillHeight: true
                }

                ListView {
                    orientation: ListView.Horizontal
                    model: {
                        const cp = helper.recentColors
                        cp.reverse()
                        return cp
                    }
                    spacing: Kirigami.Units.smallSpacing
                    clip: true

                    delegate: Rectangle {
                        color: modelData
                        radius: 4

                        width: height
                        anchors {
                            top: parent.top
                            bottom: parent.bottom
                            margins: Kirigami.Units.smallSpacing
                        }

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor

                            onClicked: root.currentColor = modelData
                        }
                    }

                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }
        }
    }
}
