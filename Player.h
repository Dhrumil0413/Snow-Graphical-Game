//
//  Player.h
//

#pragma once

#include "ObjLibrary/Vector3.h"



const double PLAYER_HALF_HEIGHT = 0.8;

class Player
{
public:
	Player ();

	const ObjLibrary::Vector3& getPosition () const;
	double getYawDegrees () const;
	ObjLibrary::Vector3 getForward () const;
	ObjLibrary::Vector3 getRight () const;

	void reset (const ObjLibrary::Vector3& start_position);
	void addPosition (const ObjLibrary::Vector3& delta);
	void addYawDegrees (double delta);
	void addPitchDegrees (double delta);
	void setPlayerPosition(ObjLibrary::Vector3& pos);

	ObjLibrary::Vector3 getVelocity() const;
	void setVelocity(ObjLibrary::Vector3& VeclocityVector);

private:
	ObjLibrary::Vector3 position;
	ObjLibrary::Vector3 velocity;
	double yaw_degrees;
	double pitch_degrees;

};
