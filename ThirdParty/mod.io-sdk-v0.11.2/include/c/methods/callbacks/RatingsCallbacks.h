#ifndef MODIO_RATINGSCALLBACKS_H
#define MODIO_RATINGSCALLBACKS_H

#include "../../../Globals.h"
#include "../../../wrappers/MinizipWrapper.h"
#include "../../../wrappers/CurlWrapper.h"
#include "../../schemas/ModioResponse.h"
#include "../../schemas/ModioMod.h"
#include "../../../ModUtility.h"
#include "../../../ModioUtility.h"

extern std::map< u32, GenericRequestParams* > add_mod_rating_callbacks;

void modioOnAddModRating(u32 call_number, u32 response_code, nlohmann::json response_json);

void clearRatingsCallbackParams();

#endif
