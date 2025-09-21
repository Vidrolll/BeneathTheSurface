#include "util/GameStartupUpdateCheck.h"

int main() {
    std::wstring installDir = L"C:\\Users\\Caden Vize\\Documents\\GitHub\\BeneathTheSurface\\game\\build\\install\\BeneathTheSurface";
    if (SoftUpdateCheckAndMaybeRun(installDir)) {
        std::exit(0);
    }

    return 0;
}