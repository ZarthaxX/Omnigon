#include "Point.h"

namespace MCTSGon {

	Point::Point() {

	}

	Point::Point(int x_, int y_)
		: x(x_), y(y_), xC(x_), yC(y_) {

	}
	Point::Point(const Point& p)
		: x(p.x), y(p.y) {

	}

	Point& Point::operator=(const Point& p) {
		x = p.x;
		y = p.y;
		return *this;
	}

	inline bool Point::operator<(const Point& p) const {
		return y < p.y || (y == p.y && x < p.x);
	}

	inline bool Point::operator>(const Point& p) const {
		return y > p.y || (y == p.y && x > p.x);
	}

	inline bool Point::operator==(const Point& p) const {
		return p.x == x && p.y == y;
	}

	inline bool Point::operator!=(const Point& p) const {
		return p.x != x || p.y != y;
	}

	inline Point Point::operator+(const Point& p) const {
		return Point(p.x + x, p.y + y);
	}

	inline Point& Point::operator+=(const Point& p) {
		x = x + p.x;
		y = y + p.y;

		return *this;
	}

	inline Point Point::operator-(const Point& p) const {
		return Point(x - p.x, y - p.y);
	}

	inline Point& Point::operator-=(const Point& p) {
		x = x - p.x;
		y = y - p.y;

		return *this;
	}

	inline void Point::Update(int x_, int y_) {
		x = x_;
		y = y_;
	}

	inline void Point::Copy(const Point& p) {
		x = p.x;
		y = p.y;
	}

#ifdef DATA_COPY

	inline void Point::Load() {
		x = xC;
		y = yC;
	}

	inline void Point::Save() {
		xC = x;
		yC = y;
	}

#endif

	Point Point::Decode_position(int position, int width) {
		return Point(position%width, position / width);
	}

	std::string Point::ToString() {
		return std::to_string(x) + " " + std::to_string(y);
	}

}