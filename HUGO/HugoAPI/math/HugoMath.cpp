#include <cmath>
#include "HugoMath.h"
#include "HugoTypes.h"

namespace HUGO {

	// Constructors
	Vector2::Vector2() : x(0), y(0) {}
	Vector2::Vector2(float n) : x(n), y(n) {}
	Vector2::Vector2(float x, float y) : x(x), y(y) {}

	// Operators
	Vector2 Vector2::operator+(const Vector2& other) const {
		return Vector2(x + other.x, y + other.y);
	}

	void Vector2::operator+=(const Vector2& other) {
		x += other.x;
		y += other.y;
	}

	Vector2 Vector2::operator-(const Vector2& other) const {
		return Vector2(x - other.x, y - other.y);
	}

	void Vector2::operator-=(const Vector2& other) {
		x -= other.x;
		y -= other.y;
	}

	Vector2 Vector2::operator*(float scalar) const {
		return Vector2(x * scalar, y * scalar);
	}

	float Vector2::dot(const Vector2& other) const {
		return x * other.x + y * other.y;
	}

	float Vector2::length() const {
		return std::sqrt(x*x + y*y);
	}

	Vector2 Vector2::normalized() const {
		float len = length();
		if (len == 0) return Vector2(0,0);
		return Vector2(x / len, y / len);
	}

	bool Vector2::operator==(const Vector2& other) const{
		return x == other.x && y == other.y;
	}
	bool Vector2::operator!=(const Vector2& other) const{
		return x != other.x && y != other.y;
	}

    Vector3::Vector3()
		: x(0.0f), y(0.0f), z(0.0f) {}
    Vector3::Vector3(float n)
		: x(n), y(n), z(n) {}
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

    Vector3 Vector3::operator* (const Vector3& other) const {
		return {x * other.x, y * other.y, z * other.z};
	}

    Vector3 Vector3::operator* (float scalar) const {
		return { x * scalar, y * scalar, z * scalar };
	}

	    Vector3 Vector3::operator/(float scalar) const
    {
		if(scalar != 0) return {x/scalar, y/scalar, z/scalar};
		else return Vector3();
    }

    Vector3 Vector3::operator/(const Vector3 &other) const
    {
		float nx = other.x != 0.0f? x / other.x : x;
		float ny = other.y != 0.0f? y / other.y : x;
		float nz = other.z != 0.0f? z / other.z : x;
        return {nx, ny, nz};
    }

	bool Vector3::operator!=(const Vector3 &other) const
	{
		return x != other.x && y != other.y && z != other.z;
	}

	bool Vector3::operator==(const Vector3 &other) const
	{
		return x == other.x && y == other.y && z == other.z;
	}

    Vector3 Vector3::cross(const Vector3 &other) const
    {
        float cx = y * other.z - z * other.y;
		float cy = z * other.x - x * other.z;
		float cz = x * other.y - y * other.x;
		return { cx, cy, cz };
    }

    float Vector3::length() const {
		return std::sqrt(x * x + y * y + z * z);
	}

    void Vector3::normalize()
    {
		float len = length();
		if (len > 0) {
			x /= len;
			y /= len;
			z /= len;
		}
    }

    Vector3 Vector3::normalized() const {
		float len = length();
		if (len > 0) {
			return { x / len, y / len, z / len };
		}
		return *this;
	}

	Vector3 operator*(float scalar, const Vector3& v) {
		return Vector3(v.x * scalar, v.y * scalar, v.z * scalar);
	}

    Vector4::Vector4() 
        : x(0), y(0), z(0), w(0) {}
    Vector4::Vector4(float n)
    	: x(n), y(n), z(n), w(n) {}
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
	
	Quaternion::Quaternion() 
		: w(1), x(0), y(0), z(0) {}
	Quaternion::Quaternion(float w, float x, float y, float z) 
		: w(w), x(x), y(y), z(z) {}

    Quaternion::Quaternion(Radian angle, const Vector3 &axis)
	{
		Vector3 k = axis.normalized();
		float halfAngle = angle.rad* 0.5f;
        float sin = std::sin(halfAngle);
        w = std::cos(halfAngle);
        x = sin*k.x;
        y = sin*k.y;
        z = sin*k.z;
	}

    Quaternion Quaternion::inverse() const
    {
        float norm = w*w + x*x + y*y + z*z;

		return Quaternion(
			w / norm,
			-x / norm,
			-y / norm,
			-z / norm
		);
    }

    Vector3 Quaternion::forward() const
	{
		return (*this) * Vector3(0,0,1);
	}

	Vector3 Quaternion::up() const
	{
		return (*this) * Vector3(0,1,0);
	}

	Vector3 Quaternion::right() const
	{
		return (*this) * Vector3(1,0,0);
	}

    void Quaternion::operator*=(const Quaternion &other)
    {
		float nw, nx, ny, nz;
		nw = w*other.w - x*other.x - y*other.y - z*other.z;
		nx = w*other.x + x*other.w + y*other.z - z*other.y;
		ny = w*other.y - x*other.z + y*other.w + z*other.x;
		nz = w*other.z + x*other.y - y*other.x + z*other.w;

		w = nw; 
		x = nx; 
		y = ny; 
		z = nz;
    }

	void Quaternion::normalize()
	{
		float mod = std::sqrt(w*w + x*x + y*y + z*z);
		if(mod > 0.0f){
			w /= mod;
			x /= mod;
			y /= mod;
			z /= mod;
		}
	}

