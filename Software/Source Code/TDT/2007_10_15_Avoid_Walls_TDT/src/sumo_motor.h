
#include <avr/io.h>

#define MOTOR_TOP	0x01FF  //for phase correct PWM frequency = 16MHz/2/MOTOR_TOP/Prescaler
//#define TORQUE		125		//add and subtract from middle value to give driving force


#define M1_OCR_A	OCR4B
#define M1_OCR_B	OCR4C

#define M2_OCR_A	OCR3B
#define M2_OCR_B	OCR3C

class SumoMotor
{
	public:
		SumoMotor()
		{
			torque =0;
			m1dir =1;
			m2dir =1;
			m1step =1;
			m2step =1;
			MOTOR1_HIGH =	(MOTOR_TOP/2)+torque;
			MOTOR1_LOW  =	(MOTOR_TOP/2)-torque;
			MOTOR2_HIGH =	(MOTOR_TOP/2)+torque;
			MOTOR2_LOW  =	(MOTOR_TOP/2)-torque;
		}

		void Motor_Step();
		void Motor1_Dir(int8_t dir);
		void Motor2_Dir(int8_t dir);
		void SetTorque(uint16_t aTorque); 
	
	private:
		uint16_t torque;  //value from 0-127

		volatile int8_t m1dir;  //1=forward, -1 reverse
		volatile int8_t m1step;  //1-4

		volatile int8_t m2dir;  //1=forward, -1 reverse
		volatile int8_t m2step;

		volatile uint16_t MOTOR1_HIGH;
		volatile uint16_t MOTOR1_LOW;
		volatile uint16_t MOTOR2_HIGH;
		volatile uint16_t MOTOR2_LOW ;
};

