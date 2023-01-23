#include <Arduino.h>
#include "Pic_data.h"

#define SPI_7381 0
#define IO8080_7381 1

#if SPI_7381
#define MISO digitalRead(6)
#define RST_RESET digitalWrite(17, LOW)
#define SS_RESET digitalWrite(5, LOW)
#define CLK_RESET digitalWrite(8, LOW)
#define MOSI_RESET digitalWrite(7, LOW)
#define RST_SET digitalWrite(17, HIGH)
#define SS_SET digitalWrite(5, HIGH)
#define CLK_SET digitalWrite(8, HIGH)
#define MOSI_SET digitalWrite(7, HIGH)
#endif

#if IO8080_7381
#define LCD_CS_SET digitalWrite(33, HIGH)  //片选端口
#define LCD_RS_SET digitalWrite(19, HIGH)  //数据/命令
#define LCD_WR_SET digitalWrite(20, HIGH)  //写数据
#define LCD_RD_SET digitalWrite(21, HIGH)  //读数据
#define RST_SET digitalWrite(17, HIGH)

#define LCD_CS_CLR digitalWrite(33, LOW)  //片选端口
#define LCD_RS_CLR digitalWrite(19, LOW)  //数据/命令
#define LCD_WR_CLR digitalWrite(20, LOW)  //写数据
#define LCD_RD_CLR digitalWrite(21, LOW)  //读数据
#define RST_RESET digitalWrite(17, LOW)
#endif

//外部晶振
#define XI_4M 0
#define XI_8M 0
#define XI_10M 1
#define XI_12M 0

// //分辨率
// #define LCD_XSIZE_TFT	800
// #define LCD_YSIZE_TFT	480
// //参数
// #define LCD_VBPD    20
// #define LCD_VFPD		12
// #define LCD_VSPW		3
// #define LCD_HBPD		140
// #define LCD_HFPD		160
// #define LCD_HSPW		20
//分辨率
#define LCD_XSIZE_TFT 800
#define LCD_YSIZE_TFT 480
//参数
#define LCD_VBPD 20
#define LCD_VFPD 147
#define LCD_VSPW 4
#define LCD_HBPD 16
#define LCD_HFPD 354
#define LCD_HSPW 40

unsigned char CCLK;  // LT738的内核时钟频率
unsigned char MCLK;  // SDRAM的时钟频率
unsigned char PCLK;  // LCD的扫描时钟频率

/* LCD color */
#define White 0xFFFF
#define Black 0x0000
#define Grey 0xF7DE
#define Blue 0x001F
#define Blue2 0x051F
#define Red 0xF800
#define Magenta 0xF81F
#define Green 0x07E0
#define Cyan 0x7FFF
#define Yellow 0xFFE0

#if SPI_7381
void SPI2_Init(void) {
  pinMode(5, OUTPUT);   //CS
  pinMode(6, INPUT);    //MISO
  pinMode(7, OUTPUT);   //MOSI
  pinMode(8, OUTPUT);   //CLK
  pinMode(17, OUTPUT);  //RESET
}

void SPI_SendByte(unsigned char buf) {
  unsigned char count = 0;
  for (count = 0; count < 8; count++) {
    CLK_RESET;
    if (buf & 0x80) MOSI_SET;
    else MOSI_RESET;
    CLK_SET;
    buf <<= 1;
  }
  CLK_RESET;
  MOSI_RESET;
}

unsigned char SPI_ReadByte(void) {
  unsigned char count = 0;
  unsigned char val = 0;
  for (count = 0; count < 8; count++) {
    val <<= 1;
    CLK_SET;
    if (MISO == 1) val |= 1;
    else val &= 0xfe;
    CLK_RESET;
  }
  CLK_RESET;
  return val;
}

void SPI_CmdWrite(unsigned char cmd) {
  SS_RESET;
  SPI_SendByte(0x00);
  SPI_SendByte(cmd);
  SS_SET;
}

void SPI_DataWrite(unsigned char data) {
  SS_RESET;
  SPI_SendByte(0x80);
  SPI_SendByte(data);
  SS_SET;
}

void SPI_DataWrite_Pixel(unsigned short data) {
  SS_RESET;
  SPI_SendByte(0x80);
  SPI_SendByte(data);
  SS_SET;
  SS_RESET;
  SPI_SendByte(0x80);
  SPI_SendByte(data >> 8);
  SS_SET;
}

unsigned char SPI_StatusRead(void) {
  unsigned char temp = 0;
  SS_RESET;
  SPI_SendByte(0x40);
  temp = SPI_ReadByte();
  SS_SET;
  return temp;
}

