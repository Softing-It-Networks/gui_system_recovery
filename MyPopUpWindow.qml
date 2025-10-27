import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
// import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.12

import AppCoreClass 1.0


Popup {
    id: popUpWindow
    property string message: ""
    property string title: "Сообщение"
    property int  fontPixelSize: 16
    signal buttonClicked()
    
    // width: 200
    // height: 150
    modal: true
    dim: true
    enter: Transition {
        ParallelAnimation {
            NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 300 }
            NumberAnimation { property: "scale"; from: 0.7; to: 1; duration: 300 }
        }
    }
    
    exit: Transition {
        ParallelAnimation {
            NumberAnimation { property: "opacity"; from: 1; to: 0; duration: 200 }
            NumberAnimation { property: "scale"; from: 1; to: 0.7; duration: 200 }
        }
    }
    
    background: Rectangle {
        color: "white"
        border.color: "lightgray"
        radius: 8
    }
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 15
        
        Text {
            text: popUpWindow.title
            font.bold: true
            font.pixelSize: fontPixelSize + 5
            Layout.alignment: Qt.AlignHCenter
        }
        
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "lightgray"
        }
        
        Text {

            font.pixelSize: fontPixelSize
            text: popUpWindow.message
            wrapMode: Text.Wrap
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
        
        Button {
            text: qsTr("Close")
            Layout.alignment: Qt.AlignHCenter
            onClicked: {
                popUpWindow.close()
                buttonClicked()
            }
        }
    }
}
