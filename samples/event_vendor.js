var event_notify = bridge.Notify();
event_notify.watch(function(error, result){
    if (!error) {
        console.log("[0xdeadbeefb00b1e56] MSG[" + result.args.message + "] FROM[" + result.args.from + "]");
    }
});
