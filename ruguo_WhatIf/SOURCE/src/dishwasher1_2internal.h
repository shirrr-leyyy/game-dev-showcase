//---------------------------------------------------------
// file:	dishwasher1_2internal.h
// author:	Dylan
// email:	dylanjoonteck.lim@digipen.edu
//
// brief:	Function declarations for Dishwasher Level 1-2
//			game - Srcub Master
//
//---------------------------------------------------------
#pragma once
#ifndef DISHWASHER1_2INTERNAL_H
#define DISHWASHER1_2INTERNAL_H

#include "cprocessing.h"  // or your CProcessing umbrella header

// Common entry points used by the two wrappers:
void Spotless_CommonInit(int hard);
void Spotless_CommonUpdate(int hard);
void Spotless_CommonExit(void);

#endif