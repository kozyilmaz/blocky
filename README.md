# Blocky
Ethereum based smart contracts for IoT devices
[create an anchor](## Install Dependencies)

## Install Dependencies

### Install Go
Download precompiled Go binaries from [official project site](https://golang.org/dl)

For x86_64 Linux
```
$ curl -O https://storage.googleapis.com/golang/go1.7.4.linux-amd64.tar.gz
```
For Raspberry Pi Linux
```
$ curl -O https://storage.googleapis.com/golang/go1.7.4.linux-armv6l.tar.gz
```
Install Go binaries
```
$ sudo tar -C /usr/local -xzf go$VERSION.$OS-$ARCH.tar.gz
```
Add path to /etc/profile (for a system-wide installation) or $HOME/.profile
```javascript
$ export PATH=$PATH:/usr/local/go/bin
```

### Install Ethereum Client
```javascript
$ git clone https://github.com/ethereum/go-ethereum.git
$ cd go-ethereum
$ make geth
$ sudo cp build/bin/geth /usr/local/bin/geth
```


## Create Private Ethereum Testnet

### Setup Node Zero
Create a new account for private net
```javascript
$ geth --datadir "~/private-iot-chain" account new
```
Note account number and pre-allocate ether for the new account in genesis.json (i.e. change account address in "alloc" accordingly)
```javascript
{
    "nonce": "0x0000000000000042",
    "timestamp": "0x0",
    "parentHash": "0x0000000000000000000000000000000000000000000000000000000000000000",
    "extraData": "0x0",
    "gasLimit": "0x8000000",
    "difficulty": "0x400",
    "mixhash": "0x0000000000000000000000000000000000000000000000000000000000000000",
    "coinbase": "0x3333333333333333333333333333333333333333",
    "alloc": {
        "0x80d4dddffd9d1292f16045166aecdaf34b82a0fb": {
            "balance": "20000000000000000000"
        }
    }
}
```
Create a private chain with the custom genesis block
```javascript
$ geth --datadir "~/private-iot-chain" init genesis.json
```
Launch geth for private net
```javascript
$ geth --rpc --rpcport "8000" --rpccorsdomain "*" --datadir "~/private-iot-chain" --port "30303" --nodiscover --ipcapi "admin,db,eth,debug,miner,net,shh,txpool,personal,web3" --rpcapi "db,eth,net,web3" --autodag --identity "zero" --networkid 666 console
```

Run ```eth.getBalance(eth.coinbase)``` command to check the account balance, pre-allocated sum should be there  
Run ```admin.nodeInfo``` to get enode url (```enode://xxxxx```) and add ip address of the interface ```[::]``` to construct the complete enode address to share with other peers
```javascript
"enode://6ad5934db83a0266c4c6d5048d02f86b3e69251d45ad411387cde9cc5a86030f2bee4bcbe200d4238d91b01c94444e562986058c9c4acca2a92cb81eb012acfc@192.168.2.41:30303?discport=0"
```

### Setup Node One

Setup second one which is a second computer on our net
```javascript
$ geth --datadir "~/private-iot-chain" account new
$ geth --datadir "~/private-iot-chain" init genesis.json
$ geth --datadir "~/private-iot-chain" --port "30303" --nodiscover --identity "one" --networkid 666 console
```

Run ```admin.addPeer``` to connect node zero
```javascript
> admin.addPeer("enode://6ad5934db83a0266c4c6d5048d02f86b3e69251d45ad411387cde9cc5a86030f2bee4bcbe200d4238d91b01c94444e562986058c9c4acca2a92cb81eb012acfc@192.168.2.41:30303")
```

Send ether from node zero to one using one's wallet address. Execute on node zero, do not forget mine transaction
```javascript
> personal.unlockAccount(eth.coinbase)
> eth.sendTransaction({from:eth.coinbase, to: '0xcb2a95f964acf8adee7fae30cf5dc6a3f5e14a5c', value: web3.toWei(.000000000001, "ether")})
> miner.start()
> miner.stop()
```

###### Ethereum
* [Creating a private chain/testnet](https://souptacular.gitbooks.io/ethereum-tutorials-and-tips-by-hudson/content/private-chain.html)  
* [Setting up an Ethereum testnet](http://billmarino2.github.io/general/2015/12/09/JURIX-2015-setting-up-an-ethereum-testnet.html)  
* [Setting up private network or local cluster](https://github.com/ethereum/go-ethereum/wiki/Setting-up-private-network-or-local-cluster)



## Create Smart Contracts

### Install Solidity Compiler
To develop smart contracts Solidity compiler is needed. Either [Solidity online compiler](https://ethereum.github.io/browser-solidity) or internal/offline compiler should be used. Console command ```eth.getCompilers()``` will list available offline compilers, if solidity is not present, build commands are as follows:
```javascript
$ git clone --recursive https://github.com/ethereum/solidity.git
$ cd solidity
$ git checkout tags/<tag_name>
$ ./scripts/install_deps.sh
$ mkdir build
$ cd build
$ cmake .. && make
```

### Create and Deploy Smart Contracts
Below is a sample smart contract which get/set a variable.
```javascript
pragma solidity ^0.4.0;

contract store {
    uint data;

    function set(uint x) {
        data = x;
    }

    function get() constant returns (uint) {
        return data;
    }
}
```
For ease of use [Solidity online compiler](https://ethereum.github.io/browser-solidity) will be used. Copy the sample contract into code window and it will generate corresponding ```web3``` code (right panel) which should be copied into ```store.js``` file
```javascript
var storeContract = web3.eth.contract([{"constant":false,"inputs":[{"name":"x","type":"uint256"}],"name":"set","outputs":[],"payable":false,"type":"function"},{"constant":true,"inputs":[],"name":"get","outputs":[{"name":"","type":"uint256"}],"payable":false,"type":"function"}]);
var store = storeContract.new(
   {
     from: web3.eth.accounts[0], 
     data: '0x606060405234610000575b6090806100176000396000f360606040526000357c01000000000000000000000000000000000000000000000000000000009004806360fe47b11460405780636d4ce63c14605a575b6000565b3460005760586004808035906020019091905050607a565b005b3460005760646085565b6040518082815260200191505060405180910390f35b806000819055505b50565b600060005490505b9056', 
     gas: '4700000'
   }, function (e, contract){
    console.log(e, contract);
    if (typeof contract.address !== 'undefined') {
         console.log('Contract mined! address: ' + contract.address + ' transactionHash: ' + contract.transactionHash);
    }
 })
```

Copied ```store.js``` file can be loaded via console (do not forget to ```personal.unlockAccount()```)
```javascript
> loadScript('store.js');
I1211 14:28:58.352087 internal/ethapi/api.go:1045] Tx(0x6a774d17b83dfaf96b1a233107f8b2fa43f7411a71e87d273403b8a9e7efe254) created: 0x366e0869aea00583c5a5ff62309214707d82e60c
null [object Object]
true
```

Contract is loaded however it should be mined to get deployed.
```javascript
> miner.start()
true
> I1211 14:29:25.804431 miner/miner.go:137] Starting mining operation (CPU=2 TOT=3)
I1211 14:29:25.808925 miner/worker.go:563] commit new work on block 271 with 1 txs & 0 uncles. Took 4.445739ms
I1211 14:29:26.362327 miner/worker.go:362] 🔨  Mined block (#271 / 1fb173c0). Wait 5 blocks for confirmation
I1211 14:29:26.362897 miner/worker.go:563] commit new work on block 272 with 0 txs & 0 uncles. Took 264.496µs
I1211 14:29:26.363044 miner/worker.go:458] 🔨 🔗  Mined 5 blocks back: block #266
null [object Object]
Contract mined! address: 0x366e0869aea00583c5a5ff62309214707d82e60c transactionHash: 0x6a774d17b83dfaf96b1a233107f8b2fa43f7411a71e87d273403b8a9e7efe254
I1211 14:29:28.336365 miner/worker.go:362] 🔨  Mined block (#272 / f58ea260). Wait 5 blocks for confirmation
I1211 14:29:28.336902 miner/worker.go:563] commit new work on block 273 with 0 txs & 0 uncles. Took 229.156µs
I1211 14:29:28.337054 miner/worker.go:458] 🔨 🔗  Mined 5 blocks back: block #267

> miner.stop()
true
```

Now contract is ready to be used
```javascript
> store
{
  abi: [{
      constant: false,
      inputs: [{...}],
      name: "set",
      outputs: [],
      payable: false,
      type: "function"
  }, {
      constant: true,
      inputs: [],
      name: "get",
      outputs: [{...}],
      payable: false,
      type: "function"
  }],
  address: "0x366e0869aea00583c5a5ff62309214707d82e60c",
  transactionHash: "0x6a774d17b83dfaf96b1a233107f8b2fa43f7411a71e87d273403b8a9e7efe254",
  allEvents: function(),
  get: function(),
  set: function()
}
```

Use ```store.get()``` and ```store.set()``` to print and modify stored data
```javascript
> store.get()
0
> personal.unlockAccount(eth.coinbase, YOUR_PASSWORD)

true
> store.set(42,{from: eth.coinbase, gas: 3000000})
I1211 14:58:32.122266 internal/ethapi/api.go:1047] Tx(0x18287c995d686f9fa2bee9136e4b0d92d0d5d0121145eaeba849fd6218a923f0) to: 0x366e0869aea00583c5a5ff62309214707d82e60c
"0x18287c995d686f9fa2bee9136e4b0d92d0d5d0121145eaeba849fd6218a923f0"
> 
> store.get()
0
> miner.start()
true
> I1211 14:58:42.441663 miner/miner.go:137] Starting mining operation (CPU=2 TOT=3)
I1211 14:58:42.446231 miner/worker.go:563] commit new work on block 273 with 1 txs & 0 uncles. Took 4.515457ms
I1211 14:58:42.723401 miner/worker.go:362] 🔨  Mined block (#273 / 54a33c4b). Wait 5 blocks for confirmation
....
> miner.stop()
true
> store.get()
42
```

In order to access contract from another node (node one) and modify data, contract ABI and ADDRESS should be known. This info can be acquired via commands below on node zero.
```javascript
> store.abi
[{
    constant: false,
    inputs: [{
        name: "x",
        type: "uint256"
    }],
    name: "set",
    outputs: [],
    payable: false,
    type: "function"
}, {
    constant: true,
    inputs: [],
    name: "get",
    outputs: [{
        name: "",
        type: "uint256"
    }],
    payable: false,
    type: "function"
}]
> store.address
"0x366e0869aea00583c5a5ff62309214707d82e60c"
```

Following command template should be executed on node one:
```javascript
> var store_on_blockchain = eth.contract(CONTRACT_ABI).at(CONTRACT_ADDRESS);
```
Paragraph and line breaks should be removed from CONTRACT_ABI, [this site](http://www.textfixer.com/tools/remove-line-breaks.php) can be used to do it
```javascript
> var store_on_blockchain = eth.contract([{ constant: false, inputs: [{ name: "x", type: "uint256" }], name: "set", outputs: [], payable: false, type: "function" }, { constant: true, inputs: [], name: "get", outputs: [{ name: "", type: "uint256" }], payable: false, type: "function" }]).at("0x366e0869aea00583c5a5ff62309214707d82e60c");
```

From now on node one can interact with the deployed "store" contract
```javascript
> personal.unlockAccount(eth.coinbase, YOUR_PASSWORD)
true
> store_on_blockchain.set(1234,{from: eth.coinbase, gas: 3000000})
I1211 17:39:00.704042 internal/ethapi/api.go:1047] Tx(0x30b562bad71cf67ea4cbf24ad5c7ec8891f6c2ed5f0c9813f3d65be6f05009a2) to: 0x366e0869aea00583c5a5ff62309214707d82e60c
"0x30b562bad71cf67ea4cbf24ad5c7ec8891f6c2ed5f0c9813f3d65be6f05009a2"
> store_on_blockchain.get()
1234
```

###### Smart Contracts
* [Introduction to Smart Contracts](http://solidity.readthedocs.io/en/latest/introduction-to-smart-contracts.html)

