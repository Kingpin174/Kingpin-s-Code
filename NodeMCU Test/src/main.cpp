#include <Arduino.h>
#include <U8g2lib.h>

// 初始化 U8G2 库 (根据你的 OLED 屏幕型号选择合适的构造函数)
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// 定义按钮引脚
const int buttonUp = D5;
const int buttonDown = D6;
const int buttonSelect = D7;

// 菜单项
const char *menuItems[] = {"Option 1", "Option 2", "Option 3", "Option 4"};
const int menuLength = sizeof(menuItems) / sizeof(menuItems[0]);

int currentMenuIndex = 0;

// 函数原型声明
void displayMenu();
void handleMenuSelection(int index);
void handleMenuSelection(int index);

void setup() {
    // 初始化 OLED 显示屏
    u8g2.begin();

    // 初始化按钮引脚
    pinMode(buttonUp, INPUT_PULLUP);
    pinMode(buttonDown, INPUT_PULLUP);
    pinMode(buttonSelect, INPUT_PULLUP);
}

void loop() {
    // 检测按钮输入
    if (digitalRead(buttonUp) == LOW) {
        currentMenuIndex = (currentMenuIndex - 1 + menuLength) % menuLength;
        delay(200); // 防抖
    }
    if (digitalRead(buttonDown) == LOW) {
        currentMenuIndex = (currentMenuIndex + 1) % menuLength;
        delay(200); // 防抖
    }
    if (digitalRead(buttonSelect) == LOW) {
        handleMenuSelection(currentMenuIndex);
        delay(200); // 防抖
    }

    // 显示菜单
    displayMenu();
}

void displayMenu() {
    u8g2.clearBuffer();
    for (int i = 0; i < menuLength; i++) {
        if (i == currentMenuIndex) {
            u8g2.setDrawColor(1); // 高亮当前选项
            u8g2.drawBox(0, i * 10, 128, 10);
            u8g2.setDrawColor(0);
        } else {
            u8g2.setDrawColor(1);
        }
        u8g2.setCursor(2, (i + 1) * 10 - 2);
        u8g2.print(menuItems[i]);
    }
    u8g2.sendBuffer();
}

void handleMenuSelection(int index) {
    // 根据选项执行操作
    switch (index) {
        case 0:
            Serial.println("Option 1 selected");
            break;
        case 1:
            Serial.println("Option 2 selected");
            break;
        case 2:
            Serial.println("Option 3 selected");
            break;
        case 3:
            Serial.println("Option 4 selected");
            break;
        default:
            break;
    }
}