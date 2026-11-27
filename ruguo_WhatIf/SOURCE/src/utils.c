//---------------------------------------------------------
// file:	utils.c
// author:	Shirley, Yi Ting
// email:	shirleywenhui.l@digipen.edu, yiting.tin@digipen.edu 
//
// brief:	Utility functions for handling clicks, hovers, 
//          and collision detection
//
//---------------------------------------------------------

#include <math.h>
#include "cprocessing.h"
#include "utils.h"
#include "audio.h"

int IsAreaClicked(float area_center_x, float area_center_y, float area_width, float area_height, float click_x, float click_y)
{
    float halfW = area_width / 2.0f;
    float halfH = area_height / 2.0f;

    if (click_x >= area_center_x - halfW &&
        click_x <= area_center_x + halfW &&
        click_y >= area_center_y - halfH &&
        click_y <= area_center_y + halfH) {
        if (CP_Input_MouseClicked()) {
            Audio_PlayClick();
            return 1; // clicked inside
        } 
    }

    return 0; // not clicked
}

int IsAreaHovered(float area_center_x, float area_center_y, float area_width, float area_height, float click_x, float click_y)
{
    float halfW = area_width / 2.0f;
    float halfH = area_height / 2.0f;

    if (click_x >= area_center_x - halfW &&
        click_x <= area_center_x + halfW &&
        click_y >= area_center_y - halfH &&
        click_y <= area_center_y + halfH) {
        return 1; // clicked inside
    }

    return 0; // not clicked
}

int IsTextHovered(const char* text, float x, float y, int fontSize, float mx, float my)
{
    int len = 0;
    while (text[len] != '\0')
        len++;

    float charWidth = fontSize * 0.48f;
    float textWidth = charWidth * len;
    float textHeight = fontSize * 0.9f;

    float halfW = textWidth / 2.0f;
    float halfH = textHeight / 2.0f;

    float left = x - halfW;
    float right = x + halfW;
    float top = y - halfH;
    float bottom = y + halfH;

    int hover = (mx >= left && mx <= right && my >= top && my <= bottom);

    return hover;
}


int IsCircleClicked(float circle_center_x, float circle_center_y, float diameter, float click_x, float click_y)
{
    float radius = diameter / 2.0f;
    float dx = click_x - circle_center_x;
    float dy = click_y - circle_center_y;
    float distanceSq = dx * dx + dy * dy;

    if (distanceSq <= radius * radius)
        return 1;  // click is inside the circle
    else
        return 0;  // click is outside
}

int IsEllipseClicked(float cx, float cy, float rx, float ry, float mx, float my)
{
    float dx = (mx - cx) / rx;
    float dy = (my - cy) / ry;

    return (dx * dx + dy * dy) <= 1.0f;
}

int AreCirclesIntersecting(float c1_x, float c1_y, float r1, float c2_x, float c2_y, float r2) {
    double x_distance = (c1_x - c2_x) * (c1_x - c2_x);
    double y_distance = (c1_y - c2_y) * (c1_y - c2_y);
    double distance = sqrt(x_distance + y_distance);
    return distance < (r1 + r2);
}

int EllipseRectCollide(
    float cx, float cy,         // ellipse center
    float rx, float ry,         // ellipse radii (half-width, half-height)
    float rx1, float ry1,       // rectangle center
    float rw, float rh)         // rectangle width and height
{
    // rectangle half-size
    float halfW = rw * 0.5f;
    float halfH = rh * 0.5f;

    // nearest point on rect to ellipse center
    float nearestX = cx;
    if (cx < rx1 - halfW) nearestX = rx1 - halfW;
    else if (cx > rx1 + halfW) nearestX = rx1 + halfW;

    float nearestY = cy;
    if (cy < ry1 - halfH) nearestY = ry1 - halfH;
    else if (cy > ry1 + halfH) nearestY = ry1 + halfH;

    // normalize into ellipse coordinate space
    float dx = (nearestX - cx) / rx;
    float dy = (nearestY - cy) / ry;

    // collide if inside ellipse formula
    return (dx * dx + dy * dy) <= 1.0f;
}