#include <benchmark/Database.h>

using namespace benchmark;
Database::Database(std::string const& folder)
    : _folder(folder),
      _storageEngine(_folder),
      _primaryIndex(_folder),
      _secondaryIndex(_folder) {}

Database::~Database() {}

bool Database::insert(std::string const& key, Slice const& value) {
  return true;
}

bool Database::remove(std::string const& key) { return true; }

Database::Slice Database::lookupSingle(std::string const& key) {
  return arangodb::velocypack::Slice();
}

std::vector<Database::Slice> Database::lookupRange(Slice const& start,
                                                   Slice const& end) {
  return std::vector<Slice>();
}
