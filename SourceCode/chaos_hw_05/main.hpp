#pragma once

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
		if (len == 0)
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
	float intersect(Vector3 ray) {
		Vector3 edge0 = pointB - pointA;
		Vector3 edge1 = pointC - pointA;
		Vector3 normal = edge0.cross(edge1).normalize();

		float dotOfRayAndNormal = ray.dot(normal);
		if (dotOfRayAndNormal == 0) return std::numeric_limits<float>::infinity();

		float dotToPlane = pointA.dot(normal);
		if (dotToPlane >= 0) return std::numeric_limits<float>::infinity();

		float distance = dotToPlane / dotOfRayAndNormal;
		Vector3 pointOfHit = ray * distance;

		Vector3 V0P = pointOfHit - pointA;
		Vector3 V1P = pointOfHit - pointB;
		Vector3 V2P = pointOfHit - pointC;

		if (normal.dot(edge0.cross(V0P)) > 0 &&
			normal.dot((pointC - pointB).cross(V1P)) > 0 &&
			normal.dot((pointA - pointC).cross(V2P)) > 0) {
			return distance;
		}
		return std::numeric_limits<float>::infinity();
	}
};