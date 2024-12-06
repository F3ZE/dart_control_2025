#include "can_receive.h"
#include "string.h"

// 声明电机变量
// static motor_measure_t motor_yaw, motor_fric[4], motor_supply;
// static motor_ecd_measure_t motor_roll;

motor_measure_t motor_fric[4];
motor_ecd_measure_t motor_yaw, motor_supply;

CAN_RxHeaderTypeDef Rx1Message;
CAN_TxHeaderTypeDef Tx1Message;

uint32_t pTxMailbox;

void CAN1_Init()
{
    CAN_FilterTypeDef canfilter;

    canfilter.FilterMode = CAN_FILTERMODE_IDMASK;
    canfilter.FilterScale = CAN_FILTERSCALE_32BIT;

    // filtrate any ID you want here
    canfilter.FilterIdHigh = 0x0000;
    canfilter.FilterIdLow = 0x0000;
    canfilter.FilterMaskIdHigh = 0x0000;
    canfilter.FilterMaskIdLow = 0x0000;

    canfilter.FilterFIFOAssignment = CAN_FilterFIFO0;
    canfilter.FilterActivation = ENABLE;
    canfilter.SlaveStartFilterBank = 14;
    // use different filter for can1&can2
    canfilter.FilterBank = 0;

    HAL_CAN_ConfigFilter(&hcan1, &canfilter);

    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);

    HAL_CAN_Start(&hcan1);
}

void CAN2_Init()
{
    CAN_FilterTypeDef canfilter;

    // canfilter.FilterNumber = 14;
    canfilter.FilterMode = CAN_FILTERMODE_IDMASK;
    canfilter.FilterScale = CAN_FILTERSCALE_32BIT;

    // filtrate any ID you want here
    canfilter.FilterIdHigh = 0x0000;
    canfilter.FilterIdLow = 0x0000;
    canfilter.FilterMaskIdHigh = 0x0000;
    canfilter.FilterMaskIdLow = 0x0000;

    canfilter.FilterFIFOAssignment = CAN_FilterFIFO1;
    canfilter.FilterActivation = ENABLE;
    canfilter.SlaveStartFilterBank = 14;
    // use different filter for can1&can2
    canfilter.FilterBank = 14;

    HAL_CAN_ConfigFilter(&hcan2, &canfilter);

    HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO1_MSG_PENDING);

    HAL_CAN_Start(&hcan2);
}

const motor_ecd_measure_t *get_Yaw_Gimbal_Motor_Measure_Point(void)
{
    return &motor_yaw;
}

/* const motor_measure_t *get_Pitch_Gimbal_Motor_Measure_Point(void)
{
	return &motor_pit;
} */

const motor_measure_t *get_Friction_Motor_Measure_Point(uint8_t i)
{
    return &motor_fric[(i & 0x03)];
}

motor_ecd_measure_t *get_Supply_Motor_Measure_Point(void)
{
    return &motor_supply;
}

/* const motor_ecd_measure_t *get_Rolling_Motor_Measure_Point(void)
{
	return &motor_roll;
} */

/* const motor_measure_t *get_Chassis_Measure_Point(uint8_t i)
{
	return &motor_chassis[(i & 0x03)];
} */

static void CAN1_Getdata(CAN_RxHeaderTypeDef *pHeader, uint8_t aData[])
{

    switch (pHeader->StdId)
    {
        case CAN_YAW_MOTOR_ID:
        {
            get_ecdmotor_measure(&motor_yaw, aData);
            break;
        }
        case CAN_SUPPLY_MOTOR_ID:
        {
            get_ecdmotor_measure(&motor_supply, aData);
            break;
        }
            // case CAN_FRICTION_LEFT_1_ID:
            // {
            // 	get_motor_measure(&motor_fric[0], aData);
            // 	break;
            // }
        case CAN_FRICTION_RIGHT_1_ID:
        {
            get_motor_measure(&motor_fric[3], aData);
            break;
        }
        case CAN_FRICTION_LEFT_2_ID:
        {
            get_motor_measure(&motor_fric[1], aData);
            break;
        }
            // case CAN_FRICTION_RIGHT_2_ID:
            // {
            // 	get_motor_measure(&motor_fric[2], aData);
            // 	break;
            // }
            /* 	case CAN_3508_M1_ID:
                case CAN_3508_M2_ID:
                case CAN_3508_M3_ID:
                case CAN_3508_M4_ID:
                {
                    static uint8_t i = 0;
                    // 处理电机ID号
                    i = pHeader->StdId - CAN_3508_M1_ID;
                    // 处理电机数据
                    get_motor_measure(&motor_chassis[i], aData);
                    break;
                } */
        default:
        {
            break;
        }
    }
}
/**
 * CAN2的数据处理 依据STDID的编号确定是哪个电机传来的信息数据
 * @param[in]   	CAN的接收结构体，输入的数据.
 * @param[out]  	无.
 * @par 修改日志
 *      		zd于2020-12-01注释
 */
