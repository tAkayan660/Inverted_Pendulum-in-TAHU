/* モータのpwm制御   */
/* 作成者 植木 文弘  */
/* 作成日 2016.06.20 */  

#ifndef MOTOR_H
#define MOTOR_H
 
class MOTOR{

private:

   int M_PWM_PIN;  //PWM信号ピンの番号
   int M_PIN1;     //デジタルピン番号
   int M_PIN2;     //デジタルピン番号
     
public:

    /* コンストラクタ */
    MOTOR(int pin1,int pin2,int pwm_pin)
    {
        M_PWM_PIN = pwm_pin;
        M_PIN1 = pin1;
        M_PIN2 = pin2;
        
        pinMode(M_PIN1,OUTPUT);
        pinMode(M_PIN2,OUTPUT);
          
    }
  
    /* モータのデューティ比の入力                     */
    /* pwm_pin : pwm信号を送るピン番号                */
    /* duty : モータのデューティ比 -255 <= duty <= 255   */
    void duty_ratio(int duty)
    {   
        int dire = 1;  // モータの向きを決める
        
        if(duty < 0){  //　向きを指定する
            duty *= -1;
            dire = 1;
        }else if(duty >= 0) dire = -1;

        if(duty > 255) duty = 255;   // 255以上255未満にしないようにする
        if(duty < -255) duty = -255;
        
        if(dire == 1){
            digitalWrite(M_PIN1,HIGH);
            digitalWrite(M_PIN2,LOW);
        }else if(dire == -1){
             digitalWrite(M_PIN1,LOW);
             digitalWrite(M_PIN2,HIGH);
        }
        
        analogWrite(M_PWM_PIN,duty);
        
    }

    MOTOR& operator=(int duty)
    {
        duty_ratio(duty);

        return *this;
    }
    
};
        
#endif