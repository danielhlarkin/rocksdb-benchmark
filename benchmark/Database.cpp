#include <benchmark/Database.h>

using namespace benchmark;
Database::Database(std::string const& folder)
    : _folder(folder),
      _random(0xdeadbeefdeadbeefULL),
      _databaseId(_random.next()),
      _collectionId(_random.next()),
      _primaryIndexId(_random.next()),
      _secondaryIndexId(_random.next()),
      _storageEngine(_folder,
                     StorageEngine::buildPrefix(_databaseId, _collectionId)),
      _primaryIndex(_folder, PrimaryIndex::buildPrefix(
                                 _databaseId, _collectionId, _primaryIndexId)),
      _secondaryIndex(_folder,
                      SecondaryIndex::buildPrefix(_databaseId, _collectionId,
                                                  _secondaryIndexId)) {}

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
