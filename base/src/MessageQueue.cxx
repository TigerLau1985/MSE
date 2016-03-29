#include "MessageQueue.h"
#include "PCMCache.h"

using namespace mse;


MessageQueue::MessageQueue():_sequencer(_buf)
{
}

MessageQueue::~MessageQueue()
{
}

void MessageQueue::pushMessage(AbstractMessage::Ptr &msg)
{
	int64_t seq =  _sequencer.Claim();
	_sequencer[seq] = msg;
	_sequencer.Publish(seq);
}

MQObserver::Ptr MessageQueue::newObserver()
{
	MQObserver::Ptr ret(new MQObserver(*this));
	return ret;
}

AbstractMessage::Ptr MQObserver::read()
{
	AbstractMessage::Ptr ret(NULL);
	int64_t seq = _barrier->WaitFor(_next);
	if (seq >= _next) {
		if (_next <= seq - (int64_t)disruptor::kDefaultRingBufferSize) {
			_next = seq - disruptor::kDefaultRingBufferSize + 1;
		} 
		ret = _mq._sequencer[_next++];
	}

	return ret;
}

MQObserver::MQObserver(MessageQueue &mq):_mq(mq), _next(0)
{
	_barrier = _mq._sequencer.NewBarrier(_track);
}

MQObserver::~MQObserver()
{
	if (_barrier) {
		delete _barrier;
	}
}
