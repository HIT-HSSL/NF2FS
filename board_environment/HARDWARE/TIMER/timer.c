#include "timer.h"
#include "led.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32H7开发板
//定时器中断驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2017/8/15
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

TIM_HandleTypeDef TIM3_Handler;      //定时器句柄 
TIM_HandleTypeDef TIM6_Handler;         //定时器6句柄 

__IO u32 ticknum=0;

//通用定时器3中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!(定时器3挂在APB1上，时钟为HCLK/2)
void TIM3_Init(u16 arr,u16 psc)
{  
    TIM3_Handler.Instance=TIM3;                          //通用定时器3
    TIM3_Handler.Init.Prescaler=psc;                     //分频
    TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //向上计数器
    TIM3_Handler.Init.Period=arr;                        //自动装载值
    TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&TIM3_Handler);
    
    HAL_TIM_Base_Start_IT(&TIM3_Handler); //使能定时器3和定时器3中断   
}

//通用定时器6初始化
//arr：自动重装值。
//psc：时钟预分频数。
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器6!(定时器6挂在APB1上，时钟为HCLK/2)
void TIM6_Init(u16 arr,u16 psc)
{  
    TIM6_Handler.Instance=TIM6;                          //通用定时器6
    TIM6_Handler.Init.Prescaler=psc;                     //分频
    TIM6_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //向上计数器
    TIM6_Handler.Init.Period=arr;                        //自动装载值
    TIM6_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&TIM6_Handler);
    
    HAL_TIM_Base_Start_IT(&TIM6_Handler); //使能定时器6和定时器6中断   
}

//定时器底册驱动，开启时钟，设置中断优先级
//此函数会被HAL_TIM_Base_Init()函数调用
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    __HAL_RCC_TIM3_CLK_ENABLE();            //使能TIM3时钟
	__HAL_RCC_TIM6_CLK_ENABLE();            //使能TIM6时钟
	
    HAL_NVIC_SetPriority(TIM3_IRQn,1,3);    //设置中断优先级，抢占优先级1，子优先级3
    HAL_NVIC_EnableIRQ(TIM3_IRQn);          //开启ITM3中断   
	
	HAL_NVIC_SetPriority(TIM6_DAC_IRQn,0,3);//设置中断优先级，抢占优先级0，子优先级3
    HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);      //开启ITM6中断 
}

//定时器3中断服务函数
void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM3_Handler);
}

//定时器6中断服务函数
void TIM6_DAC_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM6_Handler);
}

u16 frame;
vu8 frameup;
//定时器3中断服务函数调用
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim==(&TIM3_Handler))       //定时器3
    {
		//ticknum++;
        printf("frame:%d\r\n",frame);//打印帧率
		frame=0;
    }
    else if(htim==(&TIM6_Handler))  //定时器6
    {
        frameup=1;  
    }
}

//获取ticknum
u32 GetTicknum(void)
{
	return ticknum;
}

//开始定时TIM3
void TIM3_Start(void)
{	
	__HAL_TIM_ENABLE(&TIM3_Handler);
	ticknum=0;
}

//关闭定时TIM3
void TIM3_Stop(void)
{	
	__HAL_TIM_DISABLE(&TIM3_Handler);
}
