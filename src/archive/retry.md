



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
