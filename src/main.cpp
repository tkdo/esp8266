#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

ESP8266WiFiMulti WiFiMulti;
ESP8266WebServer server(80);

const char* ssid = "Mei'sHome";       // 修改为你的WiFi名称
const char* password = "MEImei666";  // 修改为你的WiFi密码

void setup() {
  Serial.begin(9600);
  delay(10);

  // 连接WiFi
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);
  
  Serial.print("Connecting to WiFi");
  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected! IP address: ");
  Serial.println(WiFi.localIP());
  Serial.printf("网关IP: %s\n", WiFi.gatewayIP().toString().c_str());
  Serial.printf("DNS IP: %s\n", WiFi.dnsIP().toString().c_str());
}

void loop() {
  if (WiFiMulti.run() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    http.setTimeout(5000000);
    http.setReuse(false);   // 禁用连接复用
    // 设置请求地址
    http.begin(client, "http://192.168.3.7:8000/");
    http.addHeader("Content-Type", "application/json");
  
    // 构建JSON请求体
    const size_t capacity = JSON_OBJECT_SIZE(1);
    StaticJsonDocument<capacity> doc;
    doc["query"] = "ok";
    String requestBody;
    serializeJson(doc, requestBody);

    // 发送POST请求
    int httpCode = http.POST(requestBody);

    // 处理响应
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      
      // 解析JSON响应
      DynamicJsonDocument resDoc(1024);
      deserializeJson(resDoc, payload);

      if (resDoc["code"] == 1) {
        const char* data = resDoc["data"];
        Serial.print("Received data: ");
        Serial.println(data);
      } else {
      Serial.println("Server returned error");
      }
    } else {
      Serial.printf("HTTP错误详情: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
  
  delay(50000); // 每5秒发送一次请求
}