unsigned char SPI_DataRead(void) {
  unsigned char temp = 0;
  SS_RESET;
  SPI_SendByte(0xc0);
  temp = SPI_ReadByte();
  SS_SET;
  return temp;
}
#endif

#if IO8080_7381
void Write_Value(unsigned short value) {
  for (unsigned char i = 1; i <= 16; i++) {
    digitalWrite(i, value & 0x01);
    value >>= 1;
  }
}

unsigned char Read_Value_8(void) {
  unsigned char value = 0;
  for (unsigned char i = 0; i < 8; i++) {
    value |= (digitalRead(i + 1) << i);
  }
  return value;
}

unsigned short Read_Value_16(void) {
  unsigned short value = 0;
  for (unsigned char i = 0; i < 16; i++) {
    value |= (digitalRead(i + 1) << i);
  }
  return value;
}

void IO8080_Init(void) {
  //DATA0~15
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(15, OUTPUT);
  pinMode(16, OUTPUT);
  pinMode(17, OUTPUT);  //RESET
  pinMode(33, OUTPUT);  //CS
  pinMode(19, OUTPUT);  //RS
  pinMode(20, OUTPUT);  //WR
  pinMode(21, OUTPUT);  //RD
  LCD_CS_SET;
  LCD_RS_SET;
  LCD_WR_SET;
  LCD_RD_SET;
}

void SET_DATA_OUTPUT(void) {
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(15, OUTPUT);
  pinMode(16, OUTPUT);
}

void SET_DATA_INPUT(void) {
  pinMode(1, INPUT);
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  pinMode(10, INPUT);
  pinMode(11, INPUT);
  pinMode(12, INPUT);
  pinMode(13, INPUT);
  pinMode(14, INPUT);
  pinMode(15, INPUT);
  pinMode(16, INPUT);
}

void DATAOUT(unsigned short data) {
  Write_Value(data);
}

void DATAOUT8(unsigned char data) {
  unsigned short data16;
  data16 = data;
  Write_Value(data);
}

void IO8080_CmdWrite(unsigned char cmd) {
  LCD_RS_CLR;  //写地址
  LCD_CS_CLR;
  DATAOUT8(cmd);
  LCD_WR_CLR;
  LCD_WR_SET;
  LCD_CS_SET;
}

void IO8080_DataWrite(unsigned char data) {
  LCD_RS_SET;
  LCD_CS_CLR;
  DATAOUT8(data);
  LCD_WR_CLR;
  LCD_WR_SET;
  LCD_CS_SET;
}

void IO8080_DataWrite_Pixel(unsigned short data) {
  LCD_RS_SET;
  LCD_CS_CLR;
  DATAOUT(data);
  LCD_WR_CLR;
  LCD_WR_SET;
  LCD_CS_SET;
}

unsigned char IO8080_StatusRead(void) {
  unsigned short temp = 0;
  SET_DATA_INPUT();
  LCD_CS_CLR;
  LCD_RS_CLR;  //写地址
  LCD_RD_CLR;
  temp = Read_Value_8();
  LCD_RD_SET;
  LCD_CS_SET;
  SET_DATA_OUTPUT();
  return temp;
}

unsigned short IO8080_DataRead(void) {
  unsigned short temp = 0;
  SET_DATA_INPUT();
  LCD_RS_SET;  //写地址
  LCD_CS_CLR;
  LCD_RD_CLR;
  temp = Read_Value_16();
  LCD_RD_SET;
  LCD_CS_SET;
  SET_DATA_OUTPUT();
  return temp;
}
#endif

void write_reg_with_data(unsigned char cmd, unsigned char data) {
#if SPI_7381
  SPI_CmdWrite(cmd);
  SPI_DataWrite(data);
#endif
#if IO8080_7381
  IO8080_CmdWrite(cmd);
  IO8080_DataWrite(data);
#endif
}

