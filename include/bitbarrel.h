#include "segment.h"

#include <list>
#include <unordered_map>
#include <string>
#include <cstdint>

class BitBarrel {
private:
    std::list<Segment*> dataStore;
    Segment* active_segment;

    // Format: key : (segment_id, offset, size)
    std::unordered_map<std::string, std::tuple<uint64_t, uint64_t, size_t>> keydir;

public:
    BitBarrel();
    void set(std::string key, std::string value);
    std::string get(std::string key);

};