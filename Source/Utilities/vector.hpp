#pragma once

namespace Utilities {
    class Vector {
    public:
        double x, y;

        Vector(double _x, double _y);
        Vector();
        Vector operator+(Vector other) const;
        Vector operator-(Vector other) const;
        Vector operator*(double scalar) const;
        Vector operator/(double scalar) const;
        Vector normalise() const;
        Vector rotate(double theta) const;
        Vector get_perpendicular() const;
        double get_slope() const;
        double dot(Vector other) const;
        double get_magnitude() const;
        double get_magnitude_squared() const;
    };
}
