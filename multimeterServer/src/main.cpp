#include <iostream>

#include "core/MultimeterServer.h"
#include "core/MultimeterDispatcher.h"

using namespace std;

int main()
{
    multimeter->initChannels();

    MultimeterServer srv{};
    srv.run();

    return 0;
}
