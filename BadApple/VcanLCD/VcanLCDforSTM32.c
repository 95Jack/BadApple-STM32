
#include "VcanLCDforSTM32.h"

#define VCAN_LCD_H 128
#define VCAN_LCD_W 128

void VcanLCD_GpioInit(void);
void VcanLCD_ResgisterInit(void);
Point GetZeroPoint(void);
Size GetVcanLCDSize(void);
void VcanLCD_GetSize(Size *size);

extern unsigned char const VcanASCII_8x16[1536];

//VcanLcdInfo_t VcanLcdInfo = {VCAN_LCD_W,VCAN_LCD_H,VcanLcdDirection_Default};

#ifdef USE_VCANLCD_STRUCT

VcanLCD_t VcanLCD = 
{
	VcanLCD_Init,
	VcanLCD_ShowMuxString,
	VcanLCD_ShowInt,
	VcanLCD_ShowFloat,
	VcanLCD_Clear,
	VcanLCD_DrawPoint,
	VcanLCD_DrawLine,
	VcanLCD_DrawPenRectangle,
	VcanLCD_DrawRectangleWithColor_t,
	VcanLCD_DrawCircle,
	VcanLCD_DrawBinaryImage,
	VcanLcd_ShowBMP,
	{VCAN_LCD_W,VCAN_LCD_H,VcanLcdDirection_Default}
};

#endif

void VcanLCD_Init(void)
{
		ColorInit();
		VcanLCD_GpioInit();
		VcanLCD_ResgisterInit();
    VcanLCD_DrawRectangleWithColor_t(GetZeroPoint(),GetVcanLCDSize(),BackColor); 
}

void VcanLCD_GpioInit(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE); 		
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1 |GPIO_Pin_2 |GPIO_Pin_3 |
										GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;
		GPIO_Init(VCAN_LCD_P0_GPIO, &GPIO_InitStructure);
	
	  VCAN_LCD_P0_OUT(0x00);
	  
		GPIO_InitStructure.GPIO_Pin =  (1 << VCAN_LCD_WR_PIN) | 
																	 (1 << VCAN_LCD_RD_PIN) |
	                                 (1 << VCAN_LCD_CS_PIN) |
	                                 (1 << VCAN_LCD_RS_PIN) |
	                                 (1 << VCAN_LCD_RST_PIN);
		GPIO_Init(VCAN_LCD_CTL_GPIO, &GPIO_InitStructure);
	
	  VCAN_LCD_WR_OUT = 0;   
    VCAN_LCD_RD_OUT = 1;   
    VCAN_LCD_CS_OUT = 1;    
    VCAN_LCD_RS_OUT  = 0;   
    VCAN_LCD_RST_OUT = 0;   
}