void lt7381_reg_config(void)  //设置驱动IC的寄存器
{
  unsigned char param;
  unsigned int temp = 0, temp1 = 0;
  unsigned short lpllOD_sclk, lpllOD_cclk, lpllOD_mclk;
  unsigned short lpllR_sclk, lpllR_cclk, lpllR_mclk;
  unsigned short lpllN_sclk, lpllN_cclk, lpllN_mclk;
  unsigned short sdram_itv;

  temp = (LCD_HBPD + LCD_HFPD + LCD_HSPW + LCD_XSIZE_TFT) * (LCD_VBPD + LCD_VFPD + LCD_VSPW + LCD_YSIZE_TFT) * 60;

  temp1 = (temp % 1000000) / 100000;
  if (temp1 >= 5)
    temp = temp / 1000000 + 1;
  else temp = temp / 1000000;

  // PCLK = temp;
  PCLK = 33;
  temp = temp * 3;
  MCLK = temp;
  CCLK = temp;

  if (CCLK > 100) CCLK = 100;
  if (MCLK > 100) MCLK = 100;
  if (PCLK > 65) PCLK = 65;

#if XI_4M

  lpllOD_sclk = 3;
  lpllOD_cclk = 3;
  lpllOD_mclk = 3;
  lpllR_sclk = 2;
  lpllR_cclk = 2;
  lpllR_mclk = 2;
  lpllN_mclk = 2 * MCLK;
  lpllN_cclk = 2 * CCLK;
  lpllN_sclk = 2 * PCLK;

#endif

#if XI_8M

  lpllOD_sclk = 3;
  lpllOD_cclk = 3;
  lpllOD_mclk = 3;
  lpllR_sclk = 2;
  lpllR_cclk = 2;
  lpllR_mclk = 2;
  lpllN_mclk = MCLK;
  lpllN_cclk = CCLK;
  lpllN_sclk = PCLK;

#endif

#if XI_10M

  lpllOD_sclk = 3;
  lpllOD_cclk = 3;
  lpllOD_mclk = 3;
  lpllR_sclk = 5;
  lpllR_cclk = 5;
  lpllR_mclk = 5;
  lpllN_mclk = 2 * MCLK;
  lpllN_cclk = 2 * CCLK;
  lpllN_sclk = 2 * PCLK;

#endif

#if XI_12M

  lpllOD_sclk = 3;
  lpllOD_cclk = 3;
  lpllOD_mclk = 3;
  lpllR_sclk = 3;
  lpllR_cclk = 3;
  lpllR_mclk = 3;
  lpllN_mclk = MCLK;
  lpllN_cclk = CCLK;
  lpllN_sclk = PCLK;

#endif

  param = 0x00;
  write_reg_with_data(0x00, param);
  //初始化时钟
  param = (lpllOD_sclk << 6) | (lpllR_sclk << 1) | ((lpllN_sclk >> 8) & 0x1);
  write_reg_with_data(0x05, param);
  param = lpllN_sclk;
  write_reg_with_data(0x06, param);
  // MCLK SDRAM时钟配置
  param = (lpllOD_mclk << 6) | (lpllR_mclk << 1) | ((lpllN_mclk >> 8) & 0x1);
  write_reg_with_data(0x07, param);
  param = lpllN_mclk;
  write_reg_with_data(0x08, param);
  // CCLK 内核时钟配置
  param = (lpllOD_cclk << 6) | (lpllR_cclk << 1) | ((lpllN_cclk >> 8) & 0x1);
  write_reg_with_data(0x09, param);
  param = lpllN_cclk;
  write_reg_with_data(0x0A, param);
  //设置的时钟配置生效
  param = 0x80;
  write_reg_with_data(0x00, param);

  delay(10);

  // SDRAM设置
  sdram_itv = (64000000 / 8192) / (1000 / MCLK);
  sdram_itv -= 2;
  param = 0x20;
  write_reg_with_data(0xe0, param);
  param = 0x03;
  write_reg_with_data(0xe1, param);
  param = sdram_itv;
  write_reg_with_data(0xe2, param);
  param = sdram_itv >> 8;
  write_reg_with_data(0xe3, param);
  param = 0x01;
  write_reg_with_data(0xe4, param);

  delay(10);

  // 16bit输出
  param = 0x51;  //16bit
  // param = 0x50;//8bit
  write_reg_with_data(0x01, param);
  //内存读写格式和方向控制
  param = 0x00;
  write_reg_with_data(0x02, param);
  //中断内存读写选择
  param = 0x00;
  write_reg_with_data(0x03, param);
  //显示设置：显示开关-关 测试 PCLK RGB模式
  //param = 0x00;//PCLK_Rising
  param = 0x80;  //PCLK_Falling
  write_reg_with_data(0x12, param);
  //显示设置：HS,VS,DE
  param = 0x03;
  write_reg_with_data(0x13, param);

  //设置显示分辨率为480*800
  param = (LCD_XSIZE_TFT / 8) - 1;
  write_reg_with_data(0x14, param);
  param = LCD_XSIZE_TFT % 8;
  write_reg_with_data(0x15, param);
  param = (LCD_YSIZE_TFT - 1) & 0xFF;  //(unsigned char)(800-1)
  write_reg_with_data(0x1A, param);
  param = (LCD_YSIZE_TFT - 1) >> 8;
  write_reg_with_data(0x1B, param);

  //水平扫描控制 后肩 前肩 脉宽
  param = (LCD_HBPD / 8) - 1;
  write_reg_with_data(0x16, param);
  param = LCD_HBPD % 8;
  write_reg_with_data(0x17, param);
  param = (LCD_HFPD / 8) - 1;
  write_reg_with_data(0x18, param);
  param = (LCD_HSPW / 8) - 1;
  write_reg_with_data(0x19, param);

  //垂直扫描控制 后肩 前肩 脉宽
  param = (LCD_VBPD - 1) & 0xFF;
  write_reg_with_data(0x1C, param);
  param = (LCD_VBPD - 1) >> 8;
  write_reg_with_data(0x1D, param);
  param = LCD_VFPD - 1;
  write_reg_with_data(0x1E, param);
  param = LCD_VSPW - 1;
  write_reg_with_data(0x1F, param);

  //内存block xy模式 16bpp
  param = 0x01;
  write_reg_with_data(0x5E, param);

  //显示设置：色深16bit，默认值不需要设置 Select_Main_Window_16bpp
  param = 0x04;
  write_reg_with_data(0x10, param);
  //设置主画面在SRAM中的起始地址 Main_Image_Start_Address
  param = 0x00;
  write_reg_with_data(0x20, param);
  param = 0x00;
  write_reg_with_data(0x21, param);
  param = 0x00;
  write_reg_with_data(0x22, param);
  param = 0x00;
  write_reg_with_data(0x23, param);
  //设定主画面的宽度 Main_Image_Width
  param = (LCD_XSIZE_TFT)&0xff;
  write_reg_with_data(0x24, param);
  param = (LCD_XSIZE_TFT >> 8) & 0xff;
  write_reg_with_data(0x25, param);
  //设定主窗口的开始坐标 Main_Window_Start_XY
  param = 0x00;
  write_reg_with_data(0x26, param);
  param = 0x00;
  write_reg_with_data(0x27, param);
  param = 0x00;
  write_reg_with_data(0x28, param);
  param = 0x00;
  write_reg_with_data(0x29, param);
  //设置写入SRAM中的起始地址 Canvas_Image_Start_address
  param = 0x00;
  write_reg_with_data(0x50, param);
  param = 0x00;
  write_reg_with_data(0x51, param);
  param = 0x00;
  write_reg_with_data(0x52, param);
  param = 0x00;
  write_reg_with_data(0x53, param);
  //设定写入SDRAM的宽度 Canvas_image_width
  param = (LCD_XSIZE_TFT)&0xff;
  write_reg_with_data(0x54, param);
  param = (LCD_XSIZE_TFT >> 8) & 0xff;
  write_reg_with_data(0x55, param);
  //设定工作的开始坐标 Active_Window_XY
  param = 0x00;
  write_reg_with_data(0x56, param);
  param = 0x00;
  write_reg_with_data(0x57, param);
  param = 0x00;
  write_reg_with_data(0x58, param);
  param = 0x00;
  write_reg_with_data(0x59, param);
  //设定工作窗口的宽度 Active_Window_WH
  param = (LCD_XSIZE_TFT)&0xff;
  write_reg_with_data(0x5A, param);
  param = (LCD_XSIZE_TFT >> 8) & 0xff;
  write_reg_with_data(0x5B, param);
  param = (LCD_YSIZE_TFT)&0xff;
  write_reg_with_data(0x5C, param);
  param = (LCD_YSIZE_TFT >> 8) & 0xff;
  write_reg_with_data(0x5D, param);
  //显示开关-开 Display_ON
  param = 0xC0;
  write_reg_with_data(0x12, param);

  //Foreground_color_65k
  param = (Black >> 8) & 0xFF;
  write_reg_with_data(0xD2, param);
  param = (Black >> 3) & 0xFF;
  write_reg_with_data(0xD3, param);
  param = (Black << 3) & 0xFF;
  write_reg_with_data(0xD4, param);

  //Square_Start_XY
  param = (0) & 0xff;
  write_reg_with_data(0x68, param);
  param = (0 >> 8) & 0xff;
  write_reg_with_data(0x69, param);
  param = (0) & 0xff;
  write_reg_with_data(0x6A, param);
  param = (0 >> 8) & 0xff;
  write_reg_with_data(0x6B, param);

  //Square_End_XY
  param = (LCD_XSIZE_TFT - 1) & 0xff;
  write_reg_with_data(0x6C, param);
  param = ((LCD_XSIZE_TFT - 1) >> 8) & 0xff;
  write_reg_with_data(0x6D, param);
  param = (LCD_YSIZE_TFT - 1) & 0xff;
  write_reg_with_data(0x6E, param);
  param = ((LCD_YSIZE_TFT - 1) >> 8) & 0xff;
  write_reg_with_data(0x6F, param);

  //Start_Square_Fill
  param = 0xE0;
  write_reg_with_data(0x76, param);

  delay(100);

  //Set BL(OPEN PWM1 100%)
  param = 0x63;  // PWM预分频99+1 =100 100M/100 =1M
  write_reg_with_data(0x84, param);
  param = 0xC8;  // PWM1输出，8分频  1M/8 =125K
  write_reg_with_data(0x85, param);
  param = 0x64;  // 时钟周期设置100 125/100 =1.25K（最终频率）
  write_reg_with_data(0x8E, param);
  param = 0x00;  // 高字节
  write_reg_with_data(0x8F, param);
  param = 0x64;  // 比较值设置100，低字节在前，占空比=100/100*100% =100%
  write_reg_with_data(0x8C, param);
  param = 0x00;  // 高字节
  write_reg_with_data(0x8D, param);
  param = 0x30;  // 开PWM1，自动重装载
  write_reg_with_data(0x86, param);
}

