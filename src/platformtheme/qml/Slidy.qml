import QtQuick 2.15
import QtQuick.Layouts 1.10
import QtQuick.Controls 2.10
import org.kde.kirigami 2.13 as Kirigami
import QtGraphicalEffects 1.12

Slider {
    id: slider

    Layout.fillWidth: true

    from: 0
    to: 1
    value: 1

    property alias gradient: reccy.gradient

    implicitHeight: Kirigami.Units.gridUnit

    handle: DraggyThingy {
        x: slider.leftPadding + slider.visualPosition * (slider.availableWidth - width)
        y: slider.topPadding + slider.availableHeight / 2 - height / 2
    }
    background: Rectangle {
        id: reccy

        radius: height

        gradient: Gradient {
            orientation: Gradient.Horizontal
            GradientStop { position: 0.0; color: "black" }
            GradientStop { position: 1.0; color: "white" }
        }
    }
}