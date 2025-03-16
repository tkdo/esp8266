#include <ArduinoJson.h>
#include <ESP8266WiFi.h>        // 本程序使用 ESP8266WiFi库
#include <ESP8266WebServer.h>   //  ESP8266WebServer库
#include <map>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

ESP8266WiFiMulti WiFiMulti;

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
  // WiFiClient client;
  // String host = "192.168.3.7";
  // String url ="/?query=" + query;
  // String httpRequest = String("GET ") + url + " HTTP/1.1\r\n" + 
  //                             "Host: " + host + "\r\n" + 
  //                             "Connection: close\r\n\r\n";
   
  // Serial.print("Connecting to "); Serial.print(host);
  // if (client.connect(host, 8000)){
  //   Serial.println(" Success!");
  //   // 向服务器发送http请求信息
  //   client.print(httpRequest);
  //   Serial.println("Sending request: ");
  //   Serial.println(httpRequest);  
  //   // 获取并显示服务器响应状态行 
  //   String status_response = client.readStringUntil('\n');
  //   Serial.print("status_response: ");
  //   Serial.println(status_response);
  //   // 使用find跳过HTTP响应头
  //   if (client.find("\r\n\r\n")) {
  //     Serial.println("Found Header End. Start Parsing.");
  //   }
  //   parseInfo(client); 
  // }
  // else {
  //   Serial.println(" connection failed!");
  // }   
  // //断开客户端与服务器连接工作
  // client.stop(); 
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
        answer = data;
        Serial.println(data);
      } else {
      Serial.println("Server returned error");
      }
    } else {
      Serial.printf("HTTP错误详情: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }
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
