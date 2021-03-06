# teslaBMSRT

![Tesla BMS RT](misc/20190319_221311.jpg)

## dependencies
- Very cool RTOS for the teensy
	- https://github.com/greiman/ChRt
- TeensyView libs
	- https://github.com/sparkfun/SparkFun_TeensyView_Arduino_Library/tree/master/examples
	
## Error codes on teensyView

| code | definition | 
|:----:|------------|
| A | Modules Fault Loop |
| B | Battery Monitor Fault |
| C | BMS Serial communication Fault |
| D | BMS Cell Over Voltage Fault |
| E | BMS Cell Under Voltage Fault |
| F | BMS Over Temperature Fault |
| G | BMS Under Temperature Fault |
| H | BMS 12V Battery Over Voltage Fault |
| I | BMS 12V Battery Under Voltage Fault |
	
## todo
- [X] assign all signals to pins
- [ ] Implement state machine
	- [X] noFault is driven differently from run to charging (run noFault is used to limit output while in charging it is used to prevent charging)
- [X] modify formats to display:
	- [X] sticky faults format
- [ ] perform schedulability analysis for rate monotonic scheduling
	- [ ] worst case execution time (wcet)
	- [ ] worst case blocking time (wcbt)
	
