



```C++                                                       
void handleRoot() {   //处理网站根目录“/”的访问请求
 // 检查HTTP请求的方法
  if (server.method() == HTTP_GET) {
    // 这是GET请求
    String totalParams = "";
    if (server.args()) {
        // 遍历所有参数
        for (uint8_t i = 0; i < server.args(); i++) {
          String argument = server.argName(i);
          String value = server.arg(i);
          totalParams =  totalParams + ("Parameter " + String(i) + ": " + argument + " = " + value);
        }
        Serial.print(totalParams);
        server.send(200, "text/plain", totalParams);
    }
  } else if (server.method() == HTTP_POST) {
    // 这是POST请求
    if (server.args() > 0) {
        String post_data = "";
        // 读取所有数据
        for (uint8_t i = 0; i < server.args(); i++) {
          String argument = server.argName(i);
          if (server.arg(i).length() > 0) {
            // 如果参数有值，将其添加到 post_data
            post_data += server.arg(i);
          }
        }
        // 发送 POST 数据的长度
        server.send(200, "text/plain", "POST data received:");
        server.sendContent(post_data);
    }
  } else {
    // 其他类型的请求
    server.send(405, "Method Not Allowed", "This server does not support this method");
  }
}
```


```C++
void switch_on() {
  if (server.method() == HTTP_GET) {
        digitalWrite(LED_PIN, HIGH);  
        server.send(200, "text/plain", "open");
    }
}

void switch_off() {
    if (server.method() == HTTP_GET) {
          digitalWrite(LED_PIN, LOW);
          server.send(200, "text/plain", "close");
      }
}
```




```C++
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
```







```C++
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>

ESP8266WiFiMulti wifiMulti;
WiFiClient client;

void setup() {
  Serial.begin(9600);
  delay(10);

  // 修改为你的WiFi信息
  wifiMulti.addAP("Mei'sHome", "MEImei666");
  
  Serial.println();
  Serial.print("等待WiFi连接...");
  
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println("");
  Serial.println("WiFi已连接");
  Serial.print("IP地址: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // 修改为你的服务器实际IP和端口
  const char* host = "192.168.3.7";  // 将0.0.0.0替换为实际IP
  const int port = 8000;
  
  Serial.print("正在连接: ");
  Serial.println(host);

  // 连接服务器
  if (!client.connect(host, port)) {
    Serial.println("连接服务器失败");
    delay(5000);
    return;
  }

  // 发送GET请求
  String url = "/?query=ok";
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

  // 等待响应
  while (client.connected() && !client.available()) delay(1);

  // 跳过HTTP响应头
  String header;
  do {
    header = client.readStringUntil('\r');
    Serial.print(header);
  } while (header != "\n" && client.available());

  // 解析JSON
  String payload = client.readString();
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, payload);

  if (error) {
    Serial.print("JSON解析失败: ");
    Serial.println(error.c_str());
    return;
  }

  // 提取data字段
  const char* data = doc["data"];
  Serial.print("解析到的数据: ");
  Serial.println(data);

  client.stop();
  delay(50000);  // 50秒后重新请求
}
```