void VcanLCD_ResgisterInit(void)
{
//��ʼ������
    VCAN_LCD_RST_OUT = 0;
    VCAN_LCD_DELAY(); 
    VCAN_LCD_RST_OUT = 1;
    VCAN_LCD_DELAY_MS(500);      //�ϵ���㹻ʱ��

    VCAN_LCD_WR_8CMD(0x11);           //Sleep out   �˳�˯��ģʽ
    VCAN_LCD_DELAY_MS(120);      //Delay 120ms
    //------------------------------------ST7735R Frame Rate-----------------------------------------//
    /* Set the frame frequency of the full Color_ts normal mode. */
    VCAN_LCD_WR_8CMD(0xB1);     //In Normal Mode (Full Color_ts)    ȫ��ģʽ
    VCAN_LCD_WR_8DATA(0x05);    //���� RTNA Set 1-line  Period  һ������
    VCAN_LCD_WR_8DATA(0x3A);    //���� FPA: Front Porch
    VCAN_LCD_WR_8DATA(0x3A);    //���� BPA: Back Porch
    //Frame rate=fosc/((RTNA x 2 + 40) x (LINE + FPA + BPA +2))
    //���� fosc = 850kHz

    /* Set the frame frequency of the Idle mode.  */
    VCAN_LCD_WR_8CMD(0xB2);     //In Idle Mode (8-Color_ts)
    VCAN_LCD_WR_8DATA(0x05);
    VCAN_LCD_WR_8DATA(0x3A);
    VCAN_LCD_WR_8DATA(0x3A);

    /* Set the frame frequency of the Partial mode/ full Color_ts. */
    VCAN_LCD_WR_8CMD(0xB3);
    VCAN_LCD_WR_8DATA(0x05);
    VCAN_LCD_WR_8DATA(0x3A);
    VCAN_LCD_WR_8DATA(0x3A);
    VCAN_LCD_WR_8DATA(0x05);
    VCAN_LCD_WR_8DATA(0x3A);
    VCAN_LCD_WR_8DATA(0x3A);

    //------------------------------------End ST7735R Frame Rate-----------------------------------------//
    VCAN_LCD_WR_8CMD(0xB4);   //Display Inversion Control  ��ʾ��ת����

    VCAN_LCD_WR_8DATA(0x07);  //LCD_ST7735R_WR_8DATA(0x03); ԭ��Ϊ3����Ϊ7
    // ����λ�Ӹߵ��ͣ��ֱ�Ϊ  full Color_ts normal mode ��Idle mode ��
    // full Color_ts partial mode  �� �㷴ת �� �з�ת ��1Ϊ�з�ת

    VCAN_LCD_WR_8CMD(0xC0);   //Power Control Setting  ��Դ��������
    VCAN_LCD_WR_8DATA(0x28);
    VCAN_LCD_WR_8DATA(0x08);
    VCAN_LCD_WR_8DATA(0x84);
    VCAN_LCD_WR_8CMD(0xC1);
    VCAN_LCD_WR_8DATA(0XC0);
    VCAN_LCD_WR_8CMD(0xC2);
    VCAN_LCD_WR_8DATA(0x0C);
    VCAN_LCD_WR_8DATA(0x00);
    VCAN_LCD_WR_8CMD(0xC3);
    VCAN_LCD_WR_8DATA(0x8C);
    VCAN_LCD_WR_8DATA(0x2A);
    VCAN_LCD_WR_8CMD(0xC4);
    VCAN_LCD_WR_8DATA(0x8A);
    VCAN_LCD_WR_8DATA(0xEE);
    //---------------------------------End ST7735R Power Sequence-------------------------------------//
    VCAN_LCD_WR_8CMD(0xC5);   //  VCOM ��ѹ����
    VCAN_LCD_WR_8DATA(0x0C); //  -0.725
    //------------------------------------ST7735R Gamma Sequence-----------------------------------------//
    VCAN_LCD_WR_8CMD(0xE0);
    VCAN_LCD_WR_8DATA(0x05);
    VCAN_LCD_WR_8DATA(0x1A);
    VCAN_LCD_WR_8DATA(0x0C);
    VCAN_LCD_WR_8DATA(0x0E);
    VCAN_LCD_WR_8DATA(0x3A);
    VCAN_LCD_WR_8DATA(0x34);
    VCAN_LCD_WR_8DATA(0x2D);
    VCAN_LCD_WR_8DATA(0x2F);
    VCAN_LCD_WR_8DATA(0x2D);
    VCAN_LCD_WR_8DATA(0x2A);
    VCAN_LCD_WR_8DATA(0x2F);
    VCAN_LCD_WR_8DATA(0x3C);
    VCAN_LCD_WR_8DATA(0x00);
    VCAN_LCD_WR_8DATA(0x01);
    VCAN_LCD_WR_8DATA(0x02);
    VCAN_LCD_WR_8DATA(0x10);
    VCAN_LCD_WR_8CMD(0xE1);
    VCAN_LCD_WR_8DATA(0x04);
    VCAN_LCD_WR_8DATA(0x1B);
    VCAN_LCD_WR_8DATA(0x0D);
    VCAN_LCD_WR_8DATA(0x0E);
    VCAN_LCD_WR_8DATA(0x2D);
    VCAN_LCD_WR_8DATA(0x29);
    VCAN_LCD_WR_8DATA(0x24);
    VCAN_LCD_WR_8DATA(0x29);
    VCAN_LCD_WR_8DATA(0x28);
    VCAN_LCD_WR_8DATA(0x26);
    VCAN_LCD_WR_8DATA(0x31);
    VCAN_LCD_WR_8DATA(0x3B);
    VCAN_LCD_WR_8DATA(0x00);
    VCAN_LCD_WR_8DATA(0x00);
    VCAN_LCD_WR_8DATA(0x03);
    VCAN_LCD_WR_8DATA(0x12);
    //------------------------------------End ST7735R Gamma Sequence-----------------------------------------//

    VCAN_LCD_WR_8CMD(0x3A); //65k mode
    VCAN_LCD_WR_8DATA(0x05);
    VCAN_LCD_WR_8CMD(0x29); //����ʾ Display on
    VCAN_LCD_WR_8CMD(0x2c); //

    VcanLCD_SetShowDirection(VcanLcdDirection_Default);
}

