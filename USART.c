#include <ioCC2530.h>
#include"USART.h"
#define uchar unsigned char
#define uint unsigned int
void InitUart();              //初始化串口
void Uart_Send_String(unsigned char *Data,int len);
 char U0Rx[100];
 int U0Rx_i=0;
/**************************************************************** 
   串口初始化函数     
***********************************************************/
void InitUart()
{
    CLKCONCMD &= ~0x40; // 设置系统时钟源为 32MHZ晶振
    while(CLKCONSTA & 0x40);                     // 等待晶振稳定 
    CLKCONCMD &= ~0x47;                          // 设置系统主时钟频率为 32MHZ

  PERCFG&=~0x01;   //有2个备用位置，0使用备用位置1；1使用备用位置2 USART0的IO使用备用位置1
  P0SEL |= 0x0C;   //P0_2 RXD P0_3 TXD 外设功能 0000 1100
  P2DIR &= ~0XC0;                             //P0优先作为UART0    
  U0CSR |= 0xC0;  //串口接收使能  1100 0000 工作UART模式+允许接受
  U0UCR |= 0x00;  //无奇偶校验，1位停止位
 
  U0CSR |= 0x80;//串口设置为UART方式
  U0GCR |= 11;           //U0GCR与U0BAUD配合     
  U0BAUD |= 216;       // 波特率设为115200 
 
  //IEN0 |= 0X04;     //开串口接收中断 'URX0IE = 1',也可以写成 URX0IE=1;
  EA=1;
    
}

void Uart0_SendCh(char ch)
{
    U0DBUF = ch;
    while(UTX0IF == 0);//发送完成后产生发送中断，UTX0IF中断标志位为置位1
    UTX0IF = 0;
}
/**************************************************************** 
串口发送字符串函数    
****************************************************************/ 
void Uart_Send_String(unsigned char *Data,int len) 
{
 { 
  int j; 
  for(j=0;j<len;j++) 
  { 
     Uart0_SendCh(*Data++); 
  } 
 }
}

#pragma vector=URX0_VECTOR
__interrupt void URX0_ISR(void)
{
  
  //进入中断后，失能中断 ，清中断标志位(要从源头开始清中断)
  URX0IE=0;
  /*问：URX0IF是收到一字节数据就会接入终端，还是总线空闲中断
  如果是1字节就会进入中断，那么我发送300个字符应该全部收到
  如果是空闲中断，那么300个字符不会全部收到
  
  实验现象：发送800个字节，产生100次中断，考虑到在中断中清除了中断，所以收到一次就会进入中断
  
  */
  /*此处我去掉了对中断标志位的判断，原因是判断的不太准*/
 // if(URX0IF==1)//收到数据中断，收到1位就进入
  //{
    // Uart0_SendCh(0x0a);
    // Uart0_SendCh(URX0IF);
    //Uart0_SendCh('2');
    //Uart0_SendCh(URX0IF);
    URX0IF=0;
    U0Rx[U0Rx_i]=U0DBUF;
    U0Rx_i++; 
 // }
 // else
  //{
 //   Uart0_SendCh('e');
 // }
  //使能中断
  URX0IE=1;   
}
