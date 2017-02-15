package main

import (
    "os"
    "log"
    "strconv"
    "reflect"
    "strings"
    "net/http"
    "encoding/json"
    "github.com/seongminnpark/serial"
    "github.com/gorilla/websocket"
)

const MESSAGE_BEGIN = "*"
const MESSAGE_END = "#"
const STATE_SENSOR = '%'
const STATE_GUI = '^'

var ValidSensors = [3]string{"Pot", "Ir", "Pho"}

type SensorData struct {
    Type string
    Pot int
    Ir  int
    Pho int
} 

type StateData struct {
    Type string
    State int 
}

var upgrader = websocket.Upgrader{
    ReadBufferSize: 1024,
    WriteBufferSize: 1024,
}

var serialCon *serial.Port 
var socketCon *websocket.Conn

var state int

func main() {

    // Extract port name:
    commandlineArgs := os.Args
    if (len(commandlineArgs)) < 2 {
        log.Printf("\n\n" + 
            "ERROR: Specify port name.\n" + 
            "On mac or linux, you can search for available COM ports with 'ls /dev/tty.*'.\n" + 
            "On Windows, go to Control Panel > Device Manager.\n\n")
        return
    } else if (len(commandlineArgs) > 2) {
        log.Printf("ERROR: Too many arguments!")
        return
    }
    comport := commandlineArgs[1]

    log.Printf("Connect to localhost:2441 in your browser to see sensor data.")

    state = 0

    // Init serial.
    initSerial(comport)

    // Start server.
    go initServer()

    // Configure serial port and start listening.
    startSerial()
}

func initServer() {

    var err error

    // Websocket connection handler.
    http.HandleFunc("/websocket", func(w http.ResponseWriter, r *http.Request) {      
        socketCon, err = upgrader.Upgrade(w, r, nil)
        log.Printf("Made connection to client.")
        handleError(err)
        for {
            _, p, err := socketCon.ReadMessage()
            handleError(err)
            // log.Printf("%s\n", p)
            if serialCon != nil {
                // log.Printf("Sending to arduino.")
                messageSlice := strings.Split(string(p), ":")
                motorName := messageSlice[0]
                motorVal := messageSlice[1]
                message := p
                switch motorName {
                case "switch":
                    if motorVal == "false" {
                        // messageArray := [1]byte{STATE_SENSOR}
                        // message = messageArray[:]
                        message = []byte("state:0\r")
                    } else {
                        // messageArray := [1]byte{STATE_GUI}
                        // message = messageArray[:]
                        message = []byte("state:1\r")
                    }

                case "servo":
                    message = []byte("servo:" + string(motorVal) + "\r")

                case "dc":
                    message = []byte("dc:" + string(motorVal) + "\r")

                case "stepper":
                    message = []byte("stepper:" + string(motorVal) + "\r")
                }

                _, err = serialCon.Write(message)
                handleError(err)

            } else {
                log.Printf("Serial port nil.")
            }
        }
    })

    // GUI HTTP Request receiver.
    http.Handle("/", http.FileServer(http.Dir("./static")))
    http.ListenAndServe(":2441", nil)
}

func initSerial(comport string) {
    log.Printf("Setting up serial port %s...", comport)
    var err error
    c := &serial.Config{Name: comport, Baud: 115200}
    serialCon, err = serial.OpenPort(c)
    handleError(err)
}


func startSerial() {

    // buf := make([]byte, 128)
    buf := make([]byte, 1)
    received := make([]byte, 1)

    log.Printf("Listening from serial com...\n" + 
               "CTRL+C to exit program.")

    for {

        _, err := serialCon.Read(buf)
        handleError(err)
        // log.Printf("%s", buf[0])

        if string(buf[0]) == MESSAGE_BEGIN {
            _, err := serialCon.Read(buf)
            handleError(err)

            for string(buf[0]) != MESSAGE_END {
                received = append(received, buf...)
                // log.Printf("%q", received)
                _, err := serialCon.Read(buf)
                handleError(err)
            }
        }

        // log.Printf("%q", received[1:])

        if (socketCon != nil) {
            if string(received) == "state-toggle" {
                if state == 0 { 
                    state = 1
                } else {
                    state = 0
                }
                sendStateInfo(state)
            } else {
                sendSensorInfo(received[1:])
            }
        } 

        received = make([]byte, 1)
    }
}

func sendSensorInfo(received []byte) {
    // log.Printf("%s", received)
    messageSlice := strings.Split(string(received), ",")

    sensorData := SensorData{Type:"sensorData"}

    for _, message := range messageSlice {
        parsed := strings.Split(message, ":")
        // log.Printf("%s", message)
        sensorName := parsed[0]
        if isValidSensorName(sensorName) && len(parsed) == 2 {
            sensorNameField := reflect.ValueOf(&sensorData).Elem().FieldByName(sensorName)
            sensorVal, err := strconv.Atoi(parsed[1])
            if err == nil {
                sensorNameField.SetInt(int64(sensorVal))
            }
            
            // log.Printf("%s: %i", sensorName, sensorVal)
        } 
    }  

    sensorJson, err := json.Marshal(sensorData)
    handleError(err)

    err = socketCon.WriteMessage(websocket.TextMessage, sensorJson)
    // log.Printf("Sending sensor info")
    handleError(err)
}

func sendStateInfo(state int) {
    stateData := StateData{Type:"stateData", State: state} 

    stateJson, err := json.Marshal(stateData)
    handleError(err)

    err = socketCon.WriteMessage(websocket.TextMessage, stateJson)
    // log.Printf("Sending sensor info")
    handleError(err)
}

func receiveSensorData(sensorName string, senorValue int) {
    log.Printf("sensorName: %s, senorValue: %d\n", sensorName, senorValue)
}

func handleError(err error) {
    if err != nil {
        log.Fatal(err)
    }
}

func isValidSensorName(name string) bool {
    for _, string := range ValidSensors {
        if string == name {
            return true
        }
    }
    return false
}