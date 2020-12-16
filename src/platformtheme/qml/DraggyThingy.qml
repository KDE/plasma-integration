import QtQuick 2.15
import QtQuick.Layouts 1.10
import QtQuick.Controls 2.10
import org.kde.kirigami 2.13 as Kirigami
import QtGraphicalEffects 1.12

Kirigami.ShadowedRectangle {
    width: 22
    height: 22
    color: "transparent"
    border.color: "white"
    border.width: 2
    radius: 11

    shadow {
        color: Qt.rgba(0,0,0,0.2)
        size: 3
        yOffset: 1
    }
}