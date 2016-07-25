/* モータのpwm制御   */
/* 作成日 2016.06.20 */  

#ifndef MOTOR_H
#define MOTOR_H
 
class MOTOR{

private:

   int M_PIN_PWM1;  //PWM信号ピンの番号
   int M_PIN_PWM2;
     
public:

    MOTOR(int pin1,int pin2)
    {
        M_PIN_PWM1 = pin1;
        M_PIN_PWM2 = pin2;
    }
  
    /* duty : モータのデューティ比 -255 <= duty <= 255   */
    void duty_ratio(int duty)
    {   
        int dire = 1;  
        
        if(duty < 0){  

            dire = 1;

        }else if(duty > 0) dire = -1;
		else if (duty == 0) dire = 0;

		if (duty < 0) duty *= -1;

        if(duty > 255) duty = 255;   
        if(duty < 0) duty = 0;
        
        if(dire == 1){
			analogWrite(M_PIN_PWM1, duty);
			analogWrite(M_PIN_PWM2, 0);
        }else if(dire == -1){
			analogWrite(M_PIN_PWM1, 0);
			analogWrite(M_PIN_PWM2, duty);
		}
		else if (dire == 0){
			analogWrite(M_PIN_PWM1, 0);
			analogWrite(M_PIN_PWM2, 0);
		}
        
    }

    MOTOR& operator=(int duty)
    {
        duty_ratio(duty);

        return *this;
    }
    
};
        
#endif