#if 0
/*
Sweepline algorithm: The main idea is that as the line sweeps from top to bottom of the plane, 
it is sufficient that test each new segment against its immediate
right and left neighboring segments for intersection. Whenever a segment is removed, we should test
its immediate left and right neighboring segments for intersection. Also, at intersections where
the segments change order, we need to test these segments against their new immediate neighbors.

Insert segment end-points into the queue Q = ordered end-points.
When upper end-point is inserted, corresponding segment should be stored with it.
Initialize empty status structure S = ordered sequence of segments intersecting sweep line.

While Q is not empty
    Remove the next point p from Q.
    Handle p

Handle p:
Let U, L, C be the segments that contain p as their upper, lower endpoint and within the interior respectively.
Note 1: These set of segments would be neighbors in the structure S
Note 2: This is not possible: C contains one segment but U and L are empty -- because in that case p cannot be an event.
Note 3: Obviously the union of U, L and C cannot be empty: It at least contains one segment.

Report p as intersection if p is contained in more than one segment of above set of segments.

"Update the status structure" by moving the sweep line beyond the point p.
    Remove the set L from S (they no longer intersect the sweep line)
    Reverse the order of segments in the set C in S.
    Add the set U to S.
    Note: at the end of this adjustment, S should be consistent with its definition.

"Update the queue" by checking if any segments newly become neighbors
    If new segments are added (U) or intersecting segments (C) are present,
    then test the leftmost segment of {U, C} with its left neighbor, and
    test the rightmost segment of {U, C} with its right neighbor.
        If they result in intersection points below the sweep line, then add them to Q.

    If no new segments are added or no intersecting segments are present --
    that is, only old segment(s) is/are removed at p,
    then test the left and right neighbors of "p". If their intersection falls below the
    sweep line, add that to Q.

*/
//references\x06-sweepline.pdf (not sure if I implemented below refering this or not)

#if 1
#include <stdio.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <utility>
#include <limits>
#include <set>
#include <cassert>

#include "geometry.h"

//http://stackoverflow.com/questions/12171317/how-do-you-install-glut-and-opengl-in-visual-studio-2012
#include <GL\glut.h>

std::vector<point_t> points;
std::vector<line_t> lines;



enum class POINT_LOC_T
{
    TOP,
    BOT,
    INT //interior
};

struct event_point_t
{
    event_point_t(point_t p, POINT_LOC_T l, const line_t& seg)
        : point(p), loc(l), line(seg) {}

    point_t         point;
    POINT_LOC_T     loc;
    const line_t    line;
};

//raster scan order:
bool operator <(const point_t& a, const point_t& b)
{
    if ((a.second > b.second) ||
        (a.second == b.second && a.first < b.first))
        return true;

    return false;
}
bool operator >(const point_t& a, const point_t& b)
{
    if (a == b) return false;
    return !(a < b);
}

bool operator <(const event_point_t& a, const event_point_t& b)
{
    //if (a.point < b.point
    //    || (a.point == b.point && (a.line.first < b.line.first
    //    || (a.line.first == b.line.first && a.line.second < b.line.second))))
    //    return true;
    //return false;
    return a.point < b.point;

}

bool events_point_full_compare(std::multiset<event_point_t>& events, const event_point_t& eve)
{
    auto range = std::equal_range(events.begin(), events.end(), eve);
    
    return std::any_of(range.first, range.second, [&eve](const event_point_t& e)
    {
        return (e.point == eve.point && e.line == eve.line);
    });
}

void helper_insert_intersection_in_events(const line_t& seg1, const line_t& seg2,
    std::multiset<event_point_t>& events, const point_t& point,
    std::set<std::pair<line_t, line_t>>& isecSegs)
{
    if (line_t{} != seg1 && line_t{} != seg2)
    {
        if (isecSegs.count(std::make_pair(seg1, seg2))
            || isecSegs.count(std::make_pair(seg2, seg1)))
        {
            //their intersection point is already inserted...
            return;
        }

        auto x = find_segment_pair_intersection(seg1, seg2);
        //if the intersection lies below the sweep-line -- i.e., lies after the current event
        if (true == x.first && x.second > point)
        {
            isecSegs.insert(std::make_pair(seg1, seg2));

            //it may so happen that these segments intersect at their end point!!
            auto loc = (seg1.second == x.second) ? POINT_LOC_T::BOT : POINT_LOC_T::INT;
            events.insert(event_point_t(x.second, loc, seg1));

            loc = (seg2.second == x.second) ? POINT_LOC_T::BOT : POINT_LOC_T::INT;
            events.insert(event_point_t(x.second, loc, seg2));
        }
    }
}

