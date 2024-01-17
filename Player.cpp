//
//  Player.cpp
//

#include "Player.h"

#include "ObjLibrary/Vector3.h"

#include "Angles.h"

using namespace ObjLibrary;



Player::Player()
	: position(0.0, PLAYER_HALF_HEIGHT, 0.0),
	velocity(0.0, 0.0, 0.0), 
	  yaw_degrees(0.0),
	  pitch_degrees(0.0)
{
}



const ObjLibrary::Vector3& Player :: getPosition () const
{
	return position;
}

double Player :: getYawDegrees () const
{
	return yaw_degrees;
}

ObjLibrary::Vector3 Player :: getForward () const
{
	Vector3 forward = Vector3::UNIT_X_PLUS;
	forward.rotateZ(degreesToRadians(pitch_degrees));
	forward.rotateY(degreesToRadians(yaw_degrees));
	return forward;
}
ObjLibrary::Vector3 Player :: getRight () const
{
	Vector3 forward = Vector3::UNIT_Z_PLUS;
	forward.rotateY(degreesToRadians(yaw_degrees));
	return forward;
}



void Player :: reset (const ObjLibrary::Vector3& start_position)
{
	position = start_position;
	yaw_degrees   = 0.0;
	pitch_degrees = 0.0;
}

void Player :: addPosition (const Vector3& delta)
{
	position += delta;
}

void Player :: addYawDegrees (double delta)
{
	yaw_degrees += delta;
}

void Player :: addPitchDegrees (double delta)
{
	static const double PITCH_MIN = -60.0;
	static const double PITCH_MAX =  60.0;

	pitch_degrees += delta;
	if(pitch_degrees < PITCH_MIN)
		pitch_degrees = PITCH_MIN;
	if(pitch_degrees > PITCH_MAX)
		pitch_degrees = PITCH_MAX;
}

void Player :: setPlayerPosition(ObjLibrary::Vector3& pos)
{
	position = pos;
}

ObjLibrary::Vector3 Player::getVelocity() const
{
	return velocity;
}

void Player :: setVelocity(ObjLibrary::Vector3& VeclocityVector)
{
	velocity = VeclocityVector;
}