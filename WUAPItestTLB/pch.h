#pragma once

#include <winsdkver.h>

#define _WIN32_WINNT _WIN32_WINNT_WIN10

#include <sdkddkver.h>


#include <windows.h>
#include <objbase.h>
#include <wuapi.h>

#include <iostream>

#include <comutil.h>
#include <comdef.h>
#include <comip.h>

#import "wuapi.tlb" no_dual_interfaces rename_namespace("WU")
