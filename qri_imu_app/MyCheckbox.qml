import QtQuick 2.0

Item {
    id:checkbox
    property var text:"hello?"
    property var checked: false;
    property var enableColor:"#b0b0b0"
    signal boxChecked(var checked);
    width:(check_img.width+lbl_text.contentWidth)*scale
    height:check_img.height
    Image{
        id:check_img
        source:"images/checkbox.png"
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        visible:true
    }
    Image{
        id:check_img_fill
        source:"images/checkbox_fill.png"
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        visible:false
    }
    Text{
        id:lbl_text
        x:check_img.x+check_img.width
        anchors.verticalCenter: parent.verticalCenter
        text:checkbox.text
        color:"#808080"
        font.pixelSize: parent.height*0.6
    }

    function setChecked(b){
        check_img.visible=!b;
        check_img_fill.visible=b;
        if(b===true){
            lbl_text.color = enableColor;
        }else{
            lbl_text.color = "#808080";
        }
        checked = b;
        boxChecked(b);
    }

    MouseArea{
        anchors.fill: check_img_fill;
        onClicked: {
            setChecked(!checked);
        }
    }
    Component.onCompleted: {
        setChecked(false);
    }
}
