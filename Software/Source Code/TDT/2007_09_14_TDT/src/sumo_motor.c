
#include "sumo_motor.h"
#include "sumo_dotmatrix.h"
#include "sprites.h"

using namespace Sumo;

void SumoMotor::Motor_Step()
{
	DotMatrix::instance()->loadSprite(		
		sc_hexDigits_mini[(m1step) % 10],
		SPRITE_ALPHANUMERIC_MINI_WIDTH,
		SPRITE_ALPHANUMERIC_MINI_HEIGHT,
		1,
		8
	);

	DotMatrix::instance()->loadSprite(
		//sc_hexDigits_mini[(voltage / 100) % 10],
		sc_hexDigits_mini[(m2step) % 10],
		SPRITE_ALPHANUMERIC_MINI_WIDTH,
		SPRITE_ALPHANUMERIC_MINI_HEIGHT,
		1,
		4
	);

	//MOTOR 1
	//-------


	switch (m1step)		
    {
        case 1:
            
			M1_OCR_A = MOTOR1_HIGH;
			M1_OCR_B = MOTOR1_LOW;			
            break;
			
		case 2:
            M1_OCR_A = MOTOR1_LOW;
			M1_OCR_B = MOTOR1_LOW;			
            break;
			
		case 3:
            
			M1_OCR_A = MOTOR1_LOW;
			M1_OCR_B = MOTOR1_HIGH;				
            break;
			
		case 4:
            M1_OCR_A = MOTOR1_HIGH;
			M1_OCR_B = MOTOR1_HIGH;
            break;
	}
	m1step += m1dir;
	if(m1step>4) m1step = 1;
	if(m1step<1) m1step = 4;

	//MOTOR 2
	//-------
	switch (m2step)		
    {
        case 1:
            
			M2_OCR_A = MOTOR2_HIGH;
			M2_OCR_B = MOTOR2_LOW;			
            break;
			
		case 2:
            M2_OCR_A = MOTOR2_LOW;
			M2_OCR_B = MOTOR2_LOW;			
            break;
			
		case 3:
            
			M2_OCR_A = MOTOR2_LOW;
			M2_OCR_B = MOTOR2_HIGH;				
            break;
			
		case 4:
            M2_OCR_A = MOTOR2_HIGH;
			M2_OCR_B = MOTOR2_HIGH;
            break;
	}
	m2step += m2dir;
	if(m2step>4) m2step = 1;
	if(m2step<1) m2step = 4;
}

void SumoMotor::Motor1_Dir(int8_t dir)
{
	m1dir = dir;
}

void SumoMotor::Motor2_Dir(int8_t dir)
{
	m2dir = dir;
}

void SumoMotor::SetTorque(uint16_t aTorque)
{
	if(aTorque<=255)
		torque = aTorque;

	MOTOR1_HIGH =	(MOTOR_TOP/2)+torque;
	MOTOR1_LOW  =	(MOTOR_TOP/2)-torque;
	MOTOR2_HIGH =	(MOTOR_TOP/2)+torque;
	MOTOR2_LOW  =	(MOTOR_TOP/2)-torque;
}
