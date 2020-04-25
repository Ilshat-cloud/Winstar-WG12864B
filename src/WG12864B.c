#include "WG12864B.h"

FontDef SysRus5x7 = {5,7,SystemRus5x7};
//----------инициализация-------//
void WG12864_init(void)
{
  WG12864_reset_pin();
  WG12864_send_cmd(Dispay_ON_CMD,3);
  WG12864_Delay();   
  WG12864_send_cmd(Dispay_SetAdrX_CMD,3);
  WG12864_Delay();  
  WG12864_send_cmd(Dispay_SetPageY_CMD,3);
  WG12864_Delay();   
  WG12864_send_cmd(Dispay_SetScrollAdr_CMD,3);
  WG12864_Delay();  
  WG12864_Fill(White,0x03);
}

//------------------------задержка 3 uS----------------------------//
//------------------init 1us Timer base----------------------------//
void WG12864_Delay(void)
{
  uint16_t Time;
  Time=TIM16->CNT;
  while (((TIM16->CNT)-Time)<3){}
}






//----------установить ХУ-------//
void WG12864_SetPos(uint8_t X,uint8_t Y)
{
  if (X<64){
    X=X&0x3F;
    WG12864_send_cmd(Dispay_SetAdrX_CMD|X,1);
    WG12864_send_cmd(Dispay_SetPageY_CMD|Y,1);
  }else{
    X=(X-64)&0x3F;
    WG12864_send_cmd(Dispay_SetAdrX_CMD|X,2);
    WG12864_send_cmd(Dispay_SetPageY_CMD|Y,2);
  }
  
}


//------------------------печатаем строку---------------------------// 
void WG12864_PrintStr(char *str,Color color,uint8_t PosX,uint8_t PosY,FontDef Fount)
{
  uint8_t j=0;
  uint16_t out;
  
  while (*str) 
  {
    
    if(Fount.FontHeight<9)      //для шрифтов в 1 страницу
    {
      for (uint8_t i=0;i<Fount.FontWidth;i++)
      {
        WG12864_SetPos(PosX+i+Fount.FontWidth*j+j,PosY);                                
        if((PosX+i+Fount.FontWidth*j+j)<64)
        {
          out=(str[0]-32)*Fount.FontWidth+i;
          WG12864_send_data(((color==White)?(Fount.data[out]):(~Fount.data[out])),1);
        }else{                  //тут если упремся в край то наплевать
          out=(str[0]-32)*Fount.FontWidth+i;
          WG12864_send_data(((color==White)?(Fount.data[out]):(~Fount.data[out])),2); 
        }  
        
      }
      WG12864_SetPos(PosX+Fount.FontWidth+Fount.FontWidth*j+j,PosY);                                
      if((PosX+Fount.FontWidth+Fount.FontWidth*j+j)<64)
      {
          WG12864_send_data((color==White)?White:Black,1);
      }else{                  //тут если упремся в край то наплевать
          WG12864_send_data((color==White)?White:Black,2); 
      }  
             
    }
    j++;              
    str++;      // Next char
  }

}  
//------------------------Заливаем---------------------------//  
void WG12864_Fill(Color color, uint8_t CS)
{
  for (uint8_t j=0;j<8;j++)
  {
    WG12864_send_cmd(Dispay_SetPageY_CMD|j,CS);
     for(uint8_t i=0;i<64;i++)
    {
      WG12864_send_data(color,CS);
    }
  }
  WG12864_reset_pin();

}

//-----------------рисуем зойдберга и бендера------------------//
void WG12864_CreateZB(void)
{
    for(uint8_t i=0;i<8;i++)
    {
      WG12864_send_cmd(Dispay_SetPageY_CMD|i,3);
      WG12864_send_cmd(Dispay_SetAdrX_CMD,3);
      for (uint8_t j=0;j<128;j++)
      {
        if (j>63){
          WG12864_send_data(zb[2+j+127*i],2);
        } else {
          WG12864_send_data(zb[2+j+127*i],1);
        }
      }
  
    }
}

//-----------------рисуем гору------------------//
void WG12864_CreateMountins(void)
{
  uint8_t i=0,j=0,k=0,m=1;
  for(k=0;k<8;k++)
    {
      m=(m<<(k/2))|m;
      WG12864_send_cmd(Dispay_SetPageY_CMD|k,3);
      for(j=0;j<4;j++)
      {
        for (i=0;i<8;i++)
        {
          WG12864_send_data(m<<i,0x01);
        }
            for (i=0;i<8;i++)
        {
          WG12864_send_data((m<<8)>>(i),0x01);
        }
      }
      for(j=0;j<4;j++)
      {
        for (i=0;i<8;i++)
        {
          WG12864_send_data(m<<i,0x02);
        }
            for (i=0;i<8;i++)
        {
          WG12864_send_data((m<<8)>>(i),0x02);
        }
      } 
    } 
}

