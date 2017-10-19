# http-qp Queue and push data via HTTP

## Overview

Receive messages via TCP socket. Message is timestamped at time of accepting connection. Message is considered complete when remote end closes socket. Message is discarded if timeout is reached or if TCP error occurs.

Time stamp

Queue message

Transmit messages via HTTP

Remove from queue when message(s) are acknowledged by server. If queue reaches maximum number of messages, remove messages on a FIFO basis.


## Command line
`http-qp destinationURL0 [destinationURL1]`
### Mandatory

### Optional
`-p [port]` TCP port to receive messages on
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

### Timestamp

Time stamp is the msgpack `timestamp 64` format:

```
timestamp 64 stores the number of seconds and nanoseconds that have elapsed since 1970-01-01 00:00:00 UTC
in 32-bit unsigned integers:
+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+
|  0xd7  |   -1   |nanoseconds in 30-bit unsigned int |  seconds in 34-bit unsigned int   |
+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+
```