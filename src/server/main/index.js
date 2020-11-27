const aedes = require("aedes")();
const server = require("net").createServer(aedes.handle);
const emitter = require("mqemitter")();
const port = process.env.PORT;

server.listen(port, function () {
  console.log("Aedes listening on port:", port);
  aedes.publish({ topic: "aedes/hello", payload: "I'm broker " + aedes.id });
});

aedes.on("subscribe", function (subscriptions, client) {
  console.log(
    "MQTT client \x1b[32m" +
      (client ? client.id : client) +
      "\x1b[0m subscribed to topics: " +
      subscriptions.map((s) => s.topic).join(", "),
    "from broker",
    aedes.id
  );
  subscriptions.forEach((topic) => {
    if (topic.topic == "getTime") {
      aedes.publish({
        topic: "getTime",
        payload: `${Math.floor(new Date().getTime() / 1000)}`,
      });
    }
  });
});

aedes.on("unsubscribe", function (subscriptions, client) {
  console.log(
    "MQTT client \x1b[32m" +
      (client ? client.id : client) +
      "\x1b[0m unsubscribed to topics: " +
      subscriptions.join("\n"),
    "from broker",
    aedes.id
  );
});

// fired when a client connects
aedes.on("client", function (client) {
  console.log(
    "Client Connected: \x1b[33m" + (client ? client.id : client) + "\x1b[0m",
    "to broker",
    aedes.id
  );
});

// fired when a client disconnects
aedes.on("clientDisconnect", function (client) {
  console.log(
    "Client Disconnected: \x1b[31m" + (client ? client.id : client) + "\x1b[0m",
    "to broker",
    aedes.id
  );
});

// fired when a message is published
aedes.on("publish", async function (packet, client) {
  // if (packet.payload != aedes.id) {
    console.log(
      "Client \x1b[33m" +
        (client ? client.id : "BROKER_" + aedes.id) +
        "\x1b[0m has published",
      packet.payload.toString(),
      "on",
      packet.topic,
      "to broker",
      aedes.id
    );
  if (packet.topic == "sendData") {
    console.log(packet.payload.toString().toJSON());
  }
  // }
});
