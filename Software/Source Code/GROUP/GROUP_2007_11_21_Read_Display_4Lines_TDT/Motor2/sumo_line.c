#include "sumo_line.h"
#include "standard.h"
//#include "sumo_dotmatrix.h"
#include "sprites.h"

using namespace Sumo;

void LineSensors::SetFunctionPtr(FunctionPtr funcptr)
{
	m_funcptr = funcptr;
		
}

void LineSensors::ClearFunctionPtr()
{
	m_funcptr = NULL;			
}

void LineSensors::ExecFunctionPtr()
{
	if (m_funcptr != NULL && funcEnable)
	{
		m_funcptr();
	}
}

//turn off/on the emergency function
//so once a line is found it doesn't keep triggering
void LineSensors::DisableFunction()
{
	funcEnable = false;
}

void LineSensors::EnableFunction()
{
	funcEnable = true;	
}

bool LineSensors::GetFunctionEnabled()
{
	return funcEnable;
}

//only returns if a line is dected on the specified sensor
//doesn't recheck the lines
bool LineSensors::GetLineDetected(Byte theLine) //base 1
{
	return lineArray[theLine-1].lineDetected;
}

//check the lines and update the lineArray.
void LineSensors::CheckLines()
{		
	Byte i;		
	for(i=0;i<numLineSensors;i++)
	{
		if(*(lineArray[i].inputPort)&_BV(lineArray[i].inputBit))
		{
			lineArray[i].lineDetected = static_cast<bool> (lineEdge);			
		}
		else
		{
			lineArray[i].lineDetected =  !(static_cast<bool> (lineEdge));
		}

		
	}

	for(i=0;i<numLineSensors;i++)
	{
		if(lineArray[i].lineDetected)
		{
			i=numLineSensors; //no need to continue if one line detected.
			ExecFunctionPtr();
		}	
	}
}


