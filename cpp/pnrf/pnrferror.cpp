#include "stdafx.h"
#include <iostream>	
#include <string>

namespace pnrfr {
	PnrfError::PnrfError()
	{
	}


	PnrfError::~PnrfError()
	{
	}

	void PnrfError::setComError(_com_error const err, char** & msg) {
		const std::wstring val(err.ErrorMessage());
		copy(std::begin(val), std::end(val), *msg);
		*(*msg + val.size()) = '\0';
	}


	void PnrfError::setError(std::exception const e, char** & msg) {
		const std::string val = e.what();
		copy(std::begin(val), std::end(val), *msg);
		*(*msg + val.size()) = '\0';
	}

	void PnrfError::setUnknownError(char** & msg) {
		*msg = (char*)"Unknown error ";
	}
}


