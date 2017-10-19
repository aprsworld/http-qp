# test-server scripts

## test-status.php
parameters (via either POST or GET)
- `request_code=value` Send HTTP response code `value`
-`random=value`  Send HTTP response code `value` when randomly selected, otherwise send request_code=value



### example of always return response code 501
```
curl http://data.aprsworld.com/http-qp/test-server/test-status.php?response_code=501 -v

*   Trying 72.251.203.219...
* Connected to data.aprsworld.com (72.251.203.219) port 80 (#0)
> GET /http-qp/test-server/test-status.php?response_code=501 HTTP/1.1
> Host: data.aprsworld.com
> User-Agent: curl/7.47.0
> Accept: */*
> 
< HTTP/1.1 501 Not Implemented
< Date: Thu, 19 Oct 2017 19:34:06 GMT
< Server: Apache/2.2.16 (Debian)
< X-Powered-By: PHP/5.3.3-7+squeeze29
< Vary: Accept-Encoding
< Content-Length: 0
< Connection: close
< Content-Type: text/html
< 
* Closing connection 0

```

### example of randomly returning response code 200 or response code 404

```
curl http://data.aprsworld.com/http-qp/test-server/test-status.php?response_code=200\&random=404 -v

*   Trying 72.251.203.219...
* Connected to data.aprsworld.com (72.251.203.219) port 80 (#0)
> GET /http-qp/test-server/test-status.php?response_code=200&random=404 HTTP/1.1
> Host: data.aprsworld.com
> User-Agent: curl/7.47.0
> Accept: */*
> 
< HTTP/1.1 404 Not Found
< Date: Thu, 19 Oct 2017 21:41:00 GMT
< Server: Apache/2.2.16 (Debian)
< X-Powered-By: PHP/5.3.3-7+squeeze29
< Vary: Accept-Encoding
< Content-Length: 0
< Content-Type: text/html
< 
* Connection #0 to host data.aprsworld.com left intact

curl http://data.aprsworld.com/http-qp/test-server/test-status.php?response_code=200\&random=404 -v

*   Trying 72.251.203.219...
* Connected to data.aprsworld.com (72.251.203.219) port 80 (#0)
> GET /http-qp/test-server/test-status.php?response_code=200&random=404 HTTP/1.1
> Host: data.aprsworld.com
> User-Agent: curl/7.47.0
> Accept: */*
> 
< HTTP/1.1 200 OK
< Date: Thu, 19 Oct 2017 21:41:01 GMT
< Server: Apache/2.2.16 (Debian)
< X-Powered-By: PHP/5.3.3-7+squeeze29
< Vary: Accept-Encoding
< Content-Length: 0
< Content-Type: text/html
< 
* Connection #0 to host data.aprsworld.com left intact
```