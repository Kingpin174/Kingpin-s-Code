#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <DHT.h>
#include <Adafruit_NeoPixel.h>

// Define missing color constant
#define ST77XX_DARKGREY 0x7BEF // Example RGB565 value for dark grey

// 定义 LCD 引脚
#define LCD_CS     15  // 芯片选择引脚 (D8)
#define LCD_RST    12  // 背光引脚 (D6)
#define LCD_DC     2   // 数据/命令引脚 (D4)
#define LCD_SCL    14  // SPI 时钟引脚 (D5)
#define LCD_SDA    13  // SPI 数据引脚 (MOSI, D7)

// 定义光敏电阻引脚
#define LIGHT_SENSOR_PIN A0 // 光敏电阻模拟输出引脚

// 定义触摸模块引脚
#define TOUCH_SENSOR_PIN 0  // 触摸模块数字输出引脚 (D3)

// 定义 DHT22 引脚和类型
#define DHT_PIN 4           // DHT22 数据引脚 (D2)
#define DHT_TYPE DHT22      // DHT22 类型
DHT dht(DHT_PIN, DHT_TYPE); // 创建 DHT 对象

// 定义继电器引脚
#define RELAY_PIN 5         // 继电器控制引脚 (D1)

// 定义红外感应模块引脚
#define IR_SENSOR_PIN 16    // 红外感应模块引脚 (D0)

// 定义 WS2812 引脚和 LED 数量
#define WS2812_PIN 1        // WS2812 控制引脚 (TX 引脚)
#define NUM_LEDS 120          // WS2812 LED 数量
Adafruit_NeoPixel strip(NUM_LEDS, WS2812_PIN, NEO_GRB + NEO_KHZ800);

// 创建 Adafruit_ST7735 对象
Adafruit_ST7735 tft = Adafruit_ST7735(LCD_CS, LCD_DC, LCD_RST);

// 初始化变量
bool touchDetected = false; // 触摸状态
float lightVoltage = 0.0;   // 光敏电阻电压值
float temperature = 0.0;    // 温度值
float humidity = 0.0;       // 湿度值
bool irDetected = false;    // 红外感应状态

// 上一次显示的数值
float lastLightVoltage = -1.0;
bool lastTouchDetected = false;
float lastTemperature = -1.0;
float lastHumidity = -1.0;
bool lastIrDetected = false;

// 定义超时时间（10分钟 = 600,000 毫秒）
#define RELAY_TIMEOUT 600000

// 初始化计时器变量
unsigned long lastIrDetectedTime = 0;
unsigned long lastRainbowUpdate = 0;
unsigned long lastSensorUpdate = 0;
unsigned long lastDisplayUpdate = 0;

// 定义消抖时间（单位：毫秒）
#define DEBOUNCE_DELAY 50

// 初始化触摸信号相关变量
bool lastTouchState = false;       // 上一次的触摸状态
unsigned long lastDebounceTime = 0; // 上一次触发消抖的时间
unsigned long touchPressStartTime = 0; // 记录触摸按下的开始时间
bool isLongPress = false;          // 标记是否为长按

// 初始化继电器状态变量
bool relayState = false; // 继电器初始状态为关闭

// 初始化彩虹流水灯速度变量
int rainbowSpeed = 100; // 默认速度为 100

void rainbowFlow() {
  static uint16_t startIndex = 0; // 用于跟踪彩虹的起始位置

  // 根据当前速度更新彩虹流水灯
  if (millis() - lastRainbowUpdate >= 10) {
    lastRainbowUpdate = millis();
    startIndex = (startIndex + rainbowSpeed) % 65536; // 根据速度调整步进值

    for (int i = 0; i < NUM_LEDS; i++) {
      uint16_t pixelIndex = (startIndex + (i * 65536 / NUM_LEDS)) % 65536;
      strip.setPixelColor(i, strip.ColorHSV(pixelIndex, 255, 255)); // 最大亮度
    }

    strip.show(); // 更新 LED 显示
  }
}

