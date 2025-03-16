// Response.cpp
#include "Response.h"

Response::Response(int c, String msg, String d)
    : code(c), message(msg), data(d) {}

String Response::toJson() {
    DynamicJsonDocument doc(1024);
    JsonObject root = doc.to<JsonObject>();
    root["code"] = code;
    root["message"] = message;
    root["data"] = data;

    String output;
    serializeJson(root, output);
    return output;
}

bool Response::fromJson(const String& jsonStr) {
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, jsonStr);

    if (error) {
        return false;
    }

    if (doc.containsKey("code")) {
        code = doc["code"].as<int>(); // 显式转换为int类型
    }
    if (doc.containsKey("message")) {
        message = doc["message"].as<String>();
    }
    if (doc.containsKey("data")) {
        data = doc["data"].as<String>();
    }

    return true;
}

int Response::getCode() const {
    return code;
}

void Response::setCode(int c) {
    code = c;
}

String Response::getMessage() const {
    return message;
}

void Response::setMessage(const String& msg) {
    message = msg;
}

String Response::getData() const {
    return data;
}

void Response::setData(const String& d) {
    data = d;
}