void VcanLCD_SetShowDirection(VcanLcdDirection_t option)
{
	VcanLCD.Info.ShowDirection  = option;
    switch(option)
    {
    case 0:
        VCAN_LCD_WR_8CMD(0x36); // �ڴ����ݷ��ʿ���  MX �е�ַ˳��, MY �е�ַ˳�� ��MV ���н��� ��ML ��ֱˢ��˳�� ,RGB  RGB-BGA˳��
        VCAN_LCD_WR_8DATA(0xc8);//0xc8  0xA8 0x08 0x68

        VCAN_LCD_WR_8CMD(0x2a);
        VCAN_LCD_WR_8DATA(0x00);
        VCAN_LCD_WR_8DATA(0x00);
        VCAN_LCD_WR_8DATA(0x00);
        VCAN_LCD_WR_8DATA(128);

        VCAN_LCD_WR_8CMD(0x2B);
        VCAN_LCD_WR_8DATA(0x00);
        VCAN_LCD_WR_8DATA(50);
        VCAN_LCD_WR_8DATA(0x00);
        VCAN_LCD_WR_8DATA(128+50);

        VcanLCD.Info.ShowHeight = VCAN_LCD_H;
        VcanLCD.Info.ShowWidth  = VCAN_LCD_W;
			//	VcanLcdInfo.ShowDirection = VcanLcdDirection_Default;
        break;
    case 1:
        VCAN_LCD_WR_8CMD(0x36); // �ڴ����ݷ��ʿ���  MX �е�ַ˳��, MY �е�ַ˳�� ��MV ���н��� ��ML ��ֱˢ��˳�� ,RGB  RGB-BGA˳��
        VCAN_LCD_WR_8DATA(0xA8);//0xc8  0xA8 0x08 0x68

        VCAN_LCD_WR_8CMD(0x2a);
        VCAN_LCD_WR_8DATA(0x00);
        VCAN_LCD_WR_8DATA(0x03);
        VCAN_LCD_WR_8DATA(0x00);
        VCAN_LCD_WR_8DATA(0x82);

        VCAN_LCD_WR_8CMD(0x2B);
        VCAN_LCD_WR_8DATA(0x00);
        VCAN_LCD_WR_8DATA(0x02);
        VCAN_LCD_WR_8DATA(0x00);
        VCAN_LCD_WR_8DATA(0x81);

        VcanLCD.Info.ShowHeight = VCAN_LCD_W;
        VcanLCD.Info.ShowWidth  = VCAN_LCD_H;
			//	VcanLcdInfo.ShowDirection = VcanLcdDirection_Rotate90;
        break;
    case 2:
        VCAN_LCD_WR_8CMD(0x36); // �ڴ����ݷ��ʿ���  MX �е�ַ˳��, MY �е�ַ˳�� ��MV ���н��� ��ML ��ֱˢ��˳�� ,RGB  RGB-BGA˳��
        VCAN_LCD_WR_8DATA(0x08);//0xc8  0xA8 0x08 0x68

        VCAN_LCD_WR_8CMD(0x2a);
        VCAN_LCD_WR_8DATA(0x00);
        VCAN_LCD_WR_8DATA(0x02);
        VCAN_LCD_WR_8DATA(0x00);
        VCAN_LCD_WR_8DATA(0x81);

        VCAN_LCD_WR_8CMD(0x2B);
        VCAN_LCD_WR_8DATA(0x00);
        VCAN_LCD_WR_8DATA(0x03);
        VCAN_LCD_WR_8DATA(0x00);
        VCAN_LCD_WR_8DATA(0x82);

        VcanLCD.Info.ShowHeight = VCAN_LCD_H;
        VcanLCD.Info.ShowWidth  = VCAN_LCD_W;
					//	VcanLcdInfo.ShowDirection = VcanLcdDirection_Rotate180;
        break;
    case 3:
        VCAN_LCD_WR_8CMD(0x36); // �ڴ����ݷ��ʿ���  MX �е�ַ˳��, MY �е�ַ˳�� ��MV ���н��� ��ML ��ֱˢ��˳�� ,RGB  RGB-BGA˳��
        VCAN_LCD_WR_8DATA(0x68);//0xc8  0xA8 0x08 0x68

        VCAN_LCD_WR_8CMD(0x2a);
        VCAN_LCD_WR_8DATA(0x00);
        VCAN_LCD_WR_8DATA(0x03);
        VCAN_LCD_WR_8DATA(0x00);
        VCAN_LCD_WR_8DATA(0x82);

        VCAN_LCD_WR_8CMD(0x2B);
        VCAN_LCD_WR_8DATA(0x00);
        VCAN_LCD_WR_8DATA(0x00);
        VCAN_LCD_WR_8DATA(0x00);
        VCAN_LCD_WR_8DATA(128);

       VcanLCD.Info.ShowWidth = VCAN_LCD_W;
        VcanLCD.Info.ShowWidth = VCAN_LCD_H;
					//	VcanLcdInfo.ShowDirection = VcanLcdDirection_Rotate270;
        break;
    default:
        break;
    }

}

