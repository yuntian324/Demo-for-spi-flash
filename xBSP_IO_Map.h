#ifndef HH__XBSP_IO_MAP__HH
#define HH__XBSP_IO_MAP__HH

//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+0X14) //0x40020814 //F1:+0X0C;F2:+0X14;F4:+0X14
#define GPIOB_ODR_Addr    (GPIOB_BASE+0X14) //0x40020C14 
#define GPIOC_ODR_Addr    (GPIOC_BASE+0X14) //0x40021014 
#define GPIOD_ODR_Addr    (GPIOD_BASE+0X14) //0x40021414 
#define GPIOE_ODR_Addr    (GPIOE_BASE+0X14) //0x40021814 
#define GPIOF_ODR_Addr    (GPIOF_BASE+0X14) //0x40021A14    
#define GPIOG_ODR_Addr    (GPIOG_BASE+0X14) //0x40021E14    

#define GPIOA_IDR_Addr    (GPIOA_BASE+0X10) //0x40020810 //F1:+0X08;F2:+0X10;F4:+0X10
#define GPIOB_IDR_Addr    (GPIOB_BASE+0X10) //0x40020C10 
#define GPIOC_IDR_Addr    (GPIOC_BASE+0X10) //0x40021010 
#define GPIOD_IDR_Addr    (GPIOD_BASE+0X10) //0x40021410 
#define GPIOE_IDR_Addr    (GPIOE_BASE+0X10) //0x40021810 
#define GPIOF_IDR_Addr    (GPIOF_BASE+0X10) //0x40021A10 
#define GPIOG_IDR_Addr    (GPIOG_BASE+0X10) //0x40021E10 
 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入



#endif