void lt7381_draw_data(unsigned short x, unsigned short y, unsigned short w, unsigned short h, unsigned short color) {
  unsigned char param;
  unsigned short i, j;
  //设定描点窗口的开始坐标 Active_Window_XY
  param = (x)&0xff;
  write_reg_with_data(0x56, param);
  param = (x >> 8) & 0xff;
  write_reg_with_data(0x57, param);
  param = (y)&0xff;
  write_reg_with_data(0x58, param);
  param = (y >> 8) & 0xff;
  write_reg_with_data(0x59, param);
  //设定描点窗口的宽度高度 Active_Window_WH
  param = (w)&0xff;
  write_reg_with_data(0x5A, param);
  param = (w >> 8) & 0xff;
  write_reg_with_data(0x5B, param);
  param = (h)&0xff;
  write_reg_with_data(0x5C, param);
  param = (h >> 8) & 0xff;
  write_reg_with_data(0x5D, param);
  //Goto_Pixel_XY必须
  param = 0x00;
  write_reg_with_data(0x5F, param);
  param = 0x00;
  write_reg_with_data(0x60, param);
  param = 0x00;
  write_reg_with_data(0x61, param);
  param = 0x00;
  write_reg_with_data(0x62, param);
  //写0x04指向SDRAM
#if SPI_7381
  SPI_CmdWrite(0x04);
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++)
      SPI_DataWrite_Pixel(color);
  }
