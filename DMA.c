 /****************************************** 
     * ����ʵ��ֻ��Ҫ�������ͷ�ļ� 
     ******************************************/  
    #include <ioCC2530.h>  
      
    #define uint8 unsigned char   //��typedef unsigned char uint;  
    #define uint16 unsigned int   
      
    /******************************************************** 
     * IAR���뻷����λ���ֶ�Ĭ��ȡ�����С��ģʽ�� 
     * ���ýṹ������ǰʹ��#pragma bitfields=reversedȡ���� 
     * ����������ָ�IARĬ�Ϸ�ʽ 
     ********************************************************/  
    #pragma bitfields=reversed  
    typedef struct {  
      uint8 SRCADDRH;       //Դ��ַ���ֽ�  
      uint8 SRCADDRL;       //Դ��ַ���ֽ�  
      uint8 DESTADDRH;       //Ŀ�ĵ�ַ���ֽ�  
      uint8 DESTADDRL;       //Ŀ�ĵ�ַ���ֽ�  
      uint8 VLEN:3;         //�䳤����ģʽ  
      uint8 LENH:5;         //���䳤�ȸ��ֽ�  
      uint8 LENL:8;         //���䳤�ȵ��ֽ�  
      uint8 WORDSIZE:1;     //�ֽڴ�����ִ���  
      uint8 TMODE:2;        //����ģʽ  
      uint8 TRIG:5;         //����ʱ��ѡ��  
      uint8 SRCINC:2;       //Դ��ַ������ʽѡ��  
      uint8 DESTINC:2;      //Ŀ�ĵ�ַ������ʽѡ��  
      uint8 IRQMASK:1;      //�ж�����λ  
      uint8 M8:1;           //�ֽڴ���ģʽʱ�ã�8��7bit���䣬�����ʺ����ֽڴ���ģʽ��  
      uint8 PRIORITY:2;     //���ȼ�ѡ��  
    }DMA_DESC;  
    #pragma bitfields=default  
      
    #define DATA_AMOUNT 350  
      
    /***************************************************** 
     * CC2530�����ֲ��ж�DMA���ݽṹ�Ľ��ܣ������³�����ֵ 
     ****************************************************/  
    #define DMA_VLEN_USE_LEN  0x00 // Use LEN for transfer count  
      
    #define DMA_WORDSIZE_BYTE 0x00 // Transfer a byte at a time  
    #define DMA_WORDSIZE_WORD 0x01 // Transfer a 16-bit word at a time  
      
    #define DMA_TMODE_SINGLE             0x00 // Transfer a single byte/word after each DMA trigger           ��������ģʽ  
    #define DMA_TMODE_BLOCK              0x01 // Transfer block of data (length len) after each DMA trigger   �鴫��ģʽ  
    #define DMA_TMODE_SINGLE_REPEATED    0x02 // Transfer single byte/word (after len transfers, rearm DMA)   �ظ���������ģʽ  
    #define DMA_TMODE_BLOCK_REPEATED     0x03 // Transfer block of data (after len transfers, rearm DMA)      �ظ��鴫��ģʽ  
      
    #define DMA_TRIG_NONE                0    // No trigger, setting DMAREQ.DMAREQx bit starts transfer       �޴���  
    #define DMA_SRCINC_0                 0x00 // Increment source pointer by 0 bytes/words after each transferԴ��ַ����0�ֽ�/��  
    #define DMA_SRCINC_1                 0x01 // Increment source pointer by 1 bytes/words after each transferԴ��ַ����1�ֽ�/��  
    #define DMA_DESTINC_1                0x01 // Increment destination pointer by 1 bytes/words after each transferĿ���ַ����1�ֽ�/��  
    #define DMA_DESTINC_0                0x00     


    #define DMA_IRQMASK_DISABLE          0x00 // Disable interrupt generation                                 ͨ�����ж�����  
    #define DMA_IRQMASK_ENABLE           0x01 // Enable interrupt generation upon DMA channel done            ͨ�����ж�ʹ��  
      
    #define DMA_M8_USE_8_BITS            0x00 // Use all 8 bits for transfer count                            ��������8λ��Ϊ���䳤��  
    #define DMA_M8_USE_7_BITS            0x01 // Use 7 LSB for transfer count                                 �����ֽڵ�7λ��Ϊ���䳤��  
      
    #define DMA_PRI_LOW                  0x00 // Low, CPU has priority                                        �����ȼ���CPU����  
    #define DMA_PRI_GUARANTEED           0x01 // Guaranteed, DMA at least every second try                    ��֤����DMA������ÿ��һ�εĳ���������  
    #define DMA_PRI_HIGH                 0x02 // High, DMA has priority                                       �����ȼ���DMA����  
    #define DMA_PRI_ABSOLUTE             0x03 // Highest, DMA has priority. Reserved for DMA port access.       
      
    #define DMAARM_DMAARM0               0x01  
    #define DMAREQ_DMAREQ0               0x01    
    #define DMAIRQ_DMAIF0                0x01  
    
      #define DMAARM_DMAARM1              0x02  
    #define DMAREQ_DMAREQ1               0x02    
    #define DMAIRQ_DMAIF1                0x02 


    #define NOP()  asm("NOP")  
      
    //DMA���ò���  
    static DMA_DESC dmaConfig0; 
    static DMA_DESC dmaConfig1;
    static DMA_DESC dmaConfig1_4[4]; 
    //���������������Ƶ��ڴ����������  
    static char src[DATA_AMOUNT] = "123456789123456789123456789"; 
    //�������渴��������������  
    char dst[DATA_AMOUNT];  
    //���ݳ���  
   void Delay_ms(uint16 ms)  
    {  
      uint16 i,j;  
      for(i = 0; i < ms; i++)  
      {  
        for(j = 0;j < 1774; j++);  
      }  
    }  
   
   
