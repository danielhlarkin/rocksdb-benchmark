#include <benchmark/Suite.h>

using namespace benchmark;

Suite::Suite(uint64_t threadCount, uint64_t keyCount, uint64_t lookupCount,
             uint64_t hotsetCount, uint64_t rangeLimit,
             std::string const& folder)
    : _threadCount(threadCount),
      _keyCount(keyCount),
      _lookupCount(lookupCount),
      _hotsetCount(hotsetCount),
      _rangeLimit(rangeLimit),
      _folder(folder),
      _db(_folder) {}

Suite::~Suite() {}

void Suite::printConfig() {
  std::cout << "Benchmarking RocksDB with parameters {" << std::endl
            << "  threadCount: " << _threadCount << "," << std::endl
            << "  keyCount = " << _keyCount << "," << std::endl
            << "  lookupCount: " << _lookupCount << "," << std::endl
            << "  hotsetCount: " << _hotsetCount << "," << std::endl
            << "  rangeLimit: " << _rangeLimit << std::endl
            << "  folder: " << _folder << std::endl
            << "}" << std::endl;
}

bool Suite::runAll() { return true; }
