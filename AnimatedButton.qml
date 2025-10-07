import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.12

import AppCoreClass 1.0

Rectangle {
    // anchors.centerIn: parent
    // width: parent.width
    // height: parent.height
    // radius: 10
    property string textButton: text
    color: hovered ? "#66ccff" : "#3399ff"
    property bool hovered: false
    signal buttonClicked()
    
    // Текст кнопки
    Text {
        id: buttonText
        anchors.centerIn: parent
        horizontalAlignment: Text.AlignHCenter
        text: textButton
        color: "white"
        font.pixelSize: mainWindow.height * 0.035
    }
    
    // Обработка мыши
    MouseArea {
        anchors.fill: parent
        hoverEnabled: false
        
        onEntered: parent.hovered = true
        onExited: parent.hovered = false
        // onClicked: console.log("Кнопка нажата!")
    }
    
    // Анимация изменения цвета
    Behavior on color {
        ColorAnimation { duration: 250 }
    }
    
    // Анимация "нажатия" (эффект увеличения/уменьшения)
    MouseArea {

        id: mouseArea
        anchors.fill: parent
        onPressed: parent.scale = 0.9
        onReleased: parent.scale = 1.0
        onClicked: {
            // console.debug("asdas");
            buttonClicked()
        }
    }
    
    Behavior on scale {
        NumberAnimation { duration: 150; easing.type: Easing.OutQuad }
    }
}
