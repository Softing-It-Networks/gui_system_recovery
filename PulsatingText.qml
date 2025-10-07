import QtQuick 2.12
import QtQuick.Window 2.12

Text {
    id: root
    property int pulseDuration: 1000
    property color startColor: color
    property color endColor: Qt.lighter(color, 1.5)
    property real maxScale: 1.2

    SequentialAnimation {
        id: scaleAnimation
        running: true
        loops: Animation.Infinite
        NumberAnimation {
            target: root
            property: "scale"
            from: 1.0; to: root.maxScale;
            duration: root.pulseDuration/2;
            easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: root
            property: "scale"
            from: root.maxScale; to: 1.0;
            duration: root.pulseDuration/2;
            easing.type: Easing.InOutQuad
        }
    }

    SequentialAnimation {
        running: true
        loops: Animation.Infinite
        ColorAnimation {
            target: root
            property: "color"
            from: root.startColor; to: root.endColor;
            duration: root.pulseDuration/2
        }
        ColorAnimation {
            target: root
            property: "color"
            from: root.endColor; to: root.startColor;
            duration: root.pulseDuration/2
        }
    }
}
