#ifndef MODIO_MODEVENT_H
#define MODIO_MODEVENT_H

#include "../../Globals.h"
#include "../../c/schemas/ModioModEvent.h"

namespace modio
{
class ModEvent
{
public:
  u32 id;
  u32 mod_id;
  u32 user_id;
  u32 event_type;
  u32 date_added;

  void initialize(ModioModEvent event);
};

extern nlohmann::json toJson(ModEvent &event);
} // namespace modio

#endif