void InitDMA0()//USART0�Ľ���DMAͨ��
{


      dmaConfig0.SRCADDRH   = (0x70c1>> 8) & 0x00FF;    
      dmaConfig0.SRCADDRL   = (0x70c1) & 0x00FF;       
      dmaConfig0.DESTADDRH = ((uint16)&dst >> 8) & 0x00FF;   
      dmaConfig0.DESTADDRL = ((uint16)&dst) & 0x00FF;        
        
      dmaConfig0.VLEN      = DMA_VLEN_USE_LEN;               
      dmaConfig0.LENH      = (DATA_AMOUNT >> 8) & 0x00FF;    
      dmaConfig0.LENL      = (DATA_AMOUNT) & 0x00FF;         
        
      dmaConfig0.WORDSIZE  = DMA_WORDSIZE_BYTE;  //�ֽ�            
      dmaConfig0.TMODE     = DMA_TMODE_SINGLE_REPEATED; //���δ���    
      dmaConfig0.TRIG      =   14;
      
        
      dmaConfig0.SRCINC    = DMA_SRCINC_0;     //Դ��ַ����0              
      dmaConfig0.DESTINC   = DMA_DESTINC_1;     //Ŀ�ĵ�ַ����1             
        
      dmaConfig0.IRQMASK   = DMA_IRQMASK_DISABLE;  //�ж� ����         
      dmaConfig0.M8        = DMA_M8_USE_8_BITS;   //           
      dmaConfig0.PRIORITY  = DMA_PRI_HIGH;      //���ȼ�             
       //DMAͨ����ַ����
      DMA0CFGH = ((uint16)&dmaConfig0 >> 8) & 0x00FF;        
      DMA0CFGL = ((uint16)&dmaConfig0) & 0x00FF;      
      
      
        /*DMA���빤��ģʽͨ��0*/  
      DMAARM |= DMAARM_DMAARM0;//Ϊ���κ�DMA�����ܹ��ڸ�ͨ���Ϸ�������λ������1�����ڷ��ظ�����ģʽ��һ����ɴ��ͣ���λ�Զ���0  
      /*һ��ͨ��׼������(�������������)��ʱ����Ҫ9��ϵͳʱ��*/  
      NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP(); // 9 NOPs  
        
      /*DMAͨ��0�������󣬼�����DMA����*/  
      DMAREQ |= DMAREQ_DMAREQ0;//����Ϊ1������DMAͨ��0(��һ�������¼�������ͬ��Ч��)����DMA���俪ʼ�����λ  
        
      /*�ȴ�DMAͨ��0�������*/  
      for (; !(DMAIRQ & DMAIRQ_DMAIF0););//��DMAͨ��0������ɣ�DMAIRQ:DMAIF0λ��1,����DMAIRQ_DMAIF0(0x01)��ȡ�Ǻ�Ϊ0�˳�ѭ��  
        
      /*����жϱ�־*/  
      DMAIRQ = ~DMAIRQ_DMAIF0;  
        
      Delay_ms(5);  
}


