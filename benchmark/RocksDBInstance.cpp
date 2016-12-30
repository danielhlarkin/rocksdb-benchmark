#include <benchmark/RocksDBInstance.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace benchmark;

static int remove_directory(const char* path);

Mutex RocksDBInstance::_rocksDbMutex;
ArangoComparator* RocksDBInstance::_comparator;
rocksdb::DB* RocksDBInstance::_db = nullptr;
std::atomic<uint64_t> RocksDBInstance::_instanceCount(0);

rocksdb::Options RocksDBInstance::generateOptions() {
  rocksdb::Options options;

  rocksdb::BlockBasedTableOptions tableOptions;
  tableOptions.cache_index_and_filter_blocks = true;
  tableOptions.filter_policy.reset(rocksdb::NewBloomFilterPolicy(12, false));

  options.comparator = _comparator;
  options.prefix_extractor.reset(new ArangoPrefixTransform());

  options.create_if_missing = true;
  options.max_open_files = -1;
  options.comparator = _comparator;

  options.write_buffer_size = static_cast<size_t>(0);
  options.max_write_buffer_number = static_cast<int>(0);
  options.delayed_write_rate = 2 * 1024 * 1024;
  options.min_write_buffer_number_to_merge = 1;
  options.num_levels = static_cast<int>(4);
  options.max_bytes_for_level_base = 256 * 1024 * 1024;
  options.max_bytes_for_level_multiplier = static_cast<int>(10);
  options.verify_checksums_in_compaction = true;
  options.optimize_filters_for_hits = true;

  options.base_background_compactions = static_cast<int>(1);
  options.max_background_compactions = static_cast<int>(1);

  options.max_log_file_size = static_cast<size_t>(0);
  options.keep_log_file_num = static_cast<size_t>(1000);
  options.log_file_time_to_roll = static_cast<size_t>(0);
  options.compaction_readahead_size = static_cast<size_t>(0);

  if (options.base_background_compactions > 1 ||
      options.max_background_compactions > 1) {
    options.env->SetBackgroundThreads(
        (std::max)(options.base_background_compactions,
                   options.max_background_compactions),
        rocksdb::Env::Priority::LOW);
  }

  return options;
}

RocksDBInstance::RocksDBInstance(std::string const& folder)
    : _dbFolder(folder) {
  MUTEX_LOCKER(locker, _rocksDbMutex);
  if (_db == nullptr) {
    _comparator = new ArangoComparator();
    auto options = generateOptions();
    auto status = rocksdb::DB::Open(options, _dbFolder, &_db);
    if (!status.ok()) {
      std::cerr << status.ToString() << std::endl;
    }
    assert(status.ok());
  }
  _instanceCount++;
}

RocksDBInstance::~RocksDBInstance() {
  MUTEX_LOCKER(locker, _rocksDbMutex);
  _instanceCount--;
  if (_instanceCount.load() == 0) {
    delete _db;
    _db = nullptr;
    delete _comparator;
    _comparator = nullptr;
    remove_directory(_dbFolder.data());
  }
}

rocksdb::DB* RocksDBInstance::db() { return _db; }
ArangoComparator* RocksDBInstance::comparator() { return _comparator; }

static int remove_directory(const char* path) {
  DIR* d = opendir(path);
  size_t path_len = strlen(path);
  int r = -1;

  if (d) {
    struct dirent* p;

    r = 0;

    while (!r && (p = readdir(d))) {
      int r2 = -1;
      char* buf;
      size_t len;

      /* Skip the names "." and ".." as we don't want to recurse on them. */
      if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, "..")) {
        continue;
      }

      len = path_len + strlen(p->d_name) + 2;
      buf = reinterpret_cast<char*>(malloc(len));

      if (buf) {
        struct stat statbuf;

        snprintf(buf, len, "%s/%s", path, p->d_name);

        if (!stat(buf, &statbuf)) {
          if (S_ISDIR(statbuf.st_mode)) {
            r2 = remove_directory(buf);
          } else {
            r2 = unlink(buf);
          }
        }

        free(buf);
      }

      r = r2;
    }

    closedir(d);
  }

  if (!r) {
    r = rmdir(path);
  }

  return r;
}
