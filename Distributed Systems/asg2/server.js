const http = require("http");
const httpProxy = require("http-proxy");
const main = require("./main");

const port = process.env.PORT || 13800;
const forwardingAddress = process.env.FORWARDING_ADDRESS;
console.log("listening on port:", port);

if (forwardingAddress) {
    // create follower instance
    console.log("forwarding to port:", forwardingAddress);
    const proxy = httpProxy.createProxyServer({});
    const server = http.createServer((req, res) => {
        setTimeout(() => {
            proxy.web(req, res, {
                target: `http://${forwardingAddress}`
            });
        }, 500);
    });
    server.listen(port);
    proxy.on("error", (err, req, res) => {
        console.log(err);
        const reqType = req.method;
        if (reqType === "GET" || reqType === "PUT" || reqType === "DELETE") {
            res.writeHead(503, {
                'Content-Type': 'application/json'
            });
            const rejectRes = {
                error: "Main instance is down",
                message: `Error in ${reqType}`,
            };
            res.end(JSON.stringify(rejectRes));
        }
    });
} else {
    // create main instance
    const server = http.createServer(main);
    server.listen(port);
}
