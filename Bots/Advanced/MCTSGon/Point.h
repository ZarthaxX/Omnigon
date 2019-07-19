#pragma once
#include <algorithm>
#include <iostream>
#include <string>
#include <stdio.h>

#define DATA_COPY

namespace MCTSGon {


	class Point {
	public:
		Point();
		Point(int x_, int y_);
		Point(const Point& p);
		Point& operator=(const Point& p);
		inline bool operator<(const Point& p) const;
		inline bool operator>(const Point& p) const;
		inline bool operator==(const Point& p) const;
		inline bool operator!=(const Point& p) const;
		inline Point operator+(const Point& p) const;
		inline Point& operator+=(const Point& p);
		inline Point operator-(const Point& p) const;
		inline Point& operator-=(const Point& p);
		inline void Update(int x_, int y_);
		inline void Copy(const Point& p);

#ifdef DATA_COPY

		inline void Load();
		inline void Save();

#endif

		static Point Decode_position(int position, int width);
		std::string ToString();

		friend std::ostream& operator<<(std::ostream& s, const Point& p) {
			s << p.x << " " << p.y;
			return s;
		}

		int x, y;
	private:

#ifdef DATA_COPY

		int xC, yC;

#endif

	};

}