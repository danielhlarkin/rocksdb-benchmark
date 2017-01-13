'use strict';

const fs = require('fs');
const path = require('path');
const util = require('util');

const throughput = {};
const usage = {};
const latencies = {};

const getFileList = () => {
  const fullFileList = fs.readdirSync( 'results' );
  // regex to extract file extension: http://stackoverflow.com/a/680982
  const ext = /(?:\.([^.]+))?$/;
  const fileList = fullFileList.filter( ( f ) => ext.exec( f )[1] == 'json' );

  return fileList;
};

const parseFile = ( f ) => {
  const rawContent = fs.readFileSync( `results/${f}` );
  const parsedContent = JSON.parse( rawContent );

  return parsedContent;
};

const processFiles = ( list ) => {
  list.forEach( ( file ) => {
    const data = parseFile( file );
    processFileData( data );
  } )
};

const processFileData = ( data ) => {
  const keyCount = data.config.keyCount;
  const threadCount = data.config.threadCount;

  data.workloads.forEach( ( w ) => {
    if ( throughput[w.name] === undefined ) {
      throughput[w.name] = {};
    }
    if ( usage[w.name] === undefined ) {
      usage[w.name] = {};
    }
    if ( latencies[w.name] === undefined ) {
      latencies[w.name] = {};
    }

    processThroughputData( throughput[w.name], w, keyCount, threadCount );
    processUsageData( usage[w.name], w, keyCount, threadCount );
    processLatencyData( latencies[w.name], w, keyCount, threadCount );
  } );
};

const processThroughputData = ( dst, data, keyCount, threadCount ) => {
  if ( dst[keyCount] === undefined ) {
    dst[keyCount] = {};
  }

  if ( dst[keyCount][threadCount] === undefined ) {
    dst[keyCount][threadCount] = {};
  }

  dst[keyCount][threadCount] = data.performance.throughput;
};

const processUsageData = ( dst, data, keyCount, threadCount ) => {
  if ( dst[keyCount] === undefined ) {
    dst[keyCount] = {};
  }

  if ( dst[keyCount][threadCount] === undefined ) {
    dst[keyCount][threadCount] = {};
  }

  dst[keyCount][threadCount] = data.usage;
};

const processLatencyData = ( dst, data, keyCount, threadCount ) => {
  if ( dst[keyCount] === undefined ) {
    dst[keyCount] = {};
  }

  if ( dst[keyCount][threadCount] === undefined ) {
    dst[keyCount][threadCount] = {};
  }

  dst[keyCount][threadCount] = data.performance.latencies;
};

const collectThroughputs = ( operation, keyCount ) => {
  const base = throughput[operation][keyCount];
  return Object.keys( base ).map( ( key ) => {
    return base[key];
  } );
};

const collectUsage = ( operation, keyCount, type, percentile ) => {
  const base = usage[operation][keyCount];
  return Object.keys( base ).map( ( key ) => {
    return base[key][type][percentile.toFixed(6)];
  } );
};

const collectLatencies = ( operation, keyCount, percentile ) => {
  const base = latencies[operation][keyCount];
  return Object.keys( base ).map( ( key ) => {
    return base[key][percentile.toFixed(6)];
  } );
};

const printHeaderRows = ( data ) => {

  const n = Object.keys( data ).length;
  let header = '| |';
  let separator = '| --- |';
  for ( let i = 0; i < n; i++ ) {
    header = header.concat(` ${Object.keys( data )[i]} |`);
    separator = separator.concat(' --- |');
  }
  console.log( header );
  console.log( separator );
};

const printRow = ( keyCount, data ) => {
  let row = `| ${keyCount} |`;
  for ( let i = 0; i < data.length; i++ ) {
    if ( Array.isArray( data[i] ) ) {
      row = row.concat(` ${data[i][0].toFixed(0)}${data[i][1]} |`);
    } else {
      row = row.concat(` ${data[i].toFixed(0)} |`);
    }
  }
  console.log( row );
};

const printThroughputTable = ( operation ) => {
  const firstKey = Object.keys( throughput[operation] )[0];
  printHeaderRows( throughput[operation][firstKey] );
  Object.keys( throughput[operation] ).forEach( ( keyCount ) => {
    printRow( keyCount, collectThroughputs( operation, keyCount ) );
  } );
};

const printLatencyTable = ( operation, percentile ) => {
  const firstKey = Object.keys( latencies[operation] )[0];
  printHeaderRows( latencies[operation][firstKey] );
  Object.keys( latencies[operation] ).forEach( ( keyCount ) => {
    printRow( keyCount, collectLatencies( operation, keyCount, percentile ) );
  } );
};

const printUsageTable = ( operation, type, percentile ) => {
  const firstKey = Object.keys( usage[operation] )[0];
  printHeaderRows(usage[operation][firstKey] );
  Object.keys( usage[operation] ).forEach( ( keyCount ) => {
    printRow( keyCount, collectUsage( operation, keyCount, type, percentile ) );
  } );
};

const printThroughputSection = () => {
  Object.keys( throughput ).sort().forEach( ( workload ) => {
    console.log( `## ${workload}` );
    printThroughputTable( workload );
  } );
};

const printLatencySection = () => {
  Object.keys( latencies ).sort().forEach( ( workload ) => {
    console.log( `## ${workload}` );
    const latencies = [0.5, 0.95, 0.99, 0.999999];
    latencies.forEach( ( latency ) => {
      console.log( `### Percentile: ${latency * 100}th` );
      printLatencyTable( workload, latency );
    } );
  } );
};

const printUsageSection = () => {
  Object.keys( usage ).sort().forEach( ( workload ) => {
    console.log( `## ${workload}` );
    const types = ['resident', 'virtual', 'disk'];
    types.forEach( ( type ) => {
      let name;
      switch ( type ) {
        case 'resident':
          name = 'Resident Memory';
          break;
        case 'virtual':
          name = 'Virtual Memory';
          break;
        case 'disk':
        default:
          name = 'Disk';
          break;
      }
      console.log( `### Maximum ${name} Usage` );
      printUsageTable( workload, type, 0.999999 );
    } );
  } );
};

const printMarkdown = () => {
  console.log( '# Throughput' );
  printThroughputSection();

  console.log( '# Latencies' );
  printLatencySection();

  console.log( '# Resource Usage' );
  printUsageSection();
};

const list = getFileList();
processFiles( list );
printMarkdown();