void VcanLCD_Ptlon(Point site, Size size)
{

    if(VcanLCD.Info.ShowDirection&0x01)
        site.X += 32;     
    else
        site.Y += 32;
    VCAN_LCD_WR_8CMD(0x2a);   //Partial Mode On  �ֲ�ģʽ
    VCAN_LCD_WR_8DATA((uint8)(site.X >> 8)); //��8λ Sx
    VCAN_LCD_WR_8DATA((uint8)site.X);   //��8λ Sx
    VCAN_LCD_WR_8DATA((uint8)((site.X + size.Width - 1) >> 8));
    VCAN_LCD_WR_8DATA((uint8)(site.X + size.Width - 1));

    VCAN_LCD_WR_8CMD(0x2B);//Row Address Set  �е�ַ����
    VCAN_LCD_WR_8DATA((uint8)(site.Y >> 8));
    VCAN_LCD_WR_8DATA((uint8)site.Y);
    VCAN_LCD_WR_8DATA((uint8)((site.Y + size.Height - 1) >> 8));
    VCAN_LCD_WR_8DATA((uint8)(site.Y + size.Height - 1));

}

//�������û�����

Point GetZeroPoint(void)
{
	Point point = {0,0};
	return point;
}

Point NewPoint(int x,int y)
{
	Point point; 
	point.X = x;point.Y = y;
	return point;
}

Size NewSize(int width,int height)
{
	Size size;
  size.Width = width;size.Height = height;
	return size;
}

Point NewPointFromSize(Point site,Size size)
{
	Point point; 
	point.X = site.X + size.Width - 1;
	point.Y = site.Y + size.Height - 1;
	return point;
}

void VcanLCD_GetSize(Size *size)
{
    size->Height = VcanLCD.Info.ShowHeight;               //��
    size->Width = VcanLCD.Info.ShowWidth;                 //��
}

Size GetVcanLCDSize(void)
{
		Size size;
		size.Width = VcanLCD.Info.ShowWidth;
		size.Height = VcanLCD.Info.ShowHeight;
	return size;
}

void VcanLCD_Clear(uint16 rgb565)     //����
{
    Point site = {0,0};
    Size size;
		VcanLCD_GetSize(&size);
    VcanLCD_DrawRectangle(site, size, rgb565);
}

void VcanLCD_DrawPoint(Point site, uint16 rgb565)
{
    Size size = {1, 1};
    VcanLCD_Ptlon(site, size);
    VCAN_LCD_RAMWR();                        //д�ڴ�
    VCAN_LCD_WR_DATA(rgb565);                //д����
}

void VcanLCD_DrawPointXY(int x,int y,uint16 rgb565)
{
    Size size = {1, 1};
		Point point;point.X = x;point.Y = y;
    VcanLCD_Ptlon(point, size);
    VCAN_LCD_RAMWR();                        //д�ڴ�
    VCAN_LCD_WR_DATA(rgb565);                //д����
}

void VcanLCD_DrawLine(Point site,unsigned char lenth,Color_t Color_t,VcanLcdAround_t around)
{
		unsigned char i = 0;
		switch(around)
		{
			case VcanLcdAround_Right:	
				VcanLCD_Ptlon(site, NewSize(UP_RANGE(site.X + lenth - 1,\
					VcanLCD.Info.ShowWidth),1));
				break;
			case VcanLcdAround_Down:
				VcanLCD_Ptlon(site,NewSize(1,UP_RANGE(site.Y + lenth - 1,\
					VcanLCD.Info.ShowHeight)));
				break;
			case VcanLcdAround_Left:
				VcanLCD_Ptlon(NewPoint(DOWM_RANGE(site.X - lenth + 1,0),site.Y), \
					NewSize(lenth,1));
				break;
			case VcanLcdAround_Up:
				VcanLCD_Ptlon(NewPoint(site.X,DOWM_RANGE(site.Y - lenth + 1,0)), \
					NewSize(1,lenth));
				break;
			default:
				return;
		}
    VCAN_LCD_RAMWR();                        //д�ڴ�
		for(;i< lenth;++i)
			VCAN_LCD_WR_DATA(Color_t.RBG565Value);     //д����
}

void VcanLCD_DrawPenRectangle(Point site,Size size,Color_t Color_t)
{
		Point point = NewPointFromSize(site,size);
		VcanLCD_DrawLine(site,size.Width, Color_t,VcanLcdAround_Right);
		VcanLCD_DrawLine(site,size.Height,Color_t,VcanLcdAround_Down);
		VcanLCD_DrawLine(point,size.Width,Color_t,VcanLcdAround_Left);
		VcanLCD_DrawLine(point,size.Height,Color_t,VcanLcdAround_Up);
}

