#include <libdragon.h>

#include "isHighRes.h"

bool is_high_res;

bool attempt_high_res()
{
    is_high_res = is_memory_expanded();
    return is_high_res;
}