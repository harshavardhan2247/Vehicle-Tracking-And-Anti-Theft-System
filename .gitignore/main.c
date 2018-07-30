#include stm32f4xx.h
#include stm32f4xx_gpio.h
#include stm32f4xx_rcc.h
#include stm32f4xx_usart.h
#include misc.h
#include stdio.h
#include stdlib.h
#include string.h

#define MAX_STRLEN 10

void gpio_config();
void gpiokey();
void USART_config();
void Mes2Usart1 (char ptr);
void USART1_SendChar (char ch);
void delay(long time);
void usart2_configure();
void Mes2Usart2 (char ptr);
void USART2_SendChar (char ch);

GPIO_InitTypeDef gpio;
GPIO_InitTypeDef key;
GPIO_InitTypeDef usart;
USART_InitTypeDef usart1, USART2_InitStruct;
NVIC_InitTypeDef NVIC1_InitStruct,NVIC2_InitStruct;

int a=0;
volatile unsigned char flg = 0, flg1 = 0;
char received_string[MAX_STRLEN+1], received_SIM[MAX_STRLEN+1], data_sms[20+1], data_sms1[40+1], data_gps[80+1];
char gps[9] = {'+', 'C', 'G', 'N', 'S', 'I', 'N', 'F', '0'};
char ref[7] = {'+', 'C', 'M', 'T', 'I', '', '0'};
char ref1[7] = {'+', 'C', 'M', 'G', 'R', '', '0'};
volatile unsigned char rflg = 0, rflg1 = 0, rflg2 = 0, rflg3 = 0, rgps = 0, smsflg = 0, sndgps = 0, sndsms = 0;
char index1[3];
char ctrlz = 0x1A;
volatile int cnt = 0, cnt1 = 0, var=0, var1=0, var2=0;
char buff[20];
int main(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB  RCC_AHB1Periph_GPIOA  RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	gpio_config();
	gpiokey();
	USART_config();
	usart2_configure();

	Mes2Usart2(ATrn);
	if(flg1)
	{
	  Mes2Usart1(received_SIM);
	  Mes2Usart1(rn);
	  cnt1 = 0;
	  flg1 = 0;
	}
	Mes2Usart2(AT+CGMIrn); Request Manufacturer.
	if(flg1)
	{
	  Mes2Usart1(received_SIM);
	  Mes2Usart1(rn);
	  cnt1 = 0;
	  flg1 = 0;
	}
	Mes2Usart2(AT+CMGF=1rn);
	if(flg1)
	{
	  Mes2Usart1(received_SIM);
	  Mes2Usart1(rn);
	  cnt1 = 0;
	  flg1 = 0;
	}
	Mes2Usart2(AT+CGNSPWR=1rn); GPS initialization
	if(flg1)
	{
		Mes2Usart1(received_SIM);
		Mes2Usart1(rn);
		cnt1 = 0;
		flg1 = 0;
	}

    while(1)
    {
    	Mes2Usart1(hello worldrn);
    	     delay(1000);

    	     if(flg1)
    	     {
    	      Mes2Usart1(received_SIM);
    	      Mes2Usart1(rn);
    	      cnt1 = 0;
    	      flg1 = 0;
    	      if (rflg)
    	      {
    	    	  USART1_SendChar(data_sms[12]);
    	    	  index1[0] = data_sms[12];
    	    	  index1[2] = '0';
    	    	  if (data_sms[13] != 'r')
    	    	  {
					index1[1] = data_sms[13];
    	    	  }
    	    	  else
    	    	  {
					index1[1] = '0';
    	    	  }
				sprintf(buff, AT+CMGR=%srn, index1);
				Mes2Usart2(buff);
    	      }
    	      rflg = 0;
    	      rflg1 = 0;
			  var = 0;
    	      if(smsflg)
    	      {
    	    	  Mes2Usart1(SMS READrn);
				Mes2Usart1(data_sms1);
				Mes2Usart1(rn);
				if(strstr(data_sms1, TRACK))
				{
					sndgps = 1;
				}
				if(strstr(data_sms1, STOP))
				{
					Mes2Usart1(Fuel cut off.rn);
					GPIO_ToggleBits(GPIOB, GPIO_Pin_0);
				}
				sprintf(buff, AT+CMGD=%srn, index1);
				Mes2Usart2(buff);
    	      }
    	      smsflg = 0;
    	      rflg2 = 0;
    	      var1 = 0;
    	     }
    	if (flg)
    	  {
    	    Mes2Usart1 (received_string);
    	    Mes2Usart1(rn);
    	    flg = 0;
    	    cnt = 0;
    	  }
    	if(sndgps)
    	{
    		if(!rgps)
    		{
    			Mes2Usart2(AT+CGNSINFrn);
    		}

    		if (rgps)
    		{
    			Mes2Usart2(AT+CMGS=7014384260rn);
    		}
    		rgps = 0;
    		rflg3 = 0;
    		var2 = 0;
    		if(sndsms)
    		{
    			sndsms = 0;
    			Mes2Usart2(data_gps);
				USART2_SendChar(ctrlz);
				sndgps = 0;
    		}
    	}
    	delay(1000);
    	USART1_SendChar('a');
    }
}

