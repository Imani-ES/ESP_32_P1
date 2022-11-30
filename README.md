# ESP_32_P1
Playing with ESP32 (using wifi)

Imani Muhammad-Graham
Project Purpose
  
  Implement a smart system (mini smart home).
  Utilize sensor(s) and actuator(s) and access/ control system elements remotely via client/ server architecture.

Features

  A website allowing:
    Remote Temperature and Humidity sensing
    Motor speed and direction control
    LED blink pattern control

Functions/ Modules

  String readDHTTemperature() -> Get temperature value from DHT, return as string

  String readDHTHumidity() -> Get Humidity Value from DHT, return as string

  light_show_switch() -> Increment light blinking pattern based on 'int light_show'    
  return current 'int light_show' as string 
    Different light blinking pattern functions selected by light_show_switch() that uses 'int brightness' and 'int fade_rate'
    breathe()
    blink()
    pulse()

  motor_speed(int speed) -> Takes in speed value, and adds it to 'int dc_speed' until abs(dc_speed) == 'max_dc_speed'. 
  Toggles 'int spr' (steps per revolution) to reflect cw & ccw rotations.
  Returns 'int dc_speed' as string  
  

Sources

  https://randomnerdtutorials.com/esp32-dht11-dht22-temperature-humidity-web-server-arduino-ide/
    How to utilize ESPAsyncWebServer
    Setting up DHT
  https://randomnerdtutorials.com/esp32-stepper-motor-28byj-48-uln2003/
    Setting up Stepper motor
