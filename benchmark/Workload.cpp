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
      _transactionSize(data.transactionSize),
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

void Workload::printResults(VPackBuilder& builder) {
  builder.add(VPackValue(VPackValueType::Object));
  builder.add("name", VPackValue(operationName()));
  printUsageStatistics(builder);
  printOpStatistics(builder);
  builder.close();
}

std::time_t Workload::minTimestamp() { return _minTimestamp; }

std::time_t Workload::maxTimestamp() { return _maxTimestamp; }

void Workload::printUsageStatistics(VPackBuilder& builder) {
  std::vector<std::string> names = {"resident", "virtual", "disk"};
  std::vector<qdigest::QDigest*> digests = {&_residentDigest, &_virtualDigest,
                                            &_diskDigest};
  std::vector<double> percentiles = {0.50, 0.60, 0.70,  0.80,    0.90,
                                     0.95, 0.99, 0.999, 0.999999};

  builder.add("usage", VPackValue(VPackValueType::Object));
  for (size_t d = 0; d < digests.size(); d++) {
    builder.add(names[d], VPackValue(VPackValueType::Object));
    for (size_t i = 0; i < percentiles.size(); i++) {
      auto space = readableSpace(digests[d]->percentile(percentiles[i]));
      builder.add(std::to_string(percentiles[i]),
                  VPackValue(VPackValueType::Array));
      builder.add(VPackValue(space.first));
      builder.add(VPackValue(space.second));
      builder.close();
    }
    builder.close();
  }
  builder.close();
}

void Workload::printOpStatistics(VPackBuilder& builder) {
  std::vector<double> percentiles = {0.50, 0.60, 0.70,  0.80,    0.90,
                                     0.95, 0.99, 0.999, 0.999999};

  builder.add("performance", VPackValue(VPackValueType::Object));
  builder.add("count", VPackValue(_opCount.load()));
  auto time = readableTime(_duration);
  builder.add("time", VPackValue(VPackValueType::Array));
  builder.add(VPackValue(time.first));
  builder.add(VPackValue(time.second));
  builder.close();
  builder.add("throughput", VPackValue(throughput()));
  builder.add("latencies", VPackValue(VPackValueType::Object));
  for (size_t i = 0; i < percentiles.size(); i++) {
    auto time = readableTime(_opDigest.percentile(percentiles[i]));
    builder.add(std::to_string(percentiles[i]),
                VPackValue(VPackValueType::Array));
    builder.add(VPackValue(time.first));
    builder.add(VPackValue(time.second));
    builder.close();
  }
  builder.close();
  builder.close();
}

std::pair<double, std::string> Workload::readableTime(uint64_t time) {
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

  return std::pair<double, std::string>(result, suffix);
}

std::pair<double, std::string> Workload::readableSpace(uint64_t space) {
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

  return std::pair<double, std::string>(result, suffix);
}

double Workload::throughput() {
  if (_duration == 0 || _opCount.load() == 0) {
    return -1;
  }
  double throughput = (_opCount.load() / (_duration / 1000000000.0));

  return throughput;
}
