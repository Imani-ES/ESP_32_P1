#include <DNSServer.h>

// Import required libraries
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>

// Network Credentials
const char* ssid = "UB_Devices";
const char* password = "goubbulls";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

//DHT Sensor
#define DHTPIN 27   
#define DHTTYPE    DHT11     
DHT dht(DHTPIN, DHTTYPE);

//Light Show
const byte led_pin = 2;
int brightness = 0;
int fade_rate = 5;
int light_show = 0;

String readDHTTemperature() {
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float t = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (isnan(t)) {    
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else {
    Serial.println(t);
    return String(t);
  }
}

String readDHTHumidity() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else {
    Serial.println(h);
    return String(h);
  }
}

const char sensors_html[] PROGMEM = R"rawliteral(
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
      text-align: left;
    }
    h4 {
      font-size: 2rem;
      text-align: left;
    }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>Imani's IOT Project</h2>

  <div class = "About">  
    <h3> About </h3>
  </div>

  <div class = "Sensors">
    <h3> Sensors </h3>
    <h4> DHT </h4>
    <p>
      <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
      <span class="dht-labels">Temperature</span> 
      <span id="temperature">%TEMPERATURE%</span>
      <sup class="units">&deg;C</sup>
    </p>
    <p>
      <i class="fas fa-tint" style="color:#00add6;"></i> 
      <span class="dht-labels">Humidity</span>
      <span id="humidity">%HUMIDITY%</span>
      <sup class="units">&percnt;</sup>
    </p>
  </div>
  
  <div class = "Actuators">    
    <h3> Actuators </h3>
    <h4> Current Light show </h4>
    <span id="light_show"> Choose a Light Show </span>
    <button onclick = "light_show_1()"> Light Show 1 </button>  
    <button onclick = "light_show_2()"> Light Show 2 </button>  
  </div>

</body>
<script>
  var 
  function light_show_1(){
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          if (this.responseText == "Success"){
            document.getElementById("light_show").innerHTML = this.responseText;
          }
        }
    };
    xhttp.open("GET", "/light_show_1", true);
    xhttp.send();
  }

  function light_show_2(){
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          if (this.responseText == "Success"){
            document.getElementById("light_show").innerHTML = this.responseText;
          }
        }
    };
    xhttp.open("GET", "/light_show_2", true);
    xhttp.send();
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

void setup(){
// Setup LEDs
    ledcAttachPin (led_pin, 0);
    ledcSetup (0,5000,8);

// Serial port for debugging purposes
    Serial.begin(115200);

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
    server.on("/about", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", about_html, processor);
    });
    server.on("/sensors", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", sensors_html, processor);
    });
    server.on("/actuators", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", actuators_html, processor);
    });

    // FEATURES
    server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/plain", readDHTTemperature().c_str());
    });
    server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/plain", readDHTHumidity().c_str());
    });
    server.on("/light_show_1", HTTP_GET, [](AsyncWebServerRequest *request){
        if (light_show_1() == 1){          
          request->send_P(200, "text/plain", "Success");
        }
        else {          
          request->send_P(200, "text/plain", "Failed");
        }
    });
    server.on("/light_show_2", HTTP_GET, [](AsyncWebServerRequest *request){
        if (light_show_2() == 1){          
          request->send_P(200, "text/plain", "Success");
        }
        else {          
          request->send_P(200, "text/plain", "Failed");
        }
        request->send_P(200, "text/plain", "Light Show 2");
    });

    // Start server
    server.begin();
}

int light_show_1(){
  //turn to light show 1
  Serial.println("Switched to Light show 1");
  light_show = 1;
  return 1;
}

int light_show_2(){
  //turn to light show 2  
  light_show = 2;
  Serial.println("Switched to Light show 2");
  return 1;
}

void breathe(){
  // Light Show 0
  ledcWrite(0,brightness);
  brightness = brightness + fade_rate;
  if (brightness >= 255 || brightness <= 0) {
    fade_rate = -fade_rate;
  }
}

void blink(){
  //Light Show 1
  ledcWrite(0,brightness);
  brightness = brightness + fade_rate*10;
  if (brightness >= 255 || brightness <= 0) {
    fade_rate = -fade_rate;
  }
}

void pulse(){
  //Light show 2
  ledcWrite(0,brightness);
  brightness = brightness + fade_rate*5;
  if (brightness >= 255 || brightness <= 0) {
    fade_rate = -fade_rate;
  }
}

void loop(){
  // Light Show
  if (light_show == 0){breathe();}
  else if (light_show == 1){blink();}
  else{pulse();}
  Serial.print("Find me here: ");
  Serial.println(WiFi.localIP());
  delay(50);
}