docker run --name="node4" --net=kv_subnet            \
           --ip=10.10.0.5  -p 13805:13800            \
           -e ADDRESS="10.10.0.5:13800"                     \
           -e VIEW="10.10.0.2:13800,10.10.0.3:13800,10.10.0.4:13800,10.10.0.5:13800"          \
           -e REPL_FACTOR=2                          \
           kv-store:4.0 