#endif
#if IO8080_7381
  IO8080_CmdWrite(0x04);
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++)
      IO8080_DataWrite_Pixel(color);
  }
#endif
  //设定描点窗口的开始坐标 Active_Window_XY
  param = (0) & 0xff;
  write_reg_with_data(0x56, param);
  param = (0 >> 8) & 0xff;
  write_reg_with_data(0x57, param);
  param = (0) & 0xff;
  write_reg_with_data(0x58, param);
  param = (0 >> 8) & 0xff;
  write_reg_with_data(0x59, param);
  //设定描点窗口的宽度高度 Active_Window_WH
  param = (LCD_XSIZE_TFT)&0xff;
  write_reg_with_data(0x5A, param);
  param = (LCD_XSIZE_TFT >> 8) & 0xff;
  write_reg_with_data(0x5B, param);
  param = (LCD_YSIZE_TFT)&0xff;
  write_reg_with_data(0x5C, param);
  param = (LCD_YSIZE_TFT >> 8) & 0xff;
  write_reg_with_data(0x5D, param);
}

void lt7381_draw_Pic_char(unsigned short x, unsigned short y, unsigned short w, unsigned short h, unsigned char *buff) {
  unsigned char param;
  unsigned short i, j;
  unsigned int k = 0;
#if IO8080_7381
  unsigned short temp = 0;
#endif
  //设定描点窗口的开始坐标 Active_Window_XY
  param = (x)&0xff;
  write_reg_with_data(0x56, param);
  param = (x >> 8) & 0xff;
  write_reg_with_data(0x57, param);
  param = (y)&0xff;
  write_reg_with_data(0x58, param);
  param = (y >> 8) & 0xff;
  write_reg_with_data(0x59, param);
  //设定描点窗口的宽度高度 Active_Window_WH
  param = (w)&0xff;
  write_reg_with_data(0x5A, param);
  param = (w >> 8) & 0xff;
  write_reg_with_data(0x5B, param);
  param = (h)&0xff;
  write_reg_with_data(0x5C, param);
  param = (h >> 8) & 0xff;
  write_reg_with_data(0x5D, param);
  //Goto_Pixel_XY必须
  param = 0x00;
  write_reg_with_data(0x5F, param);
  param = 0x00;
  write_reg_with_data(0x60, param);
  param = 0x00;
  write_reg_with_data(0x61, param);
  param = 0x00;
  write_reg_with_data(0x62, param);
  //写0x04指向SDRAM
#if SPI_7381
  SPI_CmdWrite(0x04);
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      SPI_DataWrite(buff[k]);
      SPI_DataWrite(buff[k + 1]);
      k += 2;
    }
  }
