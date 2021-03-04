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
import org.kde.private.plasmaintegration 1.0 as PI
import QtGraphicalEffects 1.12

Kirigami.ScrollablePage {
    id: savedPage
    title: i18ndc("plasma-integration-color-dialog", "@title", "Favorite Colors")
    bottomPadding: 0

    function updateColors() {

    }

    function updateRoot() {

    }

    header: RowLayout {
        // TODO: make this do something
        // TODO: removing colors
        // TODO: copy hex to clipboard
        Kirigami.SearchField {
            Layout.fillWidth: true
        }
        Button {
            id: addColorButton

            text: i18nd("plasma-integration-color-dialog", "Save Current Color")

            onClicked: state = "active"

            state: "passive"
            states: [
                State {
                    name: "passive"
                },
                State {
                    name: "active"
                    PropertyChanges {
                        target: addColorButton
                        visible: false
                    }
                    PropertyChanges {
                        target: nameField
                        visible: true
                    }
                    PropertyChanges {
                        target: submitButton
                        visible: true
                    }
                }
            ]
        }
        TextField {
            id: nameField
            placeholderText: i18nd("plasma-integration-color-dialog", "Name your color...")
            visible: false
        }
        Button {
            id: submitButton
            icon.name: "arrow-right"
            visible: false
            enabled: nameField.text.trim() !== ""
            onClicked: {
                addColorButton.state = "passive"
                let data = helper.savedColors || []
                data.push({
                    color: String(root.currentColor),
                    name: nameField.text,
                })
                data.sort((a, b) => {
                    if (a.name < b.name) { return -1; }
                    if (a.name > b.name) { return 1; }
                    return 0;
                })
                helper.savedColors = data
                colorsView.model = helper.savedColors
            }
        }
        Layout.fillWidth: true
    }

    ListView {
        id: colorsView
        model: helper.savedColors

        Layout.fillWidth: true
        Layout.preferredHeight: root.height * 0.7

        clip: true

        delegate: Kirigami.BasicListItem {
            leading: Rectangle {
                color: modelData.color
                radius: height / 2
                width: height
            }
            text: modelData.name
            subtitle: String(modelData.color)
            trailing: RowLayout {
                ToolButton {
                    id: deleteButton

                    flat: true
                    icon.name: "edit-delete"

                    onClicked: {
                        let data = helper.savedColors || []
                        data.splice(index, 1)
                        helper.savedColors = data
                        colorsView.model = helper.savedColors
                    }
                }
            }

            onClicked: root.currentColor = modelData.color
            width: parent.width
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            visible: colorsView.count == 0

            text: i18nd("plasma-integration-color-dialog", "No colors saved")
            explanation: i18nd("plasma-integration-color-dialog", "Save a color and it will show up here.")
        }
    }
}