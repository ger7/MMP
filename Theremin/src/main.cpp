/*
 * main.cpp
 * A test harness for connecting to and using the arms.  This also acts as an example of how to use the functions provided.
 *
 *  Created on: 5 Feb 2014
 *      Author: icub
 */


#include "ArmController.h"

using namespace std;
ArmController* ac;
void armPosCheck(double adjustment);

int main(int argc, char* argv[])
{
	/*
	 * Specify which robot we want to connect to.
	 * If at some point you decide to connect to the simulator,
	 * this is changed to "icubSim".  At this point, we could use a command line
	 * parameter to save recompiling to switch between robot and simulator.
	 */
	string robot = "icub";

	/**
	 * Note, if for some reason the constructor fails to initialise
	 * the arm controller, then the program will terminate.
	 *
	 * During the construction of the controller, the arms will
	 * be moved to the predefined starting positions
	 */
	ac = new ArmController(robot);


	char input=' ';

	while(input!='e')
	{
		cout << "Basic test of controls for robot arms:\n" <<
				" q - get current position of right shoulder\n" <<
				" a - move right arm out\n" <<
				" z - move right arm in\n" <<
				" o - get current position of left shoulder\n" <<
				" k - move left arm out\n" <<
				" m - move left arm in\n" <<
				"n - Check arm positioning, please input double\n" <<
				" e - exit" << endl;

		cin >> input;
		switch (input)
		{
		/**
		 * Note 1: the size of the adjustment is limited to
		 * a maximum of 10 degrees at a time and within
		 * the range of the valid joint movements
		 *
		 * Note 2: The adjustments will not block the thread
		 * of control whilst waiting for the movement to complete.
		 * A 0.1 second delay is applied here to try and prevent
		 * the iCub from being overloaded with commands, however
		 * it may not be sufficient time for the arm movement to finish.
		 *
		 */
			case 'q':
			{
				double position;
				ac->getCurrentShoulderPosition(&position, RIGHT);
				printf("Right shoulder is at position %.2f\n", position);
				break;
			}
			case 'a':
				ac->adjustJoint(RIGHT, 1.0);
				break;
			case 'z':
				ac->adjustJoint(RIGHT, -1.0);
				break;
			case 'o':
			{
				double position;
				ac->getCurrentShoulderPosition(&position, LEFT);
				printf("Left shoulder is at position %.2f\n", position);
				break;
			}
			case 'k':
				ac->adjustJoint(LEFT, 1.0);
				break;
			case 'm':
				ac->adjustJoint(LEFT, -1.0);
				break;
			case 'n':
				double adjust;
				cin>>adjust;
				armPosCheck(adjust);
				break;
			case 'e':
				cout << "exiting" << endl;
				break;
			default:
				cout << "unrecognised input" << endl;
				break;
		}



	}

	cout << "Moving arms back to starting position" << endl;
	ac->armsToRest();	//This function will block until the arms have finished moving

	ac->close();




}

void armPosCheck (double adjustment)
{
	double position;
	int iPosition;
	double newPosition;
	int iNewPosition;
	int iAdjustment;
	double observedmovement;


    ac->getCurrentShoulderPosition(&position, RIGHT);
    ac->adjustJoint(RIGHT, adjustment);
    ac->getCurrentShoulderPosition(&newPosition, RIGHT);

    iPosition=(int)(position*100);
    iNewPosition=(int)(newPosition*100);
    iAdjustment=(int)(adjustment*100);
    observedmovement=position-newPosition;


    if ((iNewPosition-iPosition>=iAdjustment-20)||(iNewPosition-iPosition<=iAdjustment+20))
    {
    	printf("Arm moved successfully by %.2f\n", adjustment);

    }

    else
    {
    	printf("Arm was not moved successfully\n");
    	printf("Expected %.2f\n", adjustment);
    	printf(" but a value of %.2f\n", observedmovement);
    	printf(" was returned\n");
    }
}
