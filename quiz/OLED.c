#include "OLED.h"

/*引脚配置*/
#define OLED_W_SCL(x)		SCL_Output(x)
#define OLED_W_SDA(x)		SDA_Output(x)

// /* === I²C 地址（7bit=0x3D；写=0x7A，读=0x7B）==================== *///ssd1309
// #define OLED_I2C_ADDR_7BIT     0x3D
// #define OLED_I2C_ADDR_WRITE    (OLED_I2C_ADDR_7BIT << 1)      /* 0x7A */
// #define OLED_I2C_ADDR_READ     ((OLED_I2C_ADDR_7BIT << 1) | 1)/* 0x7B */
/* === I²C 地址（7bit=0x3C）==================== *///ssd1306
#define OLED_I2C_ADDR_7BIT     0x3C
#define OLED_I2C_ADDR_WRITE    (OLED_I2C_ADDR_7BIT << 1)      /* 0x7A */
#define OLED_I2C_ADDR_READ     ((OLED_I2C_ADDR_7BIT << 1) | 1)/* 0x7B */



/*引脚初始化*/
void OLED_I2C_Init(void) {
  uapi_pin_set_mode(OLED_SDA_pin, 0);
  uapi_pin_set_mode(OLED_SCL_pin, 0);
  uapi_gpio_set_dir(OLED_SCL_pin, 1);
  uapi_gpio_set_dir(OLED_SDA_pin, 1);
  uapi_gpio_set_val(OLED_SDA_pin, 1);
  uapi_gpio_set_val(OLED_SCL_pin, 1);
  uapi_pin_set_pull(OLED_SDA_pin, PIN_PULL_TYPE_UP);
  uapi_pin_set_pull(OLED_SCL_pin, PIN_PULL_TYPE_UP);

  OLED_W_SCL(1);
  OLED_W_SDA(1);
}

/**
  * @brief  I2C开始
  * @param  无
  * @retval 无
  */
void OLED_I2C_Start(void) {
  OLED_W_SDA(1);
  OLED_W_SCL(1);
  OLED_W_SDA(0);
  OLED_W_SCL(0);
}

/**
  * @brief  I2C停止
  * @param  无
  * @retval 无
  */
void OLED_I2C_Stop(void) {
  OLED_W_SDA(0);
  OLED_W_SCL(1);
  OLED_W_SDA(1);
}

/**
  * @brief  I2C发送一个字节
  * @param  Byte 要发送的一个字节
  * @retval 无
  */
void OLED_I2C_SendByte(uint8_t Byte) {
  uint8_t i;
  for (i = 0; i < 8; i++) {
    OLED_W_SDA(Byte & (0x80 >> i));
    OLED_W_SCL(1);
    OLED_W_SCL(0);
  }
  OLED_W_SCL(1);	//额外的一个时钟，不处理应答信号
  OLED_W_SCL(0);
}

//1306
/**
  * @brief  OLED写命令
  * @param  Command 要写入的命令
  * @retval 无
  */
void OLED_WriteCommand(uint8_t Command) {
  OLED_I2C_Start();
  OLED_I2C_SendByte(0x78);		//从机地址
  OLED_I2C_SendByte(0x00);		//写命令
  OLED_I2C_SendByte(Command);
  OLED_I2C_Stop();
}



/**
  * @brief  OLED写数据
  * @param  Data 要写入的数据
  * @retval 无
  */
void OLED_WriteData(uint8_t Data) {
  OLED_I2C_Start();
  OLED_I2C_SendByte(0x78);		//从机地址
  OLED_I2C_SendByte(0x40);		//写数据
  OLED_I2C_SendByte(Data);
  OLED_I2C_Stop();
}

//1309
// void OLED_WriteCommand(uint8_t Command)
// {
//     OLED_I2C_Start();
//     OLED_I2C_SendByte(OLED_I2C_ADDR_WRITE);   /* ← 由 0x78 改为宏 */
//     OLED_I2C_SendByte(0x00);                  /* 写命令 */
//     OLED_I2C_SendByte(Command);
//     OLED_I2C_Stop();
// }

