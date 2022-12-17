import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import com.vtk.example 1.0

ApplicationWindow {
    id: win
    visible: true
    width: 640
    height: 640
    title: qsTr("Hello World")

    header: ToolBar {
        id: header
        RowLayout {
            anchors.fill: parent
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            Rectangle {
                color: "black"
                Layout.preferredWidth: 1
                Layout.fillHeight: true
            }
            Text {
                Layout.leftMargin: 10
                text: "vtkSource:"
            }
            ComboBox {
                id: sources
                Layout.fillHeight: true
                Layout.preferredWidth: childrenRect.width
                model: presenter.sources
            }
        }
    }

    DynamicSplitView {
        anchors.fill: parent

        itemDelegate: DynamicSplitView.ItemDelegate
        {
            Rectangle {
                id: r
                border { width: 5; color: "steelblue" }
                radius: 5
                color: "magenta"
                anchors.fill: parent

                MyVtkItem {
                    id: vtk
                    anchors.fill: parent
                    source: sources.currentText
                }
            }

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.MiddleButton | Qt.RightButton
                onClicked: mouse=> {
                    if (mouse.button === Qt.LeftButton) {
                        split(Qt.Horizontal)
                    } else if (mouse.button === Qt.RightButton) {
                        split(Qt.Vertical)
                    } else {
                        unsplit()
                    }
                }
            }
        }
    }

    Text {
        text: "Left-Click to horizontal-split;  Right-Click to vertical-split;  Middle-Click to delete-split"
        font.pointSize: 12
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }
}
