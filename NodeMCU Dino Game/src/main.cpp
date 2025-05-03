#include <Arduino.h>
#include <U8g2lib.h>

// 使用 I2C 接口初始化 U8G2 对象
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// 游戏变量
int dinoY = 40;          // 恐龙的 Y 坐标
int dinoJump = 0;        // 跳跃状态（0：不跳跃，1：跳跃中）
int obstacleX = 128;     // 障碍物的 X 坐标
bool gameOver = false;   // 游戏结束标志
int score = 0;           // 游戏得分

void setup() {
  // 初始化串口
  Serial.begin(9600);

  // 初始化 OLED 显示屏
  u8g2.begin();

  // 显示启动画面，"!" 闪烁
  for (int i = 0; i < 6; i++) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(10, 30, "Dino Game");
    if (i % 2 == 0) {
      u8g2.drawStr(80, 30, "!"); // 闪烁的 "!"
    }
    u8g2.sendBuffer();
    delay(500);
  }
}

void loop() {
  if (gameOver) {
    // 游戏结束画面
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(10, 30, "Game Over!");
    u8g2.setCursor(10, 50);
    u8g2.print("Score: ");
    u8g2.print(score);
    u8g2.sendBuffer();
    delay(2000);
    return; // 停止游戏循环
  }

  // 自动跳跃逻辑：当障碍物接近恐龙时，触发跳跃
  if (obstacleX < 40 && obstacleX > 30 && dinoJump == 0 && dinoY == 40) {
    dinoJump = 1; // 开始跳跃
  }

  // 更新恐龙位置
  if (dinoJump == 1) {
    dinoY -= 7; // 向上跳跃，增加跳跃高度
    if (dinoY <= 10) { // 到达更高的最高点
      dinoJump = 2; // 开始下落
    }
  } else if (dinoJump == 2) {
    dinoY += 7; // 下落，增加下落速度
    if (dinoY >= 40) {
      dinoY = 40; // 恢复到地面
      dinoJump = 0; // 跳跃结束
    }
  }

  // 更新障碍物位置
  obstacleX -= 5;
  if (obstacleX < 0) {
    obstacleX = 128; // 重置障碍物位置
    score++;         // 增加得分
  }

  // 检测碰撞
  if (obstacleX < 20 && obstacleX > 10 && dinoY >= 30) {
    gameOver = true; // 恐龙与障碍物碰撞，游戏结束
  }

  // 绘制游戏画面
  u8g2.clearBuffer();

  // 绘制地面
  u8g2.drawHLine(0, 50, 128);

  // 绘制恐龙（像素化模型）
  u8g2.drawBox(10, dinoY, 10, 10); // 身体
  u8g2.drawBox(5, dinoY + 5, 5, 5); // 尾巴
  u8g2.drawBox(20, dinoY + 2, 3, 3); // 头部

  // 绘制障碍物（仙人掌模型）
  u8g2.drawBox(obstacleX, 40, 5, 10); // 主干
  u8g2.drawBox(obstacleX - 3, 42, 3, 3); // 左分支
  u8g2.drawBox(obstacleX + 5, 42, 3, 3); // 右分支

  // 显示得分
  u8g2.setFont(u8g2_font_5x8_tr);
  u8g2.setCursor(0, 10);
  u8g2.print("Score: ");
  u8g2.print(score);

  // 更新显示
  u8g2.sendBuffer();

  // 延迟以控制游戏速度
  delay(50);
}