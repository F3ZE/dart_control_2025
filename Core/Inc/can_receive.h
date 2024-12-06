#ifndef CAN_RECEIVE_H
#define CAN_RECEIVE_H

#include "can.h"

#define FULLECD 8192
#define ECDDVALE_MAX FULLECD / 2
#define ROLLINGDIRECTION CLOCKWISE

enum eDIRECTION
{
  ANTICLOCKWISE = +1,
  CLOCKWISE = -1
};
/* CAN send and receive ID */
typedef enum
{
  // CAN 1设备ID
  /* CAN_CHASSIS_ALL_ID = 0x200,
  CAN_3508_M1_ID = 0x201,
  CAN_3508_M2_ID = 0x202,
  CAN_3508_M3_ID = 0x203,
  CAN_3508_M4_ID = 0x204, */
  // CAN_FRICTION_LEFT_1_ID = 0x201,
  CAN_FRICTION_RIGHT_1_ID = 0x204,
  CAN_FRICTION_LEFT_2_ID = 0x201,
  // CAN_FRICTION_RIGHT_2_ID = 0x203,

  CAN_SUPPLY_MOTOR_ID = 0x202,

  CAN_YAW_MOTOR_ID = 0x206,

  // CAN_SUPERCAP_ID = 0x211,

  // CAN 2设备ID
  // CAN_PIT_MOTOR_ID = 0x205,

  // CAN_ROLLING_MOTOR_ID = 0x203,

} can_msg_id_e;

// rm电机统一数据结构体
typedef struct
{
  uint16_t ecd;
  int16_t speed_rpm;
  int16_t given_current;
  uint8_t temperate;
  int16_t last_ecd;
} motor_measure_t;

typedef struct
{
  motor_measure_t motor_measure;
  int64_t EcdPosition;
} motor_ecd_measure_t;

extern CAN_RxHeaderTypeDef Rx1Message;

void CAN1_Init(void);
void CAN2_Init(void);
void get_motor_measure(motor_measure_t *motor, uint8_t aData[]);
void get_ecdmotor_measure(motor_ecd_measure_t *ecdmotor, uint8_t aData[]);
// extern void Ecd_Value_Reset_Rolling(void);
void Ecd_Value_Calculate(motor_ecd_measure_t *motor_ecd);

extern const motor_ecd_measure_t *get_Yaw_Gimbal_Motor_Measure_Point(void);
extern const motor_measure_t *get_Pitch_Gimbal_Motor_Measure_Point(void);
extern const motor_measure_t *get_Friction_Motor_Measure_Point(uint8_t i);
extern const motor_ecd_measure_t *get_Rolling_Motor_Measure_Point(void);
extern const motor_measure_t *get_Chassis_Measure_Point(uint8_t i);
extern motor_ecd_measure_t *get_Supply_Motor_Measure_Point(void);

// extern void CAN_CMD_Chassis(int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4);
extern void CAN_CMD_Shoot(int16_t friction_left_1, int16_t friction_right_1, int16_t friction_left_2, int16_t friction_right_2);
extern void CAN_CMD_Supply(int16_t supply);
extern void CAN_CMD_Yaw(int16_t yaw);
// extern void CAN_CMD_Pitch(int16_t pitch);
extern void Vision_Data_Send(int16_t vision_yaw, int16_t vision_pitch, int8_t distance_mode, int8_t fire);

// 中断函数全局声明
extern void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
extern void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan);

#endif
