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

_COM_SMARTPTR_TYPEDEF(IUpdateSession, IID_IUpdateSession);
_COM_SMARTPTR_TYPEDEF(IUpdateSearcher, IID_IUpdateSearcher);
_COM_SMARTPTR_TYPEDEF(ISearchResult, IID_ISearchResult);
_COM_SMARTPTR_TYPEDEF(IUpdateCollection, IID_IUpdateCollection);
_COM_SMARTPTR_TYPEDEF(IUpdate, IID_IUpdate);
_COM_SMARTPTR_TYPEDEF(IInstallationBehavior, IID_IInstallationBehavior);