//----------Отправка команды-------//
//----------CS-1-first, CS-2 second, CS-0 together-------//
//----------RS-0 команда,-------//
void WG12864_send_cmd(uint8_t DO, uint8_t CS)
{
  HAL_GPIO_WritePin(WG_D0_GPIO_Port,WG_D0_Pin,DO&0x01);
  HAL_GPIO_WritePin(WG_D1_GPIO_Port,WG_D1_Pin,DO&0x02);
  HAL_GPIO_WritePin(WG_D2_GPIO_Port,WG_D2_Pin,DO&0x04);
  HAL_GPIO_WritePin(WG_D3_GPIO_Port,WG_D3_Pin,DO&0x08);
  HAL_GPIO_WritePin(WG_D4_GPIO_Port,WG_D4_Pin,DO&0x10);
  HAL_GPIO_WritePin(WG_D5_GPIO_Port,WG_D5_Pin,DO&0x20);
  HAL_GPIO_WritePin(WG_D6_GPIO_Port,WG_D6_Pin,DO&0x40);
  HAL_GPIO_WritePin(WG_D7_GPIO_Port,WG_D7_Pin,DO&0x80);
  
  HAL_GPIO_WritePin(WG_CS1_GPIO_Port,WG_CS1_Pin,CS&0x01);
  HAL_GPIO_WritePin(WG_CS2_GPIO_Port,WG_CS2_Pin,CS&0x02);
  
  HAL_GPIO_WritePin(WG_E_GPIO_Port,WG_E_Pin,GPIO_PIN_SET);
  WG12864_Delay();                                 
  WG12864_reset_pin();

}


//-------------------reset-------------------------//
void WG12864_reset(void)
{

  HAL_GPIO_WritePin(WG_RST_GPIO_Port,WG_RST_Pin,GPIO_PIN_RESET);
  osDelay(1);                                    
  WG12864_reset_pin();

}
//----------Отправка данных-------//
//----------CS-1-first, CS-2 second, CS-3 together-------//
//----------RS-1 команда,-------//
void WG12864_send_data(uint8_t DO, uint8_t CS)
{
  HAL_GPIO_WritePin(WG_D0_GPIO_Port,WG_D0_Pin,DO&0x01);
  HAL_GPIO_WritePin(WG_D1_GPIO_Port,WG_D1_Pin,DO&0x02);
  HAL_GPIO_WritePin(WG_D2_GPIO_Port,WG_D2_Pin,DO&0x04);
  HAL_GPIO_WritePin(WG_D3_GPIO_Port,WG_D3_Pin,DO&0x08);
  HAL_GPIO_WritePin(WG_D4_GPIO_Port,WG_D4_Pin,DO&0x10);
  HAL_GPIO_WritePin(WG_D5_GPIO_Port,WG_D5_Pin,DO&0x20);
  HAL_GPIO_WritePin(WG_D6_GPIO_Port,WG_D6_Pin,DO&0x40);
  HAL_GPIO_WritePin(WG_D7_GPIO_Port,WG_D7_Pin,DO&0x80);
  
  HAL_GPIO_WritePin(WG_CS1_GPIO_Port,WG_CS1_Pin,CS&0x01);
  HAL_GPIO_WritePin(WG_CS2_GPIO_Port,WG_CS2_Pin,CS&0x02);
  
  HAL_GPIO_WritePin(WG_RS_GPIO_Port,WG_RS_Pin,GPIO_PIN_SET);
  HAL_GPIO_WritePin(WG_E_GPIO_Port,WG_E_Pin,GPIO_PIN_SET);    
  WG12864_Delay(); 
  WG12864_reset_pin();

}


//----------Сброс ножек на наальное положение-------//
//----------rst-1 чтобы не было ресета, rw-0 запись, rs-0 команда-------//

void WG12864_reset_pin(void)
{
  HAL_GPIO_WritePin(WG_RST_GPIO_Port,WG_RST_Pin,GPIO_PIN_SET);
  
  HAL_GPIO_WritePin(WG_CS1_GPIO_Port,WG_CS1_Pin,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(WG_CS2_GPIO_Port,WG_CS2_Pin,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(WG_E_GPIO_Port,WG_E_Pin,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(WG_RW_GPIO_Port,WG_RW_Pin,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(WG_RS_GPIO_Port,WG_RS_Pin,GPIO_PIN_RESET);
  
  HAL_GPIO_WritePin(WG_D0_GPIO_Port,WG_D0_Pin,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(WG_D1_GPIO_Port,WG_D1_Pin,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(WG_D2_GPIO_Port,WG_D2_Pin,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(WG_D3_GPIO_Port,WG_D3_Pin,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(WG_D4_GPIO_Port,WG_D4_Pin,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(WG_D5_GPIO_Port,WG_D5_Pin,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(WG_D6_GPIO_Port,WG_D6_Pin,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(WG_D7_GPIO_Port,WG_D7_Pin,GPIO_PIN_RESET);
  WG12864_Delay(); 
}