// websocket.js

$(document).ready(function() {

    var socket = new WebSocket("ws://localhost:2441/websocket");

    socket.onmessage = function (evt) {

    	sensorData = JSON.parse(evt.data);

        // console.log(sensorData);

        var potMonitor = $('#potMonitor');
        potMonitor.text(sensorData['Pot']);

        var imuMonitor = $('#imuMonitor');
        // ultMonitor.text(sensorData['Ult']);
        imuData = sensorData['Imu'].split(";");
        // console.log(imuData);
        // console.log("%d, %d, %d", imuData[0],imuData[1], imuData[2]);
        imuMonitor.text("X orientation: " + imuData[0].toString() +  "\n" + 
                        ", Y orientation: " + imuData[1].toString() + "\n" + 
                        ", Z orientation: " + imuData[2].toString());

        var irMonitor = $('#irMonitor');
        irMonitor.text(sensorData['Ir']);
    };

    socket.onclose = function() {

    	var connectionClosedText= "Connection closed.";

        var potMonitor = $('#potMonitor');
        potMonitor.text(connectionClosedText);

        var imuMonitor = $('#imuMonitor');
        imuMonitor.text(connectionClosedText);

        var irMonitor = $('#irMonitor');
        irMonitor.text(connectionClosedText);
    };
});


