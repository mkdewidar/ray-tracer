#ifndef INTERVAL_H
#define INTERVAL_H

#include <limits>

class Interval {
    public:
        double min, max;

        Interval();

        Interval(double mn, double mx);

        bool contains(double x) const;

        bool surrounds(double x) const;

        static Interval const empty, universe;
};

// ------

Interval const Interval::empty = Interval(std::numeric_limits<double>::infinity(),
                                          -std::numeric_limits<double>::infinity());
Interval const Interval::universe = Interval(-std::numeric_limits<double>::infinity(),
                                             std::numeric_limits<double>::infinity());

Interval::Interval() : min(empty.min), max(empty.max) { }

Interval::Interval(double mn, double mx) : min(mn), max(mx) { }

bool Interval::contains(double x) const {
    return (this->min <= x) && (x <= this->max);
}

// same as contains but exclusive
bool Interval::surrounds(double x) const {
    return (this->min < x) && (x < this->max);
}

#endif