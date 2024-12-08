#ifndef IO_H
#define IO_H

#include "main.h"
#include "usart.h"

// 宏定义 将将串口抽象
#define RemoteUsart huart3
//#define BlueUsart huart6
#define JudgeUsart huart6

// 枚举体定义
enum Choose
{
    NO = 0,
    YES
};
enum UARTDMASWITCHTYPE
{
    ENABLE_IT = 0,
    DISABLE_IT,
    RESET_IT
};

// 由于使用了dma双缓冲 需要自己设计中断回调函数
extern void DMA_DualBuff_M0_CpltCallback(DMA_HandleTypeDef *hdma);
extern void DMA_DualBuff_M1_CpltCallback(DMA_HandleTypeDef *hdma);
extern void DMA_DualBuff_ErrorCallback(DMA_HandleTypeDef *hdma);
// 配置DMA双缓冲中断的外部函数
void UART_DMA_Irregula_Length_Init(UART_HandleTypeDef *huart, uint8_t *DMAbuffer, uint16_t bufferSize);
void UART_DMA_DualBuffer_Init(UART_HandleTypeDef *huart, enum Choose enidleIT, uint8_t *DMAbuffer1, uint8_t *DMAbuffer2, uint16_t bufferSize);
// 开关串口的函数
void Usart_DMA_RX_Reset(UART_HandleTypeDef *huart, uint16_t data_Length, enum UARTDMASWITCHTYPE USECMD);
#define Usart_DMA_RX_ON(huart, data_Length) Usart_DMA_RX_Reset(huart, data_Length, ENABLE_IT)
#define Usart_DMA_RX_OFF(huart) Usart_DMA_RX_Reset(huart, 0, DISABLE_IT)
// 中断回调函数
void Remote_Uart_IDLE_CallBack(void);

#endif
