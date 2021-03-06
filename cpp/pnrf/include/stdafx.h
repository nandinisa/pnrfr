// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef PNRF_stdafx_h
#define PNRF_stdafx_h

//#include "targetver.h"

#include <stdio.h>
#include <conio.h>			
#include <fstream>
#include <typeinfo>
#include <vector>
#include <memory>
#include <algorithm>
#include <atlcomcli.h>
#include "pnrferror.h"

/* COM handling classes*/
inline void HR(HRESULT const result) {
	if (result != S_OK) {
		//throw pnrfr::PnrfError(result);
	}
}

enum class Apartment {
	Multithreaded = COINIT_MULTITHREADED,
	Singlethreaded = COINIT_APARTMENTTHREADED
};
class ComRuntime {
public:
	ComRuntime() {
		HR(CoInitializeEx(NULL, COINIT_MULTITHREADED));
	}

	~ComRuntime() {
		CoUninitialize();
	}
};

#endif