#include <benchmark/PrimaryIndex.h>

using namespace benchmark;
PrimaryIndex::PrimaryIndex(std::string const& folder, std::string const& prefix)
    : _instance(folder), _db(_instance.db()), _prefix(prefix) {}

PrimaryIndex::~PrimaryIndex() {}

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
