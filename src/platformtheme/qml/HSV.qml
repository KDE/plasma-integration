import QtQuick 2.15
import QtQuick.Layouts 1.10
import QtQuick.Controls 2.10
import org.kde.kirigami 2.13 as Kirigami
import org.kde.private.plasmaintegration 1.0 as PI
import QtGraphicalEffects 1.12

Kirigami.Page {
    id: hsvPage
    title: "HSV"
    bottomPadding: 0

    property bool updating: false

    PI.HSVCircle {
        id: canvas

        value: slider.value

        readonly property int size: 300
        readonly property int radius: size / 2

        anchors.centerIn: parent

        width: canvas.size
        height: canvas.size

        visible: false
    }

    function updateColours() {
        hsvPage.updating = true

        slider.value = root.currentColor.hsvValue
        const point = canvas.mapFromRGB(root.currentColor)
        draggyThingy.x = point.x-draggyThingy.width/2
        draggyThingy.y = point.y-draggyThingy.height/2

        hsvPage.updating = false
    }

    function updateRoot() {
        root.currentColor = canvas.mapToRGB(draggyThingy.x-draggyThingy.width/2, draggyThingy.y-draggyThingy.height/2)
    }

    Connections {
        enabled: !hsvPage.updating
        target: draggyThingy

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
            DraggyThingy {
                id: draggyThingy
                color: "transparent"

                DragHandler { margin: 11 }
            }
        }

        Slidy {
            id: slider
        }
    }
}