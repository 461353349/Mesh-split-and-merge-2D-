#include "cell.h"

Edge getEdgeCell(Point* p0, Point* p1) {
	Edge e;
	e.p0 = *p0;
	e.p1 = *p1;
	return e;
}
