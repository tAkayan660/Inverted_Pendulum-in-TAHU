/* 状態フィードバック制御用 */
/* 作成開始日 2016.07.14    */

#include "PIN_NUMBER.h"
#include "FlexiTimer2.h"
#include "PS2X_lib.h" 
#include "MOTOR.h"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

#define USE_ENC_H  // 自作ライブラリを使用するか 未定義:使用しない Encoder.hを使用する場合はここをコメントにする。
#define SHOW_ENC_PULSE
#define USE_ATTACH_ENC //エンコーダに割り込みを使う ただし、12カウント
//#define NO_USE_ATTACH_ENC 
//#define SHOW_IMU

#ifdef USE_ENC_H
  #include "ENC.h"
#else
  #include "Encoder.h"
#endif

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  #include "Wire.h"
#endif

/* 状態フィードバックゲイン */
#define F1 59.7331
#define F2 18.5993
#define F3 -25.7582
#define F4 3.7104
#define G  -0.1391
#define KP -50
#define KI 0
#define KD 0
#define T 0.01 // サンプリングタイム 
#define PULSE_TO_METER 0.000491
/* MPU-6050 */
#define OUTPUT_READABLE_YAWPITCHROLL
#define INTERRUPT_PIN 19  // use pin 2 on Arduino Uno & most boards

bool blinkState = false;

PS2X ps2x;
MOTOR motor_right(MOTOR_RIGHT1,MOTOR_RIGHT2);
MOTOR motor_left(MOTOR_LEFT1,MOTOR_LEFT2);

#ifdef USE_ENC_H
  ENC enc_right(ENC_RIGHT_A,ENC_RIGHT_B,PULLUP);
  ENC enc_left(ENC_LEFT_A,ENC_LEFT_B,PULLUP);
#else
  Encoder enc_right(ENC_RIGHT_A,ENC_RIGHT_B);
  Encoder enc_left(ENC_LEFT_A,ENC_LEFT_B);
#endif

MPU6050 mpu;

int old_pulse = 0;
long positionLeft = -999;
long positionRight = -999;
int error = 0; 
byte vibrate = 0;
int right_pulse = 0;  // エンコーダのパルス
int left_pulse = 0;
int duty = 0;         // モータのデューティ比
float x = 0,dx = 0,x0 = 0,rx = 0.0;
float e = 0,ed = 0,theta = 0,dtheta = 0,theta0 = 0;
int ref_speed = 0.0;

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

// packet structure for InvenSense teapot demo
uint8_t teapotPacket[14] = { '$', 0x02, 0,0, 0,0, 0,0, 0,0, 0x00, 0x00, '\r', '\n' };

void right_enc(void)
{
  enc_right.encode();
}

void left_enc(void)
{
  enc_left.encode();
}

void CONTROLER(void)
{
  ps2x.read_gamepad(false, vibrate);

  if(ps2x.Button(PSB_START)) //スタートボタン
    Serial.println("Start");
    
  if(ps2x.Button(PSB_SELECT)) //セレクトボタン
    Serial.println("Select");

  if(ps2x.Button(PSB_PAD_UP)) //上ボタン
    Serial.println("U");
     
  if(ps2x.Button(PSB_PAD_RIGHT)) //右ボタン 
    Serial.println("R"); 
  if(ps2x.Button(PSB_PAD_LEFT)) //左ボタン
    Serial.println("L"); 
  if(ps2x.Button(PSB_PAD_DOWN)) //下ボタン
    Serial.println("D"); 

  if(ps2x.Button(PSB_GREEN)) //三角ボタン
    Serial.println("G"); 
  if(ps2x.Button(PSB_RED)) //丸ボタン
    Serial.println("O"); 
  if(ps2x.Button(PSB_PINK)) //四角ボタン
    Serial.println("P"); 
  if(ps2x.Button(PSB_BLUE)) //エックスボタン
    Serial.println("X");

  if(ps2x.Button(PSB_L3))
    Serial.println("L3");
  if(ps2x.Button(PSB_R3))
    Serial.println("R3");
  if(ps2x.Button(PSB_L2))
    Serial.println("L2");
  if(ps2x.Button(PSB_R2))
    Serial.println("R2"); 
  if(ps2x.Button(PSB_R1))
    Serial.println("R1");

  if(ps2x.Button(PSB_L1)) // Ｌ１を押しながら、アナログスチックコントロール
  {
    Serial.print(ps2x.Analog(PSS_LY), DEC); 
    Serial.print(",");
    Serial.print(ps2x.Analog(PSS_LX), DEC); 
    Serial.print(",");
    Serial.print(ps2x.Analog(PSS_RY), DEC); 
    Serial.print(",");
    Serial.println(ps2x.Analog(PSS_RX), DEC); 
  } 

}

/* 状態フィードバック制御 */
void STATE_FEEDBACK_CONTROLL(void)
{
  #ifdef USE_ENC_H
  right_pulse = enc_right.get_pulses();
  left_pulse = enc_left.get_pulses();
  #endif
  
  enc_right.reset();
  enc_left.reset();
  
  x = (float)(left_pulse - right_pulse) / 2 * PULSE_TO_METER; 
  
  dx = (x - x0) / T;
  x0 = x;
  theta = 0.03+ypr[1];
  dtheta = ed;
  theta0 = theta;
   
  duty =( x * F1 + dx * F2 + theta * F3 + dtheta * F4 + rx * G ) * 255;
  //duty = ( KP * theta + KD * dtheta )*255;
  if(duty > 255) duty = 255;
  if(duty < -255) duty = -255;
  
  motor_right = -duty;
  motor_left = duty;
    
}

// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high

void dmpDataReady() {
    mpuInterrupt = true;
}

void setup(void)
{
  // join I2C bus (I2Cdev library doesn't do this automatically)
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
      Wire.begin();
      Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
      Fastwire::setup(400, true);
  #endif
    
  Serial.begin(115200);
  while (!Serial);

  error = ps2x.config_gamepad(CON_PIN1,CON_PIN2,CON_PIN3,CON_PIN4,true,true);
 
  if(error == 0){
    Serial.println("0 Found Controller, configured successful");
  }

  else if(error == 1)
    Serial.println("1 No controller found, check wiring, see readme.txt to enable debug. ");

  else if(error == 2)
    Serial.println("2 Controller found but not accepting commands. see readme.txt to enable debug. ");

  else if(error == 3)
    Serial.println("3 Controller refusing to enter Pressures mode, may not support it. ");

//Serial.print(ps2x.Analog(1), HEX);
   Serial.println(" PS-C2 Controller Found OK! "); //ＰＳ－２Ｃコントローラー 

  /* エンコーダの割り込み */
  #ifdef USE_ATTACH_ENC
    attachInterrupt(1,right_enc,RISING);
    attachInterrupt(1,right_enc,FALLING);
    attachInterrupt(0,left_enc,RISING);
    attachInterrupt(0,left_enc,FALLING);
  #endif
   
   FlexiTimer2::set(10,STATE_FEEDBACK_CONTROLL);
  //FlexiTimer2::set(10,CONTROLER);
   FlexiTimer2::start();

  // initialize device
   Serial.println(F("Initializing I2C devices..."));
   mpu.initialize();
   pinMode(INTERRUPT_PIN, INPUT);

    // verify connection
   Serial.println(F("Testing device connections..."));
   Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

    // wait for ready// initialize device
   Serial.println(F("Initializing I2C devices..."));
   mpu.initialize();
   pinMode(INTERRUPT_PIN, INPUT);

    // verify connection
   Serial.println(F("Testing device connections..."));
   Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

    // wait for ready
   Serial.println(F("\nSend any character to begin DMP programming and demo: "));
    

    // load and configure the DMP
   Serial.println(F("Initializing DMP..."));
   devStatus = mpu.dmpInitialize();

    // supply your own gyro offsets here, scaled for min sensitivity
   mpu.setXGyroOffset(220);
   mpu.setYGyroOffset(76);
   mpu.setZGyroOffset(-85);
   mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

    // make sure it worked (returns 0 if so)
   if (devStatus == 0) {
       // turn on the DMP, now that it's ready
       Serial.println(F("Enabling DMP..."));
       mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
       Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
       attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
       mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
       Serial.println(F("DMP ready! Waiting for first interrupt..."));
       dmpReady = true;

       // get expected DMP packet size for later comparison
       packetSize = mpu.dmpGetFIFOPacketSize();
   } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
       Serial.print(F("DMP Initialization failed (code "));
       Serial.print(devStatus);
       Serial.println(F(")"));
   }

    // configure LED for output
    Serial.println(F("\nSend any character to begin DMP programming and demo: "));
    

    // load and configure the DMP
    Serial.println(F("Initializing DMP..."));
    devStatus = mpu.dmpInitialize();

    // supply your own gyro offsets here, scaled for min sensitivity
    mpu.setXGyroOffset(220);
    mpu.setYGyroOffset(76);
    mpu.setZGyroOffset(-85);
    mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        Serial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
        attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        Serial.println(F("DMP ready! Waiting for first interrupt..."));
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
    }

}

void loop(void)
{ 
  #ifdef NO_USE_ATTACH_ENC
    enc_right.enc_count();
    enc_left.enc_count();
  #endif
  
  #ifndef USE_ENC_H
    right_pulse = enc_right.read(); 
    left_pulse = enc_left.read();
  #endif
  
  
  /* MPU-6050 */
  // if programming failed, don't try to do anything
    if (!dmpReady) return;

    // wait for MPU interrupt or extra packet(s) available
    while (!mpuInterrupt && fifoCount < packetSize) {
        // other program behavior stuff here
        // .
        // .
        // .
        // if you are really paranoid you can frequently test in between other
        // stuff to see if mpuInterrupt is true, and if so, "break;" from the
        // while() loop to immediately process the MPU data
        // .
        // .
        // .
    }

    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();

    // get current FIFO count
    fifoCount = mpu.getFIFOCount();

    // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
        // reset so we can continue cleanly
        mpu.resetFIFO();
        Serial.println(F("FIFO overflow!"));

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
    } else if (mpuIntStatus & 0x02) {
        // wait for correct available data length, should be a VERY short wait
        while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

        // read a packet from FIFO
        mpu.getFIFOBytes(fifoBuffer, packetSize);
        
        // track FIFO count here in case there is > 1 packet available
        // (this lets us immediately read more without waiting for an interrupt)
        fifoCount -= packetSize;

        #ifdef OUTPUT_READABLE_YAWPITCHROLL
            // display Euler angles in degrees
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
            #ifdef SHOW_IMU
              Serial.print("ypr\t");
              Serial.println(ypr[1] );;
            //Serial.println(duty);;
            #endif
        #endif

        // blink LED to indicate activity
        blinkState = !blinkState;
    }

  #ifdef SHOW_ENC_PULSE
    if(old_pulse != left_pulse)
     Serial.println(left_pulse);
     old_pulse = left_pulse;
  #endif
    
  #ifndef USE_ENC_H
    delay(1);
  #endif
  
}
