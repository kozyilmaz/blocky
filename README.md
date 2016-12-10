# Blocky
Ethereum based smart contracts for IoT devices

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
```
$ export PATH=$PATH:/usr/local/go/bin
```

### Install Ethereum Client
```
$ git clone https://github.com/ethereum/go-ethereum.git
$ cd go-ethereum
$ make geth
$ sudo cp build/bin/geth /usr/local/bin/geth
```


## Create Private Ethereum Testnet

### Setup Node Zero
Create a new account for private net
```
$ geth --datadir "~/private-iot-chain" account new
```
Note account number and pre-allocate ether for the new account in genesis.json (i.e. change account address in "alloc" accordingly)
```
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
```
$ geth --datadir "~/private-iot-chain" init genesis.json
```
Launch geth for private net
```
$ geth --rpc --rpcport "8000" --rpccorsdomain "*" --datadir "~/private-iot-chain" --port "30303" --nodiscover --ipcapi "admin,db,eth,debug,miner,net,shh,txpool,personal,web3" --rpcapi "db,eth,net,web3" --autodag --identity "zero" --networkid 666 console
```

Run ```eth.getBalance(eth.coinbase)``` command to check the account balance, pre-allocated sum should be there  
Run ```admin.nodeInfo``` to get enode url (```enode://xxxxx```) and add ip address of the interface ```[::]``` to construct the complete enode address to share with other peers
```
"enode://6ad5934db83a0266c4c6d5048d02f86b3e69251d45ad411387cde9cc5a86030f2bee4bcbe200d4238d91b01c94444e562986058c9c4acca2a92cb81eb012acfc@192.168.2.41:30303?discport=0"
```

### Setup Node One

Setup second one which is a second computer on our net
```
$ geth --datadir "~/private-iot-chain" account new
$ geth --datadir "~/private-iot-chain" init genesis.json
$ geth --datadir "~/private-iot-chain" --port "30303" --nodiscover --identity "one" --networkid 666 console
```

Run ```admin.addPeer``` to connect node zero
```
admin.addPeer("enode://6ad5934db83a0266c4c6d5048d02f86b3e69251d45ad411387cde9cc5a86030f2bee4bcbe200d4238d91b01c94444e562986058c9c4acca2a92cb81eb012acfc@192.168.2.41:30303")
```

Send ether from node zero to one using one's wallet address. Execute on node zero, do not forget mine transaction into blockchain
```
eth.sendTransaction({from:eth.coinbase, to: '0xcb2a95f964acf8adee7fae30cf5dc6a3f5e14a5c', value: web3.toWei(.000000000001, "ether")})
miner.start()
miner.stop()
```

###### Ethereum
* [Creating a private chain/testnet](https://souptacular.gitbooks.io/ethereum-tutorials-and-tips-by-hudson/content/private-chain.html)  
* [Setting up an Ethereum testnet](http://billmarino2.github.io/general/2015/12/09/JURIX-2015-setting-up-an-ethereum-testnet.html)  
* [Setting up private network or local cluster](https://github.com/ethereum/go-ethereum/wiki/Setting-up-private-network-or-local-cluster)



## Create Smart Contracts

To develop smart contract Solidity compiler is needed, ```eth.getCompilers()``` will list available compilers. If solidity is not present, build commands are as follows:
```
git clone --recursive https://github.com/ethereum/solidity.git
cd solidity
git checkout tags/<tag_name>
./scripts/install_deps.sh
mkdir build
cd build
cmake .. && make
```

###### Smart Contracts
* [Introduction to Smart Contracts](http://solidity.readthedocs.io/en/latest/introduction-to-smart-contracts.html)

