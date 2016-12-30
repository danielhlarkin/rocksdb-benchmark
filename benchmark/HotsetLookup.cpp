#include <benchmark/HotsetLookup.h>
#include <chrono>
#include <thread>

using namespace benchmark;

HotsetLookup::HotsetLookup(WorkloadInitializationData const& data)
    : Workload(data) {}

HotsetLookup::~HotsetLookup() {}

std::string HotsetLookup::resultsHeader() {
  return std::string("WORKLOAD: HOTSET LOOKUP")
      .append("\n")
      .append("  Lookup of ")
      .append(std::to_string(_lookupCount))
      .append(" random documents from a hotset of size ")
      .append(std::to_string(_hotsetCount))
      .append(" in parallel");
}

std::string HotsetLookup::operationName() {
  return std::string("lookupSingle");
}

void* HotsetLookup::generateWorkerInput(uint64_t i) {
  uint64_t chunkSize = (_lookupCount / _threadCount);
  if (i == (_threadCount - 1)) {
    chunkSize = _lookupCount - (chunkSize * (_threadCount - 1));
  }

  return new HotsetLookupInput(_db, _hotsetCount, chunkSize, &_opCount,
                               &_workersDone, &_workersErrored);
}

HotsetLookup::WorkerType HotsetLookup::worker() { return lookupHotset; }

void HotsetLookup::lookupHotset(void* input, qdigest::QDigest* opDigest,
                                timepoint* start, timepoint* end) {
  HotsetLookupInput* parameters = reinterpret_cast<HotsetLookupInput*>(input);
  Database* db = parameters->db;
  uint64_t hotsetCount = parameters->hotsetCount;
  uint64_t lookupCount = parameters->lookupCount;
  std::atomic<uint64_t>* opCount = parameters->opCount;
  std::atomic<uint64_t>* workersDone = parameters->workersDone;
  std::atomic<uint64_t>* workersErrored = parameters->workersErrored;
  try {
    delete parameters;

    RandomNumber r(0xdeadbeefdeadbeefULL);
    (*start) = std::chrono::high_resolution_clock::now();
    for (uint64_t i = 0; i < lookupCount; i++) {
      uint64_t current = r.nextInRange(hotsetCount) + 1;

      auto start = std::chrono::high_resolution_clock::now();
      auto document = db->lookupSingle(std::to_string(current), db->now());
      auto end = std::chrono::high_resolution_clock::now();
      assert(document.slice().get("_key").copyString() ==
             std::to_string(current));

      opDigest->insert((end - start).count(), 1);
    }
    (*end) = std::chrono::high_resolution_clock::now();
    (*opCount) += lookupCount;

    (*workersDone)++;
    return;
  } catch (...) {
    std::cout << "WORKER FAILED" << std::endl;
    (*workersErrored)++;
  }
}
