#include "remapper/remapper.h"
#include "keyhook/keyhook.h"
#include "keyhook/worker.h"
#include "logger/logger.h"

#include <filesystem>

int main(int argc, char* argv[])
{
    logger::Init();

    if (argc < 2) 
    {
        logger::Error("No config file specified.");
        return 1;
    }

    std::string configName = argv[1];

    if (!std::filesystem::path(configName).has_parent_path()) 
    {
        configName = std::string("config/") + configName;
    }

    logger::Info("Loading config: " + configName);

    remapper::Remapper remapper(configName);

    if (!remapper.Load())
    {
        return 1;
    }


    keyhook::KeyHook hook(remapper);
    keyhook::Worker worker;

    worker.Start();
    hook.Run();
    worker.Stop();
    return 0;
}