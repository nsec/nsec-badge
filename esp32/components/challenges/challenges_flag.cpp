#include "challenges_flag.h"

#include "save.h"

void challenge_RE101_save_flag()
{
    if (!Save::save_data.flag8) {
        Save::save_data.flag8 = true;
        Save::write_save();
    }
}

void challenge_RE102_save_flag()
{
    if (!Save::save_data.flag9) {
        Save::save_data.flag9 = true;
        Save::write_save();
    }
}
