#include "rpg_duck.h"

#include "save.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <iostream>

namespace rpg
{

static constexpr char alt_buffer_clear[] =
    "\x1b\x5b\x48\x1b\x5b\x32\x4a\x1b\x5b\x33\x4a";
static constexpr char alt_buffer_start[] = "\x1b\x5b\x3f\x31\x30\x34\x39\x68";
static constexpr char alt_buffer_end[] = "\x1b\x5b\x3f\x31\x30\x34\x39\x6c";

static constexpr char quack[] =
    "quackquackquickquack quickquickquack quickquack quackquickquackquick "
    "quackquickquack quickquick quackquack quickquack quackquickquick "
    "quickquickquack quackquickquackquick quackquickquack quack "
    "quackquackquack quackquackquick quick quack quackquickquickquick "
    "quickquackquick quick quickquack quackquickquick quickquickquack "
    "quickquickquick quick quack quickquickquickquick quickquick "
    "quickquickquick quickquickquackquick quickquackquickquick quickquack "
    "quackquackquick quackquickquickquick quackquick quickquickquickquack "
    "quackquackquickquick quackquickquack quickquack quickquackquack "
    "quackquack quickquickquick quackquickquackquack quickquickquick";

void rpg_duck_challenge()
{
    if (!Save::save_data.flag4) {
        Save::save_data.flag4 = true;
        Save::write_save();
    }

    std::cout << alt_buffer_start;

    for (int i = 0; quack[i] != '\0'; ++i) {
        if (quack[i] == ' ')
            std::cout << alt_buffer_clear;
        else
            std::cout << quack[i];
    }

    std::cout << alt_buffer_clear << '\n' << alt_buffer_end << std::endl;
}

} // namespace rpg