static void CAN2_Getdata(CAN_RxHeaderTypeDef *pHeader, uint8_t aData[])
{
    switch (pHeader->StdId)
    {
        /* 	case CAN_PIT_MOTOR_ID:
            {
                // 处理云台电机数据
                get_motor_measure(&motor_pit, aData);
                break;
            }
            case CAN_ROLLING_MOTOR_ID:
            {
                get_motor_measure(&motor_roll.motor_measure, aData);
                Ecd_Value_Calculate(&motor_roll);
                break;
            } */
        // case CAN_SUPPLY_MOTOR_ID:
        // {
        // 	get_ecdmotor_measure(&motor_supply, aData);
        // 	break;
        // }
        default:
        {
            break;
        }
    }
}
/**
 * FIFO——0的中断回调函数，FIFO--0分配给can1使用
 * @param[in]   	can结构体.
 * @param[out]  	无.
 * @par 修改日志
 *      		zd于2020-12-01创建
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    uint8_t aData[8];
    if (hcan->Instance == CAN1)
    {
        if (HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &Rx1Message, aData) != HAL_OK)
        {
            /* Reception Error */
            Error_Handler();
        }
        CAN1_Getdata(&Rx1Message, aData);
    }
}
/**
 * FIFO——1的中断回调函数，FIFO--1分配给can2使用
 * @param[in]   	can结构体.
 * @param[out]  	无.
 * @par 修改日志
 *      		zd于2020-12-01创建
 */
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    uint8_t aData[8];
    if (hcan->Instance == CAN2)
    {
        if (HAL_CAN_GetRxMessage(&hcan2, CAN_RX_FIFO1, &Rx1Message, aData) != HAL_OK)
        {
            /* Reception Error */
            Error_Handler();
        }
        CAN2_Getdata(&Rx1Message, aData);
    }
}

void get_motor_measure(motor_measure_t *motor, uint8_t aData[])
{
    motor->last_ecd = motor->ecd;
    motor->ecd = aData[0] << 8 | aData[1];
    motor->speed_rpm = aData[2] << 8 | aData[3];
    motor->given_current = aData[4] << 8 | aData[5];
    motor->temperate = aData[6];
}

void get_ecdmotor_measure(motor_ecd_measure_t *ecdmotor, uint8_t aData[])
{
    ecdmotor->motor_measure.last_ecd = ecdmotor->motor_measure.ecd;
    ecdmotor->motor_measure.ecd = aData[0] << 8 | aData[1];
    ecdmotor->motor_measure.speed_rpm = aData[2] << 8 | aData[3];
    ecdmotor->motor_measure.given_current = aData[4] << 8 | aData[5];
    ecdmotor->motor_measure.temperate = aData[6];
    Ecd_Value_Calculate(ecdmotor);
}

/* void CAN_CMD_Chassis(int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4)
{
	static uint8_t TxData[8];
	Tx1Message.StdId = 0x200;
	Tx1Message.IDE = CAN_ID_STD;
	Tx1Message.RTR = CAN_RTR_DATA;
	Tx1Message.DLC = 0x08;

	TxData[0] = iq1 >> 8;
	TxData[1] = iq1;
	TxData[2] = iq2 >> 8;
	TxData[3] = iq2;
	TxData[4] = iq3 >> 8;
	TxData[5] = iq3;
	TxData[6] = iq4 >> 8;
	TxData[7] = iq4;

	HAL_CAN_AddTxMessage(&hcan1, &Tx1Message, TxData, &pTxMailbox);
} */
void CAN_CMD_Shoot(int16_t friction_left_1, int16_t friction_right_1, int16_t friction_left_2, int16_t friction_right_2)
{
    static uint8_t TxData[8];
    Tx1Message.StdId = 0x200;
    Tx1Message.IDE = CAN_ID_STD;
    Tx1Message.RTR = CAN_RTR_DATA;
    Tx1Message.DLC = 0x08;

    TxData[0] = friction_left_1 >> 8;
    TxData[1] = friction_left_1;
    TxData[6] = friction_right_1 >> 8;
    TxData[7] = friction_right_1;
    TxData[2] = friction_left_2 >> 8;
    TxData[3] = friction_left_2;
    TxData[4] = friction_right_2 >> 8;
    TxData[5] = friction_right_2;

    HAL_CAN_AddTxMessage(&hcan1, &Tx1Message, TxData, &pTxMailbox);
}

