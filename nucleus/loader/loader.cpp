/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "loader.h"

Filetype detectFiletype(const std::string& filename)
{
    if (filename.size() == 0) {
		return FILETYPE_ERROR;
	}
}
