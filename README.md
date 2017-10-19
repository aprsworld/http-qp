# http-qp Queue and push data via HTTP

## Purpose

Make best effort to deliver data *from* producer(s) *to* one or more HTTP servers. Queue messages and re-transmit to HTTP server(s) until acknowledged.

## Overview

Receive messages via TCP socket. Message is timestamped at time of accepting connection. Message is considered complete when remote end closes socket. Message is discarded if timeout is reached or if TCP error occurs.

Time stamp

Queue message

Transmit messages via HTTP

Remove from queue when message(s) are acknowledged by server. If queue reaches maximum number of messages, remove messages on a FIFO basis.


## Command line
`http-qp destinationURL0 [destinationURL1]`

APRS World uses `getopt(3)` and related `unistd.h` routines for argument evaluation.

### Mandatory

One or more destination URLs. There are arguments not proceed by flags


### Optional
`-p [port]` TCP port to receive messages on
`-s [NTP stratum level]` 0 to 16. 16 indicates not NTP syncronized
`-t [timeout]` TCP port connection timeout in milliseconds. http-qp will close connection and discard message if timeout is reached
`-n` Queue size in number of message
`-v` Extra verbose output (output useful for debugging, but overkill for normal operation)


## Signals

`SIGUSR1` dump human readable status

`SIGPIPE` internal signal that is used to signify error with TCP connection

`SIGALRM` internal signal that is used to signify timeout on incomming TCP data

## Output

APRS World software generally outputs informational messages to stderr. Each line is proceeded with `#` and ended with `\n`

## Data Format

Data is received on TCP socket. A package of data is everything received until socket is closed by remote end.

*Data* gets encapsulated in *messages* within http-qp. A message contains the data, a timestamp, a time stamp quality indicator, and a message type indicator.

A message can be sent to HTTP server as a single message or as an array of messages.

All messages and arrays are encoded with `MessagePack` (https://msgpack.org/) encoding.

### Single Message Format

#### Elements
- message type
-- 0x00 | NTP stratum (5 bits representing 0 to 31 ... although NTP stratum is only defined from 0 to 16)
- timestamp
-- msgpack `timestamp 64` format (https://github.com/msgpack/msgpack/blob/master/spec.md#timestamp-extension-type)
- message
-- 1 to n bytes of 8-bit data


## Notes to programmer

- See nmeaReader.c (https://github.com/aprsworld/nmeaReader/blob/master/nmeaReader.c) for sample program with alarm handling, socket, getopt(3) command line argument parsing

- See (https://github.com/adam-p/markdown-here/wiki/Markdown-Cheatsheet) for Markdown Cheatsheet

- See (https://guides.github.com/activities/hello-world/) for introduction to Git / GitHub

- Use GitHub "Issues" tracker (https://github.com/aprsworld/http-qp/issues)

- Use `cmp` (https://github.com/camgunz/cmp) MessagePack library or equivalent in http-qp