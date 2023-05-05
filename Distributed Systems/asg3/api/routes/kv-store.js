const express = require("express");
const router = express.Router();
const request = require('request');
const axios = require('axios');

const address = process.env.ADDRESS;
const view = (process.env.VIEW).split(",");
console.log("this address:", address);
console.log("initial view:", view);

const shard = {};

/* UTIL FUNCTIONS */
const hash = (key) => {
    for (var i = 0, hash = 0; i < key.length; i++)
        hash = Math.imul(31, hash) + key.charCodeAt(i) | 0;
    return Math.abs(hash);
}

const updateView = async (newView) => {
    // append new nodes to view
    for (const node of newView) {
        if (view.indexOf(node) === -1) {
            view.push(node);
        }
    }
    console.log("new view:", view);
    console.log("starting reshard...");
    
    for (key in shard) {
        const nodeIndex = hash(key) % view.length;
        const node = view[nodeIndex];

        // if key should be moved
        if (node != address) {
            const body = {
                value: shard[key]
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
                if(response.status == 200||response.status==201) {
                    console.log("deleting key:", key);
                    delete shard[key];
                }
            } catch (err) {
                console.log(err);
            }
        }
    }

    console.log("reshard:", shard);
    return Promise.resolve("done");
};

/* ENDPOINTS */
router.get("/keys/:key", (req, res) => {
    const key = req.params.key;
    const nodeIndex = hash(key) % view.length;
    const node = view[nodeIndex];

    if (node == address) {
        if (key in shard) {
            res.status(200).json({
                doesExist: true,
                message: "Retrieved successfully",
                value: shard[key],
            });
        } else {
            res.status(404).json({
                doesExist: false,
                error: "Key does not exist",
                message: "Error in GET",
            });
        }
    } else {
        console.log("forwarding request to:", node);
        request.get(`http://${node}/kv-store/keys/${key}`, (err, proxyRes) => {
            if (err) {
                console.log(err);
            }
            const body = JSON.parse(proxyRes.body);
            body.address = node;
            res.status(proxyRes.statusCode).json(body);
        });
    }

});

router.put("/keys/:key", (req, res) => {
    const key = req.params.key;

    if (key.length > 50) {
        res.status(400).json({
            error: "Key is too long",
            message: "Error in PUT",
        });
    }

    if (req.body && req.body.value) {
        const value = req.body.value;
        const nodeIndex = hash(key) % view.length;
        const node = view[nodeIndex];

        if (node == address) {
            if (key in shard) {
                shard[key] = value;
                console.log("shard after put:", shard);
                res.status(200).json({
                    message: "Updated successfully",
                    replaced: true,
                });
            } else {
                shard[key] = value;
                console.log("shard after put:", shard);
                res.status(201).json({
                    message: "Added successfully",
                    replaced: false,
                });
            }
        } else {
            console.log("forwarding request to:", node);
            request.put(`http://${node}/kv-store/keys/${key}`, {
                json: req.body
            }, (err, proxyRes) => {
                if (err) {
                    console.log(err);
                }
                // note that proxyRes is an object type here instead of string
                const body = proxyRes.body;
                body.address = node;
                res.status(proxyRes.statusCode).json(body);
            });
        }
    } else {
        res.status(400).json({
            error: "Value is missing",
            message: "Error in PUT",
        });
    }
});

router.delete("/keys/:key", (req, res) => {
    const key = req.params.key;
    const nodeIndex = hash(key) % view.length;
    const node = view[nodeIndex];

    if (node == address) {
        if (key in shard) {
            delete shard[key];
            console.log("shard after a delete:", shard);
            res.status(200).json({
                doesExist: true,
                message: "Deleted successfully",
            });
        } else {
            res.status(404).json({
                doesExist: false,
                error: "Key does not exist",
                message: "Error in DELETE",
            });
        }
    } else {
        console.log("forwarding request to:", node);
        request.delete(`http://${node}/kv-store/keys/${key}`, (err, proxyRes) => {
            if (err) {
                console.log(err);
            }
            const body = JSON.parse(proxyRes.body);
            body.address = node;
            res.status(proxyRes.statusCode).json(body);
        });
    }

});

router.get("/key-count", (req, res) => {
    res.status(200).json({
        message: "Key count retrieved successfully",
        "key-count": Object.keys(shard).length,
    });
});

router.put("/view-change", (req, res) => {
    if (req.body && req.body.view) {
        Promise.all(view.map(async node => {
            try {
                const response = await axios({
                    method: 'PUT',
                    data: req.body,
                    url: `http://${node}/kv-store/update-view`,
                });
                // if we updated view successfully
                if(response.status == 200) {
                    console.log(node, "view has been updated");
                }
            } catch (err) {
                console.log(err);
            }
        })).then(() => {
            console.log("everyone got new view");
            Promise.all(view.map(async node => {
                try {
                    const response = await axios({
                        method: 'GET',
                        url: `http://${node}/kv-store/key-count`,
                    });

                    console.log("retrieved key count: ", response.data["key-count"]);
                    return {
                        address: node,
                        "key-count": response.data["key-count"],
                    };
                } catch (err) {
                    console.log(err);
                }
            })).then(shards => {
                res.status(200).json({
                    message: "View change successful",
                    shards: shards,
                });
            });
        });
    } else {
        res.status(400).json({
            error: "View is missing",
            message: "Error in PUT",
        });
    }
});

/* ADDITIONAL ENDPOINTS */
router.put("/update-view", (req, res) => {
    const newView = (req.body.view).split(",");
    console.log("updating view");
    updateView(newView).then(() => {
    	console.log("updated view");
        res.status(200).json({
        	message: "Update view successful",
        });
    });
});

module.exports = router;