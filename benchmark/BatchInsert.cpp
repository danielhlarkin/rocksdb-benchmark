#include <benchmark/BatchInsert.h>
#include <chrono>
#include <thread>

using namespace benchmark;

BatchInsert::BatchInsert(benchmark::Database* db, uint64_t threadCount,
                         uint64_t keyCount, uint64_t lookupCount,
                         uint64_t hotsetCount, uint64_t rangeLimit,
                         std::string const& folder)
    : Workload(db, threadCount, keyCount, lookupCount, hotsetCount, rangeLimit,
               folder) {}

BatchInsert::~BatchInsert() {}

std::string BatchInsert::resultsHeader() {
  return std::string("WORKLOAD: BULK INSERTION")
      .append("\n")
      .append("  Insertion of ")
      .append(std::to_string(_keyCount))
      .append(" documents in parallel random fashion");
}

std::string BatchInsert::operationName() { return std::string("insert"); }

void* BatchInsert::generateWorkerInput(uint64_t i) {
  uint64_t chunkSize = (_keyCount / _threadCount);
  uint64_t lower = (i * chunkSize) + 1;
  uint64_t upper = (i + 1) * chunkSize;
  if (upper > _keyCount) {
    upper = _keyCount;
  }

  return new BatchInsertInput(lower, upper, &_workersDone, &_workersErrored);
}

BatchInsert::WorkerType BatchInsert::worker() { return insertBatch; }

void BatchInsert::insertBatch(void* input, qdigest::QDigest* opDigest) {
  BatchInsertInput* parameters = reinterpret_cast<BatchInsertInput*>(input);
  uint64_t lower = parameters->lower;
  uint64_t upper = parameters->upper;
  std::atomic<uint64_t>* workersDone = parameters->workersDone;
  std::atomic<uint64_t>* workersErrored = parameters->workersErrored;
  try {
    delete parameters;

    uint64_t total = upper - lower;
    for (uint64_t i = 1; i <= total; i++) {
      opDigest->insert(lower + i, 1);
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    (*workersDone)++;
    return;
  } catch (...) {
    (*workersErrored)++;
  }
}
