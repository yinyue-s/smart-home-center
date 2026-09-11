#include "oled_ssd1315.h"
#include "i2c.h"
#include "string.h"



//
uint8_t OLED_SSD1315_InitCmd[23] = {
0xAE, 0xD5, 0x80, 0xA8, 0x3F, 0xD3, 0x00, 0x40, 0xA1, 0xC8, 0xDA, 0x12,
0x81, 0xCF, 0xD9, 0xF1, 0xDB, 0x40, 0xA4, 0xA6,0x8D, 0x14, 0xAF};

//
void OLED_SSD1315_WR_CMD(uint8_t cmd)
{
	HAL_I2C_Mem_Write(&hi2c1 ,0x78,0x00,I2C_MEMADD_SIZE_8BIT,&cmd,1,0x100);
}

void OLED_SSD1315_WR_DATA(uint8_t data)
{
	HAL_I2C_Mem_Write(&hi2c1 ,0x78,0x40,I2C_MEMADD_SIZE_8BIT,&data,1,0x100);
}
void OLED_SSD1315_full(uint8_t dat)
{
    uint8_t i,n;
    for(i=0;i<8;i++)
    {
        OLED_SSD1315_WR_CMD(0xb0+i);    //设置页地址 (0~7)
        OLED_SSD1315_WR_CMD(0x00);      //设置显示位置-列低地址
        OLED_SSD1315_WR_CMD(0x10);      //设置显示位置-列高地址
        for(n=0;n<128;n++)
            OLED_SSD1315_WR_DATA(dat);//0xff
    }
}
void OLED_SSD1315_Init(void)
{
    HAL_Delay(200);
    uint8_t i = 0;
    for(i=0; i<23; i++)
    {
        OLED_SSD1315_WR_CMD(OLED_SSD1315_InitCmd[i]);
    }OLED_SSD1315_full(0x00); //初始化结束全屏清屏
}

void OLED_SSD1315_SetPos(uint8_t page, uint8_t col)
{
    OLED_SSD1315_WR_CMD(0xb0+page);        //????? (0~7)
    OLED_SSD1315_WR_CMD(((col&0xf0)>>4)|0x10); //??????-????
    OLED_SSD1315_WR_CMD(col&0x0f);         //??????-????
}
// 单个字符显示 8*16字模
void OLED_SSD1315_Char(uint8_t page, uint8_t col, uint8_t chr)
{
    OLED_SSD1315_SetPos(page, col);    // 上半页位置
    for(int i = 0; i < 8; i++)
    {
        OLED_SSD1315_WR_DATA(F8X16[(chr - ' ') * 16 + i]);
    }
    OLED_SSD1315_SetPos(page+1, col);  // 下半页位置
    for(int i = 8; i < 16; i++)
    {
        OLED_SSD1315_WR_DATA(F8X16[(chr - ' ') * 16 + i]);
    }
}


// 字符串显示
void OLED_SSD1315_String(uint8_t page, uint8_t col, uint8_t *str)
{
    uint8_t len = strlen((char *)str);
    for(int i = 0; i < len; i++)
    {
        OLED_SSD1315_Char(page, col, *str);
        col += 8;    // 单个字符宽度8列
        str++;
    }
}
// 石
uint8_t chinese_shi[32] = {
0x02,0x02,0x02,0x02,0xC2,0xB2,0x8E,0x82,0x82,0x82,0x82,0x82,0x82,0x02,0x02,0x00,
0x08,0x04,0x02,0x01,0xFF,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0xFF,0x00,0x00,0x00,/*"石",0*/
};

// 星
uint8_t chinese_xing[32] = {
0x00,0x00,0x00,0xBE,0x2A,0x2A,0x2A,0xEA,0x2A,0x2A,0x2A,0x3E,0x00,0x00,0x00,0x00,
0x00,0x44,0x42,0x49,0x49,0x49,0x49,0x7F,0x49,0x49,0x49,0x49,0x41,0x40,0x00,0x00,/*"星",1*/
};

// 垚
uint8_t chinese_yao[32] = {

0x40,0x40,0x44,0xC4,0x44,0x44,0x44,0x7F,0x44,0x44,0x44,0xC4,0x44,0x40,0x40,0x00,
0x40,0xC4,0x44,0x7F,0x24,0x24,0x20,0x00,0x44,0x44,0x44,0x7F,0x44,0x44,0x40,0x00,/*"垚",2*/

};
// 爱心
uint8_t heart[32] = {
0x00,0x00,0x00,0x00,0xE0,0xF0,0xF0,0xE0,0xE0,0xF0,0xF0,0x60,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x07,0x07,0x03,0x01,0x00,0x00,0x00,0x00,0x00,/*"未命名文件",0*/};
void OLED_SSD1315_Chinese(uint8_t page, uint8_t col, uint8_t *chinese)
{
    OLED_SSD1315_SetPos(page, col);  //上半部分
    for(int i = 0; i < 16; i++)
    {
        OLED_SSD1315_WR_DATA(chinese[i]);
    }
    OLED_SSD1315_SetPos(page+1, col); //下半部分
    for(int i = 16; i < 32; i++)
    {
        OLED_SSD1315_WR_DATA(chinese[i]);
    }
}
static uint32_t Pow(uint8_t m,uint8_t n)
{
	uint32_t res=1;
	while(n--) res *= m;
	return res;
}

void OLED_SSD1315_ShowString(uint8_t x,uint8_t y,uint8_t *str,uint8_t size)
{
	while(*str)
	{
		// 修正：y=页码(page)，x=列(col)
		OLED_SSD1315_Char(y, x, *str);
		x += size;
		if(x > 120)
		{
			x = 0;
			y += size;
		}
		str++;
	}
}

void OLED_SSD1315_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size)
{
	uint8_t t,temp;
	uint8_t enshow=0;
	for(t=0;t<len;t++)
	{
		temp=(num/Pow(10,len-t-1))%10;
		if(enshow==0 && t<(len-1))
		{
			if(temp==0)
			{
				OLED_SSD1315_Char(y, x+t*size, ' ');
				continue;
			}else enshow=1;
		}
		OLED_SSD1315_Char(y, x+t*size, temp+'0');
	}
}