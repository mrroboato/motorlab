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

var ValidSensors = [3]string{"Pot", "Imu", "Ir"}

type SensorData struct {
    Pot int
    Imu string
    Ir  int
} 

var upgrader = websocket.Upgrader{
    ReadBufferSize: 1024,
    WriteBufferSize: 1024,
}

var connection *websocket.Conn

func main() {

    // Extract port name:
    commandlineArgs := os.Args
    if (len(commandlineArgs)) < 2 {
        log.Printf("\n" + 
            "ERROR: Specify port name.\n" + 
            "On mac or linux, you can search for available COM ports with 'ls /dev/tty.*'.\n")
        return;
    } else if (len(commandlineArgs) > 2) {
        log.Printf("ERROR: Too many arguments!")
        return;
    }

    comport := commandlineArgs[1]
    log.Printf("Connect to localhost:2441 in your browser to see sensor data.")

    // Start server.
    go initServer()

    // Configure serial port and start listening.
    initSerial(comport)
}

func initServer() {

    var err error

    http.HandleFunc("/websocket", func(w http.ResponseWriter, r *http.Request) {      
        connection, err = upgrader.Upgrade(w, r, nil)
        log.Printf("Made connection to client.")
        handleError(err)
    })

    http.Handle("/", http.FileServer(http.Dir("./static")))
    http.ListenAndServe(":2441", nil)
}

func initSerial(comport string) {
    log.Printf("Setting up serial port %s...", comport)

    c := &serial.Config{Name: comport, Baud: 115200}
    s, err := serial.OpenPort(c)
    handleError(err)

    // buf := make([]byte, 128)
    buf := make([]byte, 1)
    received := make([]byte, 1)

    log.Printf("Listening from serial com...\n" + 
               "CTRL+C to exit program.")

    for {

        _, err := s.Read(buf)
        handleError(err)

        if string(buf[0]) == MESSAGE_BEGIN {
            _, err := s.Read(buf)
            handleError(err)

            for string(buf[0]) != MESSAGE_END {
                received = append(received, buf...)
                // log.Printf("%q", received);
                _, err := s.Read(buf)
                handleError(err)
            }
        }

        // log.Printf("%q", received[1:])

        if (connection != nil) {
            sendSensorInfo(received[1:])
        } 

        received = make([]byte, 1)
    }
}

func sendSensorInfo(received []byte) {
    // log.Printf("%s", received)
    messageSlice := strings.Split(string(received), ",")

    sensorData := SensorData{}

    for _, message := range messageSlice {
        parsed := strings.Split(message, ":")
        // log.Printf("%s", message)
        sensorName := parsed[0]
        if isValidSensorName(sensorName) && len(parsed) == 2 {
            sensorNameField := reflect.ValueOf(&sensorData).Elem().FieldByName(sensorName)
            if sensorName == "Imu" {
                sensorNameField.SetString(parsed[1])
            } else {
                sensorVal, err := strconv.Atoi(parsed[1])
                if err == nil {
                    sensorNameField.SetInt(int64(sensorVal))
                }
            }
            // log.Printf("%s: %i", sensorName, sensorVal)
        } 
    }  

    sensorJson, err := json.Marshal(sensorData)
    handleError(err)

    err = connection.WriteMessage(websocket.TextMessage, sensorJson)
    // log.Printf("Sending sensor info")
    handleError(err)
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