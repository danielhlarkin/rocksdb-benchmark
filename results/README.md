# Throughput
## insert
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 16482 | 28339 | 23876 | 58370 | 64886 | 81018 |
| 1000000 | 12716 | 22956 | 40157 | 58748 | 19192 | 9275 |
| 10000000 | 8530 | 14927 | 28598 | 45397 | 46782 | 44219 |
| 100000000 | 6584 | 12867 | 23024 | 33867 | 39428 | 42701 |
## insertBatch
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 19634 | 38523 | 68550 | 110389 | 141080 | 148981 |
| 1000000 | 12509 | 24270 | 43857 | 75767 | 100103 | 129229 |
| 10000000 | 9269 | 16111 | 32525 | 53597 | 72749 | 79730 |
| 100000000 | 7663 | 13057 | 25263 | 39729 | 55443 | 64606 |
## lookupRange
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 41 | 13 | 12 | 26 | 18 | 23 |
| 1000000 | 68 | 31 | 14 | 5 | 15 | 22 |
| 10000000 | 125 | 66 | 30 | 17 | 53 | 37 |
## lookupSingle
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 86510 | 171068 | 316962 | 542151 | 676181 | 928988 |
| 1000000 | 26690 | 50626 | 96981 | 172639 | 176726 | 185906 |
| 10000000 | 24626 | 44493 | 89447 | 134140 | 181665 | 255143 |
## lookupSingleHotset
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 87072 | 174571 | 329692 | 555764 | 660907 | 973437 |
| 1000000 | 29294 | 54261 | 104290 | 193734 | 227651 | 130287 |
| 10000000 | 24936 | 45085 | 90585 | 136740 | 192728 | 265756 |
# Latencies
## insert
### Percentile: 50th
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 52μs | 56μs | 62μs | 72μs | 131μs | 170μs |
| 1000000 | 71μs | 78μs | 82μs | 100μs | 128μs | 180μs |
| 10000000 | 110μs | 125μs | 128μs | 157μs | 261μs | 348μs |
| 100000000 | 129μs | 137μs | 152μs | 187μs | 311μs | 511μs |
### Percentile: 95th
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 67μs | 75μs | 87μs | 100μs | 187μs | 297μs |
| 1000000 | 98μs | 103μs | 109μs | 135μs | 201μs | 369μs |
| 10000000 | 142μs | 160μs | 170μs | 211μs | 353μs | 684μs |
| 100000000 | 324μs | 184μs | 198μs | 333μs | 753μs | 2ms |
### Percentile: 99th
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 91μs | 106μs | 140μs | 152μs | 297μs | 524μs |
| 1000000 | 111μs | 121μs | 139μs | 172μs | 315μs | 541μs |
| 10000000 | 152μs | 175μs | 199μs | 280μs | 565μs | 892μs |
| 100000000 | 641μs | 512μs | 487μs | 770μs | 1ms | 3ms |
### Percentile: 99.9999th
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 19ms | 34ms | 67ms | 67ms | 134ms | 134ms |
| 1000000 | 134ms | 134ms | 134ms | 134ms | 134ms | 268ms |
| 10000000 | 537ms | 537ms | 537ms | 1s | 1s | 1s |
| 100000000 | 4s | 4s | 4s | 9s | 9s | 9s |
## insertBatch
### Percentile: 50th
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 50ms | 51ms | 57ms | 65ms | 94ms | 183ms |
| 1000000 | 79ms | 82ms | 90ms | 104ms | 159ms | 251ms |
| 10000000 | 104ms | 120ms | 117ms | 146ms | 219ms | 393ms |
| 100000000 | 125ms | 136ms | 141ms | 170ms | 267ms | 514ms |
### Percentile: 95th
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 53ms | 55ms | 66ms | 98ms | 145ms | 291ms |
| 1000000 | 92ms | 95ms | 107ms | 123ms | 224ms | 309ms |
| 10000000 | 124ms | 147ms | 149ms | 180ms | 334ms | 551ms |
| 100000000 | 157ms | 199ms | 213ms | 338ms | 472ms | 1s |
### Percentile: 99th
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 58ms | 64ms | 69ms | 112ms | 176ms | 316ms |
| 1000000 | 99ms | 100ms | 114ms | 138ms | 246ms | 364ms |
| 10000000 | 129ms | 155ms | 161ms | 218ms | 376ms | 599ms |
| 100000000 | 174ms | 587ms | 403ms | 522ms | 688ms | 2s |
### Percentile: 99.9999th
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 58ms | 64ms | 69ms | 112ms | 176ms | 334ms |
| 1000000 | 113ms | 111ms | 124ms | 191ms | 272ms | 394ms |
| 10000000 | 182ms | 218ms | 574ms | 807ms | 472ms | 1s |
| 100000000 | 537ms | 1s | 1s | 2s | 4s | 4s |
## lookupRange
### Percentile: 50th
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 47ms | 190ms | 32ms | 349ms | 798ms | 1s |
| 1000000 | 99ms | 122ms | 259ms | 937ms | 71ms | 156ms |
| 10000000 | 47ms | 77ms | 154ms | 402ms | 669ms | 776ms |
### Percentile: 95th
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 47ms | 190ms | 266ms | 349ms | 927ms | 1s |
| 1000000 | 108ms | 212ms | 456ms | 2s | 1s | 666ms |
| 10000000 | 77ms | 121ms | 246ms | 564ms | 1s | 2s |
### Percentile: 99th
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 47ms | 190ms | 266ms | 349ms | 927ms | 1s |
| 1000000 | 108ms | 212ms | 475ms | 2s | 1s | 1s |
| 10000000 | 85ms | 143ms | 337ms | 732ms | 1s | 2s |
### Percentile: 99.9999th
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 47ms | 190ms | 266ms | 349ms | 927ms | 1s |
| 1000000 | 108ms | 212ms | 475ms | 2s | 1s | 1s |
| 10000000 | 85ms | 144ms | 359ms | 793ms | 1s | 2s |
## lookupSingle
### Percentile: 50th
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 11μs | 11μs | 12μs | 13μs | 21μs | 24μs |
| 1000000 | 36μs | 38μs | 40μs | 44μs | 87μs | 104μs |
| 10000000 | 39μs | 43μs | 43μs | 57μs | 89μs | 118μs |
### Percentile: 95th
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 13μs | 13μs | 14μs | 16μs | 28μs | 30μs |
| 1000000 | 44μs | 46μs | 48μs | 54μs | 106μs | 137μs |
| 10000000 | 43μs | 48μs | 49μs | 65μs | 105μs | 142μs |
### Percentile: 99th
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 15μs | 15μs | 16μs | 24μs | 31μs | 34μs |
| 1000000 | 47μs | 51μs | 57μs | 63μs | 116μs | 160μs |
| 10000000 | 47μs | 53μs | 59μs | 74μs | 115μs | 159μs |
### Percentile: 99.9999th
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 500μs | 171μs | 1ms | 751μs | 2ms | 5ms |
| 1000000 | 8ms | 8ms | 8ms | 8ms | 17ms | 34ms |
| 10000000 | 17ms | 134ms | 17ms | 17ms | 17ms | 34ms |
## lookupSingleHotset
### Percentile: 50th
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 11μs | 11μs | 11μs | 13μs | 22μs | 23μs |
| 1000000 | 32μs | 35μs | 36μs | 39μs | 66μs | 95μs |
| 10000000 | 39μs | 43μs | 42μs | 56μs | 84μs | 113μs |
### Percentile: 95th
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 13μs | 13μs | 14μs | 16μs | 27μs | 29μs |
| 1000000 | 42μs | 44μs | 46μs | 51μs | 94μs | 131μs |
| 10000000 | 43μs | 47μs | 48μs | 65μs | 101μs | 138μs |
### Percentile: 99th
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 15μs | 14μs | 16μs | 18μs | 30μs | 33μs |
| 1000000 | 46μs | 49μs | 54μs | 58μs | 105μs | 153μs |
| 10000000 | 46μs | 52μs | 59μs | 73μs | 111μs | 155μs |
### Percentile: 99.9999th
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 496μs | 176μs | 726μs | 2ms | 3ms | 4ms |
| 1000000 | 8ms | 8ms | 8ms | 8ms | 17ms | 34ms |
| 10000000 | 17ms | 8ms | 17ms | 17ms | 17ms | 17ms |
# Resource Usage
## insert
### Maximum Resident Memory Usage
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 28MB | 26MB | 36MB | 10MB | 10MB | 10MB |
| 1000000 | 195MB | 199MB | 204MB | 218MB | 247MB | 327MB |
| 10000000 | 219MB | 222MB | 225MB | 237MB | 264MB | 325MB |
| 100000000 | 258MB | 254MB | 266MB | 277MB | 303MB | 358MB |
### Maximum Virtual Memory Usage
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 81MB | 93MB | 125MB | 125MB | 137MB | 185MB |
| 1000000 | 269MB | 281MB | 313MB | 393MB | 545MB | 801MB |
| 10000000 | 313MB | 329MB | 377MB | 445MB | 585MB | 841MB |
| 100000000 | 357MB | 389MB | 441MB | 529MB | 685MB | 977MB |
### Maximum Disk Usage
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 75MB | 75MB | 75MB | 75MB | 75MB | 75MB |
| 1000000 | 151MB | 183MB | 150MB | 150MB | 211MB | 207MB |
| 10000000 | 2GB | 2GB | 2GB | 2GB | 2GB | 2GB |
| 100000000 | 13GB | 12GB | 12GB | 12GB | 12GB | 12GB |
## insertBatch
### Maximum Resident Memory Usage
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 45MB | 39MB | 32MB | 0KB | 0KB | 0KB |
| 1000000 | 210MB | 211MB | 211MB | 210MB | 202MB | 224MB |
| 10000000 | 236MB | 238MB | 239MB | 244MB | 244MB | 253MB |
| 100000000 | 278MB | 291MB | 321MB | 340MB | 343MB | 360MB |
### Maximum Virtual Memory Usage
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 101MB | 121MB | 141MB | 0KB | 0KB | 0KB |
| 1000000 | 305MB | 329MB | 373MB | 469MB | 585MB | 841MB |
| 10000000 | 337MB | 369MB | 417MB | 501MB | 657MB | 937MB |
| 100000000 | 405MB | 433MB | 521MB | 601MB | 709MB | 1009MB |
### Maximum Disk Usage
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 75MB | 75MB | 75MB | 0KB | 0KB | 0KB |
| 1000000 | 363MB | 363MB | 360MB | 360MB | 349MB | 351MB |
| 10000000 | 3GB | 3GB | 3GB | 3GB | 3GB | 3GB |
| 100000000 | 23GB | 23GB | 23GB | 23GB | 23GB | 23GB |
## lookupRange
### Maximum Resident Memory Usage
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 0KB | 0KB | 0KB | 0KB | 0KB | 33MB |
| 1000000 | 199MB | 199MB | 199MB | 200MB | 154MB | 155MB |
| 10000000 | 183MB | 179MB | 215MB | 203MB | 210MB | 246MB |
### Maximum Virtual Memory Usage
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 0KB | 0KB | 0KB | 0KB | 0KB | 285MB |
| 1000000 | 273MB | 289MB | 321MB | 401MB | 545MB | 801MB |
| 10000000 | 313MB | 329MB | 377MB | 445MB | 585MB | 873MB |
### Maximum Disk Usage
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 0KB | 0KB | 0KB | 0KB | 0KB | 75MB |
| 1000000 | 151MB | 151MB | 150MB | 150MB | 178MB | 181MB |
| 10000000 | 2GB | 1GB | 1GB | 1GB | 1GB | 1GB |
## lookupSingle
### Maximum Resident Memory Usage
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 32MB | 32MB | 33MB | 0KB | 0KB | 0KB |
| 1000000 | 201MB | 204MB | 210MB | 223MB | 192MB | 238MB |
| 10000000 | 184MB | 183MB | 219MB | 209MB | 218MB | 265MB |
### Maximum Virtual Memory Usage
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 93MB | 109MB | 129MB | 0KB | 0KB | 0KB |
| 1000000 | 273MB | 285MB | 321MB | 401MB | 545MB | 801MB |
| 10000000 | 313MB | 329MB | 377MB | 445MB | 585MB | 841MB |
### Maximum Disk Usage
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 75MB | 75MB | 75MB | 0KB | 0KB | 0KB |
| 1000000 | 151MB | 151MB | 150MB | 150MB | 178MB | 181MB |
| 10000000 | 2GB | 2GB | 1GB | 1GB | 1GB | 1GB |
## lookupSingleHotset
### Maximum Resident Memory Usage
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 32MB | 32MB | 32MB | 0KB | 0KB | 0KB |
| 1000000 | 201MB | 204MB | 210MB | 223MB | 192MB | 243MB |
| 10000000 | 184MB | 182MB | 219MB | 209MB | 218MB | 265MB |
### Maximum Virtual Memory Usage
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 93MB | 109MB | 133MB | 0KB | 0KB | 0KB |
| 1000000 | 273MB | 289MB | 321MB | 401MB | 545MB | 801MB |
| 10000000 | 313MB | 329MB | 377MB | 445MB | 585MB | 841MB |
### Maximum Disk Usage
| | 1 | 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- | --- | --- |
| 100000 | 75MB | 75MB | 75MB | 0KB | 0KB | 0KB |
| 1000000 | 151MB | 151MB | 150MB | 150MB | 178MB | 181MB |
| 10000000 | 2GB | 1GB | 1GB | 1GB | 1GB | 1GB |
