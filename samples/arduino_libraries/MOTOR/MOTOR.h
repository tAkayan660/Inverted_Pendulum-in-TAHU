/* ���[�^��pwm����   */
/* �쐬�� �A�� ���O  */
/* �쐬�� 2016.06.20 */  

#ifndef MOTOR_H
#define MOTOR_H
 
class MOTOR{

private:

   int M_PWM_PIN;  //PWM�M���s���̔ԍ�
   int M_PIN1;     //�f�W�^���s���ԍ�
   int M_PIN2;     //�f�W�^���s���ԍ�
     
public:

    /* �R���X�g���N�^ */
    MOTOR(int pin1,int pin2,int pwm_pin)
    {
        M_PWM_PIN = pwm_pin;
        M_PIN1 = pin1;
        M_PIN2 = pin2;
        
        pinMode(M_PIN1,OUTPUT);
        pinMode(M_PIN2,OUTPUT);
          
    }
  
    /* ���[�^�̃f���[�e�B��̓���                     */
    /* pwm_pin : pwm�M���𑗂�s���ԍ�                */
    /* duty : ���[�^�̃f���[�e�B�� -255 <= duty <= 255   */
    void duty_ratio(int duty)
    {   
        int dire = 1;  // ���[�^�̌��������߂�
        
        if(duty < 0){  //�@�������w�肷��
            duty *= -1;
            dire = 1;
        }else if(duty >= 0) dire = -1;

        if(duty > 255) duty = 255;   // 255�ȏ�255�����ɂ��Ȃ��悤�ɂ���
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