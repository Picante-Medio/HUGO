#pragma once
 
namespace HUGO{

    struct Vector3
    {
        float x, y, z;
        Vector3() : x(0), y(0), z(0) {}
        Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
        
        Vector3 operator+ (const Vector3& other) const{
            return { x + other.x, y + other.y, z + other.z};
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
    };
    
    struct Quaternion
    {
        float w, x, y, z;

        Quaternion() : w(0), x(0), y(0), z(0) {}
        Quaternion(float w, float x, float y, float z) : w(w), x(x), y(y), z(z) {}
    };
    
}