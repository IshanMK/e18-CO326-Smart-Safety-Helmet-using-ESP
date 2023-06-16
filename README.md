___
Smart Safety Helmet using ESP
___

## CO326 - Project
### Group 11
* E/18/028 - Ariywansha P.H.J.U.
* E/18/173 - Kasthuripitiya K.A.I.M.K.
* E/18/285 - Ranasinghe S.M.T.S.C.

### Introduction
Supervising and monitoring every worker individually can be challenging in large-scale construction sites. We are developing a Smart helmet for workers and a web dashboard for supervisors to solve this scenario. The smart helmet is a safety helmet that has more additional features than the standard safety helmet. It can calculate absolute time temperature, background noises, and harmful gas levels by its sensors. The MQ9 gas sensor, thermistor, and KY-037 sound detection sensor ar those sensors. There are two actuators in the helmet. Those are LED and Piezo Buzzer. If there is any harmful noise or temperature, these actuators notify the user. All of those sensors and actuators are connected to the ESP32 module. Data is collected from the sensors in the helmet and sent to the server via the MQTT broker. Depending on the data received from the server, actuators can notify the worker about the critical environment around it

