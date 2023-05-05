# CSE 138 Assignment 3

## Authors

- **Andy Huang** (ahuang44)
- **Kevin Lim** (kclim)
- **Robert Loquinario** (rloquina)
- **Gregory Quach** (gvquach)

## Goals

Develop a distributed key-value store that stores data (key-value pairs) on
more than one machine. In this assignment, we will call the partitions that we
separate keys into as shards, where the keys in a particular shard are stored on a particular key-value store
instance, or node.

## Endpoints

`/kv-store/keys/<key>` GET, PUT, DELETE

`/kv-store/key-count` GET

`/kv-store/view-change` PUT

`/kv-store/update-view` PUT

## Design

So how we went about this was once a instance of a node is created a shard and the view is initialized and persisted in memory. Once a client sends a request to a node, the node will check to see if it contains the key and process the request, otherwise it will forward to a shard that should have the key. In the case that the `view-change` endpoint is reached, the node first contacted will assume itself to be the main instance, `reshard()` and target the other nodes `update-view` endpoint.