	bool Quaternion::operator==(const Quaternion& other) const{
		return w == other.w && x == other.x && y == other.y && z == other.z;
	}
	bool Quaternion::operator!=(const Quaternion& other) const{
		return !operator==(other);
	}

    Quaternion operator*(const Quaternion& a, const Quaternion& b)
	{
		return Quaternion(
			a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z,
			a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y,
			a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x,
			a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w
		);
	}

	Vector3 operator*(const Quaternion &q, const Vector3 &v)
    {
       HUGO::Vector3 qv(q.x, q.y, q.z);
	   HUGO::Vector3 t = 2.0f * qv.cross(v);
	   HUGO::Vector3 result = v + q.w * t + qv.cross(t);
	   return result;
    }

	Quaternion operator+(const Quaternion& l, const Quaternion& r){
		Quaternion res(l.w+r.w, l.x+r.x, l.y+r.y, l.z+r.z);
		res.normalize();
		return res;
	}

	Quaternion operator* (const Quaternion& q, const float& f){
		return Quaternion(q.w*f, q.x*f, q.y*f, q.z*f);
	}

    // --------------------- RADIAN ---------------------
	Radian::Radian(float r) 
		: rad(r) {}

    Radian::Radian(const Degree& d) {
        rad = d.valueRadians();
    }

    Radian::Radian(const Radian& rhs) 
		: rad(rhs.rad) {}

    void 
    Radian::operator=(const Radian& r) {
        this->rad = r.rad;
    }

    float Radian::valueDegrees() const {
        return static_cast<float>(rad * (180.0f / PI));
    }

    float Radian::valueRadians() const {
        return rad;
    }

    const Radian& Radian::operator+() const {
        return *this;
    }

    Radian Radian::operator+(const Radian& r) const {
        return Radian(rad + r.rad);
    }

    Radian Radian::operator+(const Degree& d) const {
        return Radian(rad + d.valueRadians());
    }

    Radian Radian::operator-() const {
        return Radian(-rad);
    }

    Radian Radian::operator-(const Radian& r) const {
        return Radian(rad - r.rad);
    }

    Radian Radian::operator-(const Degree& d) const {
        return Radian(rad - d.valueRadians());
    }

    Radian Radian::operator*(float f) const {
        return Radian(rad * f);
    }

    Radian Radian::operator*(const Radian& r) const {
        return Radian(rad * r.rad);
    }

    Radian Radian::operator/(float f) const {
        return Radian(rad / f);
    }

    bool Radian::operator<(const Radian& r) const {
        return rad < r.rad;
    }

    bool Radian::operator<=(const Radian& r) const {
        return rad <= r.rad;
    }

    bool Radian::operator==(const Radian& r) const {
        return rad == r.rad;
    }

    bool Radian::operator!=(const Radian& r) const {
        return rad != r.rad;
    }

    bool Radian::operator>=(const Radian& r) const {
        return rad >= r.rad;
    }

    bool Radian::operator>(const Radian& r) const {
        return rad > r.rad;
    }


    // --------------------- DEGREES ---------------------
	Degree::Degree(float d) : deg(d){
		deg = d;
	}
	
	Degree::Degree(const Radian& r ) 
		: deg(r.valueDegrees()) {}
	
	Degree::Degree (const HUGO::Degree& rhs) 
		: deg(rhs.deg) {}
	

	Degree& Degree::operator = ( const float& f ) { 
		deg = f; 
		return *this; 
	}
	
	Degree& Degree::operator = ( const Degree& d ) { 
		deg = d.deg; 
		return *this; 
	}

	Degree& Degree::operator = (const Radian& r ) { 
		deg = r.valueDegrees(); 
		return *this; 
	}

	float Degree::valueDegrees() const {
		return deg;
	}
	
	float Degree::valueRadians() const {
		return static_cast<float>(deg * (PI / 180.0f));
	}

	const Degree&  Degree::operator + () const 
	{ 
		return *this; 
	}

	Degree  Degree::operator + ( const Degree& d ) const { 
		return Degree ( deg + d.deg ); 
	}

	Degree  Degree::operator + ( const Radian& r ) const 
	{ 
		return Degree ( deg + r.valueDegrees() ); 
	}

	Degree Degree::operator - () const { 
		return Degree(-deg); 
	}

	Degree Degree::operator - ( const Degree& d ) const { 
		return Degree ( deg - d.deg ); 
	}

	Degree Degree::operator - ( const Radian& r ) const { 
		return Degree ( deg - r.valueDegrees() ); 
	}

	Degree Degree::operator * ( float f ) const { 
		return Degree ( deg * f ); 
	}

	Degree Degree::operator * ( const Degree& f ) const { 
		return Degree ( deg * f.deg ); 
	}

	Degree& Degree::operator *= ( float f ) { 
		deg *= f; return *this; 
	}

	Degree Degree::operator / ( float f ) const { 
		return Degree ( deg / f ); 
	}

	Degree& Degree::operator /= ( float f ) { 
		deg /= f; 
		return *this; 
	}

	bool Degree::operator <  ( const Degree& d ) const { 
		return deg <  d.deg; 
	}

	bool Degree::operator <= ( const Degree& d ) const { 
		return deg <= d.deg; 
	}
	
	bool Degree::operator == ( const Degree& d ) const { 
		return deg == d.deg; 
	}

	bool Degree::operator != ( const Degree& d ) const { 
		return deg != d.deg; 
	}

	bool Degree::operator >= ( const Degree& d ) const { 
		return deg >= d.deg; 
	}

	bool Degree::operator >  ( const Degree& d ) const { 
		return deg >  d.deg; 
	}
}