// void OLED_WriteData(uint8_t Data)
// {
//     OLED_I2C_Start();
//     OLED_I2C_SendByte(OLED_I2C_ADDR_WRITE);   /* ← 由 0x78 改为宏 */
//     OLED_I2C_SendByte(0x40);                  /* 写数据 */
//     OLED_I2C_SendByte(Data);
//     OLED_I2C_Stop();
// }

/**
  * @brief  OLED设置光标位置
  * @param  Y 以左上角为原点，向下方向的坐标，范围：0~7
  * @param  X 以左上角为原点，向右方向的坐标，范围：0~127
  * @retval 无
  */
void OLED_SetCursor(uint8_t Y, uint8_t X) {
  OLED_WriteCommand(0xB0 | Y);					//设置Y位置
  OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));	//设置X位置高4位
  OLED_WriteCommand(0x00 | (X & 0x0F));			//设置X位置低4位
}

/**
  * @brief  OLED清屏
  * @param  无
  * @retval 无
  */
void OLED_Clear(void) {
  uint8_t i, j;
  for (j = 0; j < 8; j++) {
    OLED_SetCursor(j, 0);
    for (i = 0; i < 128; i++) {
      OLED_WriteData(0x00);
    }
  }
}

/**
  * @brief  OLED显示一个字符
  * @param  Line 行位置，范围：1~4
  * @param  Column 列位置，范围：1~16
  * @param  Char 要显示的一个字符，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char) {
  uint8_t i;
  OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);		//设置光标位置在上半部分
  for (i = 0; i < 8; i++) {
    OLED_WriteData(OLED_F8x16[Char - ' '][i]);			//显示上半部分内容
  }
  OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	//设置光标位置在下半部分
  for (i = 0; i < 8; i++) {
    OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);		//显示下半部分内容
  }
}

/**
  * @brief  OLED显示字符串
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  String 要显示的字符串，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowString(uint8_t Line, uint8_t Column, char* String) {
  uint8_t i;
  for (i = 0; String[i] != '\0'; i++) {
    OLED_ShowChar(Line, Column + i, String[i]);
  }
}

/**
  * @brief  OLED次方函数
  * @retval 返回值等于X的Y次方
  */
uint32_t OLED_Pow(uint32_t X, uint32_t Y) {
  uint32_t Result = 1;
  while (Y--) {
    Result *= X;
  }
  return Result;
}

/**
  * @brief  OLED显示数字（十进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~4294967295
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length) {
  uint8_t i;
  for (i = 0; i < Length; i++) {
    OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
  }
}

/**
  * @brief  OLED显示数字（十进制，带符号数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：-2147483648~2147483647
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length) {
  uint8_t i;
  uint32_t Number1;
  if (Number >= 0) {
    OLED_ShowChar(Line, Column, '+');
    Number1 = Number;
  } else {
    OLED_ShowChar(Line, Column, '-');
    Number1 = -Number;
  }
  for (i = 0; i < Length; i++) {
    OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
  }
}

/**
  * @brief  OLED显示数字（十六进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~0xFFFFFFFF
  * @param  Length 要显示数字的长度，范围：1~8
  * @retval 无
  */
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length) {
  uint8_t i, SingleNumber;
  for (i = 0; i < Length; i++) {
    SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
    if (SingleNumber < 10) {
      OLED_ShowChar(Line, Column + i, SingleNumber + '0');
    } else {
      OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
    }
  }
}

/**
  * @brief  OLED显示数字（二进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~1111 1111 1111 1111
  * @param  Length 要显示数字的长度，范围：1~16
  * @retval 无
  */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length) {
  uint8_t i;
  for (i = 0; i < Length; i++) {
    OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
  }
}


/**
* @brief  OLED显示预设的像素图案（来自 my_pixel_data）
* @param  无
* @retval 无
*/
void OLED_ShowPix(const uint8_t* image) {
  uint8_t page, x;

  // 循环每一页（0~7）
  for (page = 0; page < 8; page++) {
    OLED_SetCursor(page, 0);  // 设置光标位置（第 page 页，X=0）

    // 写入该页的 128 字节数据
    for (x = 0; x < 128; x++) {
      OLED_WriteData(image[page * 128 + x]);
    }
  }
}

#define BYTES_PER_FRAME   976      // pro1~5 实际长度；如为 1024 请相应改动
#define FRAME_DELAY_MS    100      // 帧间延时，决定帧率（≈10 fps）