void VcanLCD_DrawRectangle(Point site, Size size, uint16 rgb565)
{
    uint32 n, temp;

    VcanLCD_Ptlon(site, size);              //����

    temp = (uint32)size.Width * size.Height;
    VCAN_LCD_RAMWR();                        //д�ڴ�
    for(n = 0; n < temp; n++)
    {
        VCAN_LCD_WR_DATA( rgb565 );          //д����
    }
}

void VcanLCD_DrawRectangleWithColor_t(Point site, Size size, Color_t Color_t)
{
		VcanLCD_DrawRectangle(site,size,Color_t.RBG565Value);
}

void VcanLCD_DrawCircle(uint16 x0,uint16 y0,uint8 r,Color_t Color_t)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //�ж��¸���λ�õı�־
	while(a<=b)
	{
		VcanLCD_DrawPointXY(x0-b,y0-a,Color_t.RBG565Value);             //3           
		VcanLCD_DrawPointXY(x0+b,y0-a,Color_t.RBG565Value);             //0           
		VcanLCD_DrawPointXY(x0-a,y0+b,Color_t.RBG565Value);             //1       
		VcanLCD_DrawPointXY(x0-b,y0-a,Color_t.RBG565Value);             //7           
		VcanLCD_DrawPointXY(x0-a,y0-b,Color_t.RBG565Value);             //2             
		VcanLCD_DrawPointXY(x0+b,y0+a,Color_t.RBG565Value);             //4               
		VcanLCD_DrawPointXY(x0+a,y0-b,Color_t.RBG565Value);             //5
		VcanLCD_DrawPointXY(x0+a,y0+b,Color_t.RBG565Value);             //6 
		VcanLCD_DrawPointXY(x0-b,y0+a,Color_t.RBG565Value);             
		a++;
		//ʹ��Bresenham�㷨��Բ     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 
		VcanLCD_DrawPointXY(x0+a,y0+b,Color_t.RBG565Value);
	}
}		

void VcanLCD_DrawBinaryImageWithColor_t(Point site, Size size, uint8 *img,uint16 OneColor_tVal,uint16 ZeroColor_tVal)
{
		uint32     total = (size.Height * size.Width) / 8;
    uint8     imgtemp;
    uint8       bitindex;
    uint8     *pimg = (uint8 *)img;

    VcanLCD_Ptlon(site, size);
    VCAN_LCD_RAMWR();  

    while(total--)
    {
        imgtemp     = *(pimg++);
        bitindex    = 8;
        while(bitindex--)
        {
            if( imgtemp & (0x01 << bitindex) )
            {
                VCAN_LCD_WR_DATA( OneColor_tVal );      //дͼ������
            }
            else
            {
                VCAN_LCD_WR_DATA(  ZeroColor_tVal  );      //дͼ������
            }
        }
    }
}

void VcanLCD_DrawGrayImage(Point site, Size size, uint8 *img)
{
		VcanLCD_DrawBinaryImageWithColor_t(site,size,img, \
			Color.White.RBG565Value,Color.Black.RBG565Value);
}

void VcanLCD_DrawBinaryImage(Point site, Size size, uint8 *img)
{
    uint32     total = (size.Height * size.Width) / 8;
    uint8     imgtemp;
    uint8       bitindex;
    uint8     *pimg = (uint8 *)img;

    VcanLCD_Ptlon(site, size);
    VCAN_LCD_RAMWR();  

    while(total--)
    {
        imgtemp     = *(pimg++);
        bitindex    = 8;
        while(bitindex--)
        {
            if( imgtemp & (0x01 << bitindex) )
            {
                VCAN_LCD_WR_DATA( 0X0000 );      //дͼ������
            }
            else
            {
                VCAN_LCD_WR_DATA(  0Xffff  );      //дͼ������
            }
        }
    }
}

void MyRowDrawBMP(Point site, Size size, uint8 *img)
{
		int x;
		Size sizeTemp = NewSize(8,128);		
		for(x = 7; x >= 0 ;--x)
		{
				VcanLCD_DrawBinaryImage(NewPoint(x*8,0),sizeTemp,img + ( 7 - x)*128);
		}
}

