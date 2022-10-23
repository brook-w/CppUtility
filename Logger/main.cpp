#include <iostream>

using namespace std;

#include "Utility/Logger.h"

using namespace brook::utility;


int main() {
    std::cout << __FILE__ << endl;
    Logger::instance()->open("../test.log");
//    Logger::instance()->level(Logger::);
//    Logger::instance()->log(Logger::DEBUG, __FILE__, __LINE__, "hello world %d", 10);
    debug("name=%s age=%d", "jack", 18);
    info("name=%s age=%d", "jack", 18);
    warn("name=%s age=%d", "jack", 18);
    error("name=%s age=%d", "jack", 18);
    fatal("name=%s age=%d", "jack", 18);

    return 0;
}