#endif
#if IO8080_7381
  IO8080_CmdWrite(0x04);
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      temp = (buff[k + 1] << 8) + buff[k];
      k += 2;
      IO8080_DataWrite_Pixel(temp);
    }
  }
#endif
  //设定描点窗口的开始坐标 Active_Window_XY
  param = (0) & 0xff;
  write_reg_with_data(0x56, param);
  param = (0 >> 8) & 0xff;
  write_reg_with_data(0x57, param);
  param = (0) & 0xff;
  write_reg_with_data(0x58, param);
  param = (0 >> 8) & 0xff;
  write_reg_with_data(0x59, param);
  //设定描点窗口的宽度高度 Active_Window_WH
  param = (LCD_XSIZE_TFT)&0xff;
  write_reg_with_data(0x5A, param);
  param = (LCD_XSIZE_TFT >> 8) & 0xff;
  write_reg_with_data(0x5B, param);
  param = (LCD_YSIZE_TFT)&0xff;
  write_reg_with_data(0x5C, param);
  param = (LCD_YSIZE_TFT >> 8) & 0xff;
  write_reg_with_data(0x5D, param);
}

void lt7381_draw_Pic_short(unsigned short x, unsigned short y, unsigned short w, unsigned short h, unsigned short *buff) {
  unsigned char param;
  unsigned short i, j;
  unsigned int k = 0;
#if IO8080_7381
  unsigned short temp = 0;
#endif
  //设定描点窗口的开始坐标 Active_Window_XY
  param = (x)&0xff;
  write_reg_with_data(0x56, param);
  param = (x >> 8) & 0xff;
  write_reg_with_data(0x57, param);
  param = (y)&0xff;
  write_reg_with_data(0x58, param);
  param = (y >> 8) & 0xff;
  write_reg_with_data(0x59, param);
  //设定描点窗口的宽度高度 Active_Window_WH
  param = (w)&0xff;
  write_reg_with_data(0x5A, param);
  param = (w >> 8) & 0xff;
  write_reg_with_data(0x5B, param);
  param = (h)&0xff;
  write_reg_with_data(0x5C, param);
  param = (h >> 8) & 0xff;
  write_reg_with_data(0x5D, param);
  //Goto_Pixel_XY必须
  param = 0x00;
  write_reg_with_data(0x5F, param);
  param = 0x00;
  write_reg_with_data(0x60, param);
  param = 0x00;
  write_reg_with_data(0x61, param);
  param = 0x00;
  write_reg_with_data(0x62, param);
  //写0x04指向SDRAM
#if SPI_7381
  SPI_CmdWrite(0x04);
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      SPI_DataWrite_Pixel(buff[k]);
      k += 1;
    }
  }
#endif
#if IO8080_7381
  IO8080_CmdWrite(0x04);
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      IO8080_DataWrite_Pixel(buff[k]);
      k += 1;
    }
  }
#endif
  //设定描点窗口的开始坐标 Active_Window_XY
  param = (0) & 0xff;
  write_reg_with_data(0x56, param);
  param = (0 >> 8) & 0xff;
  write_reg_with_data(0x57, param);
  param = (0) & 0xff;
  write_reg_with_data(0x58, param);
  param = (0 >> 8) & 0xff;
  write_reg_with_data(0x59, param);
  //设定描点窗口的宽度高度 Active_Window_WH
  param = (LCD_XSIZE_TFT)&0xff;
  write_reg_with_data(0x5A, param);
  param = (LCD_XSIZE_TFT >> 8) & 0xff;
  write_reg_with_data(0x5B, param);
  param = (LCD_YSIZE_TFT)&0xff;
  write_reg_with_data(0x5C, param);
  param = (LCD_YSIZE_TFT >> 8) & 0xff;
  write_reg_with_data(0x5D, param);
}

void Check_2D_Busy(void) {
  do {

  }
#if SPI_7381
  while (SPI_StatusRead() & 0x08);
#endif
#if IO8080_7381
  while (IO8080_StatusRead() & 0x08)
    ;
#endif
}

