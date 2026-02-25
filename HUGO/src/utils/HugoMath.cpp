#include "HugoMath.h"

namespace HUGO {

	Vector3::Vector3()
		: x(0), y(0), z(0) {}
	Vector3::Vector3(float x, float y, float z)
		: x(x), y(y), z(z) {}

	Vector3 Vector3::operator+ (const Vector3& other) const {
		return { x + other.x, y + other.y, z + other.z };
	}
	void Vector3::operator+=(const Vector3& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
	}
	Vector3 Vector3::operator- (const Vector3& other) const {
		return { x - other.x, y - other.y, z - other.z };
	}

	float Vector3::operator* (const Vector3& other) const {
		return x * other.x + y * other.y + z * other.z;
	}

	Vector3 Vector3::operator* (float scalar) const {
		return { x * scalar, y * scalar, z * scalar };
	}

	Vector3 Vector3::cross(const Vector3& other) const {
		float cx = y * other.z - z * other.y;
		float cy = z * other.x - x * other.z;
		float cz = x * other.y - y * other.x;
		return { cx, cy, cz };
	}

	float Vector3::length() const {
		return std::sqrt(x * x + y * y + z * z);
	}

	Vector3 Vector3::normalized() const {
		float len = length();
		if (len > 0) {
			return { x / len, y / len, z / len };
		}
		return *this;
	}


	Ogre::Vector3 Vector3::toOgre() const {
		return Ogre::Vector3(x, y, z);
	}

	btVector3 Vector3::toBullet() const {
		return btVector3(x, y, z);
	}

	Vector3 operator*(float scalar, const Vector3& v) {
		return Vector3(v.x * scalar, v.y * scalar, v.z * scalar);
	}

	Vector4::Vector4() 
        : x(0), y(0), z(0), w(0) {}
	Vector4::Vector4(float x, float y, float z, float w) 
        : x(x), y(y), z(z), w(w) {}

	Vector4 Vector4::Vector4::operator+ (const Vector4& other) const {
		return { x + other.x, y + other.y, z + other.z, w + other.w };
	}

	Vector4 Vector4::Vector4::operator- (const Vector4& other) const {
		return { x - other.x, y - other.y, z - other.z, w - other.w };
	}

	float Vector4::operator* (const Vector4& other) const {
		return x * other.x + y * other.y + z * other.z + w * other.w;
	}

	Vector4 Vector4::operator* (float scalar) const {
		return { x * scalar, y * scalar, z * scalar, w * scalar };
	}

	bool Vector4::operator== (const Vector4& other) const {
		return x == other.x && y == other.y && z == other.z && w == other.w;
	}

	bool Vector4::operator!= (const Vector4& other) const {
		return !(*this == other);
	}

	float Vector4::length() const {
		return std::sqrt(x * x + y * y + z * z + w * w);
	}

	Vector4 Vector4::normalized() const {
		float len = length();
		if (len > 0) {
			return { x / len, y / len, z / len, w / len };
		}
		return *this;
	}

	void Vector4::normalize() {
		float len = length();
		if (len > 0) {
			x /= len;
			y /= len;
			z /= len;
			w /= len;
		}
	}
	float w, x, y, z;

	Quaternion::Quaternion() : w(0), x(0), y(0), z(0) {}
	Quaternion::Quaternion(float w, float x, float y, float z) : w(w), x(x), y(y), z(z) {}

	Ogre::Quaternion Quaternion::toOgre() const {
		return Ogre::Quaternion(w, x, y, z);
	}

	btQuaternion Quaternion::toBullet() const {
		return btQuaternion(x, y, z, w);
	}
}