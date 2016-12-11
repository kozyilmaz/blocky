# Bridge

## Deploying Bridge Contract
Import contract
```javascript
> personal.unlockAccount(eth.coinbase, YOUR_PASSWORD)
> loadScript('bridge.js');
```

Deploy to blockchain
```javascript
> miner.start()
...
Contract mined! address: 0x3b26e8bd43effdcc0ce824c6bc29d40bac3b5aad transactionHash: 0x94075470292dfe99dfe933ba3e330a049630cf5f767c9afb46632ed4090f4cad
...
> miner.stop()
```

Check contract status
```javascript
> bridge
{
  abi: [{
      constant: true,
      inputs: [],
      name: "creator",
      outputs: [{...}],
      payable: false,
      type: "function"
  }, {
      constant: false,
      inputs: [{...}],
      name: "request",
      outputs: [],
      payable: false,
      type: "function"
  }, {
      constant: false,
      inputs: [],
      name: "kill",
      outputs: [],
      payable: false,
      type: "function"
  }, {
      constant: false,
      inputs: [{...}, {...}],
      name: "activate",
      outputs: [],
      payable: false,
      type: "function"
  }, {
      inputs: [],
      payable: false,
      type: "constructor"
  }, {
      anonymous: false,
      inputs: [{...}, {...}],
      name: "Notify",
      type: "event"
  }, {
      anonymous: false,
      inputs: [{...}, {...}, {...}],
      name: "Process",
      type: "event"
  }],
  address: "0x3b26e8bd43effdcc0ce824c6bc29d40bac3b5aad",
  transactionHash: "0x94075470292dfe99dfe933ba3e330a049630cf5f767c9afb46632ed4090f4cad",
  Notify: function(),
  Process: function(),
  activate: function(),
  allEvents: function(),
  creator: function(),
  kill: function(),
  request: function()
}
```

Export contract on another node
```javascript
> var bridge_on_blockchain = eth.contract([{ constant: true, inputs: [], name: "creator", outputs: [{ name: "", type: "address" }], payable: false, type: "function" }, { constant: false, inputs: [{ name: "data", type: "string" }], name: "request", outputs: [], payable: false, type: "function" }, { constant: false, inputs: [], name: "kill", outputs: [], payable: false, type: "function" }, { constant: false, inputs: [{ name: "gateway", type: "address" }, { name: "data", type: "string" }], name: "activate", outputs: [], payable: false, type: "function" }, { inputs: [], payable: false, type: "constructor" }, { anonymous: false, inputs: [{ indexed: true, name: "from", type: "address" }, { indexed: false, name: "message", type: "string" }], name: "Notify", type: "event" }, { anonymous: false, inputs: [{ indexed: true, name: "from", type: "address" }, { indexed: true, name: "to", type: "address" }, { indexed: false, name: "message", type: "string" }], name: "Process", type: "event" }] ).at("0x3b26e8bd43effdcc0ce824c6bc29d40bac3b5aad");
```

## Preparing Nodes
IoT vendor starts watching for Notify() events (i.e IoT gateway message)
```javascript
var event_notify = bridge.Notify();
event_notify.watch(function(error, result){
    if (!error) {
        console.log("[0xdeadbeefb00b1e56] MSG[" + result.args.message + "] FROM[" + result.args.from + "]");
    }
});
```

IoT Gateway starts watching for Process() events (i.e. IoT vendor messages)
```javascript
var event_process = bridge.Process();
event_process.watch(function(error, result){
    if (!error) {
        console.log("[0x0123456789abcdef] MSG[" + result.args.message + "] FROM[" + result.args.from + "] TO[" + result.args.to + "]");
    }
});
```

## Message Flow
IoT Gateway sends a message to IoT vendor via blockchain (either activation request or data)
```javascript
> personal.unlockAccount(eth.coinbase, YOUR_PASSWORD)
> bridge_on_blockchain.request('myjsonstring',{from: eth.coinbase})
```

IoT vendor watch notified with gateway message
```javascript
> I1212 00:46:18.675648 core/blockchain.go:1047] imported 1 blocks,     1 txs (  0.025 Mg) in  10.235ms ( 2.454 Mg/s). #407 [08be0754…]
[0xdeadbeefb00b1e56] MSG[myjsonstring] FROM[0xcb2a95f964acf8adee7fae30cf5dc6a3f5e14a5c]
```

IoT vendor replies back to gateway with activate JSON
```javascript
> personal.unlockAccount(eth.coinbase, YOUR_PASSWORD)
> bridge.activate('0xcb2a95f964acf8adee7fae30cf5dc6a3f5e14a5c', 'myactivatejson', {from: eth.coinbase})
```

IoT gateway watch notified with vendor message
```javascript
> I1212 01:01:39.107765 core/blockchain.go:1047] imported 1 blocks,     1 txs (  0.027 Mg) in   5.203ms ( 5.210 Mg/s). #415 [e77abb96…]
[0x0123456789abcdef] MSG[myactivatejson] FROM[0xdcd88ddb31d688b6ae73048969c4ab2e587c0087] TO[0xcb2a95f964acf8adee7fae30cf5dc6a3f5e14a5c]
```

