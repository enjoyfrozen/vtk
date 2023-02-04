// import related modules
import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12

// import the VTK module
import Vtk 1.0

// window containing the application
Window {
  // title of the application
  width: 800
  height: 800

  // a rectangle in the middle of the content area
  Rectangle {
    width: 100
    height: 100
    color: "blue"
    border.color: "red"
    border.width: 5
    radius: 10
  }
  Text {
    id: label
    color: "white"
    wrapMode: Text.WordWrap
    text: "Custom QML\nrectangle &\ntext"
    anchors.right: parent.right
    anchors.left: parent.left
    anchors.top: parent.top
    anchors.margins: 10
    width: 100
  }

  RowLayout {
    anchors.fill: parent

    MyConeItem {
      Layout.column: 0
      Layout.fillWidth: true
      Layout.fillHeight: true
      Layout.minimumWidth: 100
      Layout.preferredWidth: 200
      focus: true
    }
    MyWidgetItem {
      Layout.column: 1
      Layout.fillWidth: true
      Layout.fillHeight: true
      Layout.minimumWidth: 100
      Layout.preferredWidth: 400
      focus: true
    }
  }
}
