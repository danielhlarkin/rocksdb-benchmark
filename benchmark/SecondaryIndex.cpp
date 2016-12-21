#include <benchmark/SecondaryIndex.h>

using namespace benchmark;
SecondaryIndex::SecondaryIndex(std::string const& folder,
                               std::string const& prefix)
    : _instance(folder), _db(_instance.db()), _prefix(prefix) {}

SecondaryIndex::~SecondaryIndex() {}

bool SecondaryIndex::insert(std::string const& key, uint64_t revision,
                            Slice const& value) {
  return true;
}

bool SecondaryIndex::remove(std::string const& key, uint64_t revision,
                            Slice const& value) {
  return true;
}

std::vector<SecondaryIndex::Slice> SecondaryIndex::lookup(
    Slice const& start, Slice const& end, uint64_t maxRevision) {
  return std::vector<Slice>();
}
