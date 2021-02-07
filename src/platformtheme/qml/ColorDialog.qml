import QtQuick 2.10
import QtQuick.Layouts 1.10
import QtQuick.Controls 2.10
import org.kde.kirigami 2.13 as Kirigami

ColumnLayout {
    id: root

    property color currentColor: "cyan"

    width: 400
    height: 500

    spacing: 0

    Kirigami.SwipeNavigator {
        id: sn

        onCurrentIndexChanged: {
            switch (currentIndex) {
            case 0:
                break
            default:
                this.pages[currentIndex].updateColours()
            }
        }

        Layout.fillWidth: true
        Layout.fillHeight: true

        PencilBox { id: pencilBox }
        HSV { id: hsv }
        RGB { id: rgb }
    }
    ToolBar {
        Layout.fillWidth: true
        position: ToolBar.Footer

        RowLayout {
            anchors.fill: parent
            ColorCell {
                color: root.currentColor

                Layout.fillWidth: true
            }

            Button {
                text: "Pick Colour From Screen"
                onClicked: helper.pick()
            }
        }
    }
}
