#include "remapper.h"
#include "keyhook.h"

int main() {
    remapper::Remapper remapper("../config/remap.cfg");
    if (!remapper.load())
    {
        return 1;
    }

    remapper::KeyHook hook(remapper);
    hook.run();

    return 0;
}