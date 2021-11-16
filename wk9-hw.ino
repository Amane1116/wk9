#include "WiFi.h"
#include "WebServer.h"
#define LED_BUILTIN 13,14

// SSID & Password
const char* ssid = "Amane";
const char* password = "sophia030";

WebServer server(80);  // Object of WebServer(HTTP port, 80 is defult)

void setup() {
  Serial.begin(115200);
  Serial.println("Try Connecting to ");
  Serial.println(ssid);

  // Connect to your wi-fi modem
  WiFi.begin(ssid, password);

  // Check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
    pinMode(13, OUTPUT);
    pinMode(14, OUTPUT);
    ledcSetup(10,5000,10);
    ledcAttachPin(14,10);
  }
  Serial.println("");
  Serial.println("WiFi connected successfully");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP());  //Show ESP32 IP on serial

  //建立server的路徑
  createWebServer();
  
  //server啟動
  server.begin();
  Serial.println("HTTP server started");
 
}


void loop() {
  server.handleClient();  //handleClient要寫在loop
}


//----設定HTML資料----
String htmlContent = "\
  <!DOCTYPE html>\
   <html>\
    <head>\
      <style>\
        body{ display: flex; flex-direction: column;align-items:center;justify-content:center;}\
         a { background: #E6CAFF; width: 120px; height: 40px; margin: 20px; font-size: 20pt;font-family:Arial; text-decoration: none;}\
      </style>\
    </head>\
    <body>\
      <a href='/led/on'>LED ON</a>\
      <a href='/led/off'>LED OFF</a>\
      <form method='get' action='/led/setLightBrightness'>\
        <label>Led Brightness</label>\
        <input type='number' name='lightBrightness'><input type='submit'>\
      </form>\
    </body>\
  </html>";

String LEDOpenPage = "\
  <!DOCTYPE html>\
   <html>\
    <head>\
      <style>\
        body{ display: flex; flex-direction: column;align-items:center;}\
         a { background: #02DF82; width: 150px; height: 50px; margin: 20px; font-size: 30pt; text-decoration: none;}\
      </style>\
    </head>\
    <body>\
      <h1>LED OPEN</h1>\
      <a href='/'>Back</a>\
    </body>\
  </html>";

String LEDCLOSEPage = "\
  <!DOCTYPE html>\
   <html>\
    <head>\
      <style>\
        body{ display: flex; flex-direction: column;align-items:center;}\
        a { background: #FF5151; width: 150px; height: 50px; margin: 20px; font-size: 30pt; text-decoration: none;}\
      </style>\
    </head>\
    <body>\
      <h1>LED CLOSE</h1>\
      <a href='/'>Back</a>\
    </body>\
  </html>";
  
//------設定路徑------
void createWebServer()
{
  //初始頁面
  server.on("/", []() {
      server.send(200, "text/html", htmlContent);
  });
  server.on("/led/on", []() {
    Serial.println("LED On");
    
    //add your code here
    digitalWrite(13,HIGH);
    
    String response = "{\"LED On Success\"}";
    server.send(200, "text/html", LEDOpenPage);
  });
  server.on("/led/off", []() {
    Serial.println("LED Off");
    
    //add your code here
    digitalWrite(13,LOW);
    String response = "{\"LED Off Success\"}";
    server.send(200, "text/html", LEDCLOSEPage);
  });
  server.on("/led/setLightBrightness", []() {
    //以server.arg&引數名稱，取得form傳來的資料
    int lightBrightness = server.arg("lightBrightness").toInt();
    //判斷條件    
    if(lightBrightness>=0 && lightBrightness<=1024){
      Serial.print("Set Brightness: ");
      Serial.println(lightBrightness);
      //digitalWrite(14,HIGH);
      
      //add your code here
      ledcWrite(10,lightBrightness);
      String response = "Set Brightness: " + (String)lightBrightness;
      server.send(200, "application/json", response);
    } 
    else {
      String response = "{\"Error\":\"over range 0-1024\"}";
      server.send(404, "application/json", response);
    }
  });
}
