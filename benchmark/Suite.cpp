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
  std::cout << "BENCHMARKING ROCKSDB WITH PARAMETERS {" << std::endl
            << "  threadCount: " << _threadCount << "," << std::endl
            << "  keyCount: " << _keyCount << "," << std::endl
            << "  lookupCount: " << _lookupCount << "," << std::endl
            << "  hotsetCount: " << _hotsetCount << "," << std::endl
            << "  rangeLimit: " << _rangeLimit << std::endl
            << "  folder: " << _folder << std::endl
            << "}" << std::endl;
}

bool Suite::runAll() {
  WorkloadInitializationData initData(&_db, _threadCount, _keyCount,
                                      _lookupCount, _hotsetCount, _rangeLimit,
                                      _folder);
  BatchInsert batch(initData);
  batch.printHeader();
  bool ok = batch.run();
  if (!ok) {
    std::cerr << "Batch insertion workload failed." << std::endl;
    return false;
  }
  batch.printResults();

  if (_lookupCount > 0) {
    RandomLookup random(initData);
    random.printHeader();
    ok = random.run();
    if (!ok) {
      std::cerr << "Random lookup workload failed." << std::endl;
      return false;
    }
    random.printResults();
  }

  if (_lookupCount > 0 && _hotsetCount > 0) {
    HotsetLookup hotset(initData);
    hotset.printHeader();
    ok = hotset.run();
    if (!ok) {
      std::cerr << "Hotset lookup workload failed." << std::endl;
      return false;
    }
    hotset.printResults();
  }

  if (_rangeLimit > 0) {
    RangeLookup range(initData, batch.minTimestamp(), batch.maxTimestamp());
    range.printHeader();
    ok = range.run();
    if (!ok) {
      std::cerr << "Range lookup workload failed." << std::endl;
      return false;
    }
    range.printResults();
  }

  return true;
}