std::vector<point_t> sweep_for_line_intersections(const std::vector<line_t>& segs)
{   
    std::multiset<event_point_t> events;
    for (auto& seg : segs)
    {
        events.insert(event_point_t(seg.first, POINT_LOC_T::TOP, seg));
        events.insert(event_point_t(seg.second, POINT_LOC_T::BOT, seg));
    }

    std::vector<point_t> isecs;
    std::vector<line_t> status;

    /* now, during sweeping process, we may compute the intersection of the same two segments
        more than once: try with    lines.resize(3); srand(2016); in main().
        For e.g., seg a & b intersect initially at P1. Suppose seg c is added and it intersected a at P2
        where P2 is above P1. When P2 is encoutered first in sweeping, during the check for intersection
        with neighbors, seg b and seg a may be tested again to yield P3. In exact arthimetic, P1 = P3.
        However, intersection(a, b) might != intersection(b, a) due to precision errors. 
        A better way is to actually maintain whether two segments are already tested for intersetion or not.
    */
    std::set<std::pair<line_t, line_t>> isecSegs;

    while (!events.empty())
    {
        std::cout << "events pending.. " << events.size() << "\n";

        auto point = events.begin()->point;
        auto range = events.equal_range(*events.begin());

        auto numSegs = std::distance(range.first, range.second);
        if (1 < numSegs)
        {
            isecs.emplace_back(range.first->point);
        }
       
        //update the status structure
        //remove L segments if any: we dont pre-compute neighbors here. But if you are really interested
        //in pre-computing, then you should find the first and last of the set of segments in the status that
        //have bottom end point as event of interest. Then their left neighbor and right neighbor respectively.
        //So two calls of find_if and then adjust the iterators after bound-check.

        auto numFinishedSeg = std::count_if(range.first, range.second, [](const event_point_t& e)
        {
            return e.loc == POINT_LOC_T::BOT;
        });

        status.erase(std::remove_if(status.begin(), status.end(), [&point](const line_t& line) {
            //those lines which are corresponding to the (same) bottom end point in range
            return (line.second == point);
        }), status.end());

        std::vector<line_t> intersegs;
        std::for_each(range.first, range.second, [&intersegs](const event_point_t& e) {
            if (POINT_LOC_T::INT == e.loc) intersegs.push_back(e.line);
        });

        assert(intersegs.size() % 2 == 0);

        /* The idea is to do partial sorting U and C separately and merging them together.
        first, remove the set C from status: 
        you could simply reverse them within the status -- but then while inserting U, you need to 
        test each segment in U against the "reversed C" for finding its proper place.
        */
        /* Also, you could have sorted the segments C from finding them in "range".
           Below, we follow a different approach where we find them in status using the facts that 
           they are already sorted in reverse order and they appear consecutively.
        */
        if (0 != intersegs.size())
        {
            auto first = std::find_first_of(status.begin(), status.end(), intersegs.begin(),
                intersegs.end());

            std::reverse_copy(first, first + intersegs.size(), intersegs.begin());
            status.erase(first, first + intersegs.size());
        }

        //Now find the left and right neighboring segments around the current event point.
        //use std::lower_bound()? Nah! You are not comparing segments against segments but against
        //a point. Instead use partition_point: returns the first iterator where the predictate is false
        auto here = std::partition_point(status.begin(), status.end(), [&point](const line_t& seg){
            return TURN_T::LEFT == find_turn(seg.first, seg.second, point);
        });

        //rather than iterators, copy the segment by value because status may resize below invalidating the iterators
        auto left = (here == status.begin()) ? line_t{}: *(here - 1);
        auto right = (here == status.end()) ? line_t{}: *(here);

        //Insert C + U segment into status at "here"
        std::vector<line_t> update(intersegs);
        std::for_each(range.first, range.second, [&update](const event_point_t& e) {
            if(POINT_LOC_T::TOP == e.loc)
                update.push_back(e.line);
        });

        //below we are going to update event queue and status struture
        //we no longer use "range" below. We delete the current point from the event queue here.
        events.erase(range.first, range.second);

        if (!update.empty())
        {
            std::sort(update.begin() + intersegs.size(), update.end(), compare_lines_by_angle);
            if (update.size() > intersegs.size())
                std::inplace_merge(update.begin(), update.begin() + intersegs.size(), update.end(),
                    compare_lines_by_angle);

            //Where to insert U + C in status?
            //ans: at the position where the point of interest falls

            //returns iterator pointing to the first element of inserted values.
            here = status.insert(here, update.begin(), update.end());
            //remeber! always assign return value to here because it may be invalidated by the insert operation

            //update the event queue for non-empty (U + C) case:
            helper_insert_intersection_in_events(left, *here, events, point, isecSegs);
            helper_insert_intersection_in_events(right, *(update.end() - 1), events, point, isecSegs);
        }
        
        if(true == update.empty()) //remember: The check is different from checkin for non-emptyness of L
        {
            assert(0 != numFinishedSeg);
            helper_insert_intersection_in_events(left, right, events, point, isecSegs);
        }

    }

    return isecs;
}

