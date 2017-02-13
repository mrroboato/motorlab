// websocket.js

var socket = new WebSocket("ws://localhost:2441/websocket");

$(document).ready(function() {

    socket.onmessage = function (evt) {

    	sensorData = JSON.parse(evt.data);

        // console.log(sensorData);

        var potMonitor = $('#potMonitor');
        potMonitor.text(sensorData['Pot']);

        var encoderMonitor = $('#encoderMonitor');
        // ultMonitor.text(sensorData['Ult']);
        encoderData = sensorData['Enc'].split(";");
        // console.log(imuData);
        // console.log("%d, %d, %d", imuData[0],imuData[1], imuData[2]);
        encoderMonitor.text(sensorData['Enc']);

        var irMonitor = $('#irMonitor');
        irMonitor.text(sensorData['Ir']);
    };

    socket.onclose = function() {

    	var connectionClosedText= "Connection closed.";

        var potMonitor = $('#potMonitor');
        potMonitor.text(connectionClosedText);

        var encoderMonitor = $('#encoderMonitor');
        encoderMonitor.text(connectionClosedText);

        var irMonitor = $('#irMonitor');
        irMonitor.text(connectionClosedText);
    };

    $("#servoSlider").on("input change", function() { sendServoData(); });
    $("#dcSlider").on("input change", function() { sendDCData(); });
    $('#stepperButton').attr('href','javascript:sendStepperData()');
});

var sendServoData = function() {
    if(socket.readyState === socket.OPEN){
       socket.send('Servo');
       console.log("Servo message sent");
    }
}

var sendDCData = function() {
    if(socket.readyState === socket.OPEN){
       socket.send('DC');
       console.log("DC message sent");
    }
}

var sendStepperData = function() {
    if(socket.readyState === socket.OPEN){
       socket.send('Stepper');
    }
}