void InitDMA1(int length)//USART0�ķ���DMAͨ��������Ҫ����
{


      dmaConfig1.SRCADDRH   = ((uint16)&src >> 8) & 0x00FF;     
      dmaConfig1.SRCADDRL   = ((uint16)&src) & 0x00FF;        
      
      dmaConfig1.DESTADDRH   = (0x70c1>> 8) & 0x00FF;    
      dmaConfig1.DESTADDRL   = (0x70c1) & 0x00FF;   
      
      /*dmaConfig1.DESTADDRH   = ((uint16)&dst >> 8) & 0x00FF;   
      dmaConfig1.DESTADDRL   = ((uint16)&dst >> 8) & 0x00FF; */
      
      dmaConfig1.VLEN      = DMA_VLEN_USE_LEN;               
      dmaConfig1.LENH      = (length >> 8) & 0x00FF;    
      dmaConfig1.LENL      = (length) & 0x00FF;         
        
      dmaConfig1.WORDSIZE  = DMA_WORDSIZE_BYTE;  //�ֽ�            
      dmaConfig1.TMODE     = DMA_TMODE_SINGLE;//DMA_TMODE_SINGLE_REPEATED; //����   
      dmaConfig1.TRIG      =   15;
      
        
      dmaConfig1.SRCINC    = DMA_SRCINC_1;     //Դ��ַ����1             
      dmaConfig1.DESTINC   = DMA_DESTINC_0;     //Ŀ�ĵ�ַ����0             
        
      dmaConfig1.IRQMASK   = DMA_IRQMASK_DISABLE;  //�ж� ����         
      dmaConfig1.M8        = DMA_M8_USE_8_BITS;   //           
      dmaConfig1.PRIORITY  = DMA_PRI_HIGH;      //���ȼ�             
       //DMAͨ����ַ����
      DMA1CFGH = ((uint16)&dmaConfig1 >> 8) & 0x00FF;        
      DMA1CFGL = ((uint16)&dmaConfig1) & 0x00FF;      
      
      
        /*DMA���빤��ģʽͨ��1*/  
      DMAARM |= DMAARM_DMAARM1;//Ϊ���κ�DMA�����ܹ��ڸ�ͨ���Ϸ�������λ������1�����ڷ��ظ�����ģʽ��һ����ɴ��ͣ���λ�Զ���0  
      /*һ��ͨ��׼������(�������������)��ʱ����Ҫ9��ϵͳʱ��*/  
      NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP(); // 9 NOPs  
        
      /*DMAͨ��0�������󣬼�����DMA����*/  
      DMAREQ |= DMAREQ_DMAREQ1;//����Ϊ1������DMAͨ��0(��һ�������¼�������ͬ��Ч��)����DMA���俪ʼ�����λ  
        
      /*�ȴ�DMAͨ��0�������*/  
      for (; !(DMAIRQ & DMAIRQ_DMAIF1););//��DMAͨ��0������ɣ�DMAIRQ:DMAIF0λ��1,����DMAIRQ_DMAIF0(0x01)��ȡ�Ǻ�Ϊ0�˳�ѭ��  
        
      /*����жϱ�־*/  
      DMAIRQ = ~DMAIRQ_DMAIF0;  
        
      Delay_ms(5);  
}