//�����ǻ��ַ��ĺ���
void VcanLCD_ShowChar(Point site, uint8 ascii, uint16 Color_t, uint16 bkColor_t)
{
#define MAX_CHAR_POSX (VcanLCD.Info.ShowWidth-8)
#define MAX_CHAR_POSY (VcanLCD.Info.ShowHeight-16)

    uint8 temp, t, pos;
    Size size = {8, 16};

    if(site.X > MAX_CHAR_POSX || site.Y > MAX_CHAR_POSY)
    {
        return;
    }

    VcanLCD_Ptlon(site, size);
    VCAN_LCD_RAMWR();  

    for (pos = 0; pos < 16; pos++)
    {
        temp = VcanASCII_8x16[((ascii-0x20)*16)+pos];

        for(t = 0; t < 8; t++)
        {

            if(temp & 0x80)
            {
                VCAN_LCD_WR_DATA(Color_t);
            }
            else
            {
                VCAN_LCD_WR_DATA(bkColor_t);
            }
            temp <<= 1;
        }
    }
    return;
#undef MAX_CHAR_POSX
#undef MAX_CHAR_POSY
}

void VcanLCD_ShowString(unsigned char x,unsigned char y,char * str)
{
		VcanLCD_DrawString(NewPoint(x,y*16),str,\
			StringColor,BackColor);
}

void VcanLCD_ShowInt(unsigned char x,unsigned char y,int val,TextAlign_t align)
{	
		char str[NumTextShowBit] = "";
		char str_geshi[3] = "";str_geshi[0] = '%';
		str_geshi[1] = '0' + NumTextShowBit;str_geshi[2] = 'd';
		sprintf(str,str_geshi,val);
		if(align == TextAlign_Left)
				CStringAlignToLeft(str);	
		VcanLCD_DrawString(NewPoint(x,y*16),str,\
				StringColor,BackColor);
}

void VcanLCD_ShowFloat(unsigned char x,unsigned char y,float val,TextAlign_t align)
{	
		char str[8] = "";
		sprintf(str,"%8.2f",val);
		if(align == TextAlign_Left)
				CStringAlignToLeft(str);	
		VcanLCD_DrawString(NewPoint(x,y*16),str,\
				StringColor,BackColor);
}

void VcanLCD_DrawString(Point site, int8 *Str, Color_t strColor_t, Color_t bkColor_t)
{
#define MAX_CHAR_POSX (VcanLCD.Info.ShowWidth-8)
#define MAX_CHAR_POSY (VcanLCD.Info.ShowHeight-16)
    while(*Str != '\0')
    {
        if(site.X > MAX_CHAR_POSX )
        {
            //����
            site.X = 0;
            site.Y += 16;
        }
        if(site.Y > MAX_CHAR_POSY )
        {
            //һ��
            site.Y = 0;
            site.X = 0;
        }

        VcanLCD_ShowChar(site, *Str, strColor_t.RBG565Value, bkColor_t.RBG565Value);
        site.X += 8;
        Str ++ ;
    }
#undef MAX_CHAR_POSX
#undef MAX_CHAR_POSY
}

void VcanLCD_DrawChineseString(Point site, int8 *Str, uint16 strColor_t, uint16 bkColor_t)
{
#define MAX_CHAR_POSX (VcanLCD.Info.ShowWidth-8)
#define MAX_CHAR_POSY (VcanLCD.Info.ShowHeight-16)				 
//    Size size = {8, 16};
		Size HZsize = {16, 16};
		int index = 0;
    while(*Str != '\0')
    {
        if(site.X > MAX_CHAR_POSX )
        {
            //����
            site.X = 0;
            site.Y += 16;
        }
        if(site.Y > MAX_CHAR_POSY )
        {
            //һ��
            site.Y = 0;
            site.X = 0;
        }
				if(*Str > 127)
				{	
					uint8_t page , column;
					uint8_t buffer[32];		
					uint16_t tmp_char=0;
					if(site.X > MAX_CHAR_POSX || site.Y > MAX_CHAR_POSY)
					{
						return;
					}
					GetGBKCodeFromSD(buffer,(const unsigned char *)Str);
					VcanLCD_Ptlon(site, HZsize);
					VCAN_LCD_RAMWR(); 
					for(page=0; page< 16; page++)
					{
						tmp_char=buffer[page*2];
						tmp_char=(tmp_char<<8);
						tmp_char|=buffer[2*page+1];
		
						for (column=0; column< 16; column++)
						{			
							if ( tmp_char & (0x01<<15) )
							{				
									VCAN_LCD_WR_DATA(strColor_t);
							}
							else
							{			
									VCAN_LCD_WR_DATA(bkColor_t);
							}
							tmp_char <<= 1;
						}
					}	
					index ++;
					site.X += 8;
					Str ++ ;
				}
				else
					VcanLCD_ShowChar(site, *Str, strColor_t, bkColor_t);
        site.X += 8;
        Str ++ ;
	
    }
#undef MAX_CHAR_POSX
#undef MAX_CHAR_POSY
}

