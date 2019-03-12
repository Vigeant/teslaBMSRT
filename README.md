# teslaBMSRT

## dependencies
- Very cool RTOS for the teensy
	- https://github.com/greiman/ChRt
- TeensyView libs
	- https://github.com/sparkfun/SparkFun_TeensyView_Arduino_Library/tree/master/examples
	
## todo
- [X] assign all signals to pins
- [ ] start building state machine
- [ ] modify formats to display:
	- [ ] module number and cell number for all values
	- error format
		- [ ] pack loop error
		- [ ] pack comms error
		- [ ] battery monitor error
- [ ] perform schedulability analysis for rate monotonic scheduling
	- [ ] worst case execution time (wcet)
	- [ ] worst case blocking time (wcbt)
	
