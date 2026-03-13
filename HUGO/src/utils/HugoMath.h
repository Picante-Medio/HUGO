#pragma once
namespace HUGO {
	struct Degree;
    struct Radian;

	struct Vector3
	{
		float x, y, z;
		Vector3();
		Vector3(float n);
		Vector3(float x, float y, float z);

		Vector3 operator+ (const Vector3& other) const;
		void operator+=(const Vector3& other);
		Vector3 operator- (const Vector3& other) const;
		//Vector3 operator-= (const Vector3& other);

		float operator* (const Vector3& other) const;
		Vector3 operator* (float scalar) const;
		
        
		Vector3 cross(const Vector3& other) const;

		float length() const;
		Vector3 normalized() const;
	};

	Vector3 operator*(float scalar, const Vector3& v);

	struct Vector4
	{
		float x, y, z, w;
		Vector4();
		Vector4(float n);
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

		//helpers which are useful to audio listener and other contexts
		Quaternion inverse() const;

		Vector3 forward() const;
		Vector3 up() const;
		Vector3 right() const;
	};
	Quaternion operator*(const Quaternion& a, const Quaternion& b);
		// multiplying a quaternion
	Vector3 operator*(const Quaternion& q, const Vector3& v);
	struct Radian {
        float rad;

        Radian(float r = 0);
        Radian(const Degree& d);
        Radian(const HUGO::Radian& rhs);
        
        float valueDegrees() const;
        float valueRadians() const;

        const Radian& operator + () const;
        Radian operator+(const Radian& r) const;
        Radian operator+(const Degree& d) const;
        Radian operator-() const;
        Radian operator-(const Radian& r) const;
        Radian operator-(const Degree& d) const;
        Radian operator*(float f) const;
        Radian operator*(const Radian& r) const;
        Radian operator/(float f) const;
        
        bool operator <  ( const Radian& d ) const;
        bool operator <= ( const Radian& d ) const;
        bool operator == ( const Radian& d ) const;
        bool operator != ( const Radian& d ) const;
        bool operator >= ( const Radian& d ) const;
        bool operator >  ( const Radian& d ) const;
    };

	struct Degree {
		float deg;

		Degree(float d = 0);
		Degree(const Radian& r );
		Degree(const HUGO::Degree& rhs);

		Degree& operator = ( const float& f );
        Degree& operator = ( const Degree& d );
        Degree& operator = ( const Radian& r );

        float valueDegrees() const;
        float valueRadians() const;

        const Degree& operator + () const;
        Degree operator + ( const Degree& d ) const;
        Degree operator + ( const Radian& r ) const;
        Degree operator - () const;
        Degree operator - ( const Degree& d ) const;
        Degree operator - ( const Radian& r ) const;
        Degree operator * ( float f ) const;
        Degree operator * ( const Degree& f ) const;
        Degree& operator *= ( float f );
        Degree operator / ( float f ) const;
        Degree& operator /= ( float f );

        bool operator <  ( const Degree& d ) const;
        bool operator <= ( const Degree& d ) const;
        bool operator == ( const Degree& d ) const;
        bool operator != ( const Degree& d ) const;
        bool operator >= ( const Degree& d ) const;
        bool operator >  ( const Degree& d ) const;
	};

	//I dont know if this is necessary, but just in case
	using Int = int;
	using Float = float;
}