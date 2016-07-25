/* ピン番号の定義 (arduino mega用) */
/* 作成開始日 2016.06.27           */

#ifndef PIN_NUMBER_H
#define PIN_NUMBER_H

/* コントローラピン番号 */
#define CON_PIN1 11
#define CON_PIN2 10
#define CON_PIN3 12
#define CON_PIN4 13

/* エンコーダのピン番号 */
#define ENC_RIGHT_A   3   // 右A相のピン
#define ENC_RIGHT_B   30   // 右B相のピン
#define ENC_LEFT_A    2   // 左A相のピン
#define ENC_LEFT_B    32   // 左B相のピン

#define PULLUP 1
#define NC     2

/* モータのピン番号 */
#define MOTOR_RIGHT1    5  //右モータのPWMピン
#define MOTOR_RIGHT2    4  
#define MOTOR_LEFT1     46
#define MOTOR_LEFT2     44

#endif