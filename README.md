# &nbsp; SmartTrain[<img src="https://fiware.github.io/tutorials.IoT-Agent/img/fiware.png" align="left" width="162">](https://www.fiware.org/)<br/>

[![FIWARE IoT Agents](https://nexus.lab.fiware.org/repository/raw/public/badges/chapters/iot-agents.svg)](https://github.com/FIWARE/catalogue/blob/master/iot-agents/README.md)
[![License: MIT](https://img.shields.io/github/license/fiware/tutorials.Iot-Agent.svg)](https://opensource.org/licenses/MIT)
[![Support badge](https://img.shields.io/badge/tag-fiware-orange.svg?logo=stackoverflow)](https://stackoverflow.com/questions/tagged/fiware)

The SmartTrain is a device created to be implemented in the Fiware Lego Model SmartWorld. Here in this document there is information about how the train works and how to use it.

## Materials:
- Lego Train 60197
- ESP32 D1 Mini
- SX1308 DC-DC Stepup-Module
- 2 $\times$ 0.1uF - (104) Ceramic Capacitor
- 1kΩ Resistor
- Hall Effect Sensor AH661

## Dependency:
The code depends on the library that drives the lego powerup block using bluetooth: `Legoino`<br>
https://github.com/corneliusmunz/legoino


## Diagram:
![](download.png)

## API
The train is doing two things simultaneously. 
It is sending its current location and waiting for commands.

## Location:
There are magnets on the tracks. 
Based on the detection of said magnets the train is able to infer its position and send it to the Context Broker

## Commands:
To recieve commands the train listens for HTTP requests. <br>
The format is `http://TrainIP?<command>{value}`

### Set Speed:
GET `/s{speed}` <br>

Sets the speed of the train, <br>
speed : `int` , [-100, 100] <br>

example : `http://ipAddr/s/90`
___
### Set Color
GET `/c{color}` <br>

Sets the color of the train hub <br>
color : `string` : [<br>
BLACK, PINK, PURBLE,<br>
    BLUE, LIGHTBLUE, CYAN,<br>
    GREEN, YELLOW, ORANGE,<br>
    RED, WHITE, NONE ]<br>

example : `http://ipAddr/cGREEN` <br>