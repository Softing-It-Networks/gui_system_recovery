import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
// import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.12

import AppCoreClass 1.0


Window {
    id: mainWindow
    width:  480
    height:  800
     visible: true
     color: "#006f88"
     property bool initialized: false
     property bool isUpdating: false

     // visibility: Window.FullScreen

     function onTimerUpdated(newVal){
                      timerVal.text = newVal;
                      if(newVal > 15)
                         timerVal.color = "#7FFC03";
                      else if((newVal <= 15) && (newVal > 10))
                           timerVal.color = "#F8FC03";
                      else
                           timerVal.color = "#FC0203";
     }
     function onUpdateTimerText(newText){
                           hideLables();
                           timerVal.text = newText;
                           timerVal.color = "#FC0203";
     }
     function onUpdateMode(){
                      console.log("onUpdateMode()");
                      isUpdating = true;
                      timerVal.text = qsTr("Updating");
                      hideLables();
     }
     function onRecoveryMod(isRecovery){
                      rect_10_recoveryButton.visible = isRecovery;
                      console.log("onRecoveryMod");
     }
     function hideLables(){
                      processRect.visible = false;
                      rect_1_text.visible = false;
                      rect_3_text.visible = false;
                      rect_4_text.visible = false;
                      rect_5_text.visible = false;
                      rect_6_restartButton.visible = false;
                      rect_7_text.visible = false;
                      rect_8_text.visible = false;
                      rect_9_bootButton.visible = false;
                      rect_2_timer.anchors.centerIn = mainColumn;
                      rect_10_recoveryButton.visible = false;
     }

     onActiveChanged: {
             if (active && !initialized) {
                 initialized = true;
                      // timerVal.text=  appCore.getTimerVal();
                 if(!isUpdating)
                      appCore.startTimer(30);
                 else
                      appCore.update();

                 console.log("Окно активировано, запускаем таймер...");
             }
     }


     Column{
         id:mainColumn
          anchors.fill: parent
          // spacing: 10
          width: parent.width * 0.8
          anchors.topMargin: 20
          anchors.leftMargin: 20
          anchors.rightMargin: 20

          Rectangle{
               id: processRect
               width: parent.width
               // height: Math.max(10, Math.min(100, mainWindow.height * 0.06))
               height: mainWindow.height * 0.15
               color: "#7fcff1"
               anchors.horizontalCenter: parent.horizontalCenter
               PulsatingText {
                    anchors.centerIn: parent
                     horizontalAlignment: Text.AlignHCenter
                    text: qsTr("Somthing want wrong.\nPlease wait...")
                    // font.pixelSize: 22
                    font.pixelSize: mainWindow.height * 0.04

                    color: "purple"
                    pulseDuration: 800
                    maxScale: 1.15
               }
          }
          Rectangle {
                      id:rect_1_text
                      width: parent.width
                      // height: Math.max(10, Math.min(100, mainWindow.height * 0.06))
                      height: mainWindow.height * 0.06
                      anchors.horizontalCenter: parent.horizontalCenter
                      color: "transparent"
                      Text {
                            id: lable_1
                            anchors.centerIn: parent
                            font.pixelSize: mainWindow.height * 0.024
                            text: qsTr("Restarting current partition in")
                      }
          }

          Rectangle {
                      id:rect_2_timer
                      width: parent.width
                      height: mainWindow.height * 0.06
                      anchors.horizontalCenter: parent.horizontalCenter
                      color: "transparent"
                      Text {
                                            id: timerVal
                                            color: "#7efb03"
                                            anchors.centerIn: parent
                                            font.pixelSize: mainWindow.height * 0.09
                                            // text: "20"
                      }

          }

         Rectangle {

                      id:rect_3_text
                      width: parent.width
                      height: mainWindow.height * 0.06

                      anchors.horizontalCenter: parent.horizontalCenter
                      color: "transparent"
                      Text {
                                            id: lable_2
                                            anchors.centerIn: parent
                                            font.pixelSize: mainWindow.height * 0.024
                                            text: qsTr("seconds")

                      }
         }
         Rectangle {
                      id:rect_4_text
                      width: parent.width
                      height: mainWindow.height * 0.06

                      anchors.horizontalCenter: parent.horizontalCenter
                      color: "transparent"
                      // property int activeBoot: appCore.activeBoot();
                      Row{
                                            anchors.horizontalCenter: parent.horizontalCenter
                      Text {
                                            font.pixelSize: mainWindow.height * 0.024
                                            text: qsTr("Current partition is: ")
                                            Layout.alignment: Qt.AlignVCenter
                      }
                      Text {

                                            // anchors.centerIn: parent
                                            font.pixelSize: mainWindow.height * 0.024

                                            // color: "#ef285d"
                                            color: {
                                                 if(appCore.activeBoot() === 0)
                                                                  return "#ef285d"
                                            }

                                            text: {
                                                if( appCore.activeBoot() > 0)
                                                                      return appCore.activeBoot();
                                                else
                                                                      return qsTr("unable to define an active partition")
                                            }
                                            //text: qsTr("unable to define an active partition")





                                            // else{
                                            // text: appCore.activeBoot()
                                            // }

                                            Layout.alignment: Qt.AlignVCenter
                      }
                      }

         }
         Rectangle {
                      id:rect_5_text
                      width: parent.width
                      height: mainWindow.height * 0.06

                      anchors.horizontalCenter: parent.horizontalCenter
                      color: "transparent"
                      Text {
                                            anchors.centerIn: parent
                                            font.pixelSize: mainWindow.height * 0.024
                                            text: qsTr("Please select:")
                      }
         }

         Rectangle{
                      id:rect_6_restartButton
                      width: Math.min(Math.max(mainWindow.width * 0.4, 200), mainWindow.width/2)
                      height: mainWindow.height * 0.1
                      radius: 10
                      color: "transparent"
                      anchors.horizontalCenter: parent.horizontalCenter
                      AnimatedButton {
                                    id: restartButton
                                    anchors.centerIn: parent
                                    width: parent.width
                                    height: parent.height
                                    radius: 10
                                    textButton: qsTr("Restart from current\n partition now")
                                    onButtonClicked:{
                                            // hideLables()
                                            appCore.restart()
                                            // console.log("Hi from restartButton")
                                            // appCore.restart()
                                    }
                      }

         }
         Rectangle {
                      id:rect_7_text
                      width: parent.width
                      height: mainWindow.height * 0.06
                      anchors.horizontalCenter: parent.horizontalCenter
                      color: "transparent"
                      Text {
                      anchors.centerIn: parent
                      font.pixelSize: mainWindow.height * 0.024
                      text: qsTr("or")
                      }
         }
         Rectangle {
                      id:rect_8_text
                      width: mainWindow.width
                      height: mainWindow.height * 0.06

                      anchors.horizontalCenter: parent.horizontalCenter
                      color: "transparent"
                      Text {
                      anchors.centerIn: parent
                      font.pixelSize: mainWindow.height * 0.024
                      text: qsTr("Warning: Settings and data will be lost\n when booting from other partition")
                      }
         }
         Rectangle{
                      id: rect_9_bootButton
                      width: Math.min(Math.max(mainWindow.width * 0.4, 200), mainWindow.width/2)
                      height: mainWindow.height * 0.1
                      radius: 10
                      color: "transparent"
                      anchors.horizontalCenter: parent.horizontalCenter
                      AnimatedButton {
                      anchors.centerIn: parent
                      width: parent.width
                      height: parent.height
                      radius: 10
                      textButton: qsTr("Boot from other\n partition")
                      onButtonClicked:{
                                            // hideLables()
                                            appCore.boot()
                                            // console.log("Hi from bootButton")
                                            // appCore.boot()
                      }
                      }

         }
         Item {
               id: emptyItem_0
               width: parent.width
               height: 20  // размер отступа
           }
         Rectangle{
                      id: rect_10_recoveryButton
                      width: Math.min(Math.max(mainWindow.width * 0.4, 200), mainWindow.width/2)
                      height: mainWindow.height * 0.1
                      radius: 10
                      color: "transparent"
                      anchors.horizontalCenter: parent.horizontalCenter


                      AnimatedButton {
                      anchors.centerIn: parent
                      width: parent.width
                      height: parent.height
                      radius: 10
                      //anchors.topMargin: 50
                      textButton: qsTr("Recover")
                      onButtonClicked:{
                                            // hideLables()
                                            appCore.recovery()
                                            // console.log("Hi from bootButton")
                                            // appCore.boot()
                      }
                      }

         }

     }


     Component.onCompleted: {
                      appCore.setUpRecoveryMode.connect(onRecoveryMod)
                      appCore.setUpdateMode.connect(onUpdateMode)
                      appCore.currentTime.connect(onTimerUpdated)
                      appCore.timerText.connect(onUpdateTimerText)
     }
}
