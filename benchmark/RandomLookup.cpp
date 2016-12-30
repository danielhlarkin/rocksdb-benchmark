#include <benchmark/RandomLookup.h>
#include <chrono>
#include <thread>

using namespace benchmark;

RandomLookup::RandomLookup(WorkloadInitializationData const& data)
    : Workload(data) {}

RandomLookup::~RandomLookup() {}

std::string RandomLookup::resultsHeader() {
  return std::string("WORKLOAD: RANDOM LOOKUP")
      .append("\n")
      .append("  Lookup of ")
      .append(std::to_string(_lookupCount))
      .append(" random documents in parallel");
}

std::string RandomLookup::operationName() { return std::string("lookup"); }

void* RandomLookup::generateWorkerInput(uint64_t i) {
  uint64_t chunkSize = (_lookupCount / _threadCount);
  if (i == (_threadCount - 1)) {
    chunkSize = _lookupCount - (chunkSize * (_threadCount - 1));
  }

  return new RandomLookupInput(_db, _keyCount, chunkSize, &_opCount,
                               &_workersDone, &_workersErrored);
}

RandomLookup::WorkerType RandomLookup::worker() { return lookupRandom; }

void RandomLookup::lookupRandom(void* input, qdigest::QDigest* opDigest,
                                timepoint* start, timepoint* end) {
  RandomLookupInput* parameters = reinterpret_cast<RandomLookupInput*>(input);
  Database* db = parameters->db;
  uint64_t keyCount = parameters->keyCount;
  uint64_t lookupCount = parameters->lookupCount;
  std::atomic<uint64_t>* opCount = parameters->opCount;
  std::atomic<uint64_t>* workersDone = parameters->workersDone;
  std::atomic<uint64_t>* workersErrored = parameters->workersErrored;
  try {
    delete parameters;

    RandomNumber r(0xdeadbeefdeadbeefULL);
    (*start) = std::chrono::high_resolution_clock::now();
    for (uint64_t i = 0; i < lookupCount; i++) {
      uint64_t current = r.nextInRange(keyCount) + 1;

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
