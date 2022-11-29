//Imani Muhammad-Graham

#include <DNSServer.h>

// Import required libraries
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Stepper.h>


// Network Credentials
const char* ssid = "UB_Devices";
const char* password = "goubbulls";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

//DHT Sensor
#define DHTPIN 27   
#define DHTTYPE    DHT11     
DHT dht(DHTPIN, DHTTYPE);

//Get Temperature
String readDHTTemperature() {
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float t = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (isnan(t)) {    
    Serial.println("readDHTTemperature -> Failed to read from DHT sensor!");
    return "--";
  }
  else {
    Serial.print("readDHTTemperature -> ");
    Serial.println(t);
    return String(t);
  }
}

//Get Humidity
String readDHTHumidity() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("readDHTHumidity ->  Failed to read from DHT sensor!");
    return "--";
  }
  else {
    Serial.print("readDHTHumidity -> ");
    Serial.println(h);
    return String(h);
  }
}

//Light Show Setup
const byte led_pin = 2;
int brightness = 0;
int fade_rate = 5;
int light_show = 0;

//Switch Light shows
String light_show_switch(){  
  Serial.println("Switch Light show");
  brightness = 0;
  fade_rate = 5;
  light_show += 1;
  if (light_show >2){
    //Only 3 light shows (including 0)
    light_show = 0;
  }
  //return current light show
  char ret[256];
  itoa(light_show,ret,10);
  
  Serial.printf("light_show_switch -> Current Light Show: %d\n",light_show);
  return ret;
}

// Light Show 0
void breathe(){
  ledcWrite(0,brightness);
  brightness = brightness + fade_rate;
  if (brightness >= 255 || brightness <= 0) {
    fade_rate = -fade_rate;
  }
}
//Light Show 1
void blink(){
  ledcWrite(0,brightness);
  brightness = brightness + fade_rate*10;
  if (brightness >= 255 || brightness <= 0) {
    fade_rate = -fade_rate;
  }
}
//Light show 2
void pulse(){
  ledcWrite(0,brightness);
  brightness = brightness + fade_rate*5;
  if (brightness >= 255 || brightness <= 0) {
    fade_rate = -fade_rate;
  }
}

//DC Motor Driver Setup
#define IN1 19
#define IN2 18
#define IN3 5
#define IN4 17

//DC Motor Set up
const int stepspr = 2048; //Steps for revolution
int spr = 2048;
int dc_speed = 1; //Motor speed
int max_dc_speed = 15; // Max Speed
Stepper dc_motor(stepspr, IN1, IN3, IN2, IN4);//Instantiate DC Motor

//Motor speed change functionality
String motor_speed(int speed){
  dc_speed += speed;

  //Negative direction
  if (dc_speed < 0){
    //Switch direction
    if (spr > 0){     
      spr = -spr;
    }
    //Speed limit
    if (dc_speed <= -max_dc_speed){
      dc_speed = -max_dc_speed;
    }
  } 

  //Positive direction
  if (dc_speed >=0){    
    //Switch Direction
    if (spr <0){
      spr = -spr;
    }

    //Speed Limit
    if (dc_speed >= max_dc_speed) {
      dc_speed = max_dc_speed;
    }
  } 
  if (dc_speed != 0) {
   dc_motor.setSpeed(abs(dc_speed));
  }

  Serial.printf("motor_speed -> Current Speed: %d\n",dc_speed);
  char ret[256];
  itoa(dc_speed,ret,10);
  return(ret);
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     
    }
    h2 { 
      font-size: 3.0rem;
      text-align: center;
    }
    h3 {
      font-size: 2.5rem;
      text-align: center;
    }
    h4 {
      font-size: 1rem;
      text-align: left;
    }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
  </style>
</head>
<body>
  <h2>Imani's IOT Project</h2>

  <div class = "About">  
    <h3> About </h3>
  </div>

  <div class = "Sensors">
    <h3> Sensors </h3>
    <h4> Temperature </h4>
    <span id="temperature">%TEMPERATURE%</span>
    <sup class="units">&deg;C</sup>
    
    <h4> Humidity </h4>
    <span id="humidity">%HUMIDITY%</span>
      <sup class="units">&percnt;</sup>
    </p>
  </div>
  
  <div class = "Actuators">    
    <h3> Actuators </h3>

    <h4> Light show </h4>
    <span id="light_show"> %Choose a Light Show% </span>
    <button onclick = "handler(0)"> Switch Light Show </button>  

    <h4> Fan </h4>
    <span id="motor"> %Motor Speed% </span>
    <button onclick = "handler(1)"> Speed up Fan </button>  
    <button onclick = "handler(2)"> Slow down Fan </button>  
  </div>

</body>
<script>  
  function handler(n){
    //Light Show
    if (n == 0) {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            document.getElementById("light_show").innerHTML = "Light Show " +this.responseText;
          }
      };
      xhttp.open("GET", "/light_show", true);
      xhttp.send();
    }
    //Motor Speed
    else if (n == 1){
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            document.getElementById("motor").innerHTML = "Motor Speed " + this.responseText;
            if (this.responseText == "15") {
              alert("Motor has reached max speed");
            }
          }
      };
      xhttp.open("GET", "/motor_speed", true);
      xhttp.send();
    }
    //Motor Slow
    else if (n == 2){
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            document.getElementById("motor").innerHTML = "Motor Speed " + this.responseText;
            if (this.responseText == "-15") {
              alert("Motor has reached max speed");
            }
          }
      };
      xhttp.open("GET", "/motor_slow", true);
      xhttp.send();        
    }
    else{
      alert("Incorrect Input")
    }
  }

  setInterval(function ( ) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        document.getElementById("temperature").innerHTML = this.responseText;
      }
    };
    xhttp.open("GET", "/temperature", true);
    xhttp.send();
  }, 10000 ) ;

  setInterval(function ( ) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        document.getElementById("humidity").innerHTML = this.responseText;
      }
    };
    xhttp.open("GET", "/humidity", true);
    xhttp.send();
  }, 10000 ) ;

</script>
</html>)rawliteral";

// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return readDHTTemperature();
  }
  else if(var == "HUMIDITY"){
    return readDHTHumidity();
  }
  return String();
}

void setup() {
  // Setup LEDs
    ledcAttachPin (led_pin, 0);
    ledcSetup (0,5000,8);

  // Serial port for debugging purposes
    Serial.begin(115200);
  //Set up Sensors
    dht.begin();

  // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi ..");
    }
    Serial.print("We're in boys... find me at ");
    Serial.println(WiFi.localIP());
  
  // SERVER FUNCTIONALITY
    //PAGES
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", index_html, processor);
    });
    // FEATURES
    server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/plain", readDHTTemperature().c_str());
    });
    server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/plain", readDHTHumidity().c_str());
    });
    server.on("/light_show", HTTP_GET, [](AsyncWebServerRequest *request){
             
        request->send_P(200, "text/plain", light_show_switch().c_str());
    });
    server.on("/motor_slow", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/plain", motor_speed(-1).c_str());
    });
    server.on("/motor_speed", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/plain", motor_speed(1).c_str());
    });

    // Start server
    server.begin();
}



void loop(){
  // Light Show
  if (light_show == 0){breathe();}
  else if (light_show == 1){blink();}
  else{pulse();}

  //Motor 
  if (dc_speed != 0){dc_motor.step(spr);}

  delay(25);
}