void CAN_CMD_Yaw(int16_t yaw)
{
    static uint8_t TxData[8];
    Tx1Message.StdId = 0x1FF;
    Tx1Message.IDE = CAN_ID_STD;
    Tx1Message.RTR = CAN_RTR_DATA;
    Tx1Message.DLC = 0x08;

    TxData[2] = yaw >> 8;
    TxData[3] = yaw;

    HAL_CAN_AddTxMessage(&hcan1, &Tx1Message, TxData, &pTxMailbox);
}

/* void CAN_CMD_Pitch(int16_t pitch)
{
	static uint8_t TxData[8];
	Tx1Message.StdId = 0x1FF;
	Tx1Message.IDE = CAN_ID_STD;
	Tx1Message.RTR = CAN_RTR_DATA;
	Tx1Message.DLC = 0x08;

	TxData[0] = pitch >> 8;
	TxData[1] = pitch;
	HAL_CAN_AddTxMessage(&hcan2, &Tx1Message, TxData, &pTxMailbox);
}
 */
void CAN_CMD_Supply(int16_t supply)
{
    static uint8_t TxData[8];
    Tx1Message.StdId = 0x200;
    Tx1Message.IDE = CAN_ID_STD;
    Tx1Message.RTR = CAN_RTR_DATA;
    Tx1Message.DLC = 0x08;

    TxData[2] = supply >> 8;
    TxData[3] = supply;

    HAL_CAN_AddTxMessage(&hcan1, &Tx1Message, TxData, &pTxMailbox);
}

// void Vision_Data_Send(int16_t vision_yaw, int16_t vision_pitch, int8_t distance_mode, int8_t fire)
//{
//	static uint8_t TxData[8];
//	Tx1Message.StdId = 0x630;
//	Tx1Message.IDE = CAN_ID_STD;
//	Tx1Message.RTR = CAN_RTR_DATA;
//	Tx1Message.DLC = 0x08;

//	TxData[0] = vision_yaw >> 8;
//	TxData[1] = vision_yaw;
//	TxData[2] = vision_pitch >> 8;
//	TxData[3] = vision_pitch;
//	TxData[4] = distance_mode;
//	TxData[5] = fire;

//	HAL_CAN_AddTxMessage(&hcan2, &Tx1Message, TxData, &pTxMailbox);
//}

// void Ecd_Value_Reset_Rolling(void)
// {
// 	motor_roll.EcdPosition = 0;
// }

/**
 * 电机Ecd值闭环计算
 * @param[in]   	有Ecd参数的电机使用.
 * @param[out]  	无.
 * @par 修改日志
 *      		zd于2020-12-03创建
 */
void Ecd_Value_Calculate(motor_ecd_measure_t *motor_ecd)
{
    int16_t Ecd_Dvalue = motor_ecd->motor_measure.ecd - motor_ecd->motor_measure.last_ecd;

    if (Ecd_Dvalue > ECDDVALE_MAX)
    {
        Ecd_Dvalue = -FULLECD + Ecd_Dvalue;
    }
    else if (Ecd_Dvalue < -ECDDVALE_MAX)
    {
        Ecd_Dvalue = Ecd_Dvalue + FULLECD;
    }

    motor_ecd->EcdPosition += ROLLINGDIRECTION * Ecd_Dvalue;

    if (motor_ecd->EcdPosition >= 0x7FFFFFFFFFF00000)
    {
        motor_ecd->EcdPosition -= 0x7FFFFFFFFFF00000;
    }
    else if (motor_ecd->EcdPosition <= -0x7FFFFFFFFFF00000)
    {
        motor_ecd->EcdPosition += 0x7FFFFFFFFFF00000;
    }
}
