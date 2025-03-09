#include <ESP8266WiFi.h>        // 本程序使用 ESP8266WiFi库
#include <ESP8266WiFiMulti.h>   //  ESP8266WiFiMulti库
#include <ESP8266WebServer.h>   //  ESP8266WebServer库
#define LED_PIN 2  // 对应GPIO2（D4）

 
ESP8266WiFiMulti wifiMulti;     // 建立ESP8266WiFiMulti对象,对象名称是'wifiMulti'
 
ESP8266WebServer server(80);// 建立ESP8266WebServer对象，对象名称为server
                                    // 括号中的数字是网路服务器响应http请求的端口号
                                    // 网络服务器标准http端口号为80，因此这里使用80为端口号
 
                                                                           
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


// 设置处理404情况的函数'handleNotFound'
void handleNotFound(){                                 // 当浏览器请求的网络资源无法在服务器找到时，
  server.send(404, "text/plain", "404: Not found");   // NodeMCU将调用此函数。
}

void setup(void){
  pinMode(LED_PIN, OUTPUT);
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
  server.on("/switch_on", switch_on);
  server.on("/switch_off", switch_off);  
  server.onNotFound(handleNotFound);        
//--------"启动网络服务功能"程序部分结束--------
  Serial.println("HTTP server started");//  告知用户ESP8266网络服务功能已经启动
}
 
void loop(void){
  server.handleClient();     // 处理http服务器访问
}

