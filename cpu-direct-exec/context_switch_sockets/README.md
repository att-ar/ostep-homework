## Checking the activity of the sockets

Get the PID

```shell
ps auxf | grep server

ps auxf | grep client
```

Get the open file descriptors

```shell
lsof -p <PID>
```

Examine the process syscalls

```shell
dtruss -p <PID> -d
```

### Client

```shell
ps auxf | grep client
```
