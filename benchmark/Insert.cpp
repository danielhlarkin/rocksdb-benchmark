#include <benchmark/Insert.h>
#include <chrono>
#include <thread>

using namespace benchmark;

static uint64_t gcd(uint64_t u, uint64_t v);
static uint64_t relativePrime(uint64_t n);

Insert::Insert(WorkloadInitializationData const& data) : Workload(data) {}

Insert::~Insert() {}

std::string Insert::operationName() { return std::string("insert"); }

void* Insert::generateWorkerInput(uint64_t i) {
  uint64_t chunkSize = (_keyCount / _threadCount);
  uint64_t lower = (i * chunkSize) + 1;
  uint64_t upper = (i + 1) * chunkSize;
  if (i == (_threadCount - 1)) {
    upper = _keyCount;
  }

  return new InsertInput(_db, &_lock, lower, upper, &_minTimestamp,
                         &_maxTimestamp, &_opCount, &_workersDone,
                         &_workersErrored);
}

Insert::WorkerType Insert::worker() { return insertBatch; }

void Insert::insertBatch(void* input, qdigest::QDigest* opDigest,
                         timepoint* start, timepoint* end) {
  InsertInput* parameters = reinterpret_cast<InsertInput*>(input);
  Database* db = parameters->db;
  Mutex* lock = parameters->lock;
  uint64_t lower = parameters->lower;
  uint64_t upper = parameters->upper;
  std::time_t* minTimestamp = parameters->minTimestamp;
  std::time_t* maxTimestamp = parameters->maxTimestamp;
  std::atomic<uint64_t>* opCount = parameters->opCount;
  std::atomic<uint64_t>* workersDone = parameters->workersDone;
  std::atomic<uint64_t>* workersErrored = parameters->workersErrored;
  try {
    delete parameters;

    uint64_t total = upper - lower + 1;
    uint64_t step = relativePrime(total);
    uint64_t current = 0;
    std::time_t currentTimestamp =
        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    {
      MUTEX_LOCKER(locker, *lock);
      if (currentTimestamp < (*minTimestamp)) {
        (*minTimestamp) = currentTimestamp;
      }
    }
    (*start) = std::chrono::high_resolution_clock::now();
    for (uint64_t i = 1; i <= total; i++) {
      current = (current + step) % total;

      std::string key(std::to_string(lower + current));
      VPackSliceContainer document = utility::generateDocument(key);
      auto start = std::chrono::high_resolution_clock::now();
      uint64_t r = db->insert(key, document.slice());
      auto end = std::chrono::high_resolution_clock::now();
      if (r == 0) {
        std::cerr << "FAILED TO INSERT " << key << std::endl;
      }
      assert(r > 0);

      opDigest->insert((end - start).count(), 1);
    }
    (*end) = std::chrono::high_resolution_clock::now();
    (*opCount) += total;
    currentTimestamp =
        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    {
      MUTEX_LOCKER(locker, *lock);
      if (currentTimestamp > (*maxTimestamp)) {
        (*maxTimestamp) = currentTimestamp;
      }
    }

    (*workersDone)++;
    return;
  } catch (...) {
    (*workersErrored)++;
  }
}

static uint64_t gcd(uint64_t u, uint64_t v) {
  if (u == 0) {
    return v;
  }
  if (v == 0) {
    return u;
  }

  int shift;
  for (shift = 0; ((u | v) & 1) == 0; ++shift) {
    u >>= 1;
    v >>= 1;
  }

  while ((u & 1) == 0) {
    u >>= 1;
  }

  do {
    while ((v & 1) == 0) {
      v >>= 1;
    }

    if (u > v) {
      std::swap(v, u);
    }
    v = v - u;
  } while (v != 0);

  return u << shift;
}

static uint64_t relativePrime(uint64_t n) {
  RandomNumber r(0xdeadbeefdeadbeefULL);
  while (true) {
    uint64_t candidate = r.nextInRange(n);
    if (candidate < (n / 2)) {
      continue;
    }
    if (gcd(n, candidate) != 1) {
      continue;
    }

    return candidate;
  }
}
