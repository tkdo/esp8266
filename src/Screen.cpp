// Screen.cpp
#include "Screen.h"

Screen::Screen(U8G2_SSD1306_128X64_NONAME_F_SW_I2C& u8g2) 
    : display(u8g2) {
    charPositions = nullptr;
    content = nullptr;
}

Screen::~Screen() {
    delete[] charPositions;
    delete[] content;
}

void Screen::initialize() {
    display.begin();
    display.setFont(u8g2_font_wqy14_t_gb2312); // 中文支持（参考网页1）
    charWidth = display.getUTF8Width("汉");     // 动态测量（网页3优化建议）
    maxCharsPerLine = SCREEN_WIDTH / charWidth;
}

void Screen::setContent(const char* text) {
    // 深拷贝文本（网页4字符串处理规范）
    delete[] content;
    content = new char[strlen(text)+1];
    strcpy(content, text);
    
    precomputeCharPositions();
}

int Screen::getUTF8CharLength(uint8_t c) {
    if (c < 0x80) return 1;
    else if ((c & 0xE0) == 0xC0) return 2;
    else if ((c & 0xF0) == 0xE0) return 3;
    return 1;
}

void Screen::precomputeCharPositions() {
    const char *p = content;
    totalChars = 0;
    while(*p) {
        int len = getUTF8CharLength(*p);
        p += len;
        totalChars++;
    }

    delete[] charPositions;
    charPositions = new int[totalChars + 1];
    
    p = content;
    int index = 0;
    while(*p && index < totalChars) {
        charPositions[index] = p - content;
        int len = getUTF8CharLength(*p);
        p += len;
        index++;
    }
    charPositions[totalChars] = p - content;
    
    totalPages = (totalChars + (maxCharsPerLine*3 - 1)) / (maxCharsPerLine*3);
}

void Screen::render() {
    display.clearBuffer();
    int currentPos = currentPage * maxCharsPerLine * 3;
    int yPos = FONT_HEIGHT;

    for(int line=0; line<3; line++){
        int lineCharCount = 0;
        int linePixelWidth = 0;
        int lineStart = currentPos;
        
        while(currentPos < totalChars && lineCharCount < maxCharsPerLine) {
            int charLen = charPositions[currentPos+1] - charPositions[currentPos];
            const char* pChar = content + charPositions[currentPos];
            
            char temp[charLen+1];
            strncpy(temp, pChar, charLen);
            temp[charLen] = '\0';
            
            int w = display.getUTF8Width(temp);
            if(linePixelWidth + w > SCREEN_WIDTH) break;
            
            linePixelWidth += w;
            lineCharCount++;
            currentPos++;
        }

        if(lineCharCount > 0) {
            int startByte = charPositions[lineStart];
            int endByte = charPositions[lineStart + lineCharCount];
            
            char lineBuffer[endByte - startByte + 1];
            strncpy(lineBuffer, content + startByte, endByte - startByte);
            lineBuffer[endByte - startByte] = '\0';
            
            int xPos = (SCREEN_WIDTH - display.getUTF8Width(lineBuffer)) / 2;
            display.drawUTF8(xPos, yPos, lineBuffer);
            yPos += FONT_HEIGHT + LINE_SPACING;
        }
    }
    display.sendBuffer();
}

void Screen::nextPage() {
    currentPage = (currentPage + 1) % totalPages;
}

