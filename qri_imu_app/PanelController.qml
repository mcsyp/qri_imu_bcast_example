import QtQuick 2.0

Rectangle {
    anchors.fill:parent
    property real newscale:parent.width/1080
    property bool processing:false;
    PVPchart{
        id:pvp_chart
        width:parent.width
        height:700*newscale
        x:0
        y:0
    }
    AccelLinechart{
        id:accel_chart
        width:parent.width
        height:650*newscale
        x:0;
        y:pvp_chart.height
    }
    GyroLinechart{
        id:gyro_chart
        width:parent.width
        x:0;
        y:accel_chart.y+accel_chart.height-20
    }
    /*
    Canvas{
        id:canvas_2d
        anchors.fill: parent;

        onPaint:{
            var ctx = getContext("2d");
            ctx.strokeStyle="gray";
            ctx.lineWidth=3;
            ctx.beginPath();
            ctx.moveTo(0,0);
            ctx.lineTo(150,500);
            ctx.stroke();
            ctx.fill();

        }
    }
    */
    /*
    Timer{
        repeat: true;
        interval:30;
        running:processing
        onTriggered:{

        }
    }*/
}
