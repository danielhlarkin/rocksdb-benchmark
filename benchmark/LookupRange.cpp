#include <benchmark/LookupRange.h>
#include <chrono>
#include <thread>

using namespace benchmark;

LookupRange::LookupRange(WorkloadInitializationData const& data,
                         std::time_t minTimestamp, std::time_t maxTimestamp)
    : Workload(data, minTimestamp, maxTimestamp) {}

LookupRange::~LookupRange() {}

std::string LookupRange::operationName() { return std::string("lookupRange"); }

void* LookupRange::generateWorkerInput(uint64_t i) {
  uint64_t estimatedCount = (_keyCount / _threadCount);
  std::time_t chunkTime = ((_maxTimestamp - _minTimestamp) / _threadCount);
  std::time_t lower = _minTimestamp + (i * chunkTime);
  std::time_t upper = _minTimestamp + ((i + 1) * chunkTime) - 1;
  if (upper < lower) {
    upper = lower;
  }
  if (i == (_threadCount - 1)) {
    upper = _maxTimestamp;
  }

  return new LookupRangeInput(_db, estimatedCount, _rangeLimit, lower, upper,
                              &_opCount, &_workersDone, &_workersErrored);
}

LookupRange::WorkerType LookupRange::worker() { return lookupRanges; }

void LookupRange::lookupRanges(void* input, qdigest::QDigest* opDigest,
                               timepoint* start, timepoint* end) {
  LookupRangeInput* parameters = reinterpret_cast<LookupRangeInput*>(input);
  Database* db = parameters->db;
  uint64_t expectedCount = parameters->expectedCount;
  uint64_t rangeLimit = parameters->rangeLimit;
  std::time_t lower = parameters->lower;
  std::time_t upper = parameters->upper;
  std::atomic<uint64_t>* opCount = parameters->opCount;
  std::atomic<uint64_t>* workersDone = parameters->workersDone;
  std::atomic<uint64_t>* workersErrored = parameters->workersErrored;
  try {
    delete parameters;

    uint64_t chunks = static_cast<uint64_t>(std::ceil(expectedCount / 1000.0));
    std::time_t chunkTime = (upper - lower) / chunks;
    if (chunkTime == 0) {
      chunks = std::max<uint64_t>(upper - lower, 1);
      chunkTime = 1;
    }
    std::vector<Database::VPackSliceContainer> lowers;
    lowers.reserve(chunks);
    std::vector<Database::VPackSliceContainer> uppers;
    uppers.reserve(chunks);
    for (uint64_t i = 0; i < chunks; i++) {
      std::time_t thisLower = lower + (i * chunkTime);
      std::time_t thisUpper = lower + ((i + 1) * chunkTime) - 1;
      if (thisUpper < thisLower) {
        thisUpper = thisLower;
      }
      if (i == chunks - 1) {
        thisUpper = upper;
      }
      lowers.emplace_back(utility::generateSecondarySlice(thisLower));
      uppers.emplace_back(utility::generateSecondarySlice(thisUpper));
    }

    uint64_t total = 0;
    (*start) = std::chrono::high_resolution_clock::now();
    for (uint64_t i = 0; i < chunks; i++) {
      auto start = std::chrono::high_resolution_clock::now();
      auto documents =
          db->lookupRange(lowers[i].slice(), uppers[i].slice(), db->now());
      auto end = std::chrono::high_resolution_clock::now();

      total += documents.size();
      opDigest->insert((end - start).count(), 1);
      if (total > rangeLimit) {
        break;
      }
    }
    (*end) = std::chrono::high_resolution_clock::now();
    (*opCount) += chunks;

    (*workersDone)++;
    return;
  } catch (...) {
    std::cerr << "WORKER FAILED" << std::endl;
    (*workersErrored)++;
  }
}