void drawString(void *font, float x, float y, char *str) {
    /* Draws string ’str’ in font ’font’, at world (x,y,0) */
    char *ch;
    glRasterPos3f(x, y, 0.0);
    for (ch = str; *ch; ch++)
        glutBitmapCharacter(font, (int)*ch);
}

void display(void) {
    /* Called when OpenGL needs to update the display */
    glClear(GL_COLOR_BUFFER_BIT); /* Clear the window */

    //The model view matrix settings here seem to be optional
    glLoadIdentity(); /* start with a unit modelview matrix */
    gluLookAt(0.0, 0.0, 0.5, /* position of camera */
        0.0, 0.0, 0.0, /* point at which camera looks */
        0.0, 1.0, 0.0); /* "up" direction of camera */

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPointSize(4.0);
    glLineWidth(1.5);

    if (points.size())
    {
        glBegin(GL_POINTS);
        //std::for_each(lines.begin(), lines.end(), [](line_t& line) {
        //    glVertex3f(line.first.first, line.first.second, 0.0);
        //});
        std::for_each(points.begin(), points.end(), [](point_t& p) {
            glVertex3f(p.first, p.second, 0.0);
        });
        glEnd();
    }

    if (lines.size())
    {
        glBegin(GL_LINES);
        //each pair of vertices form a line
        std::for_each(lines.begin(), lines.end(), [](line_t& line) {
            glVertex3f(line.first.first, line.first.second, 0.0);
            glVertex3f(line.second.first, line.second.second, 0.0);
        });
        glEnd();
    }

    //label top end-points of lines
    auto tmp = lines;
    std::sort(tmp.begin(), tmp.end(), [](const line_t& a, const line_t& b){
        return a.first < b.first;;
    });
    int i = 0;
    for (auto& line : tmp)
    {
        //glColor3f(1.0, 1.0, 1.0); /* Select white */
        glRasterPos3f(line.first.first, line.first.second, 0.0);
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)('a' + i));
        ++i;
    }

    i = 0;
    for (auto& p : points)
    {
        glRasterPos3f(p.first, p.second, 0.0);
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)('0' + i));
        ++i;
    }

    printf(".\n");

    glFlush(); /* Force update of screen */
}

void keyboard(unsigned char key, int x, int y) {
    /* Called when a key is pressed */
    if (key == 27) exit(0); /* 27 is the Escape key */
}

int main(int argc, char** argv)
{
    lines.resize(20);

    srand(2015); //srand(2016);
    std::generate(lines.begin(), lines.end(),
        []() {
            //return randLine();
            return randLine_really_random();
        }
    );

#if 0
    auto numLines = lines.size();
    for (size_t i = 0; i < numLines; ++i)
    {
        for (size_t j = i + 1; j < numLines; ++j)
        {
            auto result = find_segment_pair_intersection(lines[i], lines[j]);
            if (result.first)
            {
                points.push_back(result.second);
            }
        }
    }
#endif

#if 0 //testing the function compare_lines_by_angle
    for (auto& line : lines)
    {
        line.first = {};
    }
    lines.push_back(std::make_pair(point_t(0.0f, 0.0f), point_t(0.75f, 0.0f)));
    lines.push_back(std::make_pair(point_t(0.0f, 0.0f), point_t(0.0f, -0.75f)));
    std::random_shuffle(lines.begin(), lines.end());
    std::sort(lines.begin(), lines.end(), compare_lines_by_angle);
#endif

    points = sweep_for_line_intersections(lines);

    glutInit(&argc, argv); /* Initialise OpenGL */
    glutCreateWindow("Canvas"); /* Create the window */
    glutDisplayFunc(display); /* Register the "display" function */
    glutKeyboardFunc(keyboard);
    glutMainLoop(); /* Enter the OpenGL main loop */

}
#endif


//std::vector<std::pair<int, int>> p(20);
//std::generate(p.begin(), p.end(), [](){
//    return std::make_pair(rand() % 5, rand() % 5);
//});
//
//auto ev = std::set<std::pair<int, int>>(p.begin(), p.end());
#endif