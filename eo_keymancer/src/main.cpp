#include "remapper/remapper.h"
#include "keyhook/keyhook.h"
#include "keyhook/worker.h"
#include "logger/logger.h"

int main() 
{
    logger::Init();

    remapper::Remapper remapper("config/remap.cfg");
    if (!remapper.Load())
    {
        return 1;
    }


    key_hook::KeyHook hook(remapper);
    key_hook::Worker worker;

    worker.Start();
    hook.Run();
    worker.Stop();
    return 0;
}