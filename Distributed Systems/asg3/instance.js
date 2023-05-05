const express = require("express");
const app = express();

const kvEndpoint = require("./api/routes/kv-store");

app.use(express.json());
app.use("/kv-store", kvEndpoint);

module.exports = app;
