// Screen.h
#pragma once
#include <U8g2lib.h>
#include <cstring>

class Screen {
private:
    U8G2_SSD1306_128X64_NONAME_F_SW_I2C& display;
    
    // 显示参数（参考网页2硬件配置）
    static const int SCREEN_WIDTH = 128;
    static const int FONT_HEIGHT = 16;
    static const int LINE_SPACING = 4;
    
    // 动态参数（根据网页3的优化建议）
    int maxCharsPerLine = 0;
    int charWidth = 12;
    int* charPositions = nullptr;
    int totalChars = 0;
    int totalPages = 0;
    int currentPage = 0;
    char* content = nullptr;

    int getUTF8CharLength(uint8_t c);
    void precomputeCharPositions();

public:
    Screen(U8G2_SSD1306_128X64_NONAME_F_SW_I2C& u8g2);
    ~Screen();
    
    void initialize();  // 初始化方法（参考网页2的begin()规范）
    void setContent(const char* text);
    void render();      // 对应原displayCurrentPage功能
    void nextPage();
};

