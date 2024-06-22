#pragma once

const float M_PI = 3.14159265358979323846;

[[gnu::pure]]
inline float degreesToRadians(float degrees) {
	return degrees * (M_PI / 180.0);
}

struct Vector3 {
	float x;
	float y;
	float z;

	Vector3(float _x, float _y, float _z) {
		x = _x;
		y = _y;
		z = _z;
	}

	[[gnu::pure]]
	Vector3 operator+(const Vector3& other) const {
		return Vector3(x + other.x, y + other.y, z + other.z);
	}

	[[gnu::pure]]
	Vector3 operator-(const Vector3& other) const {
		return Vector3(x - other.x, y - other.y, z - other.z);
	}

	[[gnu::pure]]
	Vector3 operator*(const float& other) const {
		return Vector3(x * other, y * other, z * other);
	}

	[[gnu::pure]]
	Vector3 operator/(const float& other) const {
		return Vector3(x / other, y / other, z / other);
	}

	[[gnu::pure]]
	Vector3 normalize() const {
		float len = length();
		if (len == 0 || len == -std::numeric_limits<float>::infinity())
			return Vector3(0, 0, 0); // better than error
		return Vector3(x / len, y / len, z / len);
	}

	[[gnu::pure]]
	Vector3 cross(const Vector3& other) const {
		return Vector3(
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x
		);
	}

	[[gnu::pure]]
	float dot(const Vector3& other) const {
		return x * other.x + y * other.y + z * other.z;
	}

	[[gnu::pure]]
	float length() const {
		return std::sqrt(x * x + y * y + z * z);
	}

	[[gnu::pure]]
	float area(const Vector3& other) const {
		return cross(other).length();
	}

	[[gnu::pure]]
	Vector3 rotate(const Vector3& rotation) {
		float pitch = rotation.x * (M_PI / 180.0f);
		float yaw = rotation.y * (M_PI / 180.0f);
		float roll = rotation.z * (M_PI / 180.0f);

		float cosPitch = cos(pitch);
		float sinPitch = sin(pitch);
		float cosYaw = cos(yaw);
		float sinYaw = sin(yaw);
		float cosRoll = cos(roll);
		float sinRoll = sin(roll);

		float m00 = cosYaw * cosRoll;
		float m01 = -cosYaw * sinRoll;
		float m02 = sinYaw;
		float m10 = sinPitch * sinYaw * cosRoll + cosPitch * sinRoll;
		float m11 = -sinPitch * sinYaw * sinRoll + cosPitch * cosRoll;
		float m12 = -sinPitch * cosYaw;
		float m20 = -cosPitch * sinYaw * cosRoll + sinPitch * sinRoll;
		float m21 = cosPitch * sinYaw * sinRoll + sinPitch * cosRoll;
		float m22 = cosPitch * cosYaw;

		return Vector3(
			x * m00 + y * m01 + z * m02,
			x * m10 + y * m11 + z * m12,
			x * m20 + y * m21 + z * m22
		);
	}
};

struct Triangle {
	Vector3 pointA;
	Vector3 pointB;
	Vector3 pointC;

	Triangle(Vector3 _pointA, Vector3 _pointB, Vector3 _pointC)
		: pointA(_pointA), pointB(_pointB), pointC(_pointC) {}

	[[gnu::pure]]
	Vector3 normal() const {
		Vector3 vectorAB = Vector3(pointB.x - pointA.x, pointB.y - pointA.y, pointB.z - pointA.z);
		Vector3 vectorAC = Vector3(pointC.x - pointA.x, pointC.y - pointA.y, pointC.z - pointA.z);
		return vectorAB.cross(vectorAC).normalize();
	}

	[[gnu::pure]]
	float area() const {
		Vector3 vectorAB = Vector3(pointB.x - pointA.x, pointB.y - pointA.y, pointB.z - pointA.z);
		Vector3 vectorAC = Vector3(pointC.x - pointA.x, pointC.y - pointA.y, pointC.z - pointA.z);
		return 0.5f * vectorAB.cross(vectorAC).length();
	}

	[[gnu::pure]]
	float intersect(Vector3 ray, Vector3 cameraPosition) {
		Vector3 translatedPointA = pointA - cameraPosition;
		Vector3 translatedPointB = pointB - cameraPosition;
		Vector3 translatedPointC = pointC - cameraPosition;

		Vector3 edge0 = translatedPointB - translatedPointA;
		Vector3 edge1 = translatedPointC - translatedPointA;
		Vector3 normal = edge0.cross(edge1).normalize();

		float dotOfRayAndNormal = ray.dot(normal);
		if (dotOfRayAndNormal == 0) return std::numeric_limits<float>::infinity();

		float dotToPlane = translatedPointA.dot(normal);

		/**/ // Toggle - backface culling OFF X ON
		if (dotToPlane >= 0) return std::numeric_limits<float>::infinity();
		/**/

		float distance = dotToPlane / dotOfRayAndNormal;
		Vector3 pointOfHit = ray * distance;

		Vector3 V0P = pointOfHit - translatedPointA;
		Vector3 V1P = pointOfHit - translatedPointB;
		Vector3 V2P = pointOfHit - translatedPointC;

		if (normal.dot(edge0.cross(V0P)) > 0 &&
			normal.dot((translatedPointC - translatedPointB).cross(V1P)) > 0 &&
			normal.dot((translatedPointA - translatedPointC).cross(V2P)) > 0) {
			return distance;
		}
		return std::numeric_limits<float>::infinity();
	}
};