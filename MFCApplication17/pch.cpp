// pch.cpp: source file corresponding to the pre-compiled header

#include "pch.h"
HWND g_viewHwnd = NULL;
HWND g_classViewWnd = NULL;
HWND g_propertiesViewWnd = NULL;
mutex g_mtx;
thread g_thread;
BOOL g_bImplicit = false;
// When you are using pre-compiled headers, this source file is necessary for compilation to succeed.
