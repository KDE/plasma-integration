import QtQuick 2.15
import QtQuick.Layouts 1.10
import QtQuick.Controls 2.10
import org.kde.kirigami 2.13 as Kirigami
import org.kde.private.plasmaintegration 1.0 as PI
import QtGraphicalEffects 1.12

Kirigami.Page {
    title: "HSV"
    bottomPadding: 0

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

    Connections {
        target: root
        function onCurrentColorChanged() {
            draggyThingy.suppress = true
            let point = canvas.mapFromRGB(root.currentColor)
            draggyThingy.x = point.x
            draggyThingy.y = point.y
        }
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
                property bool suppress: false

                onXChanged: {
                    color = canvas.mapToRGB(x, y)
                    if (!this.suppress) {
                        root.currentColor = canvas.mapToRGB(x, y)
                    }
                }
                onYChanged: {
                    color = canvas.mapToRGB(x, y)
                    if (!this.suppress) {
                        root.currentColor = canvas.mapToRGB(x, y)
                    }
                }

                DragHandler { margin: 11 }
            }
        }

        Slidy {
            id: slider
            onValueChanged: {
                draggyThingy.color = canvas.mapToRGB(draggyThingy.x, draggyThingy.y)
                root.currentColor = canvas.mapToRGB(draggyThingy.x, draggyThingy.y)
            }
        }
    }
}