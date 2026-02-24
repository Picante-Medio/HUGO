#pragma once
#include <cmath>
#include "Ogre.h"
 
namespace HUGO{

    struct Vector3
    {
        float x, y, z;
        Vector3() : x(0), y(0), z(0) {}
        Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
        
        Vector3 operator+ (const Vector3& other) const{
            return { x + other.x, y + other.y, z + other.z};
        }
        void operator+=(const Vector3& other)
        {
            x += other.x;
            y += other.y;
            z += other.z;
        }
        Vector3 operator- (const Vector3& other) const{
            return { x - other.x, y - other.y, z - other.z};
        }

        float operator* (const Vector3& other) const{
            return x * other.x+ y* other.y+ z* other.z;
        }

        Vector3 operator* (float scalar) const{
            return { x*scalar, y*scalar, z*scalar};
        }

        Vector3 cross (const Vector3& other) const{
            float cx = y * other.z - z * other.y;
            float cy = z * other.x - x * other.z;
            float cz = x * other.y - y * other.x;
            return {cx, cy, cz};
        }

        Ogre::Vector3 toOgre() const {
            return Ogre::Vector3(x, y, z);
        }
    };

    struct Vector4
    {
        float x, y, z, w;
        Vector4() : x(0), y(0), z(0), w(0) {}
        Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
        
        Vector4 operator+ (const Vector4& other) const {
            return { x + other.x, y + other.y, z + other.z, w + other.w };
        }

        Vector4 operator- (const Vector4& other) const {
            return { x - other.x, y - other.y, z - other.z, w - other.w };
        }

        float operator* (const Vector4& other) const {
            return x * other.x + y * other.y + z * other.z + w * other.w;
        }

        Vector4 operator* (float scalar) const {
            return { x * scalar, y * scalar, z * scalar, w * scalar };
        }

        bool operator== (const Vector4& other) const {
            return x == other.x && y == other.y && z == other.z && w == other.w;
        }

        bool operator!= (const Vector4& other) const {
            return !(*this == other);
        }

        float length() const {
            return std::sqrt(x*x + y*y + z*z + w*w);
        }

        Vector4 normalized() const {
            float len = length();
            if (len > 0) {
                return { x/len, y/len, z/len, w/len };
            }
            return *this;
        }

        void normalize() {
            float len = length();
            if (len > 0) {
                x /= len;
                y /= len;
                z /= len;
                w /= len;
            }
        }
    };
    
    struct Quaternion
    {
        float w, x, y, z;

        Quaternion() : w(0), x(0), y(0), z(0) {}
        Quaternion(float w, float x, float y, float z) : w(w), x(x), y(y), z(z) {}

        Ogre::Quaternion toOgre() const{
            return Ogre::Quaternion(w, x, y, z);
        }
    };
    
}