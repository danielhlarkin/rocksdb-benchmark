#ifndef ROCKSDB_BENCHMARK_BATCH_INSERT_H
#define ROCKSDB_BENCHMARK_BATCH_INSERT_H 1

#include <benchmark/Workload.h>

namespace benchmark {

struct BatchInsertInput {
  uint64_t lower;
  uint64_t upper;
  std::atomic<uint64_t>* workersDone;
  std::atomic<uint64_t>* workersErrored;
  BatchInsertInput(uint64_t l, uint64_t u, std::atomic<uint64_t>* d,
                   std::atomic<uint64_t>* e)
      : lower(l), upper(u), workersDone(d), workersErrored(e) {}
};

class BatchInsert : public Workload {
 public:
  BatchInsert(benchmark::Database* db, uint64_t threadCount, uint64_t keyCount,
              uint64_t lookupCount, uint64_t hotsetCount, uint64_t rangeLimit,
              std::string const& folder);
  ~BatchInsert();

 protected:
  std::string resultsHeader();
  std::string operationName();

  void* generateWorkerInput(uint64_t i);
  WorkerType worker();

  static void insertBatch(void* input, qdigest::QDigest* opDigest);
};
}

#endif
