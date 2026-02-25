#pragma once
#include <cmath>
#include "Ogre.h"
#include "../bullet3-src/src/btBulletDynamicsCommon.h"

namespace HUGO {

	struct Vector3
	{
		float x, y, z;
		Vector3();
		Vector3(float x, float y, float z);

		Vector3 operator+ (const Vector3& other) const;
		void operator+=(const Vector3& other);
		Vector3 operator- (const Vector3& other) const;

		float operator* (const Vector3& other) const;
		Vector3 operator* (float scalar) const;
        
		Vector3 cross(const Vector3& other) const;

		float length() const;
		Vector3 normalized() const;

		Ogre::Vector3 toOgre() const;
		btVector3 toBullet() const;
	};

	Vector3 operator*(float scalar, const Vector3& v);

	struct Vector4
	{
		float x, y, z, w;
		Vector4();
		Vector4(float x, float y, float z, float w);

		Vector4 operator+ (const Vector4& other) const;
		Vector4 operator- (const Vector4& other) const;
		
        float operator* (const Vector4& other) const;
		Vector4 operator* (float scalar) const;
		
        bool operator== (const Vector4& other) const;
		bool operator!= (const Vector4& other) const;

		float length() const;
		Vector4 normalized() const;
		void normalize();
	};

	struct Quaternion
	{
		float w, x, y, z;

		Quaternion();
		Quaternion(float w, float x, float y, float z);

		Ogre::Quaternion toOgre() const;
		btQuaternion toBullet() const;
	};

}