// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include <algorithm>
#include <cctype>
#include <math.h>
#include <unordered_map>
#include "framework.h"
#include "Coordinate.h"
#include "MathExpression.h"
#include "Variable.h"
extern HWND g_viewHwnd;
extern HWND g_classViewWnd;
#define WM_USER_NOTIFY WM_USER+1
#define WM_USER_SELECT WM_USER+2
#endif //PCH_H
