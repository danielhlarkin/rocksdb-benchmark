#include <benchmark/StorageEngine.h>

using namespace benchmark;
StorageEngine::StorageEngine(std::string const& folder,
                             std::string const& prefix)
    : _instance(folder), _db(_instance.db()), _prefix(prefix) {}

StorageEngine::~StorageEngine() {}

std::string StorageEngine::buildPrefix(uint64_t databaseId,
                                       uint64_t collectionId) {
  return std::string("d")
      .append(utility::intToString(databaseId))
      .append(utility::intToString(collectionId));
}

bool StorageEngine::insert(uint64_t revision, VPackSlice const& value) {
  std::string key = buildKey(revision);
  std::string data = wrapValue(value);

  auto status = _db->Put(_writeOptions, key, data);
  return status.ok();
}

bool StorageEngine::remove(uint64_t revision) {
  std::string key = buildKey(revision);

  auto status = _db->Delete(_writeOptions, key);
  return status.ok();
}

StorageEngine::VPackSliceContainer StorageEngine::lookup(
    uint64_t revision) const {
  std::string key = buildKey(revision);
  std::string data;

  auto status = _db->Get(_readOptions, key, &data);
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
