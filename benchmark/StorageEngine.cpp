#include <benchmark/StorageEngine.h>

using namespace benchmark;
StorageEngine::StorageEngine(std::string const& folder,
                             std::string const& prefix)
    : _instance(folder), _db(_instance.db()), _prefix(prefix) {}

StorageEngine::~StorageEngine() {}

bool StorageEngine::insert(uint64_t revision, Slice const& value) {
  return true;
}

bool StorageEngine::remove(uint64_t revision) { return true; }

StorageEngine::Slice StorageEngine::lookup(uint64_t revision) {
  return arangodb::velocypack::Slice();
}
