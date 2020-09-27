//FT6206
/* touch panel interface define */
sbit SDA	   =    P3^0;
sbit SCL       =    P3^1;
sbit PEN       =    P3^2;	
sbit RESET     =    P3^3;		//reset


#define uchar      unsigned char
#define uint       unsigned int
#define ulong      unsigned long

#define WHITE          0xFFFF
#define BLACK          0x0000
#define GRAY           0xF7DE
#define BLUE           0x001F
#define BLUE2          0x051F
#define RED            0xF800
#define PURPLE         0xF81F
#define GREEN          0x07E0
#define CYAN           0x7FFF
#define YELLOW         0xFFE0
#define DGREEN         0x07E0


#define COMMAND_BUFFER_INDEX   				0x20 
#define QUERY_BUFFER_INDEX						0x80
#define COMMAND_RESPONSE_BUFFER_INDEX 0xA0 
#define POINT_BUFFER_INDEX    				0xE0 
#define QUERY_SUCCESS     						0x00 
#define QUERY_BUSY     								0x01 
#define QUERY_ERROR     							0x02 
#define QUERY_POINT     							0x80 


#define POINT 		0x08
#define GESTURES	0x80
#define TAP 				0x20
#define FLICK 			0x22
#define DOUBLE_TAP 	0x23
#define UP					0x08
#define UPPER_RIGHT 0x09
#define RIGHT 			0x0A
#define LOWER_RIGHT 0x0B
#define DOWN			 	0x0C
#define LOWER_LEFT 	0x0D
#define LEFT 				0x0E
#define UPPER_LEFT 	0x0F

uint 	XResolution;
uint 	YResolution;
uchar 	Step;
  

//Touch Status	 
#define Key_Down 0x01
#define Key_Up   0x00 

struct _ts_event
{
    uint    x1;
    uint    y1;
    uint    x2;
    uint    y2;
    uint    x3;
    uint    y3;
    uchar     touch_point;
	uchar     Key_Sta;	
};


struct _ts_event ts_event; 


#define WRITE_ADD	0x8C 
#define READ_ADD	0x8D  

#define ACK		0
#define NACK 	1

void TOUCH_Init(void);
void TOUCH_Start(void);
void TOUCH_Stop(void);
void   TOUCH_Wait_Ack(void);
void TOUCH_Ack(void);
void TOUCH_NAck(void);




void delayus(unsigned int i)
{

    while(i--);

}

//IIC start
void TOUCH_Start(void)
{ 
	SDA=1;  
	_nop_();
	_nop_(); 
	SCL=1;
	_nop_();
	_nop_();
	SDA=0;  
	_nop_();
	_nop_();
	SCL=0;
	_nop_();

}	  


//IIC stop
void TOUCH_Stop(void)
{
	SDA=0;
	_nop_();
	_nop_();
	SCL=1;
	_nop_();
	_nop_();
	SDA=1;
	_nop_();
	_nop_();
	SCL=0;
	_nop_();
						   	
}






//Acknowledge
void TOUCH_Ack(void)
{	SCL=0;
	SDA=0;
	_nop_();
	_nop_();
	SCL=1;
	_nop_();
	_nop_();
	SCL=0;
	_nop_();

}



//NO Acknowledge		    
void TOUCH_NAck(void)
{	SCL=0;
	SDA=1;
	_nop_();
	_nop_();
	SCL=1;
	_nop_();
	_nop_();
	SCL=0;
	_nop_();
}	
	

//IIC send one byte		  
uchar TOUCH_Send_Byte(uchar Byte)
{	uchar t; uchar ucErrTime=0;bit ack=1;
    for(t=0;t<8;t++)
    { 	SCL=0;            
	   	SDA=(bit)(Byte & 0x80) ;
	   	Byte <<=1;
	_nop_();
	   	SCL=1;
	_nop_();
	   	SCL=0;
	_nop_();
    }	
 
	SDA=1;
	_nop_();
	SCL=1;
	_nop_();
	ack=SDA;
  	while(ack)
	{	ack=SDA;
	 	if(ucErrTime++>250)
		{
			TOUCH_Stop();
			return NACK;
		} 
	}
	SCL=0;

  	return ACK;
} 

