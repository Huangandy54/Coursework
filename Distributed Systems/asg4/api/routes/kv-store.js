const express = require("express");
const router = express.Router();
const request = require('request');
const axios = require('axios');

/* Environment Variables */
const address = process.env.ADDRESS;
let view = (process.env.VIEW).split(","); // environment var view is a string... but new views are arrays. sick
let replicationFactor = Number(process.env["REPL_FACTOR"]);
console.log("this address:", address);
console.log("initial view:", view);
console.log("replication factor:", replicationFactor);

/* State Variables */
let numShards;
let shardViews; // 2D array with shardId as indices, value as view for that shard
let myShardId;
let context; // object with shardId as keys, clock for shard as value

// this node's key value store
let shard = {};

/* Util Functions */

// assign nodes in view to store a unique shard, where the index is the shard id
const getShardViews = () => {
    const result = [];
    let start = 0;
    let end = replicationFactor;
    for (let i = 0; i < numShards; i++) {
        result.push(view.slice(start, end));
        start = end;
        end += replicationFactor;
    }
    return result;
}

const getShardId = () => {
    for (let i = 0; i < numShards; i++) {
        const shardView = shardViews[i];
        if (shardView.includes(address)) {
            return i;
        }
    }
    // we should never end here
    return -1;
}

const initializeContext = () => {
    const result = {};
    for (let i = 0; i < numShards; i++) {
        result[i] = 0;
    }
    return result;
}

// initialize our state
numShards = view.length / replicationFactor;
shardViews = getShardViews();
myShardId = getShardId();
context = initializeContext();
console.log("shardViews:", shardViews);
console.log("myShardId:", myShardId);
console.log("context:", context);

const isShardClockNewer = (someContext) => {
    // if someContext is empty
    if(Object.keys(someContext).length == 0) {
        return false;
    }
    return someContext[myShardId] > context[myShardId];
}

// examples: client {}, node {1:1,2:1}
//  client = {1:1,2:2}, node {1:1,2:1}
const max = (a, b) => {
    return a > b ? a : b;
}

const takeContext = (someContext, c) => {
    // if someContext is empty
    if (Object.keys(someContext).length == 0) {
        return c;
    }
    for (let key in someContext) {
        someContext[key] = max(someContext[key], c[key]);
    }
    return someContext;
}


// const broadcastChange = async () => {
//     const shardView = shardViews[myShardId];
//     const body = {
//         shard: shard,
//         context: context,
//     };
//     for(const node of shardView) {
//         if(node != address) {
//             try {
//                 const response = await axios({
//                     method: 'PUT',
//                     url: `http://${node}/kv-store/updateSelf`,
//                     data: body, 
//                 });

//                 if (response.status == 200) {
//                     console.log(node, "has fastforwarded");
//                 }
//             } catch (err) {
//                 console.log(err);
//             }
//         }
//     }
// }

// if we went down and came back on, update ourselves to newest replica
const fastForward = async () => {
    const shardView = shardViews[myShardId];

    let highestClock = context[myShardId];
    let newestNode;
    // go through all replicas and find highest clock
    for(const node of shardView) {
        if(node != address) {
            try {
                const response = await axios({
                    method: 'GET',
                    url: `http://${node}/kv-store/clock/${myShardId}`,
                });

                if (response.status == 200) {
                    const replicaClock = response.data["clock"];
                    console.log("checking clock", replicaClock, "from replica", node);
                    if(replicaClock > highestClock) {
                        highestClock = replicaClock;
                        newestNode = node;
                    }
                }
            } catch (err) {
                console.log("a request failed");
                // console.log(err);
            }
        }
    }

    // copy shard from replica with highest clock
    if(newestNode) {
        try {
            const response = await axios({
                method: 'GET',
                url: `http://${newestNode}/kv-store/iboomer`,
            });

            if (response.status == 200) {
                const replicaShard = response.data["shard"];
                shard = replicaShard;
                context[myShardId] = highestClock;
                console.log("fast forwarding our shard to replica:", replicaShard);
                console.log("context after fast forawrd:", context);
            }
        } catch (err) {
            console.log("a request failed");
            // console.log(err);
        }
    }
    return Promise.resolve("done");
}

const hash = (key) => {
    for (var i = 0, hash = 0; i < key.length; i++)
        hash = Math.imul(31, hash) + key.charCodeAt(i) | 0;
    return Math.abs(hash);
}

