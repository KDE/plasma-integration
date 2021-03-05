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
import QtGraphicalEffects 1.12

Kirigami.Page {
    title: i18ndc("plasma-integration-color-dialog", "@title", "Pencil Box")
    bottomPadding: 0

    // a hand-curated selection of colors.
    // mostly yoinked from my (jan's) app, ikona.
    // one of these is #3daee9, the plasma blue.
    property var colors: [
        [
            "#ff7cc5",
            "#ff8999",
            "#ff9b77",
            "#ffc178",
            "#ffe86c",
            "#daff69",
            "#7af396",
            "#4cf4b3",
            "#69f3dc",
            "#8adcff",
            "#dfa8ec",
            "#bfa0ff",
            "#101114",
            "#fcffff",
        ],
        [
            "#ff5bb6",
            "#ff6c7f",
            "#ff8255",
            "#ffb256",
            "#ffe247",
            "#d1ff43",
            "#59f07c",
            "#1ff1a0",
            "#44f0d3",
            "#6dd3ff",
            "#d792e7",
            "#af88ff",
            "#1a1b1e",
            "#eef1f5",
        ],
        [
            "#e93a9a",
            "#e93d58",
            "#e9643a",
            "#ef973c",
            "#e8cb2d",
            "#b6e521",
            "#3dd425",
            "#00d485",
            "#00d3b8",
            "#3daee9",
            "#b875dc",
            "#926ee4",
            "#232629",
            "#d1d5d9",
        ],
        [
            "#cb0b81",
            "#bf0039",
            "#cd4d25",
            "#d07d20",
            "#c4ab00",
            "#99c900",
            "#57bf2a",
            "#00b86b",
            "#00b79d",
            "#2aa1bf",
            "#9c5bc0",
            "#7655c8",
            "#2e3134",
            "#b6b9bd",
        ],
        // I thought milk was pink?
        [
            "#a20967",
            "#99002e",
            "#a43e1e",
            "#a6641a",
            "#9d8900",
            "#7aa100",
            "#469922",
            "#009356",
            "#00927e",
            "#228199",
            "#7d499a",
            "#5e44a0",
            "#393c3f",
            "#a8abb0",
        ],
    ]

    Repeater {
        model: colors

        delegate: RowLayout {
            id: row
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
            }

            property int parentIndex: index

            z: colors.length - row.parentIndex

            Repeater {
                model: modelData

                delegate: Item {
                    implicitHeight: pencil.height
                    implicitWidth: pencil.width
                    focus: true

                    Layout.alignment: Qt.AlignBottom

                    Pencil {
                        id: pencil

                        color: modelData
                        shaftHeight: Kirigami.Units.gridUnit*6 + (row.parentIndex * Kirigami.Units.gridUnit*3)
                        Behavior on shaftHeight {
                            NumberAnimation { duration: Kirigami.Units.veryShortDuration/2; easing.type: Easing.InOutQuad }
                        }

                        visible: false

                        states: [
                            State {
                                when: mouseArea.pressed
                                PropertyChanges { target: pencil; shaftHeight: Kirigami.Units.gridUnit*6.5 + (row.parentIndex * Kirigami.Units.gridUnit*3) }
                            },
                            State {
                                when: mouseArea.containsMouse || pencil.parent.activeFocus
                                PropertyChanges { target: pencil; shaftHeight: Kirigami.Units.gridUnit*7 + (row.parentIndex * Kirigami.Units.gridUnit*3) }
                            }
                        ]
                    }

                    DropShadow {
                        anchors.fill: pencil
                        horizontalOffset: 0
                        verticalOffset: -5
                        radius: 14.0
                        samples: 17
                        color: "#60000000"
                        source: pencil

                        MouseArea {
                            id: mouseArea

                            cursorShape: Qt.PointingHandCursor
                            anchors.fill: parent
                            anchors.margins: -(row.spacing/2)
                            hoverEnabled: true
                            onClicked: root.currentColor = modelData
                        }
                    }
                }
            }
        }
    }
}