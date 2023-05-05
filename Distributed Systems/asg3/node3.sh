docker run --name="node3" --net=kv_subnet            \
           --ip=10.10.0.4  -p 13804:13800            \
           -e ADDRESS="10.10.0.4:13800"                     \
           -e VIEW="10.10.0.2:13800,10.10.0.3:13800,10.10.0.4:13800"                    \
           kv-store:3.0