const updateView = async () => {
    for (key in shard) {
        const shardIndex = hash(key) % numShards;
        // reshard to 'primary' replica
        const node = shardViews[shardIndex][0];

        // if key should be moved
        if (myShardId != shardIndex) {
            const body = {
                value: shard[key],
                "causal-context": context,
            };
            console.log("moving this key:", key);
            console.log("destination: ", node);
            try {
                const response = await axios({
                    method: 'PUT',
                    url: `http://${node}/kv-store/keys/${key}`,
                    data: body,
                });
                // if we moved the key successfully, delete from this shard
                if (response.status == 200 || response.status == 201) {
                    // key doesnt belong to our shard anymore so delete
                    context[myShardId]++;
                    context = takeContext(response.data["causal-context"], context);
                    console.log("deleting key:", key);
                    delete shard[key];
                }
            } catch (err) {
                console.log("a request failed");
                // console.log(err);
            }
        }
    }

    console.log("reshard done:", shard);
    return Promise.resolve("done");
};

/* Key-Value Operations */

router.get("/keys/:key", async (req, res) => {
    const key = req.params.key;
    const shardIndex = hash(key) % numShards;
    const shardView = shardViews[shardIndex];

    if (myShardId == shardIndex) {
        // this is our shard, if we are outdated try to fast forward
        const clientContext = req.body["causal-context"];
        if(isShardClockNewer(clientContext)) {
            console.log("we are behind client, fast forwarding...");
            await fastForward();
        }
        
        // if we have same clock for this shard as client
        // we can serve client and be causally consistent
        if (isShardClockNewer(clientContext) == false) {
            context = takeContext(clientContext, context);
            if (key in shard) {
                res.status(200).json({
                    doesExist: true,
                    message: "Retrieved successfully",
                    value: shard[key],
                    "causal-context": context,
                });
            } else {
                res.status(404).json({
                    doesExist: false,
                    error: "Key does not exist",
                    message: "Error in GET",
                    "causal-context": context,
                });
            }
        }
    } else {
        // not our shard
        for (const node of shardView) {
            console.log("not our shard, forwarding GET request to: ", node);
            try {
                const response = await axios({
                    method: 'GET',
                    url: `http://${node}/kv-store/keys/${key}`,
                    data: req.body,
                });

                if (response.status == 200 || response.status == 404) {
                    response.data["address"] = node;
                    res.status(response.status).json(response.data);
                    // successful forward so we are done
                    return;
                }
            } catch (err) {
                // this forwarding failed, move on
                console.log("a request failed");
                // console.log(err);
            }
        }
    }
});

router.put("/keys/:key", async (req, res) => {
    const key = req.params.key;

    if (key.length > 50) {
        res.status(400).json({
            error: "Key is too long",
            message: "Error in PUT",
        });
    }
    
    const clientContext = req.body["causal-context"];
    // if valid request
    if (req.body && req.body.value) {
        const shardIndex = hash(key) % numShards;
        const shardView = shardViews[shardIndex];

        if (myShardId == shardIndex) {
            // this is our shard, if we are outdated try to fast forward
            if(isShardClockNewer(clientContext)) {
                console.log("we are behind client, fast forwarding...");
                await fastForward();
            }
            
            const value = req.body.value;
            // if we have same clock for this shard as client
            // we can serve client and be causally consistent
            if(isShardClockNewer(clientContext) == false) {
                // increment our clock for this shard
                context[shardIndex]++;
                context = takeContext(clientContext, context);
                if (key in shard) {
                    shard[key] = value;
                    console.log("shard after put:", shard);
                    res.status(200).json({
                        message: "Updated successfully",
                        replaced: true,
                        "causal-context": context,
                    });
                } else {
                    shard[key] = value;
                    console.log("shard after put:", shard);
                    res.status(201).json({
                        message: "Added successfully",
                        replaced: false,
                        "causal-context": context,
                    });
                }
                // propagate to other nodes that have the same shard
                const replicateBody = req.body;
                replicateBody["causal-context"] = context;
                for (const node of shardView) {
                    if (node != address) {
                        console.log("replicating PUT", key, "change to replica: ", node);
                        request.put(`http://${node}/kv-store/replicate/${key}`, {
                            json: replicateBody
                        }, (err, proxyRes) => {
                            if (err) {
                                console.log("a request failed");
                                // console.log(err);
                            }
                        });
                    }
                }
            }
        } else {
            // not our shard
            for (const node of shardView) {
                console.log("not our shard, forwarding PUT request to: ", node);
                try {
                    const response = await axios({
                        method: 'PUT',
                        url: `http://${node}/kv-store/keys/${key}`,
                        data: req.body,
                    });

                    if (response.status == 200 || response.status == 201) {
                        response.data["address"] = node;
                        res.status(response.status).json(response.data);
                        // successful forward so we are done
                        return;
                    }
                } catch (err) {
                    // this forwarding failed, move on
                    console.log("a request failed");
                    // console.log(err);
                }
                
            }
        }
    } else {
        res.status(400).json({
            error: "Value is missing",
            message: "Error in PUT",
            "causal-context": clientContext,
        });
    }
});


