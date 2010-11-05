/**
 * NanoNetCronAction
 *
 * Call timeout periodically.
 */

#include <UnixTimeCron.h>
#include <NanoNet.h>

class NanoNetCronAction : public UnixTimeCronAction {
	public:
		NanoNetCronAction(NanoNet *net,unsigned long long seconds);
		NanoNet *net;

		void go(unsigned long long time);
};
