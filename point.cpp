#include <math.h>

#include "point.h"

// retourne la distance au carre entre 2 points
float dist2(Point *a, Point *b) {
    float x = b->x - a->x;
    float y = b->y - a->y;
    return x * x + y * y;
}

float dist(Point *a, Point *b) {
    float x = b->x - a->x;
    float y = b->y - a->y;
    return sqrt(x * x + y * y);
}

float minimumDistance(Point *a, Point *b, Point *p) {
    float ab_x = b->x - a->x;
    float ab_y = b->y - a->y;
    float apoint_x = p->x - a->x; 
    float apoint_y = p->y - a->y; 
    float dotAPoint = ab_x * apoint_x + ab_y * apoint_y;

    float length2 = dist2(a, b);
    if (length2 == 0.0) return dist(p, a);

    float t = dotAPoint / length2;
    if (t < 0.0) return dist(p, a);
    if (t > 1.0) return dist(p, b);
    Point proj {a->x + t * ab_x, a->y + t * ab_y};
    return dist(p, &proj);
}
