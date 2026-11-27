//---------------------------------------------------------
// file:	utils.h
// author:	Shirley, Yi Ting
// email:	shirleywenhui.l@digipen.edu, yiting.tin@digipen.edu 
//
// brief:	Function declarations for Utility functions for 
//          handling clicks, hovers, and collision detection
//
//---------------------------------------------------------

#pragma once

int IsAreaClicked(float area_center_x, float area_center_y, float area_width, float area_height, float click_x, float click_y);
int IsAreaHovered(float area_center_x, float area_center_y, float area_width, float area_height, float click_x, float click_y);
int IsTextHovered(const char* text, float x, float y, int fontSize, float mx, float my);
int IsCircleClicked(float circle_center_x, float circle_center_y, float diameter, float click_x, float click_y);
int IsEllipseClicked(float cx, float cy,
    float rx, float ry,
    float mx, float my);
int AreCirclesIntersecting(float c1_x, float c1_y, float r1, float c2_x, float c2_y, float r2);
int EllipseRectCollide(
    float cx, float cy,         // ellipse center
    float rx, float ry,         // ellipse radii (half-width, half-height)
    float rx1, float ry1,       // rectangle center
    float rw, float rh);