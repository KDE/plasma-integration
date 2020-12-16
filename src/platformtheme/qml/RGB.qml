import QtQuick 2.15
import QtQuick.Layouts 1.10
import QtQuick.Controls 2.10
import org.kde.kirigami 2.13 as Kirigami
import QtGraphicalEffects 1.12

Kirigami.Page {
    title: "RGB"
    bottomPadding: 0

    ColumnLayout {
        anchors.centerIn: parent
        spacing: Kirigami.Units.largeSpacing * 2

        Canvas {
            id: canvas

            readonly property int size: 300

            anchors.centerIn: parent

            width: canvas.size
            height: canvas.size
            implicitWidth: canvas.size
            implicitHeight: canvas.size

            function mapToRGB(x, y) {
                let h = (Math.atan2(x - canvas.radius, y - canvas.radius) + Math.PI) / (2.0 * Math.PI)
                let s = Math.sqrt( Math.pow(x - canvas.radius, 2) + Math.pow(y - canvas.radius, 2) ) / canvas.radius
                let v = slider.value

                let color = Qt.hsva(h, s, v, 1.0)

                return color
            }

            onPaint: {
                let ctx = getContext("2d")

                for (let x = 0; x <= canvas.size; x++) {
                    for (let y = 0; y <= canvas.size; y++) {
                        ctx.fillStyle = mapToRGB(x, y)
                        ctx.fillRect( x, y, 1, 1 )
                    }
                }
            }

            DraggyThingy {
                onXChanged: {
                    color = canvas.mapToRGB(x, y)
                    root.currentColor = canvas.mapToRGB(x, y)
                }
                onYChanged: {
                    color = canvas.mapToRGB(x, y)
                    root.currentColor = canvas.mapToRGB(x, y)
                }

                DragHandler { margin: 11 }
            }
        }

        Slidy {
            id: slider
            onValueChanged: canvas.requestPaint()
        }

        Slidy {
            id: sliderTwo
            onValueChanged: canvas.requestPaint()
        }
    }
}