//Read one byte，ack=0，Send Acknowledge，ack=1，NO Acknowledge   
uchar TOUCH_Read_Byte(uchar ack)
{	uchar t,receive=0;

	SCL=0;
	SDA=1;
	for(t = 0; t < 8; t++)
	{		_nop_();
	 	SCL = 1;
		_nop_();
	 	receive<<=1;
	 	if(SDA == 1)
	 	receive=receive|0x01;
		_nop_();
		_nop_();
	 	SCL=0;
		_nop_();
		_nop_();
	}

					 
   	if (ack)  TOUCH_NAck();//NO Acknowledge 
   	else       TOUCH_Ack(); //Send Acknowledge   
    
	 return receive;
}




 uchar MemoryWriteBuffer(uchar addr, uchar* pdat, uint len) 
{ 

  TOUCH_Start(); 

  if(TOUCH_Send_Byte(WRITE_ADD) == NACK)
	{
			TOUCH_Stop();
			return 0;
	}


  if(TOUCH_Send_Byte(addr) == NACK)
	{
			TOUCH_Stop();
			return 0;
	}

	
	while(len--)
	{
		if(TOUCH_Send_Byte(*pdat++) == NACK)
		{
			TOUCH_Stop();
			return 0;
		}
	}	
	
  TOUCH_Stop();
  return 1;
}



 uchar MemoryReadBuffer(uchar addr, uchar* pdat, uint len) 
{ 
  TOUCH_Start(); 

  if(TOUCH_Send_Byte(WRITE_ADD) == NACK)
	{
		TOUCH_Stop();
		return 0;
	}
	
	if(TOUCH_Send_Byte(addr) == NACK)
	{
	  TOUCH_Stop();
		return 0;
	}
		
	TOUCH_Start(); 
					
	if(TOUCH_Send_Byte(READ_ADD) == NACK)  
	{
		TOUCH_Stop();
		return 0; 
	}
							
	while(--len)
	{
		*pdat++ = TOUCH_Read_Byte(ACK);
	}
							
	*pdat = TOUCH_Read_Byte(NACK);
	
	TOUCH_Stop();
	return 1;
}

 
 void CTP_CheckBusy(void)
{
	uchar query, result;
	uint i=0;
	
	do
	{
		result = MemoryReadBuffer(QUERY_BUFFER_INDEX, &query, 1);
		i++;
		if(i>500)
			return;
			
	}while((query & QUERY_BUSY) || result == 0);
}
  


uchar CTP_IdentifyCapSensor(void)
{
	uchar cmd = 0;
	uchar dat[10];
	
	CTP_CheckBusy();
	if(!MemoryWriteBuffer(COMMAND_BUFFER_INDEX, &cmd, 1)) return 0;

	CTP_CheckBusy();
	if(!MemoryReadBuffer(COMMAND_RESPONSE_BUFFER_INDEX, dat, 10)) return 0;
	
	if(dat[1] == 'I' && dat[2] == 'T' && dat[3] == 'E' && dat[4] == '7' && dat[5] == '2'  && dat[6] == '6'  && dat[7] == '0')
		return 1;
	
	return 0;
	
}



 uchar CTP_SetInterruptNotification(void)
{
	uchar cmd[4] = {0x02, 0x04, 1, 0};
	uchar dat[2];
	
	CTP_CheckBusy();
	if(!MemoryWriteBuffer(COMMAND_BUFFER_INDEX, cmd, 4)) return 0;

	CTP_CheckBusy();
	if(!MemoryReadBuffer(COMMAND_RESPONSE_BUFFER_INDEX, dat, 2)) return 0;
	
	if(*(uint *)dat == 0)
		return 1;
	
	return 0;
}

uchar CTP_GetFirmwareInformation(uchar* info)
{
	uchar cmd[2] = {0x01, 0x00};
	
	CTP_CheckBusy();
	if(!MemoryWriteBuffer(COMMAND_BUFFER_INDEX, cmd, 2)) return 0;

	CTP_CheckBusy();
	if(!MemoryReadBuffer(COMMAND_RESPONSE_BUFFER_INDEX, info, 9)) return 0;
	
	if(info[5]== 0  && info[6] == 0  && info[7] == 0  && info[8] == 0)
			return 0;
	
	return 1;

}


 uchar CTP_Get2DResolutions(uint *pwXResolution, uint *pwYResolution, uchar *pucStep)
{
	uchar cmd[2] = {0x01, 0x02};
	uchar dat[14];
	
	CTP_CheckBusy();
	if(!MemoryWriteBuffer(COMMAND_BUFFER_INDEX, cmd, 2)) return 0;

	CTP_CheckBusy();
	if(!MemoryReadBuffer(COMMAND_RESPONSE_BUFFER_INDEX, dat, 14)) return 0;
	
	if(pwXResolution != NULL)
		*pwXResolution = dat[2] + (dat[3] << 8);

	if(pwYResolution != NULL)
		*pwYResolution = dat[4] + (dat[5] << 8);

	if(pucStep != NULL)
		*pucStep = dat[6];
	
	return 1;
}	






