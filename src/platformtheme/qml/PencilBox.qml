import QtQuick 2.10
import QtQuick.Layouts 1.10
import QtQuick.Controls 2.10
import org.kde.kirigami 2.13 as Kirigami

Kirigami.Page {
    title: "Pencil Box"
    bottomPadding: 0

    RowLayout {
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
        }
        Repeater {
            model: [
                "white",
            ]
            delegate: Pencil {
                color: modelData
                shaftHeight: 80
            }
        }
    }
}