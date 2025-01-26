#include <cmath>
#include "vector.hpp"

namespace Utilities {
    Vector::Vector(double _x, double _y) : x(_x), y(_y) {}


    Vector::Vector() : x(0), y(0) {}


    Vector Vector::operator+(Vector other) const {return Vector(x + other.x, y + other.y);}


    Vector Vector::operator-(Vector other) const {return Vector(x - other.x, y - other.y);}


    Vector Vector::operator*(double scalar) const {return Vector(x * scalar, y * scalar);}


    Vector Vector::operator/(double scalar) const {return Vector(x / scalar, y / scalar);}


    Vector Vector::normalise() const {
        double magnitude = get_magnitude();
        return (magnitude > 0) ? *this / magnitude : *this;
    }


    Vector Vector::rotate(double theta) const {
        double _x = x * cos(theta) - y * sin(theta);
        double _y = x * sin(theta) + y * cos(theta);
        return Vector(_x, _y);
    }


    Vector Vector::get_perpendicular() const {return Vector(y, -x);}


    double Vector::get_slope() const {return y / x;}


    double Vector::dot(Vector other) const {return x * other.x + y * other.y;}


    double Vector::get_magnitude() const {return sqrt(x * x + y * y);}


    double Vector::get_magnitude_squared() const {return x * x + y * y;}
}