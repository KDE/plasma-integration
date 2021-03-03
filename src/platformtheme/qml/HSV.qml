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

Kirigami.Page {
    id: hsvPage
    title: i18ndc("plasma-integration-color-dialog", "@title", "HSV")
    bottomPadding: 0

    property bool updating: false

    PI.HSVCircle {
        id: canvas

        value: slider.value

        readonly property int size: Kirigami.Units.gridUnit * 20
        readonly property int radius: size / 2
        readonly property int radiusSquared: radius ** 2

        anchors.centerIn: parent

        width: canvas.size
        height: canvas.size

        visible: false
    }

    function updateColors() {
        hsvPage.updating = true

        slider.value = root.currentColor.hsvValue
        const point = canvas.mapFromRGB(root.currentColor)
        colorHandle.x = point.x-colorHandle.width/2
        colorHandle.y = point.y-colorHandle.height/2

        hsvPage.updating = false
    }

    function updateRoot() {
        root.currentColor = canvas.mapToRGB(colorHandle.x-colorHandle.width/2, colorHandle.y-colorHandle.height/2)
    }

    Connections {
        enabled: !hsvPage.updating
        target: colorHandle

        function onXChanged() { updateRoot() }
        function onYChanged() { updateRoot() }
    }
    Connections {
        enabled: !hsvPage.updating
        target: slider

        function onValueChanged() { updateRoot() }
    }

    ColumnLayout {
        anchors.centerIn: parent
        spacing: Kirigami.Units.largeSpacing

        OpacityMask {
            implicitWidth: canvas.size
            implicitHeight: canvas.size
            source: canvas
            maskSource: Rectangle {
                width: canvas.size
                height: canvas.size
                radius: canvas.radius
            }
            MouseArea {
                anchors.fill: parent
                onPositionChanged: (mouse) => {
                    const distanceFromCenter = Math.pow(canvas.radius - mouseX, 2) + Math.pow(canvas.radius - mouseY, 2)
                    if (distanceFromCenter > canvas.radiusSquared) {
                        return
                    }

                    colorHandle.x = mouseX - (colorHandle.width/2)
                    colorHandle.y = mouseY - (colorHandle.height/2)
                }
            }
            ColorHandle {
                id: colorHandle
                color: root.currentColor
            }
        }

        GradientedSlider {
            id: slider
        }
    }
}