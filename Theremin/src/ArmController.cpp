/*
 * armReach.cpp
 *
 *  Created on: 6 Apr 2011
 *      Author: icub
 */

#include "ArmController.h"

using namespace std;
#ifdef HAS_YARP
//using namespace yarp::os;
using namespace yarp::dev;
#endif

ArmController::ArmController(string rbt)
{
	robot = rbt;
	bool success = initYarp();
	if(!success)
	{
		cout << "Failed to initialise yarp arm ports correctly, quitting" << endl;
		exit(2);
	}


	armMoving = false;
	moveCalled = false;

	armsToRest();
}



void ArmController::armsToRest()
{	//first move doesn't block, while second does, so both arms move together.
	//and method doesn't return till both arms at rest.

#ifdef RIGHT_ARM
	move(HOME_RIGHT, false, true);	//right arm		//don't block so both move together
#endif
#ifdef LEFT_ARM
	move(HOME_LEFT, true, false);	//left arm
#endif
}

void ArmController::armRest(bool rightArm)
{
	if(rightArm)
		move(HOME_RIGHT, true, rightArm);
	else
		move(HOME_LEFT, true, rightArm);
}


bool ArmController::adjustJoint(bool rightArm, double change, int joint)
{
	bool success = false;

	//Limit the size of an adjustment
	if(change>10)
		change = 10;
	else if(change <-10)
		change = -10;
#ifdef HAS_YARP

	double position;
	if(rightArm)
	{
#ifdef RIGHT_ARM
		success = rightArmEnc->getEncoder(joint, &position);
#endif
	}
	else
	{
#ifdef LEFT_ARM
		success = leftArmEnc->getEncoder(joint, &position);
#endif
	}

	if(!success)
		return success;		//Unable to get encoder positions for specified joint
	/**
	 * NOTE: It would be very dangerous to continue if current position was not correctly obtained.
	 */

	double adjPos = position += change;

	if(adjPos > maxJointRanges[joint])
		adjPos = maxJointRanges[joint];
	else if(adjPos < minJointRanges[joint])
		adjPos = minJointRanges[joint];

	printf("Adjusting joint to position %.2f\n", adjPos);

	if(adjPos < maxJointRanges[joint] && adjPos > minJointRanges[joint])
	{

		if(rightArm)
		{
#ifdef RIGHT_ARM
			success = rightArmPos->positionMove(joint, adjPos);
#endif
		}
		else
		{
#ifdef LEFT_ARM
			success = leftArmPos->positionMove(joint, adjPos);
#endif
		}

	}
#endif

	yarp::os::Time::delay(1.0);
	return success;

}


void ArmController::move(const double* position, bool block, bool rightArm, bool moveHand)
{
#ifdef HAS_YARP
	moveCalled = true;


	if(rightArm)
		cout << "Moving right arm, position requested: [";
	else
		cout << "Moving left arm, position requested: [";
	for(int i=0; i<16; i++)
	{
		cout << position[i];
		if(i!=15)
			cout << ", ";
	}
	cout << "]" << endl;

	double* checked = new double[16];
	//Perform safety check on movement specified
	for(int i=0; i<16; i++)	//Safety ranges only defined for first 6 joints
	{

		if(position[i] < minJointRanges[i])
		{
			checked[i] = minJointRanges[i];
			printf("Adjusting joint %i from requested %.2f to min %.2f\n", i, position[i], checked[i]);
		}
		else if(position[i] > maxJointRanges[i])
		{
			checked[i] = maxJointRanges[i];
			printf("Adjusting joint %i from requested %.2f to max %.2f\n", i, position[i], checked[i]);
		}
		else
			checked[i] = position[i];

	}

	cout << "Moving to checked position: ";
	for (int i=0; i<16; i++)
	{
		cout << " " << checked[i];
	}
	cout << endl;


	if(rightArm)
	{
#ifdef RIGHT_ARM
		rightArmPos->positionMove(checked);
#endif
	}
	else
	{
#ifdef LEFT_ARM
		leftArmPos->positionMove(checked);
#endif
	}
	do{
		yarp::os::Time::delay(0.2);
		armMoving = true;
	}while(!armsStationary());
	armMoving = false;
#endif
}



