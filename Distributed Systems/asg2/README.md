# CSE 138 Assignment 2

## Authors

- **Andy Huang** (ahuang44)
- **Kevin Lim** (kclim)
- **Robert Loquinario** (rloquina)

## Endpoints

`/kv-store/:key` will accept GET, PUT, and DELETE requests with JSON content type. The response will be in JSON format and return a status code as appropriate.

## Multi-site coordination

The key-value store may be started as either: a **main** instance or a **follower** instance depending on whether the environment variable, `FORWARDING_ADDRESS` exists. The main instance will respond directly to requests. The follower instance forwards requests to the main instance, then forwards the response to the client.

## Docker Network Management

In the following, we explain a scenario where we have one main instance, one follower instance, and a Docker subnet named **kv_subnet**.

Create subnet, **kv_subnet**, with IP range `10.10.0.0/16`:

```bash
docker network create --subnet=10.10.0.0/16 kv_subnet
```

Build Docker image containing the key-value store implementation

```bash
docker build -t kv-store:2.0 .
```

Run the main instance at `10.10.0.2:13800` in a Docker container named main-instance:

```bash
docker run -p 13800:13800 --net=kv_subnet --ip=10.10.0.2 --name="main-instance" kv-store:2.0
```

Run the follower instance at `10.10.0.3:13800` in a Docker container named follower-instance,
which will forward requests to the main instance:

```bash
docker run -p 13801:13800 \
           --net=kv_subnet \
           --ip=10.10.0.3 \
           --name="follower-instance" \
           -e FORWARDING_ADDRESS="10.10.0.2:13800" \
           kv-store:2.0
```

Stop and remove containers:

```bash
docker stop main-instance
docker stop follower-instance
docker rm main-instance
docker rm follower-instance
```
