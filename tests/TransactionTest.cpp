#include <benchmark/Database.h>
#include <benchmark/HybridLogicalClock.h>
#include <benchmark/utility.h>
#include <velocypack/vpack.h>
#include <cassert>
#include <chrono>
#include <iostream>
#include <vector>

using namespace benchmark;

typedef arangodb::velocypack::Slice VPackSlice;
typedef arangodb::velocypack::SliceContainer VPackSliceContainer;

int main(int, char**) {
  std::string folder("/tmp/test_rocksdb-benchmark_transactions");
  Database db(folder);
  HybridLogicalClock clock;

  int segments = 10;

  for (int s = 0; s < segments; s++) {
    std::vector<std::string> keys;
    std::vector<VPackSliceContainer> documents;

    for (int i = 0; i < (1 << s); i++) {
      keys.emplace_back(
          std::to_string(s).append("_").append(std::to_string(i)));
      documents.emplace_back(utility::generateDocument(keys[i]));
    }
    std::cout << "Generated segment " << s << std::endl;

    auto r = db.insertBatch(keys, documents);
    assert(r.size == (1 << s));
    std::cout << "Inserted segment " << s << std::endl;

    keys.clear();
    documents.clear();
  }

  {
    RocksDBInstance instance(folder);
    auto readOptions = rocksdb::ReadOptions();
    auto writeOptions = rocksdb::WriteOptions();
    auto txOptions = rocksdb::TransactionOptions();
    auto db = instance.db();

    auto tx1 = db->BeginTransaction(writeOptions, txOptions);
    auto tx2 = db->BeginTransaction(writeOptions, txOptions);

    std::cout << "Checking write conflict behavior..." << std::endl;

    auto status = tx1->Put("a", "valuea1");
    assert(status.ok());
    status = tx1->Put("b", "valueb1");
    assert(status.ok());
    status = tx1->Put("c", "valuec1");
    assert(status.ok());
    std::cout << "Wrote a, b, c in tx1." << std::endl;

    std::cout << "Attempting to write d, b, e in tx2." << std::endl;
    status = tx2->Put("d", "valued2");
    assert(status.ok());
    status = tx2->Put("b", "valueb2");
    assert(!status.ok());
    std::cout << "Detected write conflict on b with status \""
              << status.ToString() << "\"" << std::endl;
    status = tx2->Put("e", "valuee2");
    assert(status.ok());
    std::cout << "Proceeded to write e." << std::endl;

    status = tx1->Commit();
    assert(status.ok());
    status = tx2->Commit();
    assert(status.ok());
    std::cout << "Commited tx1 and tx2." << std::endl;
    delete tx1;
    delete tx2;

    std::cout << "Checking that values written match expectations..."
              << std::endl;
    auto tx3 = db->BeginTransaction(writeOptions, txOptions);
    std::string buffer;
    status = tx3->Get(readOptions, "a", &buffer);
    assert(status.ok());
    assert(buffer == "valuea1");
    status = tx3->Get(readOptions, "b", &buffer);
    assert(status.ok());
    assert(buffer == "valueb1");
    status = tx3->Get(readOptions, "c", &buffer);
    assert(status.ok());
    assert(buffer == "valuec1");
    status = tx3->Get(readOptions, "d", &buffer);
    assert(status.ok());
    assert(buffer == "valued2");
    status = tx3->Get(readOptions, "e", &buffer);
    assert(status.ok());
    assert(buffer == "valuee2");
    std::cout << "Values match!" << std::endl;
  }

  std::cout << "All good!" << std::endl;

  return 0;
}
