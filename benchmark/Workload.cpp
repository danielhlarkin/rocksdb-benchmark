#include <benchmark/Workload.h>

using namespace benchmark;

Workload::Workload(WorkloadInitializationData const& data,
                   std::time_t minTimestamp, std::time_t maxTimestamp)
    : _db(data.db),
      _random(0xdeadbeefdeadbeefULL),
      _opDigest(10000),
      _residentDigest(10000),
      _virtualDigest(10000),
      _diskDigest(10000),
      _opCount(0),
      _threadCount(data.threadCount),
      _keyCount(data.keyCount),
      _lookupCount(data.lookupCount),
      _hotsetCount(data.hotsetCount),
      _rangeLimit(data.rangeLimit),
      _folder(data.folder),
      _workersDone(0),
      _workersErrored(0),
      _monitorInterval(std::chrono::seconds(1)),
      _minTimestamp(minTimestamp),
      _maxTimestamp(maxTimestamp) {}

Workload::~Workload() {}

bool Workload::run() {
  // allocate and reserve all our vectors for each thread
  std::vector<void*> inputs;
  inputs.reserve(_threadCount);

  std::vector<qdigest::QDigest*> digests;
  digests.reserve(_threadCount);

  std::vector<timepoint> starts;
  starts.reserve(_threadCount);

  std::vector<timepoint> ends;
  ends.reserve(_threadCount);

  std::vector<std::thread> threads;
  threads.reserve(_threadCount);

  // actually generate the inputs
  for (uint64_t i = 0; i < _threadCount; i++) {
    inputs.emplace_back(generateWorkerInput(i));
    digests.emplace_back(new qdigest::QDigest(10000));
  }

  // start our monitoring thread
  std::thread monitor =
      std::thread(UsageMonitor::monitor, _folder, _threadCount,
                  _monitorInterval, &_workersDone, &_workersErrored,
                  &_residentDigest, &_virtualDigest, &_diskDigest);
  // start our worker threads
  for (uint64_t i = 0; i < _threadCount; i++) {
    threads.emplace_back(std::thread(worker(), inputs[i], digests[i],
                                     starts.data() + i, ends.data() + i));
  }

  // join threads
  for (uint64_t i = 0; i < _threadCount; i++) {
    threads[i].join();
  }
  monitor.join();

  // collect stats and cleanup after workers
  timepoint start = *std::min_element(std::begin(starts), std::end(starts));
  timepoint end = *std::max_element(std::begin(ends), std::end(ends));
  _duration = (end - start).count();
  for (uint64_t i = 0; i < _threadCount; i++) {
    _opDigest += *digests[i];
    delete digests[i];
  }

  return (_workersDone.load() == _threadCount && _workersErrored.load() == 0);
}

void Workload::printHeader() { std::cout << resultsHeader() << std::endl; }

void Workload::printResults() {
  printUsageStatistics();
  printOpStatistics();
}

std::time_t Workload::minTimestamp() { return _minTimestamp; }

std::time_t Workload::maxTimestamp() { return _maxTimestamp; }

void Workload::printUsageStatistics() {
  std::string resi50 = readableSpace(_residentDigest.percentile(0.50));
  std::string resi95 = readableSpace(_residentDigest.percentile(0.95));
  std::string resi99 = readableSpace(_residentDigest.percentile(0.99));
  std::string virt50 = readableSpace(_virtualDigest.percentile(0.50));
  std::string virt95 = readableSpace(_virtualDigest.percentile(0.95));
  std::string virt99 = readableSpace(_virtualDigest.percentile(0.99));
  std::string disk50 = readableSpace(_diskDigest.percentile(0.50));
  std::string disk95 = readableSpace(_diskDigest.percentile(0.95));
  std::string disk99 = readableSpace(_diskDigest.percentile(0.99));
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
  std::string op500 = readableTime(_opDigest.percentile(0.500));
  std::string op750 = readableTime(_opDigest.percentile(0.750));
  std::string op950 = readableTime(_opDigest.percentile(0.950));
  std::string op990 = readableTime(_opDigest.percentile(0.990));
  std::string op999 = readableTime(_opDigest.percentile(0.999));
  std::cout << "  OPERATION STATISTICS" << std::endl
            << "    Total Operations: " << _opCount.load() << std::endl
            << "    Total Time: " << readableTime(_duration) << std::endl
            << "    Throughput: " << throughput() << std::endl
            << "    Latencies: {" << std::endl
            << "      " << operationName() << ": {" << std::endl
            << "        50.0th: " << op500 << "," << std::endl
            << "        75.0th: " << op750 << "," << std::endl
            << "        95.0th: " << op950 << "," << std::endl
            << "        99.0th: " << op990 << "," << std::endl
            << "        99.9th: " << op999 << std::endl
            << "      }" << std::endl
            << "    }" << std::endl;
}

std::string Workload::readableTime(uint64_t time) {
  double result;
  std::string suffix;
  if (time < 1000) {
    result = (time / 1.0);
    suffix = "ns";
  } else if (time < 1000000) {
    result = (time / 1000.0);
    suffix = "\u03BCs";
  } else if (time < 1000000000) {
    result = (time / 1000000.0);
    suffix = "ms";
  } else if (time < 60000000000) {
    result = (time / 1000000000.0);
    suffix = "s";
  } else if (time < 3600000000000) {
    result = (time / 60000000000.0);
    suffix = "m";
  } else {
    result = (time / 3600000000000.0);
    suffix = "h";
  }

  std::stringstream ss;
  ss << std::fixed << std::setprecision(1) << result;
  return ss.str().append(suffix);
}

std::string Workload::readableSpace(uint64_t space) {
  double result;
  std::string suffix;
  if (space < 1024) {
    result = (space / 1.0);
    suffix = "KB";
  } else if (space < 1048576) {
    result = (space / 1024.0);
    suffix = "MB";
  } else if (space < 1073741824) {
    result = (space / 1048576.0);
    suffix = "GB";
  } else {
    result = (space / 1073741824.0);
    suffix = "TB";
  }

  std::stringstream ss;
  ss << std::fixed << std::setprecision(1) << result;
  return ss.str().append(suffix);
}

std::string Workload::throughput() {
  if (_duration == 0 || _opCount.load() == 0) {
    return std::string("N/A");
  }
  double throughput = (_opCount.load() / (_duration / 1000000000.0));
  std::stringstream ss;
  ss << std::fixed << std::setprecision(1) << throughput;
  return ss.str().append("ops/s");
}