// 声明函数原型
void drawUI();
void updateUI(); // Ensure the function prototype is declared only once

void setup() {
  // 初始化串口
  Serial.begin(9600);
  Serial.println("Testing ST7735S...");

  // 初始化 SPI 和 LCD
  SPI.begin();
  tft.initR(INITR_GREENTAB); // 使用绿色标签初始化参数
  tft.fillScreen(ST77XX_BLACK); // 清屏为黑色

  // 设置传感器和继电器引脚
  pinMode(LIGHT_SENSOR_PIN, INPUT);
  pinMode(TOUCH_SENSOR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(IR_SENSOR_PIN, INPUT);

  // 初始化 DHT22
  dht.begin();

  // 初始化 WS2812
  strip.begin();
  strip.show(); // 清空所有 LED

  // 绘制初始界面
  drawUI();
}

void drawUI() {
  // 清屏
  tft.fillScreen(ST77XX_BLACK);

  // 绘制标题
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setCursor(20, 5);
  tft.print("Desktop");

  // 绘制分隔线
  tft.drawLine(0, 25, 160, 25, ST77XX_WHITE);

  // 绘制光敏电阻区域
  tft.fillRect(5, 30, 150, 20, ST77XX_DARKGREY); // 背景色块
  tft.setTextSize(1);
  tft.setCursor(10, 35);
  tft.setTextColor(ST77XX_YELLOW);
  tft.print("Light:");
  tft.setCursor(80, 35);
  tft.setTextColor(ST77XX_WHITE);
  tft.print("0.0 V");

  // 绘制触摸状态区域
  tft.fillRect(5, 55, 150, 20, ST77XX_DARKGREY); // 背景色块
  tft.setCursor(10, 60);
  tft.setTextColor(ST77XX_CYAN);
  tft.print("Touch:");
  tft.setCursor(80, 60);
  tft.setTextColor(ST77XX_WHITE);
  tft.print("No");

  // 绘制温度区域
  tft.fillRect(5, 80, 150, 20, ST77XX_DARKGREY); // 背景色块
  tft.setCursor(10, 85);
  tft.setTextColor(ST77XX_RED);
  tft.print("Temp:");
  tft.setCursor(80, 85);
  tft.setTextColor(ST77XX_WHITE);
  tft.print("0.0 C");

  // 绘制湿度区域
  tft.fillRect(5, 105, 150, 20, ST77XX_DARKGREY); // 背景色块
  tft.setCursor(10, 110);
  tft.setTextColor(ST77XX_BLUE);
  tft.print("Humidity:");
  tft.setCursor(80, 110);
  tft.setTextColor(ST77XX_WHITE);
  tft.print("0.0 %");

  // 绘制红外感应状态区域
  tft.fillRect(5, 130, 150, 20, ST77XX_DARKGREY); // 背景色块
  tft.setCursor(10, 135);
  tft.setTextColor(ST77XX_GREEN);
  tft.print("IR:");
  tft.setCursor(80, 135);
  tft.setTextColor(ST77XX_WHITE);
  tft.print("None");
}

void updateUI() {
  // 更新光敏电阻电压值
  if (abs(lightVoltage - lastLightVoltage) > 0.1) {
    tft.fillRect(80, 30, 70, 20, ST77XX_DARKGREY); // 清除旧值
    tft.setCursor(80, 35);
    tft.setTextColor(ST77XX_WHITE);
    tft.print(String(lightVoltage, 1)); // 保留一位小数
    tft.print(" V");
    lastLightVoltage = round(lightVoltage * 10) / 10.0;
  }

  // 更新触摸状态
  if (touchDetected != lastTouchDetected) {
    tft.fillRect(80, 55, 70, 20, ST77XX_DARKGREY); // 清除旧值
    tft.setCursor(80, 60);
    tft.setTextColor(ST77XX_WHITE);
    tft.print(touchDetected ? "Yes" : "No");
    lastTouchDetected = touchDetected;
  }

  // 更新温度值
  if (temperature != lastTemperature) {
    tft.fillRect(80, 80, 70, 20, ST77XX_DARKGREY); // 清除旧值
    tft.setCursor(80, 85);
    tft.setTextColor(ST77XX_WHITE);
    tft.print(String(temperature, 1)); // 保留一位小数
    tft.print(" C");
    lastTemperature = temperature;
  }

  // 更新湿度值
  if (humidity != lastHumidity) {
    tft.fillRect(80, 105, 70, 20, ST77XX_DARKGREY); // 清除旧值
    tft.setCursor(80, 110);
    tft.setTextColor(ST77XX_WHITE);
    tft.print(String(humidity, 1)); // 保留一位小数
    tft.print(" %");
    lastHumidity = humidity;
  }

  // 更新红外感应状态
  if (irDetected != lastIrDetected) {
    tft.fillRect(80, 130, 70, 20, ST77XX_DARKGREY); // 清除旧值
    tft.setCursor(80, 135);
    tft.setTextColor(ST77XX_WHITE);
    tft.print(irDetected ? "Detected" : "None");
    lastIrDetected = irDetected;
  }
}

void loop() {
  unsigned long currentMillis = millis();

  // 更新传感器数据（每 500 毫秒）
  if (currentMillis - lastSensorUpdate >= 500) {
    lastSensorUpdate = currentMillis;

    // 读取光敏电阻电压值
    int lightValue = analogRead(LIGHT_SENSOR_PIN);
    lightVoltage = (lightValue / 1023.0) * 3.3; // 将模拟值转换为电压值

    // 读取 DHT22 温湿度
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();

    // 检测红外感应模块状态
    irDetected = digitalRead(IR_SENSOR_PIN);

    // 如果检测到红外信号，更新最后检测时间
    if (irDetected) {
      lastIrDetectedTime = currentMillis;
    }

    // 检查是否超时（如果继电器是打开状态）
    if (relayState && (currentMillis - lastIrDetectedTime > RELAY_TIMEOUT)) {
      relayState = false; // 超时后关闭继电器
      digitalWrite(RELAY_PIN, LOW);
    }
  }

  // 读取触摸模块的原始状态
  bool currentTouchState = digitalRead(TOUCH_SENSOR_PIN);

  // 检查触摸信号是否发生变化
  if (currentTouchState != lastTouchState) {
    lastDebounceTime = currentMillis;

    // 如果是按下事件，记录按下的开始时间
    if (currentTouchState) {
      touchPressStartTime = currentMillis;
      isLongPress = false; // 重置长按标记
    } else {
      // 如果是松开事件，判断是短按还是长按
      unsigned long pressDuration = currentMillis - touchPressStartTime;
      if (pressDuration >= 1000) { // 长按（按下时间超过 1 秒）
        isLongPress = true;
      } else { // 短按
        isLongPress = false;
      }
    }
  }

  // 如果触摸信号稳定超过消抖时间，则处理按键逻辑
  if ((currentMillis - lastDebounceTime) > DEBOUNCE_DELAY) {
    if (!currentTouchState && touchDetected) { // 松开时触发逻辑
      if (isLongPress) {
        // 长按：切换彩虹流水灯速度
        if (rainbowSpeed == 100) {
          rainbowSpeed = 20;
        } else if (rainbowSpeed == 20) {
          rainbowSpeed = 10;
        } else {
          rainbowSpeed = 100;
        }
      } else {
        // 短按：切换继电器状态
        relayState = !relayState;
        digitalWrite(RELAY_PIN, relayState ? HIGH : LOW);
      }
    }

    // 更新触摸状态
    touchDetected = currentTouchState;
  }

  // 更新上一次的触摸状态
  lastTouchState = currentTouchState;

  // 更新显示（每 100 毫秒）
  if (currentMillis - lastDisplayUpdate >= 100) {
    lastDisplayUpdate = currentMillis;
    updateUI();
  }

  // 显示彩虹流水灯效果
  rainbowFlow();
}

