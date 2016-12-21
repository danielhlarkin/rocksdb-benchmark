#include <benchmark/PrimaryIndex.h>

using namespace benchmark;
PrimaryIndex::PrimaryIndex(std::string const& folder, std::string const& prefix)
    : _instance(folder), _db(_instance.db()), _prefix(prefix) {}

PrimaryIndex::~PrimaryIndex() {}

std::string PrimaryIndex::buildPrefix(uint64_t databaseId,
                                      uint64_t collectionId, uint64_t indexId) {
  return std::string("d")
      .append(std::to_string(databaseId))
      .append(std::to_string(collectionId))
      .append(std::to_string(indexId));
}

bool PrimaryIndex::insert(std::string const& key, uint64_t revision,
                          arangodb::velocypack::Slice const& value) {
  return true;
}

bool PrimaryIndex::remove(std::string const& key, uint64_t revision) {
  return true;
}

arangodb::velocypack::Slice PrimaryIndex::lookup(std::string const& key,
                                                 uint64_t maxRevision) {
  return arangodb::velocypack::Slice();
}
