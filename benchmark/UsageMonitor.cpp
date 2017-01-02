#include <benchmark/UsageMonitor.h>

using namespace benchmark;

UsageMonitor::UsageMonitor(std::string const& folder) : _folder(folder) {}

UsageMonitor::~UsageMonitor() {}

uint64_t UsageMonitor::getResidentUsage() {
  // modified from http://stackoverflow.com/a/671389
  using std::ios_base;
  using std::ifstream;
  using std::string;

  // 'file' stat seems to give the most reliable results
  //
  ifstream stat_stream("/proc/self/stat", ios_base::in);

  // dummy vars for leading entries in stat that we don't care about
  //
  string pid, comm, state, ppid, pgrp, session, tty_nr;
  string tpgid, flags, minflt, cminflt, majflt, cmajflt;
  string utime, stime, cutime, cstime, priority, nice;
  string O, itrealvalue, starttime;

  // the two fields we want
  //
  uint64_t vsize;
  uint64_t rss;

  stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr >>
      tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt >> utime >>
      stime >> cutime >> cstime >> priority >> nice >> O >> itrealvalue >>
      starttime >> vsize >> rss;  // don't care about the rest

  stat_stream.close();

  uint64_t page_size_kb =
      sysconf(_SC_PAGE_SIZE) /
      1024;  // in case x86-64 is configured to use 2MB pages
  return (rss * page_size_kb);
}

uint64_t UsageMonitor::getVirtualUsage() {
  // modified from http://stackoverflow.com/a/671389
  using std::ios_base;
  using std::ifstream;
  using std::string;

  // 'file' stat seems to give the most reliable results
  //
  ifstream stat_stream("/proc/self/stat", ios_base::in);

  // dummy vars for leading entries in stat that we don't care about
  //
  string pid, comm, state, ppid, pgrp, session, tty_nr;
  string tpgid, flags, minflt, cminflt, majflt, cmajflt;
  string utime, stime, cutime, cstime, priority, nice;
  string O, itrealvalue, starttime;

  // the two fields we want
  //
  uint64_t vsize;
  uint64_t rss;

  stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr >>
      tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt >> utime >>
      stime >> cutime >> cstime >> priority >> nice >> O >> itrealvalue >>
      starttime >> vsize >> rss;  // don't care about the rest

  stat_stream.close();

  return (vsize / 1024);
}

uint64_t UsageMonitor::getDiskUsage() {
  // modified from http://stackoverflow.com/a/15497931

  // command to be executed
  std::string cmd("du -s --block-size=1024 ");
  cmd.append(_folder);
  cmd.append(" | cut -f1 2>&1\0");

  // execute above command and get the output
  char buffer[128];
  std::string result = "";
  std::shared_ptr<FILE> pipe(popen(cmd.data(), "r"), pclose);
  if (!pipe) throw std::runtime_error("popen() failed!");
  while (!feof(pipe.get())) {
    if (fgets(buffer, 128, pipe.get()) != NULL) result += buffer;
  }

  return atoll(result.data());
}

void UsageMonitor::monitor(std::string const& folder, uint64_t threadCount,
                           std::chrono::nanoseconds interval,
                           std::atomic<uint64_t> const* workersDone,
                           std::atomic<uint64_t> const* workersErrored,
                           qdigest::QDigest* residentDigest,
                           qdigest::QDigest* virtualDigest,
                           qdigest::QDigest* diskDigest) {
  UsageMonitor monitor(folder);
  while (workersDone->load() + workersErrored->load() < threadCount) {
    auto start = std::chrono::high_resolution_clock::now();
    try {
      residentDigest->insert(monitor.getResidentUsage(), 1);
      virtualDigest->insert(monitor.getVirtualUsage(), 1);
      diskDigest->insert(monitor.getDiskUsage(), 1);
    } catch (...) {  // nothing to do here
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(
        std::max(interval - (end - start), std::chrono::nanoseconds(0)));
  }
}
