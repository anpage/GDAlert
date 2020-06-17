#include "RedAlertContent.h"

#include "pk.h"
#include "gamefile.h"

using namespace godot;

// From PK.H
const long RedAlertContent::FAST_KEY_EXP = 65537L;

// From CONST.CPP, base64 encoded
// "AihRvNoIbTn85FZRYNZRcT+i6KpU+maCsEqr3Q5q+LDB5tH7Tz2qQ38V"
const char RedAlertContent::FAST_KEY_MOD[42] = { '\x02', '\x28', '\x51', '\xbc', '\xda', '\x08', '\x6d',
                                                 '\x39', '\xfc', '\xe4', '\x56', '\x51', '\x60', '\xd6',
                                                 '\x51', '\x71', '\x3f', '\xa2', '\xe8', '\xaa', '\x54',
                                                 '\xfa', '\x66', '\x82', '\xb0', '\x4a', '\xab', '\xdd',
                                                 '\x0e', '\x6a', '\xf8', '\xb0', '\xc1', '\xe6', '\xd1',
                                                 '\xfb', '\x4f', '\x3d', '\xaa', '\x43', '\x7f', '\x15' };

void RedAlertContent::_register_methods() {
}

RedAlertContent::RedAlertContent() {
}

RedAlertContent::~RedAlertContent() {
}

void RedAlertContent::_init() {
    PKey fast_key = PKey();
    uint8_t buffer[512];
    Int<MAX_UNIT_PRECISION> exp(FAST_KEY_EXP);
    MPMath::DER_Encode(exp, buffer, MAX_UNIT_PRECISION);
    fast_key.Decode_Exponent(buffer);
    fast_key.Decode_Modulus(FAST_KEY_MOD);

    GameMixFile redalert_mix("RedAlert\\REDALERT.MIX", &fast_key);
    GameMixFile* local_mix = new GameMixFile("LOCAL.MIX", &fast_key);
    GameMixFile::Cache("LOCAL.MIX");
    GameMixFile* speech_mix = new GameMixFile("SPEECH.MIX", &fast_key);
    GameMixFile::Cache("SPEECH.MIX");
    GameMixFile* hires_mix = new GameMixFile("HIRES.MIX", &fast_key);
    GameMixFile::Cache("HIRES.MIX");

    GameMixFile main_mix("RedAlert\\MAIN.MIX", &fast_key);
    GameMixFile* sounds_mix = new GameMixFile("SOUNDS.MIX", &fast_key);
    GameMixFile::Cache("SOUNDS.MIX");
    GameMixFile* allies_mix = new GameMixFile("ALLIES.MIX", &fast_key);
    GameMixFile::Cache("ALLIES.MIX");
    GameMixFile* russian_mix = new GameMixFile("RUSSIAN.MIX", &fast_key);
    GameMixFile::Cache("RUSSIAN.MIX");
    GameMixFile* scores_mix = new GameMixFile("SCORES.MIX", &fast_key);
    GameMixFile::Cache("SCORES.MIX");
}
