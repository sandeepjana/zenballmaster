#include <utility>
#include <vector>
#include <cassert>

typedef std::pair<float, float> point_t;
typedef std::pair<point_t, point_t> line_t;

struct triangle_t
{
    triangle_t() {}
    triangle_t(const point_t& _u, const point_t& _v, const point_t& _w)
        : u(_u), v(_v), w(_w), isColored(false) {}
    
    point_t u;
    point_t v;
    point_t w;

    bool isColored;
    char color[3]{};

    enum class EDGE_T
    {
        NO, UV, VW, WU
    };

    enum class VERTEX_T
    {
        U = 0,  V, W, NO
    };
};

triangle_t::VERTEX_T find_vertex(const triangle_t& a, const point_t& b)
{
    if (a.u == b) return triangle_t::VERTEX_T::U;
    if (a.v == b) return triangle_t::VERTEX_T::V;
    if (a.w == b) return triangle_t::VERTEX_T::W;

    return triangle_t::VERTEX_T::NO;
}

triangle_t::VERTEX_T find_other_vertex(triangle_t::VERTEX_T v1, triangle_t::VERTEX_T v2)
{
    assert(v1 != v2);

    if (v2 < v1) std::swap(v1, v2);

    if (triangle_t::VERTEX_T::U == v1 && triangle_t::VERTEX_T::V == v2)
        return (triangle_t::VERTEX_T::W);

    if (triangle_t::VERTEX_T::U == v1 && triangle_t::VERTEX_T::W == v2)
        return (triangle_t::VERTEX_T::V);

    if (triangle_t::VERTEX_T::V == v1 && triangle_t::VERTEX_T::W == v2)
        return (triangle_t::VERTEX_T::U);

    assert(!"find_other_vertex gone wrong");
    return triangle_t::VERTEX_T::NO;
}



enum class TURN_T
{
    RIGHT,
    LEFT,
    COLLINEAR
};

enum class POINT_LOC_T
{
    INSIDE,
    OUTSIDE,
    ON_BOUNDARY
};

point_t operator-(const point_t& a, const point_t& b)
{
    return std::make_pair(a.first - b.first, a.second - b.second);
}

point_t operator+(const point_t& a, const point_t& b)
{
    return std::make_pair(a.first + b.first, a.second + b.second);
}

inline bool nearly_equal(float a, float b)
{
    //not so trivial but works for now
    return std::abs(a - b) < 0.000001;
}

TURN_T find_turn(const point_t& p1, const point_t& p2, const point_t& p3)
{
    /*
    a = p2 - p1, b = p3 - p2 say
    */
    auto a0 = p2.first - p1.first;
    auto a1 = p2.second - p1.second;

    auto b0 = p3.first - p2.first;
    auto b1 = p3.second - p2.second;

    auto det = a0 * b1 - a1 * b0;

    return (det > 0) ? TURN_T::LEFT : (det == 0.0 ? TURN_T::COLLINEAR : TURN_T::RIGHT);
}

float randfloat() //[-1, 1]
{
    return (float)(rand() % 100) / 50.0f - 1.0f;
}
float randfloat_narrow() //[0.0, 0.2]
{
    return (float)(rand() % 100) / 500.0f;
}

point_t randPoint()
{
    return std::make_pair(randfloat(), randfloat());
}
point_t randPoint_topband()
{
    return std::make_pair(randfloat(), randfloat_narrow() + 0.8f);
}
point_t randPoint_bottomband()
{
    return std::make_pair(randfloat(), randfloat_narrow() - 1.0f);
}

line_t randLine()
{
    auto p1 = randPoint_topband();
    auto p2 = randPoint_bottomband();

    return std::make_pair(p1, p2);
}

line_t randLine_really_random()
{
    auto p1 = randPoint();
    auto p2 = randPoint();

    if (p1.second < p2.second ||
        (p1.second == p2.second && p1.first < p2.first))
    {
        std::swap(p1, p2); //make p1 the top point.
    }

    return std::make_pair(p1, p2);
}

float cross_product(const point_t& a, const point_t& b)
{
    return (a.first * b.second - a.second * b.first);
}

float dot_product(const point_t& a, const point_t& b)
{
    return (a.first * b.first + a.second * b.second);
}

