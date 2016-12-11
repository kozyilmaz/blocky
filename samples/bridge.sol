pragma solidity ^0.4.0;

contract Bridge {
    // contract owner
    address public creator;

    // IoT vendor event handler
    event Notify(
        address indexed from,
        string message
    );
    // IoT gateway event handler
    event Process(
        address indexed from,
        address indexed to,
        string message
    );

    // This is the constructor whose code is
    // run only when the contract is created.
    function Bridge() {
        creator = msg.sender;
    }

    // This is the message from IoT gateway to vendor node
    // either to request activation or push data (broadcast)
    function request(string data) {
        Notify(msg.sender, data);
    }

    // This is the message from vendor node to IoT gateway
    // to proceed with activation (unicast)
    function activate(address gateway, string data) {
        Process(msg.sender, gateway, data);
    }

    // Kills this contract and sends remaining
    // funds back to creator
    function kill() {
        if (msg.sender == creator) {
            suicide(creator);
        }
    }
}
