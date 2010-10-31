#include "NanoNetChronAction.h"

NanoNetChronAction::NanoNetChronAction(NanoNet *n,unsigned long long seconds) : UnixTimeChronAction(0,seconds){
	net = n;
}

/*
 * Call the processTimeout function see nanonet.
 */

void NanoNetChronAction::go(unsigned long long time){
	if(net)
		net->processTimeout();
}