bool ArmController::initYarp()
{
	bool ok=true;
#ifdef HAS_YARP
	yarp::os::Network yarp;

	//Check drivers have been correctly established


	string limb = "/";
	limb += robot;
	string fullLimb = limb + "/right_arm";


	yarp::os::Property rightArmOptions;
	string hport = "/smc";
	if(robot.compare("icubSim")==0)
		hport += "Sim/";
	else hport +="/";
	string fullLocal;
#ifdef RIGHT_ARM

	fullLocal = hport + "right_arm_move";
	rightArmOptions.put("part", "right_arm");
	rightArmOptions.put("device", "remote_controlboard");
	rightArmOptions.put("local", fullLocal.c_str());
	rightArmOptions.put("remote", fullLimb.c_str());

	rightArmDriver=new PolyDriver;
	rightArmDriver->open(rightArmOptions);

	ok &= rightArmDriver->isValid();
#endif

	yarp::os::Property leftArmOptions;
#ifdef LEFT_ARM
	fullLocal = hport + "left_arm_move";
	fullLimb = limb + "/left_arm";
	leftArmOptions.put("part", "left_arm");
	leftArmOptions.put("device", "remote_controlboard");
	leftArmOptions.put("local", fullLocal.c_str());
	leftArmOptions.put("remote", fullLimb.c_str());

	leftArmDriver=new PolyDriver;
	leftArmDriver->open(leftArmOptions);

	ok &= leftArmDriver->isValid();
#endif


	if (!ok)
	{
		cout << "Failed to open one or both of the arm motor drivers\n";
		return ok;
	}


	//Obtain position and encoder interfaces to drivers

#ifdef RIGHT_ARM
	ok &= rightArmDriver->view(rightArmPos);
	ok &= rightArmDriver->view(rightArmEnc);
#endif
#ifdef LEFT_ARM
	ok = leftArmDriver->view(leftArmPos);
	ok &= leftArmDriver->view(leftArmEnc);
#endif

	if (!ok)
	{
		cout << "Failed to obtain one or more of the arm position and encoder interfaces\n";
		return ok;
	}

#ifdef RIGHT_ARM
	rightArmPos->setRefSpeeds(ARM_VELOCITY);
#endif
#ifdef LEFT_ARM
	leftArmPos->setRefSpeeds(ARM_VELOCITY);
#endif

#endif	//has yarp
	return ok;
}

void ArmController::close()
{
#ifdef HAS_YARP
#ifdef RIGHT_ARM
	rightArmDriver->close();
#endif
#ifdef LEFT_ARM
	leftArmDriver->close();
#endif
#endif	//has yarp
}


/**
 * Compares the arm positions to previous positions
 *
 */
bool ArmController::armsStationary()
{

	bool leftmotionCompleted = true;
	bool rightmotionCompleted = true;
#ifdef HAS_YARP
	bool joint;
	for(int i=0; i<6; i++)
	{
#ifdef RIGHT_ARM
		joint=true;
		rightArmPos->checkMotionDone(i, &joint);
		rightmotionCompleted &=joint;
#endif
#ifdef LEFT_ARM
		joint=true;
		leftArmPos->checkMotionDone(i, &joint);
		leftmotionCompleted &=joint;
#endif
	}
#endif
	if(leftmotionCompleted && rightmotionCompleted)
	{
		if(moveCalled)
		{
//			cout << "arm motion done" << endl;
			moveCalled = false;
		}
		return true;
	}
	else
	{
//		cout << "still moving" << endl;
		return false;
	}

}




bool ArmController::getCurrentPosition(double* position, const bool rightArm)
{
	bool success;
#ifdef HAS_YARP
	if(rightArm)
	{
#ifdef RIGHT_ARM
		success = rightArmEnc->getEncoders(position);
#endif
	}
	else
	{
#ifdef LEFT_ARM
		success = leftArmEnc->getEncoders(position);
#endif
	}
#endif
	return success;
}



bool ArmController::isAtRest(const bool rightArm)
{
	if(rightArm)
		return isAtPosition(HOME_RIGHT, rightArm);
	else
		return isAtPosition(HOME_LEFT, rightArm);
}


//Note, this will also return false if it was unable to obtain the current arm configuration
bool ArmController::isAtPosition(const double* compPos, const bool rightArm)
{
	double* currentPos = new double[16];
	bool success = getCurrentPosition(currentPos, rightArm);
	if(success)
		return isAtPosition(currentPos, compPos);
	else
		return success;
}



bool ArmController::isAtPosition(const double* pos1, const double* pos2)
{
	bool match = true;
	for(int i=0; i<6; i++)
	{
		if(!((floor(pos1[i])==(int)pos2[i]) || (ceil(pos1[i]) == (int)pos2[i])))
		{
			match = false;
			break;
		}

	}
	return match;
}

bool ArmController::getCurrentShoulderPosition(double* position, const bool rightArm)
{
	bool success = false;
#ifdef HAS_YARP
	if(rightArm)
	{
#ifdef RIGHT_ARM
		success = rightArmEnc->getEncoder(shoulderRoll, position);
#endif
	}
	else
	{
#ifdef LEFT_ARM
		success = leftArmEnc->getEncoder(shoulderRoll, position);
#endif
	}
#endif
	return success;
}


const double* ArmController::getMaxJointRanges()
{
    return maxJointRanges;
}

const double* ArmController::getMinJointRanges()
{
    return minJointRanges;
}
