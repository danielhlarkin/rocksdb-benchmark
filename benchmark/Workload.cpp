#include <benchmark/Workload.h>

using namespace benchmark;

Workload::Workload(benchmark::Database* db, uint64_t threadCount,
                   uint64_t keyCount, uint64_t lookupCount,
                   uint64_t hotsetCount, uint64_t rangeLimit,
                   std::string const& folder)
    : _db(db),
      _random(0xdeadbeefdeadbeefULL),
      _opDigest(10000),
      _residentDigest(10000),
      _virtualDigest(10000),
      _diskDigest(10000),
      _threadCount(threadCount),
      _keyCount(keyCount),
      _lookupCount(lookupCount),
      _hotsetCount(hotsetCount),
      _folder(folder),
      _rangeLimit(rangeLimit),
      _workersDone(0),
      _workersErrored(0),
      _monitorInterval(std::chrono::milliseconds(10)) {}

Workload::~Workload() {}

bool Workload::run() {
  std::vector<void*> inputs;
  inputs.reserve(_threadCount);
  for (uint64_t i = 0; i < _threadCount; i++) {
    inputs.emplace_back(generateWorkerInput(i));
  }

  std::vector<qdigest::QDigest*> digests;
  digests.reserve(_threadCount);
  for (uint64_t i = 0; i < _threadCount; i++) {
    digests.emplace_back(new qdigest::QDigest(10000));
  }

  std::vector<std::thread> threads;
  threads.reserve(_threadCount);
  std::thread monitor =
      std::thread(UsageMonitor::monitor, _folder, _threadCount,
                  _monitorInterval, &_workersDone, &_workersErrored,
                  &_residentDigest, &_virtualDigest, &_diskDigest);
  for (uint64_t i = 0; i < _threadCount; i++) {
    threads.emplace_back(std::thread(worker(), inputs[i], digests[i]));
  }

  for (uint64_t i = 0; i < _threadCount; i++) {
    threads[i].join();
  }
  monitor.join();

  // cleanup after workers
  for (uint64_t i = 0; i < _threadCount; i++) {
    _opDigest += *digests[i];
    delete digests[i];
  }

  return (_workersDone.load() == _threadCount && _workersErrored.load() == 0);
}

void Workload::printResults() {
  printHeader();
  printUsageStatistics();
  printOpStatistics();
}

void Workload::printHeader() { std::cout << resultsHeader() << std::endl; }

void Workload::printUsageStatistics() {
  uint64_t resi50 = _residentDigest.percentile(0.50);
  uint64_t resi95 = _residentDigest.percentile(0.95);
  uint64_t resi99 = _residentDigest.percentile(0.99);
  uint64_t virt50 = _virtualDigest.percentile(0.50);
  uint64_t virt95 = _virtualDigest.percentile(0.95);
  uint64_t virt99 = _virtualDigest.percentile(0.99);
  uint64_t disk50 = _diskDigest.percentile(0.50);
  uint64_t disk95 = _diskDigest.percentile(0.95);
  uint64_t disk99 = _diskDigest.percentile(0.99);
  std::cout << "  USAGE STATISTICS" << std::endl
            << "    Resident Memory: {" << std::endl
            << "      50th: " << resi50 << "," << std::endl
            << "      95th: " << resi95 << "," << std::endl
            << "      99th: " << resi99 << std::endl
            << "    }" << std::endl
            << "    Virtual Memory: {" << std::endl
            << "      50th: " << virt50 << "," << std::endl
            << "      95th: " << virt95 << "," << std::endl
            << "      99th: " << virt99 << std::endl
            << "    }" << std::endl
            << "    Disk Space: {" << std::endl
            << "      50th: " << disk50 << "," << std::endl
            << "      95th: " << disk95 << "," << std::endl
            << "      99th: " << disk99 << std::endl
            << "    }" << std::endl;
}

void Workload::printOpStatistics() {
  uint64_t op500 = _opDigest.percentile(0.500);
  uint64_t op750 = _opDigest.percentile(0.750);
  uint64_t op950 = _opDigest.percentile(0.950);
  uint64_t op990 = _opDigest.percentile(0.990);
  uint64_t op999 = _opDigest.percentile(0.999);
  std::cout << "  OPERATION STATISTICS" << std::endl
            << "    " << operationName() << ": {" << std::endl
            << "      50.0th: " << op500 << "," << std::endl
            << "      75.0th: " << op750 << "," << std::endl
            << "      95.0th: " << op950 << "," << std::endl
            << "      99.0th: " << op990 << std::endl
            << "      99.9th: " << op999 << std::endl
            << "    }" << std::endl;
}
