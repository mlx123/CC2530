 /****************************************** 
     * 基础实验只需要添加以下头文件 
     ******************************************/  
    #include <ioCC2530.h>  
      
    #define uint8 unsigned char   //或typedef unsigned char uint;  
    #define uint16 unsigned int   
      
    /******************************************************** 
     * IAR编译环境中位域字段默认取向采用小端模式， 
     * 配置结构体声明前使用#pragma bitfields=reversed取反向， 
     * 声明结束后恢复IAR默认方式 
     ********************************************************/  
    #pragma bitfields=reversed  
    typedef struct {  
      uint8 SRCADDRH;       //源地址高字节  
      uint8 SRCADDRL;       //源地址低字节  
      uint8 DESTADDRH;       //目的地址高字节  
      uint8 DESTADDRL;       //目的地址低字节  
      uint8 VLEN:3;         //变长传输模式  
      uint8 LENH:5;         //传输长度高字节  
      uint8 LENL:8;         //传输长度低字节  
      uint8 WORDSIZE:1;     //字节传输或字传输  
      uint8 TMODE:2;        //传输模式  
      uint8 TRIG:5;         //触发时间选择  
      uint8 SRCINC:2;       //源地址增量方式选择  
      uint8 DESTINC:2;      //目的地址增量方式选择  
      uint8 IRQMASK:1;      //中断屏蔽位  
      uint8 M8:1;           //字节传输模式时用，8或7bit传输，仅仅适合在字节传输模式下  
      uint8 PRIORITY:2;     //优先级选择  
    }DMA_DESC;  
    #pragma bitfields=default  
      
    #define DATA_AMOUNT 350  
      
    /***************************************************** 
     * CC2530数据手册中对DMA数据结构的介绍，对以下常量赋值 
     ****************************************************/  
    #define DMA_VLEN_USE_LEN  0x00 // Use LEN for transfer count  
      
    #define DMA_WORDSIZE_BYTE 0x00 // Transfer a byte at a time  
    #define DMA_WORDSIZE_WORD 0x01 // Transfer a 16-bit word at a time  
      
    #define DMA_TMODE_SINGLE             0x00 // Transfer a single byte/word after each DMA trigger           单个传输模式  
    #define DMA_TMODE_BLOCK              0x01 // Transfer block of data (length len) after each DMA trigger   块传输模式  
    #define DMA_TMODE_SINGLE_REPEATED    0x02 // Transfer single byte/word (after len transfers, rearm DMA)   重复单个传输模式  
    #define DMA_TMODE_BLOCK_REPEATED     0x03 // Transfer block of data (after len transfers, rearm DMA)      重复块传输模式  
      
    #define DMA_TRIG_NONE                0    // No trigger, setting DMAREQ.DMAREQx bit starts transfer       无触发  
    #define DMA_SRCINC_0                 0x00 // Increment source pointer by 0 bytes/words after each transfer源地址递增0字节/字  
    #define DMA_SRCINC_1                 0x01 // Increment source pointer by 1 bytes/words after each transfer源地址递增1字节/字  
    #define DMA_DESTINC_1                0x01 // Increment destination pointer by 1 bytes/words after each transfer目标地址递增1字节/字  
    #define DMA_DESTINC_0                0x00     


    #define DMA_IRQMASK_DISABLE          0x00 // Disable interrupt generation                                 通道的中断屏蔽  
    #define DMA_IRQMASK_ENABLE           0x01 // Enable interrupt generation upon DMA channel done            通道的中断使能  
      
    #define DMA_M8_USE_8_BITS            0x00 // Use all 8 bits for transfer count                            采用所有8位作为传输长度  
    #define DMA_M8_USE_7_BITS            0x01 // Use 7 LSB for transfer count                                 采用字节地7位作为传输长度  
      
    #define DMA_PRI_LOW                  0x00 // Low, CPU has priority                                        低优先级，CPU优先  
    #define DMA_PRI_GUARANTEED           0x01 // Guaranteed, DMA at least every second try                    保证级，DMA至少在每秒一次的尝试中优先  
    #define DMA_PRI_HIGH                 0x02 // High, DMA has priority                                       高优先级，DMA优先  
    #define DMA_PRI_ABSOLUTE             0x03 // Highest, DMA has priority. Reserved for DMA port access.       
      
    #define DMAARM_DMAARM0               0x01  
    #define DMAREQ_DMAREQ0               0x01    
    #define DMAIRQ_DMAIF0                0x01  
    
      #define DMAARM_DMAARM1              0x02  
    #define DMAREQ_DMAREQ1               0x02    
    #define DMAIRQ_DMAIF1                0x02 


    #define NOP()  asm("NOP")  
      
    //DMA配置参数  
    static DMA_DESC dmaConfig0; 
    static DMA_DESC dmaConfig1;
    static DMA_DESC dmaConfig1_4[4]; 
    //此数据是用来复制到内存的其他区域  
    static char src[DATA_AMOUNT] = "123456789123456789123456789"; 
    //用来保存复制来的数据区域  
    char dst[DATA_AMOUNT];  
    //数据长度  
   void Delay_ms(uint16 ms)  
    {  
      uint16 i,j;  
      for(i = 0; i < ms; i++)  
      {  
        for(j = 0;j < 1774; j++);  
      }  
    }  
   
   
