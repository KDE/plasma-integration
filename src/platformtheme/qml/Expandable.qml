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

import QtQuick 2.7

Item {
    default property Item contentItem: null

    id: root
    property bool childVisible: false
    implicitHeight: childVisible ? contentItem.height : 0
    implicitWidth: contentItem.implicitWidth
    clip: true

    enum Direction {
        FromBottom,
        FromTop
    }
    property int direction: Expandable.Direction.FromBottom

    Behavior on implicitHeight {
        NumberAnimation {
            duration: 250
            easing.type: Easing.InOutQuart
        }
    }

    states: [
        State {
            name: "fromTop"
            when: root.direction === Expandable.Direction.FromTop

            AnchorChanges {
                target: contentItem

                anchors.bottom: root.bottom
                anchors.top: undefined
            }
        },
        State {
            name: "fromBottom"
            when: root.direction === Expandable.Direction.FromBottom

            AnchorChanges {
                target: contentItem

                anchors.bottom: undefined
                anchors.top: root.top
            }
        }
    ]

    onContentItemChanged: contentItem.parent = this
}
