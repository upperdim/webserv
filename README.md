## Description

HTTP server from scratch for static file serving.

**Key features:**
- Single process
- Non-blocking
- Event-driven
- I/O multiplexed
- High concurrency
- High throughput
- Error handling
- Partial write management
- Full HTTP request lifecycle
- Incremental HTTP request parser
- Incremental HTTP response streaming
- Configuration with subset of nginx syntax
- Virtual servers
- Routing rules
- Directory autoindexing
- CGI support

## Modules

| Module  | Description |
|---------|-------------|
| Config  | Parses the configuration file for initialization of the server(s) |
| Network | Handles server creation, management, clients, data transmission |
| HTTP    | Parses transmitted data into HTTP |
| Handler | Performs operations requested via HTTP |

## Measurements

```
$ wrk -t12 -c10000 -d60s http://127.0.0.1:8081/canvas/5_rock_paper_scissors.html
Running 1m test @ http://127.0.0.1:8081/canvas/5_rock_paper_scissors.html
12 threads and 10000 connections
Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     3.94ms   52.38ms   1.99s    99.51%
    Req/Sec     1.25k     0.92k    7.63k    69.61%
752456 requests in 1.00m, 0.87GB read
Socket errors: connect 0, read 752472, write 0, timeout 876
Requests/sec:  12525.81
Transfer/sec:     14.78MB
```

## Limitations & Improvement Potentials

Here are the list of improvements that are not implemented due to time cost and diminishing returns:
    
1. **O(n) poll()**

    Backbone of the project is the `poll()` functionon, providing I/O multiplexing and concurrency. However, it is O(n) instead of O(1) `epoll()` of Linux or `kqueue()` of macOS. Project was made on macOS but since this is mainly an educational project, we chose to move forward with the portable option despite the performance tradeoff.

2. **Syscall heavy hot path**

    Hot path of the server code was not implemented with the minimum amount of system calls in mind.

3. **No zero-copy strategy**

    Hot path of the server code was not implemented with a zero copy strategy in mind.

4. **Filesystem calls in request path**

    Hot path of the server code was not implemented with the minimum amount of filesystem calls in mind.

5. **String-heavy response pipeline**

6. **User mode - kernel mode switching on sending**

    Due to project requirements, we are forced to use `send()` instead of `sendfile()`, being forced to unnecessarily switch between user and kernel mode for each send operation.

7. **No HTTP keep-alive**

    HTTP keep-alive is not implemented, causing every request to form a new TCP connection with a full TCP handshake and teardown overhead per request.
