#ifndef PNRF_pnrferror_h
#define PNRF_pnrferror_h

#include <comdef.h>
#include <exception>

namespace pnrfr {
	class PnrfError
	{
		HRESULT error;
	public:
		PnrfError();
		PnrfError(HRESULT const hr) : error(hr) {};
		~PnrfError();

		static void setComError(const _com_error err, char** & msg);
		static void setError(const std::exception e, char** & msg);
		static void setUnknownError(char** & msg);
	};
}
#endif

