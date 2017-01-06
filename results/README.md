# Results Summary

The full results for each data run can be found in the corresponding `.out` files. In particular, the naming convention is `results.[keyCount].[threadCount].out`, where the other parameters have been chosen programmatically based on `keyCount` (`lookupCount = keyCount * 5`, `hotsetCount = keyCount / 20`, and `rangeLimit = keyCount / 5`). We summarize results in the tables below. In each table, each row corresponds to the `keyCount` value given at left, and each column corresponds to the `threadCount` value given at the top. Throughput is measured in operations per second.

# BatchInsert performance

Insert a batch of documents in random order, splitting the work over all threads.

## Throughput

|          |       1 |       2 |       4 |       8 |      16 |      32 |
| -------- | ------- | ------- | ------- | ------- | ------- | ------- |
|   100000 | 15528.5 | 28826.4 | 26477.2 | 59441.7 | 68940.6 | 54981.4 |
|  1000000 | 14461.7 | 24779.6 | 35328.5 | 51555.6 | 25664.3 |  8431.6 |
| 10000000 |  3882.3 |  7264.4 | 12196.4 | 18308.4 | 20987.7 | 23294.5 |

## Median Latency

|          |       1 |       2 |       4 |       8 |      16 |      32 |
| -------- | ------- | ------- | ------- | ------- | ------- | ------- |
|   100000 |  56.5μs |  54.8μs |  69.4μs |  84.6μs | 148.0μs | 468.9μs |
|  1000000 |  62.4μs |  70.4μs |  90.9μs | 115.6μs | 134.7μs | 235.2μs |
| 10000000 | 232.4μs | 245.8μs | 288.0μs | 366.8μs | 588.7μs | 820.4μs |

## 99th Percentile Latency

|          |       1 |       2 |       4 |       8 |      16 |      32 |
| -------- | ------- | ------- | ------- | ------- | ------- | ------- |
|   100000 |  94.2μs | 108.0μs | 123.9μs | 178.2μs | 235.5μs |   1.1ms |
|  1000000 |  97.3μs | 118.8μs | 178.7μs | 237.6μs | 301.1μs | 892.9μs |
| 10000000 | 486.4μs | 471.0μs | 512.0μs | 679.9μs |   1.3ms |   1.8ms |

# RandomLookup performance

Perform lots of lookups using a uniform distribution over the keys.

## Throughput

|          |       1 |        2 |        4 |        8 |       16 |        32 |
| -------- | ------- | -------- | -------- | -------- | -------- | --------- |
|   100000 | 77422.7 | 215909.8 | 419294.5 | 752425.6 | 833915.6 | 1216290.1 |
|  1000000 | 30774.3 |  56720.6 | 110143.4 | 196622.8 | 207290.8 |  161732.6 |
| 10000000 | 11643.4 |  22255.7 |  43683.1 |  71341.2 | 151938.5 |  190669.1 |

## Median Latency

|          |      1 |      2 |      4 |       8 |     16 |      32 |
| -------- | ------ | ------ | ------ | ------- | ------ | ------- |
|   100000 | 12.1μs |  8.5μs |  8.8μs |   9.5μs | 16.8μs |  18.3μs |
|  1000000 | 31.7μs | 33.8μs | 34.9μs |  38.8μs | 75.1μs |  99.9μs |
| 10000000 | 76.8μs | 82.9μs | 86.3μs | 107.0μs | 84.9μs | 156.8μs |

## 99th Percentile Latency

|          |       1 |       2 |       4 |       8 |      16 |      32 |
| -------- | ------- | ------- | ------- | ------- | ------- | ------- |
|   100000 |  16.0μs |  12.4μs |  16.6μs |  13.9μs |  24.2μs |  26.1μs |
|  1000000 |  41.0μs |  45.0μs |  49.2μs |  56.6μs | 110.1μs | 157.7μs |
| 10000000 | 145.4μs | 145.4μs | 121.6μs | 130.3μs | 150.0μs | 195.3μs |

# HotsetLookup performance

Same as RandomLookup, but restricting the keyspace to the hot set.

## Throughput

|          |       1 |        2 |        4 |        8 |        16 |        32 |
| -------- | ------- | -------- | -------- | -------- | --------- | --------- |
|   100000 | 80132.1 | 226993.6 | 421468.3 | 776940.8 | 1416978.5 | 1214016.4 |
|  1000000 | 33343.7 |  63449.9 | 121030.2 | 222480.3 |  254656.3 |  125083.9 |
| 10000000 | 11835.3 |  22267.0 |  44376.2 |  73080.1 |  160043.9 |  192630.8 |

## Median Latency

|          |      1 |      2 |      4 |       8 |     16 |      32 |
| -------- | ------ | ------ | ------ | ------- | ------ | ------- |
|   100000 | 11.7μs |  8.0μs |  8.7μs |   9.2μs | 11.5μs |  18.7μs |
|  1000000 | 28.4μs | 30.1μs | 31.1μs |  33.8μs | 57.1μs |  88.0μs |
| 10000000 | 75.2μs | 81.6μs | 84.7μs | 104.3μs | 83.6μs | 152.7μs |

## 99th Percentile Latency

|          |       1 |       2 |       4 |       8 |      16 |      32 |
| -------- | ------- | ------- | ------- | ------- | ------- | ------- |
|   100000 |  15.2μs |  11.4μs |  12.5μs |  13.8μs |  22.9μs |  26.2μs |
|  1000000 |  40.1μs |  43.0μs |  46.3μs |  50.7μs |  98.6μs | 148.2μs |
| 10000000 | 143.9μs | 142.3μs | 119.8μs | 131.1μs | 147.9μs | 192.9μs |
