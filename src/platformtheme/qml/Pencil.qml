import QtQuick 2.10
import QtQuick.Layouts 1.10
import QtQuick.Shapes 1.15
import org.kde.kirigami 2.12 as Kirigami
import QtGraphicalEffects 1.12

ColumnLayout {
    property int pencilWidth: 22

	component PencilTip : Canvas {
		id: tip
        property int nibHeight: 10
        property int trueHeight: 25
		implicitHeight: trueHeight + 10
		implicitWidth: pencil.pencilWidth
		property color color
		onPaint: {
            var painty = getContext("2d")
            painty.translate(0, 10)

			var gradient = painty.createLinearGradient(0, 0, implicitWidth, 0)
			gradient.addColorStop(  0/100, "#9C725A")
			gradient.addColorStop( 65/100, "#F1C6A4")
			gradient.addColorStop(100/100, "#E2BF95")
            painty.beginPath()

            var tipGradient = painty.createLinearGradient(0, 0, implicitWidth, 0)
			tipGradient.addColorStop(  0/100, "transparent")
			tipGradient.addColorStop( 45/100, Qt.rgba(255,255,255,0.3))
            tipGradient.addColorStop(100/100, Qt.rgba(0, 0, 0, 0.1))

			painty.moveTo(0, trueHeight)
			painty.lineTo(implicitWidth/2, 0)
			painty.lineTo(implicitWidth, trueHeight)
			painty.lineTo(0, trueHeight)
            painty.lineWidth = 0

			painty.fillStyle = gradient
            painty.fill()

            painty.clip()

            painty.fillStyle = color
            painty.fillRect(0, 0, implicitWidth, nibHeight)

            painty.fillStyle = tipGradient
            painty.fillRect(0, 0, implicitWidth, nibHeight)
		}
	}
	component PencilShaft : Rectangle {
		implicitWidth: pencil.pencilWidth

        Rectangle {
            height: parent.width
            width: parent.height
            anchors {
                top: parent.top
                left: parent.left
            }
            rotation: 90
            transformOrigin: Item.TopLeft
            transform: Translate { x: pencil.pencilWidth }
            y: parent.height

            gradient: Gradient {
                GradientStop { position: 1-0; color: Qt.rgba(0, 0, 0, 0.4) }
                GradientStop { position: 1-0.5; color: Qt.rgba(0, 0, 0, 0.15) }
                GradientStop { position: 1-0.7; color: Qt.rgba(255, 255, 255, 0.10) }
                GradientStop { position: 1-0.8; color: Qt.rgba(0, 0, 0, 0.10) }
                GradientStop { position: 1-1; color: Qt.rgba(0, 0, 0, 0.20) }
            }
        }
	}

    id: pencil
    spacing: 0

    property color color
    property int shaftHeight

    PencilTip { color: pencil.color }
    PencilShaft {
        color: pencil.color
        Layout.minimumHeight: pencil.shaftHeight
        Layout.fillHeight: true
    }

    MouseArea {
        anchors.fill: parent
        onClicked: PencilShaft
    }
}
