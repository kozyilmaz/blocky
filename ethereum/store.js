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