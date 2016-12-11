# Sample
Ethereum based smart contract samples

## Coin
To deploy
```javascript
> personal.unlockAccount(eth.coinbase, YOUR_PASSWORD)
true
> loadScript('coin.js');
I1211 18:37:50.946285 internal/ethapi/api.go:1045] Tx(0xc9b646bea27db7d87017c97dfe4300fb8d06932474855c252161807b9517c487) created: 0x1d6404c68e0f3574eff9437136a6dcd0b443cc15
null [object Object]
true
>
```

To export
```javascript
> var coin = eth.contract([{ constant: true, inputs: [], name: "minter", outputs: [{ name: "", type: "address" }], payable: false, type: "function" }, { constant: true, inputs: [{ name: "", type: "address" }], name: "balances", outputs: [{ name: "", type: "uint256" }], payable: false, type: "function" }, { constant: false, inputs: [{ name: "receiver", type: "address" }, { name: "amount", type: "uint256" }], name: "mint", outputs: [], payable: false, type: "function" }, { constant: false, inputs: [{ name: "receiver", type: "address" }, { name: "amount", type: "uint256" }], name: "send", outputs: [], payable: false, type: "function" }, { inputs: [], payable: false, type: "constructor" }, { anonymous: false, inputs: [{ indexed: false, name: "from", type: "address" }, { indexed: false, name: "to", type: "address" }, { indexed: false, name: "amount", type: "uint256" }], name: "Sent", type: "event" }]).at("0x1d6404c68e0f3574eff9437136a6dcd0b443cc15");
```

To run







var bridge_00 = eth.contract([{ constant: false, inputs: [{ name: "data", type: "string" }], name: "request", outputs: [], payable: false, type: "function" }, { constant: false, inputs: [], name: "kill", outputs: [], payable: false, type: "function" }, { constant: false, inputs: [{ name: "gateway_address", type: "address" }, { name: "data", type: "string" }], name: "activate", outputs: [], payable: false, type: "function" }, { inputs: [], payable: false, type: "constructor" }, { anonymous: false, inputs: [{ indexed: true, name: "_from", type: "address" }, { indexed: false, name: "_value", type: "string" }], name: "Notify", type: "event" }] ).at("0x166358685f493bb5fb3eb3bd7cdc7576e8d8230a");




## Bridge
To deploy
```javascript
> personal.unlockAccount(eth.coinbase, YOUR_PASSWORD)
true
> loadScript('bridge.js');
I1212 00:09:54.484559 internal/ethapi/api.go:1045] Tx(0x94075470292dfe99dfe933ba3e330a049630cf5f767c9afb46632ed4090f4cad) created: 0x3b26e8bd43effdcc0ce824c6bc29d40bac3b5aad
null [object Object]
true
```

```
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