router.delete("/keys/:key", async (req, res) => {
    const key = req.params.key;
    const shardIndex = hash(key) % numShards;
    const shardView = shardViews[shardIndex];

    if (myShardId == shardIndex) {
        // this is our shard, if we are outdated try to fast forward
        const clientContext = req.body["causal-context"];
        if(isShardClockNewer(clientContext)) {
            console.log("we are behind client, fast forwarding...");
            await fastForward();
        }
        
        // if we have same clock for this shard as client
        // we can serve client and be causally consistent
        if (isShardClockNewer(clientContext) == false) {
            context = takeContext(clientContext, context);            
            if (key in shard) {
                // increment our clock for this shard
                context[shardIndex]++;
                delete shard[key];
                console.log("shard after a delete:", shard);
                res.status(200).json({
                    doesExist: true,
                    message: "Deleted successfully",
                    "causal-context": context,
                });

                // propagate to other nodes that have the same shard
                const replicateBody = req.body;
                replicateBody["causal-context"] = context;
                replicateBody["delete"] = true;
                for (const node of shardView) {
                    if (node != address) {
                        console.log("replicating DELETE", key, "change to replica: ", node);
                        request.put(`http://${node}/kv-store/replicate/${key}`, {
                            json: replicateBody
                        }, (err, proxyRes) => {
                            if (err) {
                                console.log("a request failed");
                                // console.log(err);
                            }
                        });
                    }
                }
            } else {
                res.status(404).json({
                    doesExist: false,
                    error: "Key does not exist",
                    message: "Error in DELETE",
                    "causal-context": context,
                });
            }
        }
    } else {
        // not our shard
        for (const node of shardView) {
            console.log("not our shard, forwarding DELETE request to: ", node);
            try {
                const response = await axios({
                    method: 'DELETE',
                    url: `http://${node}/kv-store/keys/${key}`,
                    data: req.body,
                });

                if (response.status == 200 || response.status == 404) {
                    response.data["address"] = node;
                    res.status(response.status).json(response.data);
                    // successful forward so we are done
                    return;
                }
            } catch (err) {
                // this forwarding failed, move on
                console.log("a request failed");
                // console.log(err);
            }
        }
    }
});

/* Administrative Operations */

router.get("/key-count", async (req, res) => {
    const clientContext = req.body["causal-context"];
    if(isShardClockNewer(clientContext)) {
        console.log("we are behind client, fast forwarding...");
        await fastForward();
    }
    // if we have same clock for this shard as client
    // we can serve client and be causally consistent
    if(isShardClockNewer(clientContext) == false) {
        context = takeContext(clientContext, context);
        res.status(200).json({
            message: "Key count retrieved successfully",
            "key-count": Object.keys(shard).length,
            "shard-id": myShardId,
            "causal-context": context,
        });
    }
});

router.get("/shards", async (req, res) => {
    const clientContext = req.body["causal-context"];
    if(isShardClockNewer(clientContext)) {
        console.log("we are behind client, fast forwarding...");
        await fastForward();
    }
    // if we have same clock for this shard as client
    // we can serve client and be causally consistent
    if(isShardClockNewer(clientContext) == false) {
        context = takeContext(clientContext, context);
        res.status(200).json({
            message: "Shard membership retrieved successfully",
            "causal-context": context,
            shards: Object.keys(context),
        });
    }
});

router.get("/shards/:id", async (req, res) => {
    const shardId = req.params.id;

    if (shardId == myShardId) {
        // this is our shard
        const clientContext = req.body["causal-context"];
        if(isShardClockNewer(clientContext)) {
            console.log("we are behind client, fast forwarding...");
            await fastForward();
        }
        // if we have same clock for this shard as client
        // we can serve client and be causally consistent
        if(isShardClockNewer(clientContext) == false) {
            context = takeContext(clientContext, context);
            res.status(200).json({
                message: "Shard information retrieved successfully",
                "shard-id": shardId,
                "key-count": Object.keys(shard).length,
                "causal-context": context,
                replicas: shardViews[shardId],
            });
        }
    } else {
        // not our shard
        const shardView = shardViews[shardId];
        for (const node of shardView) {
            console.log("not our shard, forwarding GET shard id request to: ", node);
            try {
                const response = await axios({
                    method: 'GET',
                    url: `http://${node}/kv-store/shards/${shardId}`,
                    data: req.body,
                });

                if (response.status == 200) {
                    console.log("body: ",response.data);
                    res.status(response.status).json(response.data);
                    // successful forward so we are done
                    return;
                }
            } catch (err) {
                // this forwarding failed, move on
                console.log("a request failed");
                // console.log(err);
            }
        }
    }
});

