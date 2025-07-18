#include "remapper/remapper.h"
#include "keyhook/keyhook.h"
#include "logger/logger.h"

int main() 
{
    logger::init();

    remapper::Remapper remapper("config/remap.cfg");
    if (!remapper.load())
    {
        return 1;
    }

    remapper::KeyHook hook(remapper);
    hook.run();

    return 0;
}