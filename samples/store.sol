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
