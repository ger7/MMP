/*
 * armReachV2.h
 *
 *  Created on: 8 Aug 2011
 *      Author: icub
 */

#define HAS_YARP

#ifndef ARM_CONTROLLER
	#define ARM_CONTROLLER

	#define LEFT_ARM
	#define RIGHT_ARM	//used to indicate functional arms


#include <yarp/os/all.h>
#include <yarp/dev/all.h>



#include <iostream>
#include <string.h>
#include <math.h>


const int LEFT = 0;
const int RIGHT = 1;
const int BOTH = 2;

//Arm ranges:
//								 	  0	   1    2    3    4    5    6   7   8   9   10  11   12  13   14  15
const double absMinJointRanges[] = {-95,  10, -35,  15, -80, -65, -15,  0,  5,  5,   0,  0,   0,  0,   0,  0};
const double absMaxJointRanges[] = {  5, 140,  75, 100,  50,   0,  25, 60, 90, 90, 180, 90, 175, 90, 175, 200};

const double ARM_VELOCITY[] = 	  { 12,  12,  12,  10,  10,  10,  10, 20, 40, 50,  60, 60,  70, 70,  80, 100};

//Values for playing Theremin
//								   	  0    1    2    3    4    5    6   7   8   9   10  11   12  13   14  15
const double minJointRanges[] =    {-35,  20, -23,  55, -19,   0,   0, 55, 12, 50,   5,  0,   0,  0,   0,  0};
const double maxJointRanges[] =    {-40,  40, -13,  55, -19,   0,   0, 55, 12, 50,   5,  5,   5,  5,   5,  5};
//const double minJointRanges[] =    {-78,  20,   0,  40, -10,   0,   0, 35, 10, 50,   5,  0,   0,  0,   0,  0};
//const double maxJointRanges[] =    {-65,  45,   0,  50,  10,   0,   0, 35, 10, 50,   5,  5,   5,  5,   5,  5};


const double HOME_LEFT[]     	=	   {-37,  21, -13,  55, -19,   0,   0, 55, 12, 50,   5,  5,   5,  5,   5,  5};
const double HOME_RIGHT[]     	=	   {-37,  21, -23,  55, -19,   0,   0, 55, 12, 50,   5,  5,   5,  5,   5,  5};
//const double HOME_LEFT[]	=	   {-68,  42,   0,  23, -10,   0,   0, 35, 10, 50,   5,  5,   5,  5,   5,  5};
//const double HOME_RIGHT[]	=	  {-76,  37,   0,  15,   0,   0,   0, 35, 10, 50,   5,  5,   5,  5,   5,  5};


class ArmController{
public:
	ArmController(std::string robot);
	void close();

	bool armsStationary();



	/**
	 * Note, this function limits the maximum size of the change to 10, and within the valid ranges of the joint
	 */
	bool adjustJoint(bool rightArm, double change=1.0, int joint=shoulderRoll);


	void armsToRest();		//Moves both arms to rest
	void armRest(const bool rightArm=true);	//move selected arm to rest.


	bool getCurrentPosition(double* position, const bool rightArm=true);
	bool getCurrentShoulderPosition(double* position, const bool rightArm=true);
//	bool getCurrentWristPosition(double* position, const bool rightArm=true);


	bool isAtRest(const bool rightArm=true);
	bool isAtPosition(const double* compPos, const bool rightArm=true);
	bool isAtPosition(const double* pos1, const double* pos2);

	void reachLeft(const double* position);
	void reachRight(const double* position);


	const double* getMaxJointRanges();
	const double* getMinJointRanges();





protected:
	bool initYarp();
	void move(const double* position, bool block=true, bool rightArm=true, bool moveHand=false);
//	int randGenerator(const int range);

#ifdef HAS_YARP
	yarp::dev::PolyDriver *leftArmDriver;
	yarp::dev::IPositionControl *leftArmPos;
	yarp::dev::IEncoders *leftArmEnc;

	yarp::dev::PolyDriver *rightArmDriver;
	yarp::dev::IPositionControl *rightArmPos;
	yarp::dev::IEncoders *rightArmEnc;
#endif


	//Arm joint IDs
	//Rotational joints:
	 static const int shoulderPitch = 0;
	 static const int shoulderYaw = 2;
	 static const int wristProSup = 4; //wrist pronosupination
	 static const int wristYaw = 6;
	//Flexion joints:
	 static const int shoulderRoll = 1;
	 static const int elbow = 3;
	 static const int wristPitch = 5;
	//Hand joint IDs:
	 static const int handAdduction = 7; //adduction/abduction (spreading fingers out)
	 static const int thumbOppose = 8;
	 static const int thumbProximal = 9;
	 static const int thumbDistal = 10;
	 static const int indexProximal = 11;
	 static const int indexDistal = 12;
	 static const int midProximal = 13;
	 static const int midDistal = 14;
	 static const int pinky = 15;


	 bool armMoving, moveCalled;

	 std::string robot;

};



#endif
