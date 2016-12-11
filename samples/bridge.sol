pragma solidity ^0.4.0;

contract Bridge {

    address creator;

    event Notify(
        address indexed _from,
        string  _value
    );

    // This is the constructor whose code is
    // run only when the contract is created.
    function Bridge() {
        creator = msg.sender;
    }

    // This is the message from IoT gateway to vendor node
    // either to request activation or push data
    function request(string data) {
        Notify(msg.sender, data);
    }

    // This is the message from vendor node to IoT gateway
    // to proceed with activation
    function activate(address gateway_address, string data) {
        Notify(msg.sender, data);
    }

    function kill() {
        // kills this contract and sends remaining
        // funds back to creator
        if (msg.sender == creator) {
            suicide(creator);
        }
    }
}