void LT738_DrawSquare_Fill(
  unsigned short X1  // X1位置
  ,
  unsigned short Y1  // Y1位置
  ,
  unsigned short X2  // X2位置
  ,
  unsigned short Y2  // Y2位置
  ,
  unsigned long ForegroundColor  // 背景颜色
) {
  unsigned char param;
  //Foreground_color_65k
  param = (ForegroundColor >> 8) & 0xFF;
  write_reg_with_data(0xD2, param);
  param = (ForegroundColor >> 3) & 0xFF;
  write_reg_with_data(0xD3, param);
  param = (ForegroundColor << 3) & 0xFF;
  write_reg_with_data(0xD4, param);

  //Square_Start_XY
  param = (X1)&0xff;
  write_reg_with_data(0x68, param);
  param = (X1 >> 8) & 0xff;
  write_reg_with_data(0x69, param);
  param = (Y1)&0xff;
  write_reg_with_data(0x6A, param);
  param = (Y1 >> 8) & 0xff;
  write_reg_with_data(0x6B, param);

  //Square_End_XY
  param = (X2)&0xff;
  write_reg_with_data(0x6C, param);
  param = ((X2) >> 8) & 0xff;
  write_reg_with_data(0x6D, param);
  param = (Y2)&0xff;
  write_reg_with_data(0x6E, param);
  param = ((Y2) >> 8) & 0xff;
  write_reg_with_data(0x6F, param);

  //Start_Square_Fill
  param = 0xE0;
  write_reg_with_data(0x76, param);

  Check_2D_Busy();
}

//复位LT738
void LT738_HW_Reset(void) {
  RST_SET;
  delay(100);
  RST_RESET;
  delay(100);
  RST_SET;
  delay(100);
}

void LT738_RESET_CHECK(void) {
  //Must get 0x50 and 0xc8
#if SPI_7381
  Serial.printf("SPI_StatusRead = %x\n", SPI_StatusRead());  //0x50
  delay(10);
  SPI_CmdWrite(0x01);
  delay(10);
  Serial.printf("IO8080_DataRead = %x\n", (unsigned char)SPI_DataRead());  //0xc8
  delay(10);
#endif
#if IO8080_7381
  Serial.printf("SPI_StatusRead = %x\n", IO8080_StatusRead());  //0x50
  delay(10);
  IO8080_CmdWrite(0x01);
  delay(10);
  Serial.printf("IO8080_DataRead = %x\n", (unsigned char)IO8080_DataRead());  //0xc8
  delay(10);
#endif
}

void Display_RGB_Gray() {
  int i, w, h;
  unsigned short c;
  w = LCD_XSIZE_TFT / 3;    //LCD_XSIZE_TFT根据实际情况修改
  h = LCD_YSIZE_TFT / 32;   //LCD_YSIZE_TFT根据实际情况修改
  for (i = 0; i < 31; i++)  //红色
  {
    c = 31 - i;
    LT738_DrawSquare_Fill(0, h * i, w - 1, h * (i + 1), (c << 11) & 0xF800);
  }
  c = 0;
  LT738_DrawSquare_Fill(0, h * 31, w - 1, LCD_YSIZE_TFT - 1, c);

  h = LCD_YSIZE_TFT / 64;
  for (i = 0; i < 63; i++)  //绿色
  {
    c = 63 - i;
    LT738_DrawSquare_Fill(w, h * i, w * 2 - 1, h * (i + 1), (c << 5) & 0x7E0);
  }
  c = 0;
  LT738_DrawSquare_Fill(w, h * 63, w * 2 - 1, LCD_YSIZE_TFT - 1, c);

  h = LCD_YSIZE_TFT / 32;
  for (i = 0; i < 31; i++)  //蓝色
  {
    c = 31 - i;
    LT738_DrawSquare_Fill(w * 2, h * i, LCD_XSIZE_TFT - 1, h * (i + 1), c & 0x1F);
  }
  c = 0;
  LT738_DrawSquare_Fill(w * 2, h * 31, LCD_XSIZE_TFT - 1, LCD_YSIZE_TFT - 1, c);
}

