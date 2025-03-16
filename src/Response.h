// Response.h
#pragma once
#include <ArduinoJson.h>
#include <WString.h>

class Response {

private:
    int code;
    String message;
    String data;

public:
    Response(int c = 0, String msg = "", String d = "");
    
    String toJson();
    bool fromJson(const String& jsonStr);
    
    // Getter/Setter 方法
    int getCode() const;
    void setCode(int c);
    String getMessage() const;
    void setMessage(const String& msg);
    String getData() const;
    void setData(const String& d);
    
};


