#include "Response.h"
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <map>
#include <string>
#include <pins_arduino.h>
#include <U8g2lib.h> 
#include "Screen.h"


U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, SCL, SDA, U8X8_PIN_NONE);
Screen screen(u8g2);

String httpRequest(String query);
ESP8266WiFiMulti wifiMulti; // 建立ESP8266WiFiMulti对象,对象名称是'wifiMulti'
ESP8266WebServer server(80);// 建立ESP8266WebServer对象，对象名称为server
                            // 括号中的数字是网路服务器响应http请求的端口号
                            // 网络服务器标准http端口号为80，因此这里使用80为端口号
const char* ssid = "Mei'sHome";
const char* password = "MEImei666";
const String LLM_URL = "http://192.168.3.7:8000/";
const String CONTENT_TYPE = "application/json; charset=utf-8";
                                 
String httpRequest(String query){
    WiFiClient client;
    HTTPClient http;
    http.setTimeout(30000);
    http.setReuse(false);
    http.begin(client, LLM_URL);
    http.addHeader("Content-Type", CONTENT_TYPE);
    const size_t capacity = JSON_OBJECT_SIZE(1);
    StaticJsonDocument<capacity> doc;
    doc["query"] =  query;
    String requestBody;
    serializeJson(doc, requestBody);
    int httpCode = http.POST(requestBody);
    String result = "error:request_failed";
    if (httpCode == HTTP_CODE_OK) {
      result = http.getString();
      Serial.printf("LLM结果详情: %s\n", result.c_str());
    } else {
      result = "error:http_" + String(httpCode);
      Serial.printf("LLM错误详情: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
    return result;
};

void chat(){
  Response res = Response();
    if (server.method() == HTTP_GET && server.hasArg("query")) {
        String query = server.arg("query");
        String resJson = httpRequest(query);
        res.fromJson(resJson);
    }
    // 更新OLED屏幕内容
    screen.setContent(res.getData().c_str());
    // 返回响应
    server.send(200, CONTENT_TYPE, res.toJson());
}

void handleNotFound(){         
  Response res = Response(404, "Not Found");
  server.send(404, CONTENT_TYPE, res.toJson());
}

void initWIFI(){
  wifiMulti.addAP(ssid, password);  // 这三条语句通过调用函数addAP来记录3个不同的WiFi网络信息。
  int i = 0;                                 
  while (wifiMulti.run() != WL_CONNECTED) {  // 此处的wifiMulti.run()是重点。通过wifiMulti.run()，NodeMCU将会在当前
    delay(1000);                             // 环境中搜索addAP函数所存储的WiFi。如果搜到多个存储的WiFi那么NodeMCU
    Serial.print(i++); Serial.print(' ');    // 将会连接信号最强的那一个WiFi信号。
  }
  Serial.println('\n');                     // WiFi连接成功后
  Serial.print("Connected to ");            // NodeMCU将通过串口监视器输出。
  Serial.println(WiFi.SSID());              // 连接的WiFI名称
  Serial.print("IP address:\t");            // 以及
  Serial.println(WiFi.localIP());           // NodeMCU的IP地址
};

void initServer(){
  server.begin();                 
  server.on("/chat", chat);
  server.onNotFound(handleNotFound);        
  Serial.println("HTTP server started");
}


void setup(void){
  u8g2.begin();

  Serial.begin(9600);          // 启动串口通讯
  initWIFI();                  // 初始化WiFi
  initServer();                // 初始化HTTP服务器

  screen.initialize();
  screen.setContent("你好...");
}
 
void loop(void){
  server.handleClient();     // 处理http服务器访问
  screen.render();
  delay(3000);
  screen.nextPage();
}

