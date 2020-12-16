import QtQuick 2.15
import QtQuick.Layouts 1.10
import QtQuick.Controls 2.10
import org.kde.kirigami 2.13 as Kirigami
import QtGraphicalEffects 1.12

Kirigami.Page {
    title: "Pencil Box"
    bottomPadding: 0

    property var colors: [
        [
            "#ff7cc5",
            "#ff8999",
            "#ff9b77",
            "#ffc178",
            "#ffe86c",
            "#daff69",
            "#7af396",
            "#4cf4b3",
            "#69f3dc",
            "#8adcff",
            "#dfa8ec",
            "#bfa0ff",
            "#101114",
            "#fcffff",
        ],
        [
            "#ff5bb6",
            "#ff6c7f",
            "#ff8255",
            "#ffb256",
            "#ffe247",
            "#d1ff43",
            "#59f07c",
            "#1ff1a0",
            "#44f0d3",
            "#6dd3ff",
            "#d792e7",
            "#af88ff",
            "#1a1b1e",
            "#eef1f5",
        ],
        [
            "#e93a9a",
            "#e93d58",
            "#e9643a",
            "#ef973c",
            "#e8cb2d",
            "#b6e521",
            "#3dd425",
            "#00d485",
            "#00d3b8",
            "#3daee9",
            "#b875dc",
            "#926ee4",
            "#232629",
            "#d1d5d9",
        ],
        [
            "#cb0b81",
            "#bf0039",
            "#cd4d25",
            "#d07d20",
            "#c4ab00",
            "#99c900",
            "#57bf2a",
            "#00b86b",
            "#00b79d",
            "#2aa1bf",
            "#9c5bc0",
            "#7655c8",
            "#2e3134",
            "#b6b9bd",
        ],
        [
            "#a20967",
            "#99002e",
            "#a43e1e",
            "#a6641a",
            "#9d8900",
            "#7aa100",
            "#469922",
            "#009356",
            "#00927e",
            "#228199",
            "#7d499a",
            "#5e44a0",
            "#393c3f",
            "#a8abb0",
        ],
    ]

    Repeater {
        model: colors

        delegate: RowLayout {
            id: row
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
            }

            property int parentIndex: index

            z: colors.length - row.parentIndex

            Repeater {
                model: modelData

                delegate: Item {
                    implicitHeight: pencil.height
                    implicitWidth: pencil.width

                    Pencil {
                        id: pencil

                        color: modelData
                        shaftHeight: 80 + (row.parentIndex * 50)

                        visible: false
                    }

                    DropShadow {
                        anchors.fill: pencil
                        horizontalOffset: 0
                        verticalOffset: -5
                        radius: 14.0
                        samples: 17
                        color: "#60000000"
                        source: pencil

                        MouseArea {
                            anchors.fill: parent
                            onClicked: root.currentColor = modelData
                        }
                    }
                }
            }
        }
    }
}