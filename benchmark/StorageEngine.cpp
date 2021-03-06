#include <benchmark/StorageEngine.h>

using namespace benchmark;
StorageEngine::StorageEngine(uint32_t slug) : _prefix(buildPrefix(slug)) {}

StorageEngine::~StorageEngine() {}

std::string StorageEngine::buildPrefix(uint32_t slug) {
  return std::string("d").append(utility::shortToString(slug));
}

bool StorageEngine::insert(rocksdb::Transaction* tx, uint64_t revision,
                           VPackSlice const& value) {
  std::string key = buildKey(revision);
  std::string data = wrapValue(value);

  auto status = tx->Put(key, data);
  return status.ok();
}

bool StorageEngine::remove(rocksdb::Transaction* tx, uint64_t revision) {
  std::string key = buildKey(revision);

  auto status = tx->Delete(key);
  return status.ok();
}

StorageEngine::VPackSliceContainer StorageEngine::lookup(
    rocksdb::Transaction* tx, uint64_t revision) const {
  std::string key = buildKey(revision);
  std::string data;

  auto status = tx->Get(_readOptions, key, &data);
  return status.ok() ? VPackSliceContainer(VPackSlice(data.data()))
                     : VPackSliceContainer(VPackSlice());
}

std::string StorageEngine::buildKey(uint64_t revision) const {
  return std::string(_prefix).append(utility::intToString(revision));
}

std::string StorageEngine::wrapValue(VPackSlice const& value) const {
  return std::string(reinterpret_cast<const char*>(value.begin()),
                     value.byteSize());
}
