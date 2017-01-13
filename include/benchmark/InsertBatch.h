#ifndef ROCKSDB_BENCHMARK_INSERT_BATCH_H
#define ROCKSDB_BENCHMARK_INSERT_BATCH_H 1

#include <benchmark/RandomNumber.h>
#include <benchmark/Workload.h>

namespace benchmark {

struct InsertBatchInput {
  benchmark::Database* db;
  benchmark::Mutex* lock;
  uint64_t lower;
  uint64_t upper;
  uint64_t batchSize;
  std::time_t* minTimestamp;
  std::time_t* maxTimestamp;
  std::atomic<uint64_t>* opCount;
  std::atomic<uint64_t>* workersDone;
  std::atomic<uint64_t>* workersErrored;
  InsertBatchInput(benchmark::Database* d, benchmark::Mutex* lk, uint64_t l,
                   uint64_t u, uint64_t bs, std::time_t* minT,
                   std::time_t* maxT, std::atomic<uint64_t>* ops,
                   std::atomic<uint64_t>* done, std::atomic<uint64_t>* errored)
      : db(d),
        lock(lk),
        lower(l),
        upper(u),
        batchSize(bs),
        minTimestamp(minT),
        maxTimestamp(maxT),
        opCount(ops),
        workersDone(done),
        workersErrored(errored) {}
};

class InsertBatch : public Workload {
 public:
  InsertBatch(WorkloadInitializationData const& data);
  ~InsertBatch();

 protected:
  std::string operationName();

  void* generateWorkerInput(uint64_t i);
  WorkerType worker();

  static void insertBatch(void* input, qdigest::QDigest* opDigest,
                          timepoint* start, timepoint* end);
};
}

#endif
