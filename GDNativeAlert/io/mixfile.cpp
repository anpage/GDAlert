/**
 * @file
 *
 * @author CCHyper
 * @author OmniBlade
 *
 * @brief  Handles the "Mix" file archives used by Westwood games.
 *
 * @copyright Chronoshift is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#include "mixfile.h"
#include "gamefile.h"

// Explicit template instantiation.
template class MixFileClass<GameFileClass>;
