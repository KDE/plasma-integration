import QtQuick 2.15
import QtQuick.Layouts 1.10
import QtQuick.Controls 2.10
import org.kde.kirigami 2.13 as Kirigami
import QtGraphicalEffects 1.12

Kirigami.Page {
    title: "RGB"
    bottomPadding: 0

    ColumnLayout {
        id: rgbCol

        anchors.centerIn: parent
        spacing: Kirigami.Units.largeSpacing * 2

        property int sliderLength: Kirigami.Units.gridUnit*20

        Label {
            text: "Red"
        }

        Slidy {
            id: slider

            Layout.preferredWidth: rgbCol.sliderLength

            gradient: Gradient {
                orientation: Gradient.Horizontal
                GradientStop { position: 0.0; color: Qt.rgba(0.0, sliderTwo.value, sliderThree.value) }
                GradientStop { position: 1.0; color: Qt.rgba(1.0, sliderTwo.value, sliderThree.value) }
            }
        }

        Label {
            text: "Green"
        }

        Slidy {
            id: sliderTwo

            Layout.preferredWidth: rgbCol.sliderLength

            gradient: Gradient {
                orientation: Gradient.Horizontal
                GradientStop { position: 0.0; color: Qt.rgba(slider.value, 0.0, sliderThree.value) }
                GradientStop { position: 1.0; color: Qt.rgba(slider.value, 1.0, sliderThree.value) }
            }
        }

        Label {
            text: "Blue"
        }

        Slidy {
            id: sliderThree

            Layout.preferredWidth: rgbCol.sliderLength

            gradient: Gradient {
                orientation: Gradient.Horizontal
                GradientStop { position: 0.0; color: Qt.rgba(slider.value, sliderTwo.value, 0.0) }
                GradientStop { position: 1.0; color: Qt.rgba(slider.value, sliderTwo.value, 1.0) }
            }
        }
    }
}