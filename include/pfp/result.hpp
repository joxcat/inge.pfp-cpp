#ifndef __PFP_RESULT_HPP__
#define __PFP_RESULT_HPP__

#include "asenum.h"

enum class GenericResultCode {
	Success,
	Error
};

using GenericResult = asenum::AsEnum<
	asenum::Case11<GenericResultCode, GenericResultCode::Success, void>,
	asenum::Case11<GenericResultCode, GenericResultCode::Error, const char *>
>;

#endif // __PFP_RESULT_HPP__