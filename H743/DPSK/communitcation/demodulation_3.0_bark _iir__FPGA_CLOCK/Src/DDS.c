#include "DDS.h"
#include "math.h"
#include "arm_math.h"
#include "adc.h"
#define Fs 1000  //1M
 __IO uint16_t *u16DDS_DataOut = (__IO uint16_t*)0x38000000;
u8 DAC_HALF = 0;
u8 DAC_FULL = 0;
extern float filter_buf[2000];
u16 DDSTable[Table_NUM];
u32 PhaseAccumulator = 0;
u32 PhaseDelta = 0;

void InitSinTable(void)
{
    u32 i;
    for(i = 0; i < Table_NUM; i++)     // 2Π 重新映射 Table_NUM份 产生正弦码表
    {
        float temp = (float)(2 * 3.141592653) / (float)Table_NUM * (float)i;
        DDSTable[i] = 32767.0 + 32767.0 * arm_sin_f32(temp);
    }
}


void DDS_init()
{
    hdma_bdma_generator0.XferCpltCallback         = DMA1_stream0_FULL_Handler;
    hdma_bdma_generator0.XferHalfCpltCallback     = DMA1_stream0_HALF_Handler;
    HAL_DMAEx_EnableMuxRequestGenerator(&hdma_bdma_generator0);
    HAL_DMA_Start_IT(&hdma_bdma_generator0, (uint32_t)u16DDS_DataOut, (uint32_t)&GPIOB->ODR, 1000);

    HAL_LPTIM_PWM_Start(&hlptim2, 100 - 1, 0);		//100M/2 主频
 
}


void DMA1_stream0_HALF_Handler(DMA_HandleTypeDef* hdma)							//半满
{
    //@arg DMA_IT_TC: Transfer complete interrupt mask.
    //@arg DMA_IT_HT: Half transfer complete interrupt mask.
    // 半满
//		if(ADC_HALF)
//		{
	            for(int i = 0; i < 500; i++)
            {
                u16DDS_DataOut[i] = ((int)filter_buf[i] << 4);
//                u16DDS_DataOut[i] = decode[i] ;
				if((u16)filter_buf[i] > 2200)
                    u16DDS_DataOut[i] = 0;
                else
                    u16DDS_DataOut[i] = 65535;
            }
DAC_HALF = 1;

//			SCB_CleanDCache_by_Addr((uint32_t *) &u16DDS_DataOut[0], 1024);

}

void DMA1_stream0_FULL_Handler(DMA_HandleTypeDef* hdma)
{
    // 全满
	            for(int i = 500; i < 1000; i++)
            {
                u16DDS_DataOut[i] = ((int)filter_buf[i] << 4);
				if((u16)filter_buf[i] > 2200)
                    u16DDS_DataOut[i] = 0;
                else
                    u16DDS_DataOut[i] = 65535;

            }
DAC_FULL = 1;


}

