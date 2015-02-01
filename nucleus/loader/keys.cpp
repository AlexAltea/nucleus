/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "keys.h"
#include "nucleus/emulator.h"

static SelfKey self_keyvault[] = {
    {}
};

const SelfKey* getSelfKey(u32 type, u64 version, u16 revision)
{
    for (auto& key : self_keyvault) {
        if (key.self_type != type) {
            continue;
        }
        if (key.version != version && (type == KEY_LV1 || type == KEY_LV2 || type == KEY_UNK7 || type == KEY_ISO)) {
            continue;
        }
        if (key.revision != revision && (type == KEY_APP || type == KEY_ISO || type == KEY_NPDRM)) {
            continue;
        }
        return &key;
    }

    nucleus.log.error(LOG_LOADER, "Key not found in keyvault!");
    return nullptr;
}