std::pair<bool, point_t> find_segment_pair_intersection(const line_t& seg1, 
    const line_t& seg2)
{
    point_t p = seg1.first;
    point_t r = seg1.second - seg1.first;

    point_t q = seg2.first;
    point_t s = seg2.second - seg2.first;

    //The line segments are (p, p + r) and (q, q + s)
    //case 1: collinear if r x s = 0 and (q - p) x r = 0
    //    r x s = 0 means vectors r and s are collinear
    //    Now, the segments will be collinear if r (and hence s) is in the same direction as (q - p)

    //  p ___p+r q___q+s
    //    \      /
    //     \    /
    //      \  /
    //       \/
    //        Origin

    //case 1A: segments overlap if length of (q - p) is less than r (or s) AND the 
    // direction of (q - p) is same as r (or opposite to direction of s).
    // That is, either 0 <= (q - p).r <= r.r or 0 <= (p - q).s <= s.s 
    // Length = projection onto (unit) vector because we already know the vectors r, s, (p -q) are collinear)

    //case 1B: segments also overlap if |q - p| <= |r| + |s| and the directions of r (or q - p) and s are opposite
    //  That is, r.s < 0 and 0 <= (q - p).r <= (r - s).r //i added this formula so be careful.

    //case 1C: segments are non overlapping otherwise

    //case 2: segments are parallel (and not collinear) if r x s = 0 and (q - p) x r != 0
    //  (p + r)  (q + s)
    //        |  |
    //        |  |
    //      p \  / q
    //         \/

    //case 3: the lines obtained by extending the segments intersect if r x s != 0
    //In parametric form, the lines are given by p + tr and q + us where t and u are scalars.
    //If at all the lines intersect, then
    //    | t | = | (q - p) x s | / | r x s | and 
    //    | u | = | (q - p) x r | / | r x s | 
    //    the sign of t or u is + ve if the numerator and denominator have same sign(without abs).

    //case 3A: the segments intersect if 0 <= t, u <= 1
    // The point of intersection is given by p + tr. 
    //case 3B: otherwise the segments are neither parallel nor intersect


    auto dummy = point_t(0.f, 0.f);
    auto rxs = cross_product(r, s);
    auto qminusp = q - p;
    auto qminuspxr = cross_product(qminusp, r);

    //case 1
    if (nearly_equal(rxs, 0.0f) && nearly_equal(qminuspxr, 0.0f))
    {
        //segments are collinear
        auto qminuspdotr = dot_product(qminusp, r);
        auto pminusqdots = dot_product(p - q, s);
        auto rdotr = dot_product(r, r);
        auto sdots = dot_product(s, s);

        if (0 <= qminuspdotr && qminuspdotr <= rdotr)
            return std::make_pair(true, seg2.first);

        if(0 <= pminusqdots && pminusqdots <= sdots)
            return std::make_pair(true, seg1.first);

        if (dot_product(r, s) < 0)
        {
            if (0 <= qminuspdotr && qminuspdotr <= dot_product(r - s, r))
                return std::make_pair(true, seg1.second);
        }
           
        //collinear but disjoint
        return std::make_pair(false, dummy);
    }

    //case 2
    if (nearly_equal(rxs, 0.0f) /* and qminuspxr is non zero */)
    {
        //parallel but not collinear
        return std::make_pair(false, dummy);
    }

    auto t = cross_product(qminusp, s) / rxs;
    auto u = qminuspxr / rxs;

    if (0 <= t && t <= 1 && 0 <= u && u <= 1)
    {
        auto i = p;
        i.first += t * r.first;
        i.second += t * r.second;
        return std::make_pair(true, i);
    }

    return std::make_pair(false, point_t(0.f, 0.f));
}

bool compare_lines_by_angle(const line_t& a, const line_t& b) //angle with -ve x-axis
{
    auto deltax1 = a.first.first - a.second.first; // top - bottom
    auto deltax2 = b.first.first - b.second.first; // ditto
    auto deltay1 = a.first.second - a.second.second;
    auto deltay2 = b.first.second - b.second.second;

    assert(deltay1 >= 0);
    assert(deltay2 >= 0);

    bool result = true;

    //Now comes the tricky part with discontinues towards infinities and misbehaving inequalities
    //To be checked: arctan(slope1) < arctan(slope2) 
    //<-- assuming that the top point of segments is origin and the angle is measured from -ve x-axis
    //If slope1 and slope2 are of same sign, then it is (slope1 < slope2)
    //If slope1 and slope2 are of opposite sign (i.e., one seg is in 3rd and another is in 4th quadrant)
    //then +ve slope is smaller angle

    // slope 1 < slope 2 ==>
    // deltay1/deltax1 < deltay2/deltax2
    // deltax1/deltay1 > deltax2/deltay2
    // we know deltay1 and deltay2 are +ve
    // deltax1 * deltay2 > deltax2 * deltay1
    if (deltax1 * deltax2 >= 0) //same delta-x sign and anyway we know delta-y sign is +ve
    {
        result = (deltax1 * deltay2 > deltax2 * deltay1);
        //This condition also covers the corner cases:
        //if (0 == deltax1)
        //    return (deltax2 < 0);
        //if (0 == deltax2)
        //    return (deltax1 > 0);
    }
    else //opposite sign slopes
    {
        result = (deltax1 > 0); //condition - 2
    }

    //Aha, the other way is to find the cross product of the segments
    //let the segment-vectors to be defined such that the direction is from their
    //upper end point to the lower end point. And then position these vectors such 
    //that they start at origin: then vector = bottom - top
    auto v1 = point_t(-deltax1, -deltay1);
    auto v2 = point_t(-deltax2, -deltay2);
    auto sign = (0 < cross_product(v1, v2)); 
    //cross product (deltax1 * deltay2 - deltax2 * deltay1) > 0?? Exactly same as above...
    //Therefore, the condition 2 need not be tested separately!!
    assert(sign == result);

    return result;
}

inline POINT_LOC_T is_point_inside_triangle(const triangle_t& t, const point_t& p)
{
    auto c1 = cross_product(t.v - t.u, p - t.v);
    auto c2 = cross_product(t.w - t.v, p - t.w);
    auto c3 = cross_product(t.u - t.w, p - t.u);

    //why to check for the negative c's?! because the points can be CW or CCW
    if ((c1 > 0 && c2 > 0 && c3 > 0) || (c1 < 0 && c2 < 0 && c3 < 0))
        return POINT_LOC_T::INSIDE;

    if ((c1 == 0 && c2 * c3 >= 0) || (c2 == 0 && c3*c1 >= 0) || (c3 == 0 && c1 * c2 >= 0))
    {
        return POINT_LOC_T::ON_BOUNDARY;
    }

    return POINT_LOC_T::OUTSIDE;
}