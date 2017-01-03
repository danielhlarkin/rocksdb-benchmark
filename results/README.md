# Results Summary

The full results for each data run can be found in the corresponding `.out` files. In particular, the naming convention is `results.[keyCount].[threadCount].out`, where the other parameters have been chosen programmatically based on `keyCount` (`lookupCount = keyCount * 5`, `hotsetCount = keyCount / 20`, and `rangeLimit = keyCount / 5`). We summarize results in the tables below. In each table, each row corresponds to the `keyCount` value given at left, and each column corresponds to the `threadCount` value given at the top. Throughput is measured in operations per second.

# BatchInsert performance

Insert a batch of documents in random order, splitting the work over all threads.

## Throughput

|          |      1 |      2 |       4 |       8 |      16 |      32 |
| -------- | ------ | ------ | ------- | ------- | ------- | ------- |
|   100000 | 2859.3 | 3350.0 |  2495.8 |  6029.6 | 27561.0 | 31399.2 |
|  1000000 | 4505.9 | 7695.8 | 11564.8 | 13523.6 | 10798.7 |  6433.9 |
| 10000000 | 3882.3 | 7264.4 | 12196.4 | 18308.4 | 20987.7 | 23294.5 |

## Median Latency

|          |       1 |       2 |       4 |       8 |      16 |      32 |
| -------- | ------- | ------- | ------- | ------- | ------- | ------- |
|   100000 | 127.5μs | 134.0μs | 143.0μs | 199.0μs | 287.3μs | 458.0μs |
|  1000000 | 177.4μs | 186.0μs | 205.8μs | 247.6μs | 316.5μs | 471.4μs |
| 10000000 | 232.4μs | 245.8μs | 288.0μs | 366.8μs | 588.7μs | 820.4μs |

## 99th Percentile Latency

|          |       1 |       2 |       4 |       8 |      16 |    32 |
| -------- | ------- | ------- | ------- | ------- | ------- | ----- |
|   100000 | 303.1μs | 319.5μs | 389.1μs | 524.3μs | 722.9μs | 1.1ms |
|  1000000 | 414.7μs | 409.1μs | 413.7μs | 520.2μs | 679.9μs | 1.5ms |
| 10000000 | 486.4μs | 471.0μs | 512.0μs | 679.9μs |   1.3ms | 1.8ms |

# RandomLookup performance

Perform lots of lookups using a uniform distribution over the keys.

## Throughput

|          |       1 |       2 |        4 |        8 |       16 |       32 |
| -------- | ------- | ------- | -------- | -------- | -------- | -------- |
|   100000 | 33914.5 | 69911.3 | 143644.2 | 243211.1 | 396035.5 | 532250.9 |
|  1000000 | 12266.9 | 24372.1 |  51084.8 |  92714.7 |  93196.4 | 324438.5 |
| 10000000 | 11643.4 | 22255.7 |  43683.1 |  71341.2 | 151938.5 | 190669.1 |

## Median Latency

|          |      1 |      2 |      4 |       8 |     16 |      32 |
| -------- | ------ | ------ | ------ | ------- | ------ | ------- |
|   100000 | 26.0μs | 25.4μs | 25.3μs |  28.8μs | 32.0μs |  49.9μs |
|  1000000 | 73.7μs | 73.8μs | 73.2μs |  81.3μs | 86.9μs |  89.8μs |
| 10000000 | 76.8μs | 82.9μs | 86.3μs | 107.0μs | 84.9μs | 156.8μs |

## 99th Percentile Latency

|          |       1 |       2 |       4 |       8 |      16 |      32 |
| -------- | ------- | ------- | ------- | ------- | ------- | ------- |
|   100000 |  54.8μs |  52.2μs |  47.1μs |  61.3μs |  84.1μs |  90.1μs |
|  1000000 | 132.4μs | 119.3μs | 108.9μs | 112.6μs | 156.2μs | 121.3μs |
| 10000000 | 145.4μs | 145.4μs | 121.6μs | 130.3μs | 150.0μs | 195.3μs |

# HotsetLookup performance

Same as RandomLookup, but restricting the keyspace to the hot set.

## Throughput

|          |       1 |       2 |        4 |        8 |       16 |       32 |
| -------- | ------- | ------- | -------- | -------- | -------- | -------- |
|   100000 | 34060.0 | 70969.9 | 141880.7 | 261048.2 | 385218.6 | 539333.9 |
|  1000000 | 12837.8 | 26432.6 |  54122.3 | 102168.5 | 176319.6 | 336821.7 |
| 10000000 | 11835.3 | 22267.0 |  44376.2 |  73080.1 | 160043.9 | 192630.8 |

## Median Latency

|          |      1 |      2 |      4 |       8 |     16 |      32 |
| -------- | ------ | ------ | ------ | ------- | ------ | ------- |
|   100000 | 25.5μs | 24.6μs | 24.7μs |  27.3μs | 33.1μs |  51.7μs |
|  1000000 | 69.4μs | 69.5μs | 68.8μs |  73.5μs | 77.9μs |  85.0μs |
| 10000000 | 75.2μs | 81.6μs | 84.7μs | 104.3μs | 83.6μs | 152.7μs |

## 99th Percentile Latency

|          |       1 |       2 |       4 |       8 |      16 |      32 |
| -------- | ------- | ------- | ------- | ------- | ------- | ------- |
|   100000 |  54.6μs |  51.1μs |  48.9μs |  58.2μs |  82.0μs |  81.9μs |
|  1000000 | 127.0μs | 121.3μs | 106.0μs | 102.1μs | 149.2μs | 117.8μs |
| 10000000 | 143.9μs | 142.3μs | 119.8μs | 131.1μs | 147.9μs | 192.9μs |
