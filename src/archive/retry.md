



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
#include <ESP8266WiFi.h>        // 本程序使用 ESP8266WiFi库
#include <ESP8266WiFiMulti.h>   //  ESP8266WiFiMulti库
#include <ESP8266WebServer.h>   //  ESP8266WebServer库
#include <map>
 
ESP8266WiFiMulti wifiMulti;     // 建立ESP8266WiFiMulti对象,对象名称是'wifiMulti'
 
ESP8266WebServer server(80);// 建立ESP8266WebServer对象，对象名称为server
                                    // 括号中的数字是网路服务器响应http请求的端口号
                                    // 网络服务器标准http端口号为80，因此这里使用80为端口号

String answer = "";  // 改为全局变量存储解析结果

void parseInfo(WiFiClient &client) {
    // 步骤1：流式解析替代完整读取
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, client);  // 直接解析网络流‌:ml-citation{ref="2" data="citationList"}
    
    // 步骤2：错误处理优化
    if (error) {
        Serial.println("JSON解析失败");
        return;
    }

    // 步骤3：字段提取改进
    if (!doc["data"].isNull()) {
       answer = doc["data"].as<String>();
        Serial.printf("解析结果: %s\n", answer.c_str());
    } else {
        Serial.println("JSON中无data字段");
    }
}
                                    

// 向服务器请求信息并对信息进行解析
void httpRequest(String query){
  WiFiClient client;
  String host = "192.168.3.7";
  String url ="/?query=" + query;
  String httpRequest = String("GET ") + url + " HTTP/1.1\r\n" + 
                              "Host: " + host + "\r\n" + 
                              "Connection: close\r\n\r\n";
   
  Serial.print("Connecting to "); Serial.print(host);
  if (client.connect(host, 8000)){
    Serial.println(" Success!");
    // 向服务器发送http请求信息
    client.print(httpRequest);
    Serial.println("Sending request: ");
    Serial.println(httpRequest);  
    // 获取并显示服务器响应状态行 
    String status_response = client.readStringUntil('\n');
    Serial.print("status_response: ");
    Serial.println(status_response);
    // 使用find跳过HTTP响应头
    if (client.find("\r\n\r\n")) {
      Serial.println("Found Header End. Start Parsing.");
    }
    parseInfo(client); 
  }
  else {
    Serial.println(" connection failed!");
  }   
  //断开客户端与服务器连接工作
  client.stop(); 
}



void chat(){ 
  if (server.method() == HTTP_GET) {
    std::map<String, String> paramMap;
    if (server.args()) {
        for (uint8_t i = 0; i < server.args(); i++) {
          String argument = server.argName(i);
          String value = server.arg(i);
          paramMap[argument] = value;  
        }
        bool hasQuery = (paramMap.find("query") != paramMap.end());
        String queryValue = "";
      if(hasQuery) {
          queryValue = paramMap["query"];
          httpRequest(queryValue);
      }
        server.send(200, "text/plain", answer);
    }
}
}

// 设置处理404情况的函数'handleNotFound'
void handleNotFound(){                                 // 当浏览器请求的网络资源无法在服务器找到时，
  server.send(404, "text/plain", "404: Not found");   // NodeMCU将调用此函数。
}

void setup(void){
  Serial.begin(9600);          // 启动串口通讯
 
  //通过addAp函数存储  WiFi名称       WiFi密码
  wifiMulti.addAP("Mei'sHome", "MEImei666");  // 这三条语句通过调用函数addAP来记录3个不同的WiFi网络信息。
  wifiMulti.addAP("taichi-maker2", "87654321"); // 这3个WiFi网络名称分别是taichi-maker, taichi-maker2, taichi-maker3。
  wifiMulti.addAP("taichi-maker3", "13572468"); // 这3个网络的密码分别是123456789，87654321，13572468。
                                                // 此处WiFi信息只是示例，请在使用时将需要连接的WiFi信息填入相应位置。
                                                // 另外这里只存储了3个WiFi信息，您可以存储更多的WiFi信息在此处。
 
  int i = 0;                                 
  while (wifiMulti.run() != WL_CONNECTED) {  // 此处的wifiMulti.run()是重点。通过wifiMulti.run()，NodeMCU将会在当前
    delay(1000);                             // 环境中搜索addAP函数所存储的WiFi。如果搜到多个存储的WiFi那么NodeMCU
    Serial.print(i++); Serial.print(' ');    // 将会连接信号最强的那一个WiFi信号。
  }
  Serial.print("#############################");        
                                    // 一旦连接WiFI成功，wifiMulti.run()将会返回“WL_CONNECTED”。这也是
                                             // 此处while循环判断是否跳出循环的条件。
 
  // WiFi连接成功后将通过串口监视器输出连接成功信息 
  Serial.println('\n');                     // WiFi连接成功后
  Serial.print("Connected to ");            // NodeMCU将通过串口监视器输出。
  Serial.println(WiFi.SSID());              // 连接的WiFI名称
  Serial.print("IP address:\t");            // 以及
  Serial.println(WiFi.localIP());           // NodeMCU的IP地址
  
//--------"启动网络服务功能"程序部分开始-------- //  此部分为程序为本示例程序重点1
  server.begin();                 
  server.on("/chat", chat);
  server.onNotFound(handleNotFound);        
//--------"启动网络服务功能"程序部分结束--------
  Serial.println("HTTP server started");//  告知用户ESP8266网络服务功能已经启动
}
 
void loop(void){
  server.handleClient();     // 处理http服务器访问
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
