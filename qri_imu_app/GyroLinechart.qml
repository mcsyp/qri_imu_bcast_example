import QtQuick 2.0
import "Queue.js" as Queue
Rectangle{
    property real newscale:width/1080
    property var gyro_data_x;
    property var gyro_data_y;
    property var gyro_data_z;

    property int sample_max:100
    property int sample_interval: width/(sample_max+1)

    MyCheckbox{
        id:gyro_z;
        x:parent.width-gyro_z.width-25
        y:-5;
        scale:newscale
        text:"Gyro Z"
        enableColor: "#0000FF"
    }
    MyCheckbox{
        id:gyro_y;
        x:gyro_z.x-gyro_y.width-5
        y:gyro_z.y;
        scale:newscale
        text:"Gyro Y"
        enableColor: "#00FF00"
    }
    MyCheckbox{
        id:gyro_x;
        x:gyro_y.x-gyro_x.width-5
        y:gyro_z.y
        scale:newscale
        text:"Gyro X"
        enableColor: "#FF0000"
    }
    Image{
        source:"images/gyro_logo.png"
        width:parent.width
        height:50*width/1080
        x:0
        y:0
        z:5
    }

    Image{
        id:img_background
        source:"images/acc_gyro_recorder.png"
        width:parent.width
        height:500*parent.width/1080
        x:0;
        y:gyro_z.height;
        z:0

        Canvas{
            id:canvas_2d
            property int zero_y:img_background.height/2;
            property int zero_x:10;
            anchors.fill:img_background
            onPaint:{
                var ctx = canvas_2d.getContext("2d");
                ctx.drawImage(img_background,0,0);
                if(gyro_x.checked){
                    drawCurve(ctx,gyro_data_x,"#FF0000");
                }
                if(gyro_y.checked){
                    drawCurve(ctx,gyro_data_y,"#00FF00");
                }
                if(gyro_z.checked){
                    drawCurve(ctx,gyro_data_z,"#0000FF");
                }
            }
            function drawCurve(ctx,queue,color){
                ctx.strokeStyle=color;
                ctx.lineWidth=2;
                ctx.beginPath();
                ctx.moveTo(zero_x,zero_y);
                for(var i=0;i<queue.size();i++){
                    var x= zero_x+i*sample_interval;
                    var y = queue.data[i]+zero_y;
                    ctx.lineTo(x,y);
                }
                ctx.stroke();
            }
        }
    }


    Connections{
        target:cstController
        onCharGyroUpdated:{
            //process gx
            var agx=gx*10/32768.9;
            gyro_data_x.enqueue(agx);
            if(gyro_data_x.size()>sample_max){
                gyro_data_x.dequeue();
            }
            //process gy
            var agy=gy*10/32768.9;
            gyro_data_y.enqueue(agy);
            if(gyro_data_y.size()>sample_max){
                gyro_data_y.dequeue();
            }

            //process gz
            var agz=gz*10/32768.9;
            gyro_data_z.enqueue(agz);
            if(gyro_data_z.size()>sample_max){
                gyro_data_z.dequeue();
            }

            //console.log(agx+","+agy+","+agz);
            canvas_2d.requestPaint();
        }
    }
    Connections{
        target:gyro_x
        onBoxChecked:{
            canvas_2d.requestPaint();
        }
    }
    Connections{
        target:gyro_y
        onBoxChecked:{
            canvas_2d.requestPaint();
        }
    }
    Connections{
        target:gyro_z
        onBoxChecked:{
            canvas_2d.requestPaint();
        }
    }

    Component.onCompleted: {
        gyro_data_x = new Queue.Queue();
        gyro_data_y = new Queue.Queue();
        gyro_data_z = new Queue.Queue();
        for(var i=0;i<sample_max;i++){
            var y = Math.random(i)*200-100;
            gyro_data_x.enqueue(y);

            y = Math.random(i)*200-100;
            gyro_data_y.enqueue(y);

            y = Math.random(i)*200-100;
            gyro_data_z.enqueue(y);
        }
        canvas_2d.requestPaint();
    }

}