void dis_font(unsigned short x, unsigned short y, unsigned short sizew, unsigned short sizeh, unsigned short font_color, unsigned short bg_color, unsigned char *font_buff, unsigned char num) {
  unsigned int i, j;
  unsigned char param, temp, k;
  //设定描点窗口的开始坐标 Active_Window_XY
  param = (x)&0xff;
  write_reg_with_data(0x56, param);
  param = (x >> 8) & 0xff;
  write_reg_with_data(0x57, param);
  param = (y)&0xff;
  write_reg_with_data(0x58, param);
  param = (y >> 8) & 0xff;
  write_reg_with_data(0x59, param);
  //设定描点窗口的宽度高度 Active_Window_WH
  param = (sizew)&0xff;
  write_reg_with_data(0x5A, param);
  param = (sizew >> 8) & 0xff;
  write_reg_with_data(0x5B, param);
  param = (sizeh)&0xff;
  write_reg_with_data(0x5C, param);
  param = (sizeh >> 8) & 0xff;
  write_reg_with_data(0x5D, param);
  //Goto_Pixel_XY必须
  param = 0x00;
  write_reg_with_data(0x5F, param);
  param = 0x00;
  write_reg_with_data(0x60, param);
  param = 0x00;
  write_reg_with_data(0x61, param);
  param = 0x00;
  write_reg_with_data(0x62, param);
#if SPI_7381
  SPI_CmdWrite(0x04);
  for (j = 0; j < sizeh; j++) {
    for (i = 0; i < sizew / 8; i++)  //写入一行数据
    {
      for (k = 0; k < 8; k++) {
        temp = ((font_buff[4 + num * (sizew / 8) * sizeh + i + j * (sizew / 8)] >> (7 - k)) & 0x1);
        if (temp)
          SPI_DataWrite_Pixel(font_color);
        else
          SPI_DataWrite_Pixel(bg_color);
      }
    }
  }
#endif
#if IO8080_7381
  IO8080_CmdWrite(0x04);
  for (j = 0; j < sizeh; j++) {
    for (i = 0; i < sizew / 8; i++)  //写入一行数据
    {
      for (k = 0; k < 8; k++) {
        temp = ((font_buff[4 + num * (sizew / 8) * sizeh + i + j * (sizew / 8)] >> (7 - k)) & 0x1);
        if (temp)
          IO8080_DataWrite_Pixel(font_color);
        else
          IO8080_DataWrite_Pixel(bg_color);
      }
    }
  }
#endif
  //设定描点窗口的开始坐标 Active_Window_XY
  param = (0) & 0xff;
  write_reg_with_data(0x56, param);
  param = (0 >> 8) & 0xff;
  write_reg_with_data(0x57, param);
  param = (0) & 0xff;
  write_reg_with_data(0x58, param);
  param = (0 >> 8) & 0xff;
  write_reg_with_data(0x59, param);
  //设定描点窗口的宽度高度 Active_Window_WH
  param = (LCD_XSIZE_TFT)&0xff;
  write_reg_with_data(0x5A, param);
  param = (LCD_XSIZE_TFT >> 8) & 0xff;
  write_reg_with_data(0x5B, param);
  param = (LCD_YSIZE_TFT)&0xff;
  write_reg_with_data(0x5C, param);
  param = (LCD_YSIZE_TFT >> 8) & 0xff;
  write_reg_with_data(0x5D, param);
}

void setup() {
  unsigned char temp;
  Serial.begin(115200);
  Serial.printf("ESP32S2 ON\n");
  pinMode(LED_BUILTIN, OUTPUT);
#if SPI_7381
  SPI2_Init();  //初始化模拟SPI
#endif
#if IO8080_7381
  IO8080_Init();  //初始化模拟并口
#endif
  LT738_HW_Reset();  //LT738复位
  delay(50);
  LT738_RESET_CHECK();  //检查驱动是否正常
  lt7381_reg_config();  //寄存器初始化

//开窗描点demo
#if 1
  LT738_DrawSquare_Fill(0, 0, LCD_XSIZE_TFT, LCD_YSIZE_TFT, Black);
  lt7381_draw_data(5, 5, 30, 200, Red);
  lt7381_draw_data(50, 5, 30, 200, Green);
  lt7381_draw_data(100, 5, 30, 200, Blue);
  
  delay(1000);
#endif

//用矩形函数，全屏填充红、绿、蓝三色
#if 1
  LT738_DrawSquare_Fill(0, 0, LCD_XSIZE_TFT, LCD_YSIZE_TFT, Red);
  LT738_DrawSquare_Fill(50, 50, LCD_XSIZE_TFT - 50, LCD_YSIZE_TFT - 50, Green);
  LT738_DrawSquare_Fill(100, 100, LCD_XSIZE_TFT - 100, LCD_YSIZE_TFT - 100, Blue);
  delay(1000);
#endif

//用矩形函数显示RGB灰阶
#if 1
  Display_RGB_Gray();
  delay(1000);
#endif

//Draw Picture
#if 1
  LT738_DrawSquare_Fill(0, 0, LCD_XSIZE_TFT, LCD_YSIZE_TFT, Black);
  lt7381_draw_Pic_char((LCD_XSIZE_TFT - 200) / 2, (LCD_YSIZE_TFT - 200) / 2, 200, 200, demo_pic_data);
  for (temp = 0; temp < 31; temp++) {
    dis_font(5 + 25 * temp, 5, 24, 32, Red, Black, UbuntuBold, 1 + temp);
  }
  delay(1000);
#endif
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}