void VcanLCD_ShowChineseString(int x,int y, int8 *Str)
{
			VcanLCD_DrawChineseString(NewPoint(x,y*16),Str,\
					StringColor.RBG565Value,BackColor.RBG565Value);
}


void VcanLCD_num_BC(Point site, uint32 num, uint8 max_num_bit, uint16 Color_t, uint16 bkColor_t)
{
    uint32 res = num;
    uint8 t = 0;
    Point sitetemp;
    Size size;

    sitetemp.Y = site.Y;

    if( num == 0 )
    {
        VcanLCD_ShowChar(site, '0', Color_t, bkColor_t);

        site.X += 8;
        size.Height  = 16;
        size.Width  = 8 * (max_num_bit - 1);
        VcanLCD_DrawRectangle(site, size, bkColor_t);

        return;
    }
    while( res )            /*�õ����ֳ���t*/
    {
        res /= 10;
        t++;
    }
    if(t >= max_num_bit )    //������󳤶�
    {
        t = max_num_bit;
    }

    res = t;

    while( t != 0 )
    {
        sitetemp.X = site.X + (8 * (t--) - 8);
        VcanLCD_ShowChar(sitetemp, (num % 10) + '0', Color_t, bkColor_t);
        num /= 10 ;
    }

    if(res != max_num_bit )
    {
        size.Width = 8 * (max_num_bit - res);
        site.X += (8 * res);
        size.Height  = 16;
        VcanLCD_DrawRectangle(site, size, bkColor_t);
    }
}