router.put("/view-change", (req, res) => {
    if (req.body && req.body.view) {
        // update each node in view
        Promise.all(view.map(async node => {
            try {
                const response = await axios({
                    method: 'PUT',
                    url: `http://${node}/kv-store/update-view`,
                    data: req.body,
                });
                // if we updated view successfully
                if (response.status == 200) {
                    console.log(node, "view has been updated");
                }
            } catch (err) {
                console.log("a request failed");
                // console.log(err);
            }
        })).then(() => {
            console.log("everyone got new view");
            Promise.all(shardViews.map(async shardView => {
                // tell 'primary' replica of a shard to reshard
                const node = shardView[0];
                try {
                    const response = await axios({
                        method: 'PUT',
                        url: `http://${node}/kv-store/reshard`,
                    });
                    // if we updated view successfully
                    if (response.status == 200) {
                        console.log(node, "is resharding");
                    }
                } catch (err) {
                    console.log("a request failed");
                    // console.log(err);
                }
            })).then(() => {
                console.log("all primary nodes have resharded");
                const body = {
                    "causal-context": context,
                };
                Promise.all(shardViews.map(async (shardView, index) => {
                    // for each shardId, request key count from 'primary' node
                    const node = shardView[0];
                    try {
                        const response = await axios({
                            method: 'GET',
                            url: `http://${node}/kv-store/key-count`,
                            data: body,
                        });
    
                        console.log("retrieved key count: ", response.data["key-count"]);
                        context = takeContext(response.data["causal-context"], context);
                        return {
                            "shard-id": index,
                            "key-count": response.data["key-count"],
                            "replicas": shardView,
                        };
                    } catch (err) {
                        console.log("a request failed");
                        // console.log(err);
                    }
                })).then(shards => {
                    res.status(200).json({
                        message: "View change successful",
                        "causal-context": context,
                        shards: shards,
                    });
                    console.log("context after view change", context);
                });
            })
        });
    } else {
        res.status(400).json({
            error: "View is missing",
            message: "Error in PUT",
            "causal-context": context,
        });
    }
});

/* Additional Endpoints */

router.put("/update-view", (req, res) => {
    // const newView = (req.body.view).split(",");
    const newView = req.body.view; // new view is an array now, last assignment was string lol sick
    const newReplFactor = req.body["repl-factor"];    
    // set new view and replication factor
    view = newView;
    replicationFactor = newReplFactor;
    numShards = view.length / replicationFactor;
    shardViews = getShardViews();
    myShardId = getShardId();
    context = initializeContext();
    console.log("shardViews:", shardViews);
    console.log("myShardId:", myShardId);
    console.log("context:", context);
    res.sendStatus(200);
});

router.put("/reshard", (req, res) => {
    console.log("we are primary... resharding");
    updateView().then(() => {
        res.sendStatus(200);
        console.log("reshard finished");
    });
})

// replicate a key value on this node
router.put("/replicate/:key", (req, res) => {
    const key = req.params.key;
    if("delete" in req.body) {
        delete shard[key];
    } else if("value" in req.body) {
        shard[key] = req.body.value;
    }
    context[myShardId]++;
    // console.log("shard after replicate:", shard);
    res.sendStatus(200);
});

// get context and shard from node (for when a node needs to fast forward to another replica)
router.get("/iboomer", (req, res) => {
    res.status(200).json({
        shard: shard,
    });
});

// get clock for a specific shardId
router.get("/clock/:id", (req, res) => {
    const shardId = req.params.id;
    res.status(200).json({
        "clock": context[shardId],
    });
})

// router.put("/updateSelf", (req, res) => {
//     const newShard = req.body.shard;
//     const latestContext = req.body.context;
//     shard = newShard;
//     context[myShardId] = latestContext[myShardId]; 
//     console.log("received broadcast from another replica");
//     console.log("shard is now", shard);
//     console.log("context is now", context);
//     res.sendStatus(200);
// })

module.exports = router;