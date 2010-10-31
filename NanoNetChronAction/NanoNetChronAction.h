/**
 * NanoNetChronAction
 *
 * Call timeout periodically.
 */

#include <UnixTimeChron.h>
#include <NanoNet.h>

class NanoNetChronAction : public UnixTimeChronAction {
	public:
		NanoNetChronAction(NanoNet *net,unsigned long long seconds);
		NanoNet *net;

		void go(unsigned long long time);
};
