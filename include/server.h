#include "segment.h"

#include <list>

class Server {
private:
    std::list<Segment*> dataStore;

public:
    Server();
    void run();
};