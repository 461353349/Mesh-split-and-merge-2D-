#pragma once

struct Point {
	float x;
	float y;

    Point operator+(const Point& a)
    {
        return Point {
            a.x + x,
            a.y + y
        };
    }

    Point operator*(const int i)
    {
        return Point {
            i * x,
            i * y
        };
    }

    bool operator==(const Point& a) const
    {
        return x == a.x && y == a.y;
    }
};

// retourne la distance au carre entre 2 points
float dist2(Point *a, Point *b);
// retourne la distance entre 2 points
float dist(Point *a, Point *b);

// retourne la distance entre le point p et le segment ab
// http://stackoverflow.com/questions/849211/shortest-distance-between-a-point-and-a-line-segment
float minimumDistance(Point *a, Point *b, Point *p);