unsigned char const VcanASCII_8x16[1536] =  //96X16=1536      //ascii�ַ���ΧΪ[32,127] = 96���ַ�,ÿ���ַ�Ҫ��16���ֽ�����ʾ  ������96X16=1536���ֽ�
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x18, 0x3C, 0x3C, 0x3C, 0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x66, 0x66, 0x66, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x6C, 0x6C, 0xFE, 0x6C, 0x6C, 0x6C, 0xFE, 0x6C, 0x6C, 0x00, 0x00, 0x00, 0x00,
    0x18, 0x18, 0x7C, 0xC6, 0xC2, 0xC0, 0x7C, 0x06, 0x86, 0xC6, 0x7C, 0x18, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xC2, 0xC6, 0x0C, 0x18, 0x30, 0x60, 0xC6, 0x86, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x38, 0x6C, 0x6C, 0x38, 0x76, 0xDC, 0xCC, 0xCC, 0xCC, 0x76, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x30, 0x30, 0x30, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x0C, 0x18, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x18, 0x0C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x7E, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x30, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x02, 0x06, 0x0C, 0x18, 0x30, 0x60, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x7C, 0xC6, 0xC6, 0xCE, 0xD6, 0xD6, 0xE6, 0xC6, 0xC6, 0x7C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x18, 0x38, 0x78, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x7C, 0xC6, 0x06, 0x0C, 0x18, 0x30, 0x60, 0xC0, 0xC6, 0xFE, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x7C, 0xC6, 0x06, 0x06, 0x3C, 0x06, 0x06, 0x06, 0xC6, 0x7C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x0C, 0x1C, 0x3C, 0x6C, 0xCC, 0xFE, 0x0C, 0x0C, 0x0C, 0x1E, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xFE, 0xC0, 0xC0, 0xC0, 0xFC, 0x0E, 0x06, 0x06, 0xC6, 0x7C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x38, 0x60, 0xC0, 0xC0, 0xFC, 0xC6, 0xC6, 0xC6, 0xC6, 0x7C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xFE, 0xC6, 0x06, 0x06, 0x0C, 0x18, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x7C, 0xC6, 0xC6, 0xC6, 0x7C, 0xC6, 0xC6, 0xC6, 0xC6, 0x7C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x7C, 0xC6, 0xC6, 0xC6, 0x7E, 0x06, 0x06, 0x06, 0x0C, 0x78, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x18, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x00, 0x00, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x7C, 0xC6, 0xC6, 0x0C, 0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x7C, 0xC6, 0xC6, 0xDE, 0xDE, 0xDE, 0xDC, 0xC0, 0x7C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x10, 0x38, 0x6C, 0xC6, 0xC6, 0xFE, 0xC6, 0xC6, 0xC6, 0xC6, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xFC, 0x66, 0x66, 0x66, 0x7C, 0x66, 0x66, 0x66, 0x66, 0xFC, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x3C, 0x66, 0xC2, 0xC0, 0xC0, 0xC0, 0xC0, 0xC2, 0x66, 0x3C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xF8, 0x6C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x6C, 0xF8, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xFE, 0x66, 0x62, 0x68, 0x78, 0x68, 0x60, 0x62, 0x66, 0xFE, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xFE, 0x66, 0x62, 0x68, 0x78, 0x68, 0x60, 0x60, 0x60, 0xF0, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x3C, 0x66, 0xC2, 0xC0, 0xC0, 0xDE, 0xC6, 0xC6, 0x66, 0x3A, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xC6, 0xC6, 0xC6, 0xC6, 0xFE, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x3C, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x1E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0xCC, 0xCC, 0xCC, 0x78, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xE6, 0x66, 0x6C, 0x6C, 0x78, 0x78, 0x6C, 0x66, 0x66, 0xE6, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xF0, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x62, 0x66, 0xFE, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xC6, 0xEE, 0xFE, 0xFE, 0xD6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xC6, 0xE6, 0xF6, 0xFE, 0xDE, 0xCE, 0xC6, 0xC6, 0xC6, 0xC6, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x38, 0x6C, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x6C, 0x38, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xFC, 0x66, 0x66, 0x66, 0x7C, 0x60, 0x60, 0x60, 0x60, 0xF0, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x7C, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xD6, 0xDE, 0x7C, 0x0C, 0x0E, 0x00, 0x00,
    0x00, 0x00, 0xFC, 0x66, 0x66, 0x66, 0x7C, 0x6C, 0x66, 0x66, 0x66, 0xE6, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x7C, 0xC6, 0xC6, 0x60, 0x38, 0x0C, 0x06, 0xC6, 0xC6, 0x7C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x7E, 0x7E, 0x5A, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x7C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x6C, 0x38, 0x10, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xD6, 0xD6, 0xFE, 0x6C, 0x6C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xC6, 0xC6, 0x6C, 0x6C, 0x38, 0x38, 0x6C, 0x6C, 0xC6, 0xC6, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xFE, 0xC6, 0x86, 0x0C, 0x18, 0x30, 0x60, 0xC2, 0xC6, 0xFE, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x3C, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0x70, 0x38, 0x1C, 0x0E, 0x06, 0x02, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x3C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x3C, 0x00, 0x00, 0x00, 0x00,
    0x10, 0x38, 0x6C, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00,
    0x30, 0x30, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x0C, 0x7C, 0xCC, 0xCC, 0xCC, 0x76, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xE0, 0x60, 0x60, 0x78, 0x6C, 0x66, 0x66, 0x66, 0x66, 0xDC, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0xC6, 0xC0, 0xC0, 0xC0, 0xC6, 0x7C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x1C, 0x0C, 0x0C, 0x3C, 0x6C, 0xCC, 0xCC, 0xCC, 0xCC, 0x76, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0xC6, 0xFE, 0xC0, 0xC0, 0xC6, 0x7C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x38, 0x6C, 0x64, 0x60, 0xF0, 0x60, 0x60, 0x60, 0x60, 0xF0, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0x7C, 0x0C, 0xCC, 0x78, 0x00,
    0x00, 0x00, 0xE0, 0x60, 0x60, 0x6C, 0x76, 0x66, 0x66, 0x66, 0x66, 0xE6, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x18, 0x18, 0x00, 0x38, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x06, 0x06, 0x00, 0x0E, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x66, 0x66, 0x3C, 0x00,
    0x00, 0x00, 0xE0, 0x60, 0x60, 0x66, 0x6C, 0x78, 0x78, 0x6C, 0x66, 0xE6, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x38, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xEC, 0xFE, 0xD6, 0xD6, 0xD6, 0xD6, 0xD6, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xDC, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x7C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xDC, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7C, 0x60, 0x60, 0xF0, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0x7C, 0x0C, 0x0C, 0x1E, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xDC, 0x76, 0x62, 0x60, 0x60, 0x60, 0xF0, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0xC6, 0x60, 0x38, 0x0C, 0xC6, 0x7C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x10, 0x30, 0x30, 0xFC, 0x30, 0x30, 0x30, 0x30, 0x36, 0x1C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0x76, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xC6, 0xC6, 0xC6, 0xD6, 0xD6, 0xFE, 0x6C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xC6, 0x6C, 0x38, 0x38, 0x38, 0x6C, 0xC6, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x7E, 0x06, 0x0C, 0xF8, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xCC, 0x18, 0x30, 0x60, 0xC6, 0xFE, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x0E, 0x18, 0x18, 0x18, 0x70, 0x18, 0x18, 0x18, 0x18, 0x0E, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x70, 0x18, 0x18, 0x18, 0x0E, 0x18, 0x18, 0x18, 0x18, 0x70, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x76, 0xDC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x10, 0x38, 0x6C, 0xC6, 0xC6, 0xC6, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00
};






