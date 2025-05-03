#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// 定义WS2812的引脚和LED数量
#define LED_PIN    1  // NodeMCU的D4引脚
#define LED_COUNT  120  // WS2812灯珠数量

// 创建一个Adafruit_NeoPixel对象
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// 函数声明（函数原型）
void rainbowCycle(uint8_t wait);
uint32_t Wheel(byte WheelPos);

void setup() {
  // 初始化NeoPixel库
  strip.begin();
  strip.show(); // 确保所有LED初始化为关闭状态
}

void loop() {
  // 调用不同的效果函数
  rainbowCycle(5); // 彩虹循环效果
}

// 彩虹循环效果函数
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5次完整的彩虹循环
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait * 3);
  }
}

// 颜色生成函数
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}