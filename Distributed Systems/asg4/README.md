# CSE 138 Assignment 4

## Authors

- **Andy Huang** (ahuang44)
- **Kevin Lim** (kclim)
- **Robert Loquinario** (rloquina)
- **Gregory Quach** (gvquach)

## Goals

Develop a distributed key-value store that is partition-tolerant, available, and causally consistent. Sharding key-value pairs achieves scalability, but the key-value store must replicate (make identical copies) each shard to provide redundancy, and thus be partition-tolerant.

## Endpoints

`/kv-store/keys/<key>` GET, PUT

`/kv-store/key-count` GET

`/kv-store/shards` GET

`/kv-store/shards/<id>` GET

`/kv-store/view-change` PUT

`/kv-store/keys/<key>` DELETE (for extra credit)

## Design

