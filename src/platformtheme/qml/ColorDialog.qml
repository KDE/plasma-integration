import QtQuick 2.10
import QtQuick.Layouts 1.10
import QtQuick.Controls 2.10
import org.kde.kirigami 2.13 as Kirigami

Item {
    id: root

    property color currentColor: "cyan"
    onCurrentColorChanged: {
        helper.changed(currentColor)
    }

    width: 400
    height: 500

    ColumnLayout {
        anchors.fill: parent
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
            id: tb

            Layout.fillWidth: true
            position: ToolBar.Footer

            RowLayout {
                anchors.fill: parent

                Item {
                    Layout.preferredWidth: expanderHandle.implicitWidth
                }

                ColorCell {
                    color: root.currentColor

                    Layout.fillWidth: true
                }

                Button {
                    text: i18nd("plasma-integration-color-dialog", "Pick Colour From Screen")
                    onClicked: helper.pick()
                }
            }
        }
    }
    Button {
        id: expanderHandle

        anchors {
            left: parent.left
            bottom: expander.top
            bottomMargin: tb.bottomPadding
            leftMargin: tb.leftPadding
        }
        text: expander.childVisible ? i18nd("plasma-integration-color-dialog", "Close") : i18nd("plasma-integration-color-dialog", "View Saved Colours")

        onClicked: expander.childVisible = !expander.childVisible
    }
    Expandable {
        id: expander

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        Control {
            topPadding: 20
            leftPadding: 20
            rightPadding: 20
            bottomPadding: 20
            padding: 20
            anchors.left: parent.left
            anchors.right: parent.right

            Kirigami.Theme.colorSet: Kirigami.Theme.View
            Kirigami.Theme.inherit: false

            background: Rectangle {
                color: Kirigami.Theme.backgroundColor

                Kirigami.Separator {
                    anchors {
                        top: parent.top
                        left: parent.left
                        right: parent.right
                    }
                }
            }

            contentItem: ColumnLayout {
                RowLayout {
                    // TODO: make this do something
                    // TODO: removing colors
                    // TODO: copy hex to clipboard
                    Kirigami.SearchField {
                        Layout.fillWidth: true
                    }
                    Button {
                        id: addColourButton

                        text: i18nd("plasma-integration-color-dialog", "Save Color")
                        icon {
                            source: Qt.resolvedUrl("circle.svg")
                            color: root.currentColor
                        }

                        onClicked: state = "active"

                        state: "passive"
                        states: [
                            State {
                                name: "passive"
                            },
                            State {
                                name: "active"
                                PropertyChanges {
                                    target: addColourButton
                                    visible: false
                                }
                                PropertyChanges {
                                    target: nameField
                                    visible: true
                                }
                                PropertyChanges {
                                    target: submitButton
                                    visible: true
                                }
                            }
                        ]
                    }
                    TextField {
                        id: nameField
                        placeholderText: i18nd("plasma-integration-color-dialog", "Name your color...")
                        visible: false
                    }
                    Button {
                        id: submitButton
                        icon.name: "arrow-right"
                        visible: false
                        enabled: nameField.text.trim() !== ""
                        onClicked: {
                            addColourButton.state = "passive"
                            let data = helper.savedColors || []
                            data.push({
                                color: String(root.currentColor),
                                name: nameField.text,
                            })
                            data.sort((a, b) => {
                                if (a.name < b.name) { return -1; }
                                if (a.name > b.name) { return 1; }
                                return 0;
                            })
                            helper.savedColors = data
                            colorsView.model = helper.savedColors
                        }
                    }
                    Layout.fillWidth: true
                }
                ListView {
                    id: colorsView
                    model: helper.savedColors

                    Layout.fillWidth: true
                    Layout.preferredHeight: root.height * 0.7

                    spacing: Kirigami.Units.largeSpacing
                    clip: true

                    delegate: Kirigami.AbstractCard {
                        //TODO: kirigami.abstractcard has padding issues,
                        // so we nest a plain control to bypass them
                        leftPadding: 0
                        rightPadding: 0
                        topPadding: 0
                        bottomPadding: 0
                        contentItem: Control {
                            leftPadding: Kirigami.Units.largeSpacing
                            rightPadding: Kirigami.Units.largeSpacing
                            topPadding: Kirigami.Units.largeSpacing
                            bottomPadding: Kirigami.Units.largeSpacing
                            contentItem: RowLayout {
                                Kirigami.Heading {
                                    text: modelData.name
                                    level: 2
                                }
                                Item { Layout.fillWidth: true }
                                Rectangle {
                                    color: modelData.color
                                    radius: height / 2

                                    Layout.preferredWidth: height
                                    Layout.fillHeight: true
                                }
                            }
                        }
                        onClicked: root.currentColor = modelData.color
                        width: parent.width
                    }

                    ColumnLayout {
                        anchors.centerIn: parent
                        visible: colorsView.count == 0

                        Kirigami.Heading {
                            text: i18nd("plasma-integration-color-dialog", "No colors saved")
                            level: 3

                            horizontalAlignment: Text.AlignHCenter
                            Layout.fillWidth: true
                        }
                        Label {
                            text: i18nd("plasma-integration-color-dialog", "Save a color and it will show up here.")

                            horizontalAlignment: Text.AlignHCenter
                            Layout.fillWidth: true
                        }
                    }
                }
            }
        }
    }
}
