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
        Layout.fillWidth: true
        Layout.fillHeight: true

        PencilBox {}
        HSV {}
        RGB {}
    }
    ToolBar {
        Layout.fillWidth: true
        position: ToolBar.Footer

        RowLayout {
            anchors.fill: parent
            ColorCell {
                color: root.currentColor
            }
        }
    }
}
