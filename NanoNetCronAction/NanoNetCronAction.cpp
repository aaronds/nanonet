#include "NanoNetCronAction.h"

NanoNetCronAction::NanoNetCronAction(NanoNet *n,unsigned long long seconds) : UnixTimeCronAction(0,seconds){
	net = n;
}

/*
 * Call the processTimeout function see nanonet.
 */

void NanoNetCronAction::go(unsigned long long time){
	if(net)
		net->processTimeout();
}
