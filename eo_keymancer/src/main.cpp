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

    keyHook::KeyHook hook(remapper);
    hook.run();

    return 0;
}