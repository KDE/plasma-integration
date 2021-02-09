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
import QtQml 2.15
import QtQuick.Layouts 1.10
import QtQuick.Controls 2.10
import org.kde.kirigami 2.13 as Kirigami
import QtGraphicalEffects 1.12

Kirigami.Page {
    id: rgbPage

    title: i18ndc("plasma-integration-color-dialog", "@title", "RGB")
    bottomPadding: 0

    property bool updating: false

    function updateColours() {
        rgbPage.updating = true

        slider.value = root.currentColor.r
        sliderTwo.value = root.currentColor.g
        sliderThree.value = root.currentColor.b

        spin.value = root.currentColor.r * 255
        spinTwo.value = root.currentColor.g * 255
        spinThree.value = root.currentColor.b * 255

        rgbPage.updating = false
    }

    function updateRoot() {
        root.currentColor = Qt.rgba(slider.value, sliderTwo.value, sliderThree.value)
    }

    Connections {
        enabled: !rgbPage.updating
        target: slider
        function onValueChanged() { updateRoot() }
    }
    Connections {
        enabled: !rgbPage.updating
        target: sliderTwo
        function onValueChanged() { updateRoot() }
    }
    Connections {
        enabled: !rgbPage.updating
        target: sliderThree
        function onValueChanged() { updateRoot() }
    }

    ColumnLayout {
        id: rgbCol

        anchors.centerIn: parent
        spacing: Kirigami.Units.largeSpacing * 2

        property int sliderLength: Kirigami.Units.gridUnit*20

        Label {
            text: i18ndc("plasma-integration-color-dialog", "component of RGB; label for slider", "Red")
        }

        GradientedSlider {
            id: slider

            Layout.preferredWidth: rgbCol.sliderLength

            gradient: Gradient {
                orientation: Gradient.Horizontal
                GradientStop { position: 0.0; color: Qt.rgba(0.0, sliderTwo.value, sliderThree.value) }
                GradientStop { position: 1.0; color: Qt.rgba(1.0, sliderTwo.value, sliderThree.value) }
            }
        }

        SpinBox {
            id: spin

            from: 0
            to: 255

            Binding on value {
                when: slider.pressed
                value: slider.value * 255
                restoreMode: Binding.RestoreNone
            }

            onValueChanged: slider.value = value / 255

            Layout.alignment: Qt.AlignRight
        }

        Label {
            text: i18ndc("plasma-integration-color-dialog", "component of RGB; label for slider", "Green")
        }

        GradientedSlider {
            id: sliderTwo

            Layout.preferredWidth: rgbCol.sliderLength

            gradient: Gradient {
                orientation: Gradient.Horizontal
                GradientStop { position: 0.0; color: Qt.rgba(slider.value, 0.0, sliderThree.value) }
                GradientStop { position: 1.0; color: Qt.rgba(slider.value, 1.0, sliderThree.value) }
            }
        }

        SpinBox {
            id: spinTwo

            from: 0
            to: 255

            Binding on value {
                when: sliderTwo.pressed
                value: sliderTwo.value * 255
                restoreMode: Binding.RestoreNone
            }

            onValueChanged: sliderTwo.value = value / 255

            Layout.alignment: Qt.AlignRight
        }

        Label {
            text: i18ndc("plasma-integration-color-dialog", "component of RGB; label for slider", "Blue")
        }

        GradientedSlider {
            id: sliderThree

            Layout.preferredWidth: rgbCol.sliderLength

            gradient: Gradient {
                orientation: Gradient.Horizontal
                GradientStop { position: 0.0; color: Qt.rgba(slider.value, sliderTwo.value, 0.0) }
                GradientStop { position: 1.0; color: Qt.rgba(slider.value, sliderTwo.value, 1.0) }
            }
        }

        SpinBox {
            id: spinThree

            from: 0
            to: 255

            Binding on value {
                when: sliderThree.pressed
                value: sliderThree.value * 255
                restoreMode: Binding.RestoreNone
            }

            onValueChanged: sliderThree.value = value / 255

            Layout.alignment: Qt.AlignRight
        }
    }
}