/******************************************************************************************
*Function name：Draw_Big_Point(u16 x,u16 y)
* Parameter：uint x,uint y xy
* Return Value：void
* Function：Draw touch pen nib point
*********************************************************************************************/		   
void Draw_Big_Point(uint x,uint y,uint colour)
{uint num;	    
	
	LCD_SetPos(x-3,y-3,x+3,y+3);
	    for(num=0;num<49;num++)
		{    
          	 Write_Data(colour>>8,colour);
		}
}


void inttostr(uint value,uchar *str)
{
	str[0]=value/1000+48;
	str[1]=value%1000/100+48;
	str[2]=value%100/10+48;
	str[3]=value%100%10+48;

}



////////////////////////////////////
void  counter0(void) interrupt 0
{
 	if(PEN==0)										//Detect the occurrence of an interrupt
 	{
		ts_event.Key_Sta=Key_Down;                              

 	}
}


 uchar CTP_CheckNewPoint(void)
{
	uchar query;
	
	if(!MemoryReadBuffer(QUERY_BUFFER_INDEX, &query, 1)) return 0;

		if (query & QUERY_POINT)  return 1;
}



void read_pointdata() 
{ 
	uchar pointdata[14];
//	if(CTP_CheckNewPoint())
//	{
		if(!MemoryReadBuffer(POINT_BUFFER_INDEX, pointdata, 5)) return;
		
		if(pointdata[0] & POINT)													//point
		{
		  ts_event.x1 = ((pointdata[3] & 0x0F) << 8) + pointdata[2];
			ts_event.y1 = ((pointdata[3] & 0xF0) << 4) + pointdata[4];

		}
		
	/*	if(pointdata[0] & GESTURES)													//GESTURES
		{
//			if(pointdata[1] == TAP ||  pointdata[1] == DOUBLE_TAP)		

			
			if(pointdata[1] == FLICK)																
			{
				switch(pointdata[10])
				{
					case UP:																						
					case UPPER_RIGHT:
						break;
					
					case RIGHT:																					
					case LOWER_RIGHT:
						break;
					
					case DOWN:																				
					case LOWER_LEFT:
						break;
					
					case LEFT:																					
					case UPPER_LEFT:
						break;
				}
			}
		} */
//	}  
}








void IT7259_Init(void)
{  uchar firmwareinfo[9];
	
	RESET=0;
 	delayms(20);
	RESET=1;
  	delayms(500);
	

	if(!CTP_IdentifyCapSensor()) return;	//识别触摸屏
	
//	if(!CTP_GetFirmwareInformation(firmwareinfo)) return; //检测固件状态
	
//	if(!CTP_Get2DResolutions(&XResolution, &YResolution, &Step)) return;	//获取触摸屏分辨率
	
	if(!CTP_SetInterruptNotification()) return; //设置触摸产生下降沿中断
	

}


void TPTEST(void)
{  
	uchar ss[2]=0;	
	 IT0=1;        //Falling edge trigger  
	 EX0=1;
	 EA=1;

   IT7259_Init();

 	ClearScreen(0x0000);					
    showzifustr(110,100,"TOUCH",RED,BLACK);	




	while(KEY)
	{
		if(ts_event.Key_Sta==Key_Down)        //The touch screen is pressed
			{
				EX0=0;//Close interrupt
			
			do
		{	
				
					read_pointdata();
					ts_event.Key_Sta=Key_Up;
		
		/*			inttostr(ts_event.x1,ss);
					showzifustr(40,70,"X1:",BLUE,WHITE);
					showzifustr(70,70,ss,RED,WHITE);	
					inttostr(ts_event.y1,ss);
					showzifustr(40,85,"Y1:",BLUE,WHITE);
					showzifustr(70,85,ss,RED,WHITE);	
			  */

					Draw_Big_Point(ts_event.x1,ts_event.y1,RED);						
                     
				}while(PEN==0);
				EX0=1;
			}


    }



}