/* frames 是“指向帧首地址的指针数组” */
void OLED_ShowGif(const uint8_t* const frames[], uint8_t frameCnt) {
  if (!frames || frameCnt == 0) return;

  for (uint8_t i = 0; i < frameCnt; i++) {
    OLED_ShowPix(frames[i]);      // 直接把第 i 帧首地址交给显示函数
    //OLED_DelayMs(FRAME_DELAY_MS); // 你自己的延时函数
  }
}


/**
  * @brief  OLED初始化
  * @param  无
  * @retval 无
  */
void OLED_Init(void) {
  OLED_I2C_Init();			//端口初始化

  OLED_WriteCommand(0xAE);	//关闭显示

  OLED_WriteCommand(0xD5);	//设置显示时钟分频比/振荡器频率
  OLED_WriteCommand(0x80);

  OLED_WriteCommand(0xA8);	//设置多路复用率
  OLED_WriteCommand(0x3F);

  OLED_WriteCommand(0xD3);	//设置显示偏移
  OLED_WriteCommand(0x00);

  OLED_WriteCommand(0x40);	//设置显示开始行

  OLED_WriteCommand(0xA1);	//设置左右方向，0xA1正常 0xA0左右反置

  OLED_WriteCommand(0xC8);	//设置上下方向，0xC8正常 0xC0上下反置

  OLED_WriteCommand(0xDA);	//设置COM引脚硬件配置
  OLED_WriteCommand(0x12);

  OLED_WriteCommand(0x81);	//设置对比度控制
  OLED_WriteCommand(0xCF);

  OLED_WriteCommand(0xD9);	//设置预充电周期
  OLED_WriteCommand(0xF1);

  OLED_WriteCommand(0xDB);	//设置VCOMH取消选择级别
  OLED_WriteCommand(0x30);

  OLED_WriteCommand(0xA4);	//设置整个显示打开/关闭

  OLED_WriteCommand(0xA6);	//设置正常/倒转显示

  OLED_WriteCommand(0x8D);	//设置充电泵
  OLED_WriteCommand(0x14);

  OLED_WriteCommand(0xAF);	//开启显示

  OLED_Clear();				//OLED清屏
}


void OLED_Init_1309(void) {
  OLED_I2C_Init();			// 端口初始化(保持不变)

  OLED_WriteCommand(0xAE);	// 关闭显示

  OLED_WriteCommand(0xD5);	// 设置显示时钟分频比
  OLED_WriteCommand(0x80);	// 建议值

  OLED_WriteCommand(0xA8);	// 设置复用率
  OLED_WriteCommand(0x3F);	// 1/64 duty (保持不变)

  OLED_WriteCommand(0xD3);	// 设置显示偏移
  OLED_WriteCommand(0x00);	// 无偏移

  OLED_WriteCommand(0x40);	// 设置显示开始行(行0)

  // +++ 新增内存地址模式设置 +++
  OLED_WriteCommand(0x20);
  OLED_WriteCommand(0x02);

  OLED_WriteCommand(0xA1);	// 段重映射(正常)
  OLED_WriteCommand(0xC8);	// COM输出扫描方向(反向)

  // +++ COM引脚配置调整 +++
  OLED_WriteCommand(0xDA);	// COM硬件配置
  OLED_WriteCommand(0x12);	// 保持原值(但SSD1309需要此配置)

  // +++ 对比度调整 +++
  OLED_WriteCommand(0x81);	// 对比度设置
  OLED_WriteCommand(0xFF);	// 最大值(原0xCF)

  OLED_WriteCommand(0xD9);	// 预充电周期
  OLED_WriteCommand(0x22);	// 调整值(原F1)

  OLED_WriteCommand(0xDB);	// VCOMH电平
  OLED_WriteCommand(0x40);	// 约0.77xVCC(原0x30)

  // +++ 新增电荷泵配置 +++
  OLED_WriteCommand(0xAD);      // DC-DC控制命令
  OLED_WriteCommand(0x8B);      // 启用内置DC-DC

  OLED_WriteCommand(0xA4);	// 使用RAM内容显示
  OLED_WriteCommand(0xA6);	// 正常显示(非反转)

  OLED_WriteCommand(0xAF);	// 开启显示

  OLED_Clear();				// OLED清屏
}
