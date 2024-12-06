#ifndef REMOTE_H
#define REMOTE_H

/* ----------------------- RC Switch Definition----------------------------- */
#define RC_UP ((uint8_t)1)
#define RC_MID ((uint8_t)3)
#define RC_DOWN ((uint8_t)2)

/*-------------------------PC Key Definition---------------------------------*/
#define KEY_PRESSED_OFFSET_W ((uint16_t)0x01 << 0)
#define KEY_PRESSED_OFFSET_S ((uint16_t)0x01 << 1)
#define KEY_PRESSED_OFFSET_A ((uint16_t)0x01 << 2)
#define KEY_PRESSED_OFFSET_D ((uint16_t)0x01 << 3)

#define KEY_PRESSED_OFFSET_SHIFT ((uint16_t)0x01 << 4)
#define KEY_PRESSED_OFFSET_CTRL ((uint16_t)0x01 << 5)
#define KEY_PRESSED_OFFSET_Q ((uint16_t)0x01 << 6)
#define KEY_PRESSED_OFFSET_E ((uint16_t)0x01 << 7)

#define KEY_PRESSED_OFFSET_R ((uint16_t)0x01 << 8)
#define KEY_PRESSED_OFFSET_F ((uint16_t)0x01 << 9)
#define KEY_PRESSED_OFFSET_G ((uint16_t)0x01 << 10)
#define KEY_PRESSED_OFFSET_Z ((uint16_t)0x01 << 11)
#define KEY_PRESSED_OFFSET_X ((uint16_t)0x01 << 12)
#define KEY_PRESSED_OFFSET_C ((uint16_t)0x01 << 13)
#define KEY_PRESSED_OFFSET_V ((uint16_t)0x01 << 14)
#define KEY_PRESSED_OFFSET_B ((uint16_t)0x01 << 15)

#define RC_DATA_LENGTH 18u

#include "main.h"

/* ----------------------- Data Struct ------------------------------------- */
typedef __packed struct
{
        __packed struct
        {
                int16_t ch[4];
                char sleft;
                char sright;
                int16_t wheel;
        } rc;
        __packed struct
        {
                int16_t x;
                int16_t y;
                int16_t z;
                uint8_t press_l;
                uint8_t press_r;
        } mouse;
        __packed struct
        {
                uint16_t v;
        } key;

        } RC_ctrl_t;

extern RC_ctrl_t rc_ctrl;
extern uint8_t RC_flag;

/*-----------------���ң��������״̬-----------------------*/
#define IF_RC_LEFT_UP (rc_ctrl.rc.sleft == RC_UP)
#define IF_RC_LEFT_MID (rc_ctrl.rc.sleft == RC_MID)
#define IF_RC_LEFT_DOWN (rc_ctrl.rc.sleft == RC_DOWN)
#define IF_RC_RIGHT_UP (rc_ctrl.rc.sright == RC_UP)
#define IF_RC_RIGHT_MID (rc_ctrl.rc.sright == RC_MID)
#define IF_RC_RIGHT_DOWN (rc_ctrl.rc.sright == RC_DOWN)

/*-----------------��ȡ���������ƶ��ٶ�-------------------*/
#define MOUSE_X_MOVE_SPEED (rc_ctrl.mouse.x)
#define MOUSE_Y_MOVE_SPEED (rc_ctrl.mouse.y)
#define MOUSE_Z_MOVE_SPEED (rc_ctrl.mouse.z)

/*-----------------�����갴��״̬---------------------------*/
#define IF_MOUSE_PRESSED_LEFT (rc_ctrl.mouse.press_l == 1)
#define IF_MOUSE_PRESSED_RIGHT (rc_ctrl.mouse.press_r == 1)

/*-----------------�����̰���״̬---------------------------*/
#define IF_KEY_PRESSED (rc_ctrl.key.v)
#define IF_KEY_PRESSED_W ((rc_ctrl.key.v & KEY_PRESSED_OFFSET_W) != 0)
#define IF_KEY_PRESSED_S ((rc_ctrl.key.v & KEY_PRESSED_OFFSET_S) != 0)
#define IF_KEY_PRESSED_A ((rc_ctrl.key.v & KEY_PRESSED_OFFSET_A) != 0)
#define IF_KEY_PRESSED_D ((rc_ctrl.key.v & KEY_PRESSED_OFFSET_D) != 0)
#define IF_KEY_PRESSED_Q ((rc_ctrl.key.v & KEY_PRESSED_OFFSET_Q) != 0)
#define IF_KEY_PRESSED_E ((rc_ctrl.key.v & KEY_PRESSED_OFFSET_E) != 0)
#define IF_KEY_PRESSED_G ((rc_ctrl.key.v & KEY_PRESSED_OFFSET_G) != 0)
#define IF_KEY_PRESSED_X ((rc_ctrl.key.v & KEY_PRESSED_OFFSET_X) != 0)
#define IF_KEY_PRESSED_Z ((rc_ctrl.key.v & KEY_PRESSED_OFFSET_Z) != 0)
#define IF_KEY_PRESSED_C ((rc_ctrl.key.v & KEY_PRESSED_OFFSET_C) != 0)
#define IF_KEY_PRESSED_B ((rc_ctrl.key.v & KEY_PRESSED_OFFSET_B) != 0)
#define IF_KEY_PRESSED_V ((rc_ctrl.key.v & KEY_PRESSED_OFFSET_V) != 0)
#define IF_KEY_PRESSED_F ((rc_ctrl.key.v & KEY_PRESSED_OFFSET_F) != 0)
#define IF_KEY_PRESSED_R ((rc_ctrl.key.v & KEY_PRESSED_OFFSET_R) != 0)
#define IF_KEY_PRESSED_CTRL ((rc_ctrl.key.v & KEY_PRESSED_OFFSET_CTRL) != 0)
#define IF_KEY_PRESSED_SHIFT ((rc_ctrl.key.v & KEY_PRESSED_OFFSET_SHIFT) != 0)

/*****************************************************************/
extern uint8_t teleData_Rx[2][18];

extern const RC_ctrl_t *get_remote_control_point(void);

extern uint8_t RC_data_is_error(void);
void SBUS_TO_RC(uint8_t *sbus_buf, RC_ctrl_t *rc_ctrl);

#endif
