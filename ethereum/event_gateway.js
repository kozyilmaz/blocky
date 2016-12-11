var event_process = bridge.Process();
event_process.watch(function(error, result){
    if (!error) {
        console.log("[0x0123456789abcdef] MSG[" + result.args.message + "] FROM[" + result.args.from + "] TO[" + result.args.to + "]");
    }
});