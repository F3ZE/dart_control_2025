#include "io.h"
#include "remote.h"


/**
 * @Description: 开启dma双缓冲的函数
 * @Auther: zd
 * @Date: 2021-01-05 23:30:46
 * @param {UART_HandleTypeDef} *huart 	串口结构体指针
 * @param {enumChoose} enidleIT 		是否开始空闲中断
 * @param {uint8_t} * DMAbuffer1		DMA缓冲区1的地址
 * @param {uint8_t} * DMAbuffer2		DMA缓冲区2的地址
 * @param {uint16_t} bufferSize			缓冲深度
 */
void UART_DMA_DualBuffer_Init(UART_HandleTypeDef *huart, enum Choose enidleIT, uint8_t *DMAbuffer1, uint8_t *DMAbuffer2, uint16_t bufferSize)
{
    // 使能DMA串口接收
    SET_BIT(huart->Instance->CR3, USART_CR3_DMAR);
    if (enidleIT)
    {
        __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE); // 使能串口空闲中断
    }
    // 开启DMA双缓冲
    // 在配置DMA中断 双缓冲模式前 需要先将对应的中断回调函数分配到结构体中
    // 将所有的DMA接收中断全部绑定在一个函数中类似于HAL库的中断回调函数
    huart->hdmarx->XferCpltCallback = DMA_DualBuff_M0_CpltCallback;
    huart->hdmarx->XferM1CpltCallback = DMA_DualBuff_M1_CpltCallback;
    huart->hdmarx->XferErrorCallback = DMA_DualBuff_ErrorCallback;
    // 失效dma
    __HAL_DMA_DISABLE(huart->hdmarx);
    if (HAL_DMAEx_MultiBufferStart_IT(huart->hdmarx, (uint32_t)(&huart->Instance->DR),
                                      (uint32_t)DMAbuffer1, (uint32_t)DMAbuffer2, bufferSize) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @Description: 缓冲区0满之后的回调函数
 * @Auther: zd
 * @Date: 2021-01-05 23:32:16
 * @param {DMA_HandleTypeDef} *hdma 	dma结构体指针
 */
void DMA_DualBuff_M0_CpltCallback(DMA_HandleTypeDef *hdma)
{
    if (hdma == RemoteUsart.hdmarx)
    {
        // 将接收完的数据进行处理
        RC_data_is_error();
        SBUS_TO_RC(teleData_Rx[0], &rc_ctrl);
    }
    else if (hdma == JudgeUsart.hdmarx)
    {
        // 裁判系统数据
        // 直接转运进入FIFO缓冲区由task进行解析
//        fifo_s_puts(&referee_fifo, (char *)judge_Rx_Buffer[0], JUDGE_RX_BUFFER_LENGTH);
//        referee_dma_finish_flag = 1;
    }
}

/**
 * @Description: 缓冲区1满之后的回调函数
 * @Auther: zd
 * @Date: 2021-01-05 23:34:50]
 * @param {DMA_HandleTypeDef} *hdma 	dma结构体指针
 */
void DMA_DualBuff_M1_CpltCallback(DMA_HandleTypeDef *hdma)
{
    if (hdma == RemoteUsart.hdmarx)
    {
        // 遥控器信息 由于数据量较小 数据处理在中断中执行
        // 将接收完的数据进行处理
        RC_data_is_error();
        SBUS_TO_RC(teleData_Rx[1], &rc_ctrl);
    }
    else if (hdma == JudgeUsart.hdmarx)
    {
        // 裁判系统数据
        // 直接转运进入FIFO缓冲区由task进行解析
//        fifo_s_puts(&referee_fifo, (char *)judge_Rx_Buffer[1], JUDGE_RX_BUFFER_LENGTH);
//        referee_dma_finish_flag = 1;
    }
}

/**
 * @Description: DMA双缓冲出错之后的处理函数
 * @Auther: zd
 * @Date: 2021-01-05 23:35:44
 * @param {DMA_HandleTypeDef} *hdma 	dma结构体指针
 */
void DMA_DualBuff_ErrorCallback(DMA_HandleTypeDef *hdma)
{
    if (hdma == RemoteUsart.hdmarx)
    {
    }
    else if (hdma == JudgeUsart.hdmarx)
    {
    }
}

/**
 * @Description: 关闭或者开始串口及其接收DMA的通用函数
 * @Auther: zd
 * @Date: 2021-01-13 10:20:33
 * @param {UART_HandleTypeDef} *huart		串口结构体
 * @param {uint16_t} data_Length			开启或者重置时的数据长度
 * @param {enumUARTDMASWITCHTYPE} USECMD	开启关闭还是重置的枚举变量
 */
void Usart_DMA_RX_Reset(UART_HandleTypeDef *huart, uint16_t data_Length, enum UARTDMASWITCHTYPE USECMD)
{
    if (USECMD == DISABLE_IT || USECMD == RESET_IT)
    {
        __HAL_UART_DISABLE(huart);
        __HAL_DMA_DISABLE(huart->hdmarx);
    }

    if (USECMD == ENABLE_IT || USECMD == RESET_IT)
    {
        __HAL_DMA_SET_COUNTER(huart->hdmarx, data_Length);
        __HAL_DMA_ENABLE(huart->hdmarx);
        __HAL_UART_ENABLE(huart);
    }
}

/**
 * @Description: 串口1的空闲中断回调函数 防止出现数据错误
 * @Auther: zd
 * @Date: 2021-01-05 23:37:29
 */
void Remote_Uart_IDLE_CallBack(void)
{
    if (__HAL_UART_GET_FLAG(&RemoteUsart, UART_FLAG_IDLE))
    {
        __HAL_UART_CLEAR_IDLEFLAG(&RemoteUsart);
        uint16_t recieveDataLength = 0;
        // 发现串口1进入空闲中断后 优先查看使用的是哪个缓冲区
        if ((RemoteUsart.hdmarx->Instance->CR & DMA_SxCR_CT) == RESET)
        {
            // 使用的是缓冲区0
            recieveDataLength = RC_DATA_LENGTH - __HAL_DMA_GET_COUNTER(RemoteUsart.hdmarx);
            if (!(recieveDataLength == 0 || recieveDataLength == RC_DATA_LENGTH))
            {
                // 失效DMA
                __HAL_DMA_DISABLE(RemoteUsart.hdmarx);
                // 重设数据长度
                __HAL_DMA_SET_COUNTER(RemoteUsart.hdmarx, RC_DATA_LENGTH);
                // 启动DMA
                __HAL_DMA_ENABLE(RemoteUsart.hdmarx);
            }
            // 关闭dma
        }
        else
        {
            // 使用的是缓冲区1
            recieveDataLength = RC_DATA_LENGTH - __HAL_DMA_GET_COUNTER(RemoteUsart.hdmarx);
            if (!(recieveDataLength == 0 || recieveDataLength == RC_DATA_LENGTH))
            {
                // 失效DMA
                __HAL_DMA_DISABLE(RemoteUsart.hdmarx);
                // 重设数据长度
                __HAL_DMA_SET_COUNTER(RemoteUsart.hdmarx, RC_DATA_LENGTH);
                // 启动DMA
                __HAL_DMA_ENABLE(RemoteUsart.hdmarx);
            }
        }
    }
}

/**
 * GPIO中断回调函数
 * @param[in]   	针脚名.
 * @param[out]  	无.
 */
// 重新定义GPIO的中断回调函数
//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
//    // 此中断是通过内部软件中断触发的，触发引脚为PG0
//    if (GPIO_Pin == IMU_INT_Pin)
//    {
//        // wake up the task
//        // 唤醒任务
//        if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
//        {
//            static BaseType_t xHigherPriorityTaskWoken;
//            vTaskNotifyGiveFromISR(IMU_Task_local_handler, &xHigherPriorityTaskWoken);
//            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
//        }
//    }
//    else if (GPIO_Pin == INT1_ACCEL_Pin)
//    {
//        // detect_hook(BOARD_ACCEL_TOE);
//        accel_update_flag |= 1 << IMU_DR_SHFITS;
//        accel_temp_update_flag |= 1 << IMU_DR_SHFITS;
//        if (imu_start_dma_flag)
//        {
//            imu_cmd_spi_dma();
//        }
//    }
//    else if (GPIO_Pin == INT1_GYRO_Pin)
//    {
//        // detect_hook(BOARD_GYRO_TOE);
//        gyro_update_flag |= 1 << IMU_DR_SHFITS;
//        if (imu_start_dma_flag)
//        {
//            imu_cmd_spi_dma();
//        }
//    }
//    else if (GPIO_Pin == DRDY_IST8310_Pin)
//    {
//        // detect_hook(BOARD_MAG_TOE);
//        mag_update_flag |= 1 << IMU_DR_SHFITS;
//    }
//}
