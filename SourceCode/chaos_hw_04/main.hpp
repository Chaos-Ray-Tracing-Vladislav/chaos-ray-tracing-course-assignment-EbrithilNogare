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

	Vector3 normalize() const {
		float len = length();
		if (len == 0)
			return Vector3(0, 0, 0); // better than error
		return Vector3(x / len, y / len, z / len);
	}

	Vector3 cross(const Vector3& other) const {
		return Vector3(
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x
		);
	}

	float length() const {
		return std::sqrt(x * x + y * y + z * z);
	}

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

	Vector3 normal() const {
		Vector3 vectorAB = Vector3(pointB.x - pointA.x, pointB.y - pointA.y, pointB.z - pointA.z);
		Vector3 vectorAC = Vector3(pointC.x - pointA.x, pointC.y - pointA.y, pointC.z - pointA.z);
		return vectorAB.cross(vectorAC).normalize();
	}

	float area() const {
		Vector3 vectorAB = Vector3(pointB.x - pointA.x, pointB.y - pointA.y, pointB.z - pointA.z);
		Vector3 vectorAC = Vector3(pointC.x - pointA.x, pointC.y - pointA.y, pointC.z - pointA.z);
		return 0.5f * vectorAB.cross(vectorAC).length();
	}
};