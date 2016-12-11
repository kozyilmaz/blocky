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
