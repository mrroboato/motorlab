// websocket.js

var socket = new WebSocket("ws://localhost:2441/websocket");

$(document).ready(function() {

    socket.onmessage = function (evt) {

    	sensorData = JSON.parse(evt.data);

        // console.log(sensorData);

        var potMonitor = $('#potMonitor');
        potMonitor.text(sensorData['Pot']);

        var irMonitor = $('#irMonitor');
        // ultMonitor.text(sensorData['Ult']);
        irData = sensorData['Enc'].split(";");
        // console.log(imuData);
        // console.log("%d, %d, %d", imuData[0],imuData[1], imuData[2]);
        irMonitor.text(irData['Enc']);

        var photoMonitor = $('#photoMonitor');
        photoMonitor.text(sensorData['Ir']);
    };

    socket.onclose = function() {

    	var connectionClosedText= "Connection closed.";

        var potMonitor = $('#potMonitor');
        potMonitor.text(connectionClosedText);

        var encoderMonitor = $('#irMonitor');
        encoderMonitor.text(connectionClosedText);

        var photoMonitor = $('#photoMonitor');
        photoMonitor.text(connectionClosedText);
    };

    toggleControls(false);

    $('#controlModeSwitch').change(function() { onSwitchClick(); }); 
    $("#servoSlider").on("input change", function() { sendServoData(); });
    $("#dcSlider").on("input change", function() { sendDCData(); });
    $('#stepperButton').attr('href','javascript:sendStepperData()');
});


var onSwitchClick = function() {
    var switchVal = $('#controlModeSwitch').prop('checked');
    toggleControls(switchVal);
    sendSwitchData(switchVal);
}


var sendSwitchData = function(switchVal) {
    if(socket.readyState === socket.OPEN) {
        socket.send('switch:' + switchVal.toString());
        // console.log("Switch message sent");
    }
}

var sendServoData = function() {
    if(socket.readyState === socket.OPEN) {
        var servoVal = $('#servoSlider').val();
        socket.send('servo:' + servoVal.toString());
        // console.log("Servo message sent");
    }
}

var sendDCData = function() {
    if(socket.readyState === socket.OPEN) {
        var dcVal = $('#dcSlider').val();
        socket.send('dc:' + dcVal.toString());
        // console.log("DC message sent");
    }
}

var sendStepperData = function() {
    if(socket.readyState === socket.OPEN) {
        socket.send('stepper:clicked');
    }
}


var toggleControls = function(switchVal) {
    if (switchVal) {
        hideSensorMonitors();
        showMotorControls();
    } else {
        showSensorMonitors();
        hideMotorControls();
    }
}


var hideSensorMonitors = function() {
    $('#potMonitorContainer').hide();
    $('#irMonitorContainer').hide();
    $('#photoMonitorContainer').hide();
}

var showSensorMonitors = function() {
    $('#potMonitorContainer').show();
    $('#irMonitorContainer').show();
    $('#photoMonitorContainer').show();
}

var hideMotorControls = function() {
    $('#servoSlider').hide();
    $('#dcSlider').hide();
    $('#stepperButton').hide();
}

var showMotorControls = function() {
    $('#servoSlider').show();
    $('#dcSlider').show();
    $('#stepperButton').show();
}