// websocket.js

var socket = new WebSocket("ws://localhost:2441/websocket");

$(document).ready(function() {

    socket.onmessage = function (evt) {

    	receivedData = JSON.parse(evt.data);

        // console.log(receivedData);

        if (receivedData['Type'] == 'stateData') {
            isGUIState = receivedData['State'] == 1;
            toggleControls(isGUIState);
            // console.log(receivedData['State']);
            return;
        }

        var potMonitor = $('#potMonitor');
        potMonitor.text(receivedData['Pot']);

        var irMonitor = $('#irMonitor');
        // ultMonitor.text(receivedData['Ult']);
        irData = receivedData['Ir'];
        // console.log(imuData);
        // console.log("%d, %d, %d", imuData[0],imuData[1], imuData[2]);
        irMonitor.text(irData);

        var photoMonitor = $('#photoMonitor');
        photoMonitor.text(receivedData['Pho']/100);
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

    toggleControls(0);

    $('#controlModeSwitch').change(function() { onSwitchClick(); }); 
    $('#dcModeSwitch').change(function() { onDCSwitchClick(); }); 
    $("#servoSlider").on("change", function() { sendServoData(); });
    $("#dcSlider").on("change", function() { sendDCData(); });
    $('#stepperRunButton').attr('href','javascript:sendStepperRunData()');
    $('#stepperStopButton').attr('href','javascript:sendStepperStopData()');
});


var onSwitchClick = function() {
    var switchVal = $('#controlModeSwitch').prop('checked');
    toggleControls(switchVal);
    sendSwitchData(switchVal);
}

var onDCSwitchClick = function() {
    var switchVal = $('#dcModeSwitch').prop('checked');
    sendDCSwitchData(switchVal);
}  


var sendSwitchData = function(switchVal) {
    if(socket.readyState === socket.OPEN) {
        socket.send('switch:' + switchVal.toString());
        // console.log("Switch message sent");
    }
}

var sendDCSwitchData = function(switchVal) {
    if(socket.readyState === socket.OPEN) {
        socket.send('switch_dc:' + switchVal.toString());
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

var sendStepperRunData = function() {
    if(socket.readyState === socket.OPEN) {
        socket.send('stepper:1');
    }
}

var sendStepperStopData = function() {
    if(socket.readyState === socket.OPEN) {
        socket.send('stepper:0');
    }
}


var toggleControls = function(switchVal) {
    $('#controlModeSwitch').prop( "checked", switchVal );
    if (switchVal) {
        hideSensorMonitors();
        showMotorControls();
    } else {
        showSensorMonitors();
        hideMotorControls();
    }
}

var changeDCMode = function(switchVal) {
    if (switchVal) {
        $('#dcSlider').prop({
            'min': -100,
            'max': 100
        });
    } else {
        $('#dcSlider').prop({
            'min': 0,
            'max': 360
        });
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
    $('#stepperRunButton').hide();
    $('#stepperStopButton').hide();
    $('#dcModeSwitchContainer').hide();
}

var showMotorControls = function() {
    $('#servoSlider').show();
    $('#dcSlider').show();
    $('#stepperRunButton').show();
    $('#stepperStopButton').show();
    $('#dcModeSwitchContainer').show();
}