void delay(long time)
{
	for(int i = 0; i  1000; i++)
	for(int j = 0; j  time; j++);
}

void gpio_config ()
{
	gpio.GPIO_Pin = GPIO_Pin_0;
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB,&gpio);              &gpio gives address of GPIO_InitTypeDef.
}

void gpiokey()
{
	key.GPIO_Pin = GPIO_Pin_6;
	key.GPIO_Mode = GPIO_Mode_OUT;
	key.GPIO_OType = GPIO_OType_PP;
	key.GPIO_PuPd = GPIO_PuPd_NOPULL;
	key.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOE,&key);
	GPIO_ResetBits(GPIOE, GPIO_Pin_6);
}

void USART_config()
{
	usart.GPIO_Pin = GPIO_Pin_9  GPIO_Pin_10  GPIO_Pin_2  GPIO_Pin_3;
	usart.GPIO_Mode = GPIO_Mode_AF;
	usart.GPIO_OType = GPIO_OType_PP;
	usart.GPIO_PuPd = GPIO_PuPd_UP;
	usart.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA,&usart);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

	usart1.USART_BaudRate = 115200;
	usart1.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart1.USART_Mode = USART_Mode_Tx  USART_Mode_Rx;
	usart1.USART_Parity = USART_Parity_No;
	usart1.USART_StopBits = USART_StopBits_1;
	usart1.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &usart1);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

   	 NVIC1_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	 NVIC1_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	 NVIC1_InitStruct.NVIC_IRQChannelSubPriority = 0;
	 NVIC1_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	 NVIC_Init(&NVIC1_InitStruct);

	USART_Cmd(USART1, ENABLE);
}

void Mes2Usart1 (char ptr)
{
	while (ptr)
	USART1_SendChar(ptr++);
}

void USART1_SendChar (char ch)
{
  USART_SendData(USART1, ch);
  while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
}

void USART1_IRQHandler(void)
{
  char t = (char)USART_ReceiveData(USART1);
  if (flg) return;    if buffer is full, do not accept any new chars

  if (t != 'r' && t != 'n')
  {
    received_string[cnt] = t;
    if (cnt  MAX_STRLEN) cnt++;
  }
     else
  {
      received_string[cnt]='0';
   flg = 1;
   Mes2Usart1(received_string);
  }
}

void usart2_configure()
{
 USART2_InitStruct.USART_BaudRate = 9600;
 USART2_InitStruct.USART_WordLength = USART_WordLength_8b;
 USART2_InitStruct.USART_StopBits = USART_StopBits_1;
 USART2_InitStruct.USART_Parity = USART_Parity_No;
 USART2_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
 USART2_InitStruct.USART_Mode = USART_Mode_Rx  USART_Mode_Tx;
 USART_Init(USART2, &USART2_InitStruct);

 USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

 NVIC2_InitStruct.NVIC_IRQChannel = USART2_IRQn;
 NVIC2_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
 NVIC2_InitStruct.NVIC_IRQChannelSubPriority = 0;
 NVIC2_InitStruct.NVIC_IRQChannelCmd = ENABLE;
 NVIC_Init(&NVIC2_InitStruct);

 USART_Cmd(USART2, ENABLE);
}

void Mes2Usart2 (char ptr)
{
 while (ptr)
 USART2_SendChar(ptr++);
}

void USART2_SendChar (char ch)
{
 while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
  USART_SendData(USART2, (uint16_t)ch);
  while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
}

void USART2_IRQHandler(void)
{
		received_SIM[cnt1] = (char)USART_ReceiveData(USART2);

		if(received_SIM[cnt1] == '')
		{
			sndsms = 1;
		}
		if((received_SIM[cnt1] == ref[var])  (rflg1))
		{
			data_sms[var] = received_SIM[cnt1];
			var++;
			data_sms[var] = '0';
			if(var == 4)
			{
				rflg1 = 1;
			}
		}
		else
		{
			var = 0;
			data_sms[var] = '0';
		}
		if((received_SIM[cnt1] == ref1[var1])  (rflg2))
		{
			data_sms1[var1] = received_SIM[cnt1];
			var1++;
			data_sms1[var1] = '0';
			if(var1 == 5)
			{
				rflg2 = 1;
			}
		}
		else
		{
			var1 = 0;
			data_sms1[var1] = '0';
		}
		if((received_SIM[cnt1] == gps[var2])  (rflg3))
		{
			data_gps[var2] = received_SIM[cnt1];
			var2++;
			data_gps[var2] = '0';
			if(var2 == 8)
			{
				rflg3 = 1;
			}
		}
		else
		{
			var2 = 0;
			data_gps[var2] = '0';

		}
		cnt1++;

	  received_SIM[cnt1] = '0';

	  if(cnt1 == MAX_STRLEN)
	  {
		  if(rflg1)
			  rflg = 1;
		  if(rflg2)
			  smsflg = 1;
		  if(rflg3)
			  rgps = 1;
		  flg1 = 1;
	  }

}
