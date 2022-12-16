// import related modules
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

// import the VTK module
import Vtk 1.0

// window containing the application
ApplicationWindow {
  // title of the application
  title: qsTr("VTK QtQuick App")
  width: 800
  height: 600
  color: palette.window

  SystemPalette {
    id: palette
    colorGroup: SystemPalette.Active
  }

  // menubar with two menus
  menuBar: MenuBar {
    Menu {
      title: qsTr("File")
      MenuItem {
        text: qsTr("&Quit")
        onTriggered: Qt.quit()
      }
    }
    Menu {
      title: qsTr("Edit")
    }
  }

  SplitView {
    anchors.fill: parent
    orientation: Qt.Horizontal

    MyVolumeItem {
      SplitView.fillHeight: true
      SplitView.fillWidth: true
      SplitView.minimumHeight: 100
      SplitView.minimumWidth: 100
      SplitView.preferredHeight: 200
      SplitView.preferredWidth: 200
    }

    ColumnLayout {
      SplitView.fillHeight: true
      SplitView.fillWidth: true
      SplitView.minimumWidth: 200
      SplitView.preferredWidth: 200
      MyGlyphItem {
        focus: true
        Layout.fillHeight: true
        Layout.fillWidth: true
      }
      MyGeomItem {
        Layout.fillHeight: true
        Layout.fillWidth: true
      }
    }
  }
}
