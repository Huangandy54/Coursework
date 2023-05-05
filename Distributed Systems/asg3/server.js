const http = require("http");
const instance = require("./instance");

const port = process.env.PORT || 13800;
console.log("listening on port:", port);

const server = http.createServer(instance);
server.listen(port);
