////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2016 ArangoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Dr. Frank Celler
/// @author Achim Brandt
////////////////////////////////////////////////////////////////////////////////

#ifndef ROCKSDB_BENCHMARK_MUTEX_LOCKER_H
#define ROCKSDB_BENCHMARK_MUTEX_LOCKER_H 1

#include <benchmark/Mutex.h>
#include <cstdint>

////////////////////////////////////////////////////////////////////////////////
/// @brief construct locker with file and line information
///
/// Ones needs to use macros twice to get a unique variable based on the line
/// number.
////////////////////////////////////////////////////////////////////////////////

#define MUTEX_LOCKER_VAR_B(a) MUTEX_LOCKER_VAR_A(a)

#define MUTEX_LOCKER(obj, lock) benchmark::MutexLocker obj(&lock)

namespace benchmark {

////////////////////////////////////////////////////////////////////////////////
/// @brief mutex locker
///
/// A MutexLocker locks a mutex during its lifetime und unlocks the mutex
/// when it is destroyed.
////////////////////////////////////////////////////////////////////////////////

class MutexLocker {
  MutexLocker(MutexLocker const&) = delete;
  MutexLocker& operator=(MutexLocker const&) = delete;

 public:
  ////////////////////////////////////////////////////////////////////////////////
  /// @brief aquires a lock
  ///
  /// The constructor aquires a lock, the destructor releases the lock.
  ////////////////////////////////////////////////////////////////////////////////

  explicit MutexLocker(Mutex* mutex);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief releases the lock
  //////////////////////////////////////////////////////////////////////////////

  ~MutexLocker();

 private:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief the mutex
  //////////////////////////////////////////////////////////////////////////////

  Mutex* _mutex;
};
}

#endif
