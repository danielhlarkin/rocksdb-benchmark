#include <benchmark/Suite.h>

using namespace benchmark;

Suite::Suite(uint64_t threadCount, uint64_t keyCount, uint64_t lookupCount,
             uint64_t hotsetCount, uint64_t rangeLimit,
             uint64_t transactionSize, std::string const& folder)
    : _threadCount(threadCount),
      _keyCount(keyCount),
      _lookupCount(lookupCount),
      _hotsetCount(hotsetCount),
      _rangeLimit(rangeLimit),
      _transactionSize(transactionSize),
      _folder(folder),
      _db(_folder) {}

Suite::~Suite() {}

void Suite::printConfig(VPackBuilder& builder) {
  builder.add("config", VPackValue(VPackValueType::Object));
  builder.add("threadCount", VPackValue(_threadCount));
  builder.add("keyCount", VPackValue(_keyCount));
  builder.add("lookupCount", VPackValue(_lookupCount));
  builder.add("hotsetCount", VPackValue(_hotsetCount));
  builder.add("rangeLimit", VPackValue(_rangeLimit));
  builder.add("transactionSize", VPackValue(_transactionSize));
  builder.add("folder", VPackValue(_folder));
  builder.close();
}

bool Suite::runAll(VPackBuilder& builder) {
  WorkloadInitializationData initData(&_db, _threadCount, _keyCount,
                                      _lookupCount, _hotsetCount, _rangeLimit,
                                      _transactionSize, _folder);
  Insert insert(initData);
  bool ok = insert.run();
  if (!ok) {
    std::cerr << "Insertion workload failed." << std::endl;
    return false;
  }
  insert.printResults(builder);

  if (_lookupCount > 0) {
    LookupSingleRandom random(initData);
    ok = random.run();
    if (!ok) {
      std::cerr << "Random lookup workload failed." << std::endl;
      return false;
    }
    random.printResults(builder);
  }

  if (_lookupCount > 0 && _hotsetCount > 0) {
    LookupSingleHotset hotset(initData);
    ok = hotset.run();
    if (!ok) {
      std::cerr << "Hotset lookup workload failed." << std::endl;
      return false;
    }
    hotset.printResults(builder);
  }

  if (_rangeLimit > 0) {
    LookupRange range(initData, insert.minTimestamp(), insert.maxTimestamp());
    ok = range.run();
    if (!ok) {
      std::cerr << "Range lookup workload failed." << std::endl;
      return false;
    }
    range.printResults(builder);
  }

  if (_transactionSize > 0) {
    InsertBatch batch(initData);
    ok = batch.run();
    if (!ok) {
      std::cerr << "Batch insertion workload failed." << std::endl;
      return false;
    }
    batch.printResults(builder);
  }

  return true;
}