void InitDMA0()//USART0的接收DMA通道
{


      dmaConfig0.SRCADDRH   = (0x70c1>> 8) & 0x00FF;    
      dmaConfig0.SRCADDRL   = (0x70c1) & 0x00FF;       
      dmaConfig0.DESTADDRH = ((uint16)&dst >> 8) & 0x00FF;   
      dmaConfig0.DESTADDRL = ((uint16)&dst) & 0x00FF;        
        
      dmaConfig0.VLEN      = DMA_VLEN_USE_LEN;               
      dmaConfig0.LENH      = (DATA_AMOUNT >> 8) & 0x00FF;    
      dmaConfig0.LENL      = (DATA_AMOUNT) & 0x00FF;         
        
      dmaConfig0.WORDSIZE  = DMA_WORDSIZE_BYTE;  //字节            
      dmaConfig0.TMODE     = DMA_TMODE_SINGLE_REPEATED; //单次传输    
      dmaConfig0.TRIG      =   14;
      
        
      dmaConfig0.SRCINC    = DMA_SRCINC_0;     //源地址增量0              
      dmaConfig0.DESTINC   = DMA_DESTINC_1;     //目的地址增量1             
        
      dmaConfig0.IRQMASK   = DMA_IRQMASK_DISABLE;  //中断 屏蔽         
      dmaConfig0.M8        = DMA_M8_USE_8_BITS;   //           
      dmaConfig0.PRIORITY  = DMA_PRI_HIGH;      //优先级             
       //DMA通道地址配置
      DMA0CFGH = ((uint16)&dmaConfig0 >> 8) & 0x00FF;        
      DMA0CFGL = ((uint16)&dmaConfig0) & 0x00FF;      
      
      
        /*DMA进入工作模式通道0*/  
      DMAARM |= DMAARM_DMAARM0;//为了任何DMA传输能够在该通道上发生，该位必须置1，对于非重复传输模式，一旦完成传送，该位自动清0  
      /*一个通道准备工作(即获得配置数据)的时间需要9个系统时钟*/  
      NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP(); // 9 NOPs  
        
      /*DMA通道0传送请求，即触发DMA传送*/  
      DMAREQ |= DMAREQ_DMAREQ0;//设置为1，激活DMA通道0(与一个触发事件具有相同的效果)，当DMA传输开始清除该位  
        
      /*等待DMA通道0传送完毕*/  
      for (; !(DMAIRQ & DMAIRQ_DMAIF0););//当DMA通道0传送完成，DMAIRQ:DMAIF0位置1,与上DMAIRQ_DMAIF0(0x01)，取非后为0退出循环  
        
      /*清除中断标志*/  
      DMAIRQ = ~DMAIRQ_DMAIF0;  
        
      Delay_ms(5);  
}


void InitDMA1(int length)//USART0的发送DMA通道，还需要改善
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
        
      dmaConfig1.WORDSIZE  = DMA_WORDSIZE_BYTE;  //字节            
      dmaConfig1.TMODE     = DMA_TMODE_SINGLE;//DMA_TMODE_SINGLE_REPEATED; //单次   
      dmaConfig1.TRIG      =   15;
      
        
      dmaConfig1.SRCINC    = DMA_SRCINC_1;     //源地址增量1             
      dmaConfig1.DESTINC   = DMA_DESTINC_0;     //目的地址增量0             
        
      dmaConfig1.IRQMASK   = DMA_IRQMASK_DISABLE;  //中断 屏蔽         
      dmaConfig1.M8        = DMA_M8_USE_8_BITS;   //           
      dmaConfig1.PRIORITY  = DMA_PRI_HIGH;      //优先级             
       //DMA通道地址配置
      DMA1CFGH = ((uint16)&dmaConfig1 >> 8) & 0x00FF;        
      DMA1CFGL = ((uint16)&dmaConfig1) & 0x00FF;      
      
      
        /*DMA进入工作模式通道1*/  
      DMAARM |= DMAARM_DMAARM1;//为了任何DMA传输能够在该通道上发生，该位必须置1，对于非重复传输模式，一旦完成传送，该位自动清0  
      /*一个通道准备工作(即获得配置数据)的时间需要9个系统时钟*/  
      NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP(); // 9 NOPs  
        
      /*DMA通道0传送请求，即触发DMA传送*/  
      DMAREQ |= DMAREQ_DMAREQ1;//设置为1，激活DMA通道0(与一个触发事件具有相同的效果)，当DMA传输开始清除该位  
        
      /*等待DMA通道0传送完毕*/  
      for (; !(DMAIRQ & DMAIRQ_DMAIF1););//当DMA通道0传送完成，DMAIRQ:DMAIF0位置1,与上DMAIRQ_DMAIF0(0x01)，取非后为0退出循环  
        
      /*清除中断标志*/  
      DMAIRQ = ~DMAIRQ_DMAIF0;  
        
      Delay_ms(5);  
}
