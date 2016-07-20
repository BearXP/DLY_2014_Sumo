#include "sumo_common.h"
#include "sumo_ports.h"
#include <avr/io.h>			

enum Line_Edge {   //defines which edge is the line.  For us it is the falling
line_falling =0,
line_rising=1
};

const uint8_t numLineSensors = 4;  	//how many line sensors are there?

namespace Sumo
{

	class LineSensors
	{
		class LineSensor //private class to store data for each sensor
		{
			public:
				uint8_t inputBit;  //the bit in the port that this linesensor is connected to
				RegPointer inputPort;	//the port that the bit is in
				bool lineDetected;		//has the line been detected?
		
		};

		private:
				
			uint8_t lineEdge;  	//defines the mask to indicate which edge indicates the line is found
								//falling or rising

			FunctionPtr* m_funcptr;	//callback function that is called if a line is detected.
									// ie.  Stop motors

			bool funcEnable;		//enable or disable the call back function

			LineSensor lineArray[numLineSensors];	

//main program must define the pin change interrupt vector 
//as it is shared amongst other functions

			void Setup_Interrupts()  //turn on pin change interrupts for the lines
			{
					//Setup Pin Change Inerrupt
				PCICR |= _BV(PCIE1) | _BV(PCIE2);   //enable Pin Change int 1 for PCINT 8-15  
										//and Pin CHange Interrupt 2 for PCINT 16-23

				PCMSK1 |= _BV(PCINT13)|_BV(PCINT14)|_BV(PCINT15);  //line sensors
				PCMSK2 |= _BV(PCINT23);  //line sensors

				
			}

		public:
			LineSensors(uint8_t edge):
			lineEdge(edge),funcEnable(false)
			{			
				lineArray[0].inputPort = &LINE_PORT1;
				lineArray[0].inputBit = LINE_1;
				lineArray[0].lineDetected = false;
				lineArray[1].inputPort = &LINE_PORT1;
				lineArray[1].inputBit = LINE_2;
				lineArray[1].lineDetected = false;
				lineArray[2].inputPort = &LINE_PORT1;
				lineArray[2].inputBit = LINE_3;
				lineArray[2].lineDetected = false;
				lineArray[3].inputPort = &LINE_PORT2;
				lineArray[3].inputBit = LINE_4;
				lineArray[3].lineDetected = false;
			
				ClearFunctionPtr();
				Setup_Interrupts();
			}

			~LineSensors()
			{
			}

//check the lines, update the lineArray and exec callback function if line detected.
			void CheckLines();

			uint8_t GetLineEdge()
			{
				return lineEdge;
			}

			//only returns if a line is dected on the specified sensor
			//doesn't recheck the line
			bool GetLineDetected(UBYTE theLine);  //1-4 (base 1)

			//emergency function that is called when a line is detected.
			// ie.  Stop motors
			void SetFunctionPtr(FunctionPtr funcptr);
			void ClearFunctionPtr();
			void ExecFunctionPtr();

			//turn off/on the emergency function
			void DisableFunction();
			void EnableFunction();
			bool GetFunctionEnabled();
			

	};
};
