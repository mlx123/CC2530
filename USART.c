#include <ioCC2530.h>
#include"USART.h"
#define uchar unsigned char
#define uint unsigned int
void InitUart();              //��ʼ������
void Uart_Send_String(unsigned char *Data,int len);
 char U0Rx[100];
 int U0Rx_i=0;
/**************************************************************** 
   ���ڳ�ʼ������     
***********************************************************/
void InitUart()
{
    CLKCONCMD &= ~0x40; // ����ϵͳʱ��ԴΪ 32MHZ����
    while(CLKCONSTA & 0x40);                     // �ȴ������ȶ� 
    CLKCONCMD &= ~0x47;                          // ����ϵͳ��ʱ��Ƶ��Ϊ 32MHZ

  PERCFG&=~0x01;   //��2������λ�ã�0ʹ�ñ���λ��1��1ʹ�ñ���λ��2 USART0��IOʹ�ñ���λ��1
  P0SEL |= 0x0C;   //P0_2 RXD P0_3 TXD ���蹦�� 0000 1100
  P2DIR &= ~0XC0;                             //P0������ΪUART0    
  U0CSR |= 0xC0;  //���ڽ���ʹ��  1100 0000 ����UARTģʽ+�������
  U0UCR |= 0x00;  //����żУ�飬1λֹͣλ
 
  U0CSR |= 0x80;//��������ΪUART��ʽ
  U0GCR |= 11;           //U0GCR��U0BAUD���     
  U0BAUD |= 216;       // ��������Ϊ115200 
 
  //IEN0 |= 0X04;     //�����ڽ����ж� 'URX0IE = 1',Ҳ����д�� URX0IE=1;
  EA=1;
    
}

void Uart0_SendCh(char ch)
{
    U0DBUF = ch;
    while(UTX0IF == 0);//������ɺ���������жϣ�UTX0IF�жϱ�־λΪ��λ1
    UTX0IF = 0;
}
/**************************************************************** 
���ڷ����ַ�������    
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
  
  //�����жϺ�ʧ���ж� �����жϱ�־λ(Ҫ��Դͷ��ʼ���ж�)
  URX0IE=0;
  /*�ʣ�URX0IF���յ�һ�ֽ����ݾͻ�����նˣ��������߿����ж�
  �����1�ֽھͻ�����жϣ���ô�ҷ���300���ַ�Ӧ��ȫ���յ�
  ����ǿ����жϣ���ô300���ַ�����ȫ���յ�
  
  ʵ�����󣺷���800���ֽڣ�����100���жϣ����ǵ����ж���������жϣ������յ�һ�ξͻ�����ж�
  
  */
  /*�˴���ȥ���˶��жϱ�־λ���жϣ�ԭ�����жϵĲ�̫׼*/
 // if(URX0IF==1)//�յ������жϣ��յ�1λ�ͽ���
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
  //ʹ���ж�
  URX0IE=1;   
}
