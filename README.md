# httpd
HTTP server in C

## building
```
mkdir build
cmake ..
make
```

## running
usage is similar for each executable produced.
`httpd <config_file>`

- httpd: the default, linear version. 
- httpd-fork: clients are served by a forked process
- httpd-thread: clients are served by a process thread
- httpd-select: linear but with non-blocking io

