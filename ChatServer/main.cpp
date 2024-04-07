#include "Server.h"

int main()
{
    Singleton<Server>::instance()->run();
    return 0;
}