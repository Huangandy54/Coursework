const express = require("express");
const app = express();

const kvEndpoint = require("./api/routes/kv-store");

app.use(express.json());
app.use((req, res, next) => {
    if ((req.url).includes("view-change") == false) {
        res.setTimeout(5000, () => {
            // callback function is called when request timed out
            res.status(503).json({
                error: "Unable to satisfy request",
                message: `Error in ${req.method}`,
            });
        });
    }

    // pass control to the next middleware function
    next();
});
app.use("/kv-store", kvEndpoint);

module.exports = app;
