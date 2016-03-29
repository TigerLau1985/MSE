// Copyright (c) 2011-2015, LiuDa
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the MSE nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL FRANCOIS SAINT-JACQUES BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once

#include <cstdint>
#include <memory>
#include <array>
#include <vector>
#include "AbstractMessage.h"
#include "disruptor/sequencer.h"
#include "disruptor/claim_strategy.h"

namespace mse {

class MQObserver {
public:
    typedef std::auto_ptr<MQObserver> Ptr;
    virtual AbstractMessage::Ptr read();
protected:
	friend class MessageQueue;
	friend class std::auto_ptr<MQObserver>;
	explicit MQObserver(MessageQueue &mq);
	~MQObserver();
	MessageQueue &_mq;
    disruptor::SequenceBarrier<disruptor::YieldingStrategy<>> *_barrier;
	std::vector<disruptor::Sequence*> _track;
	int64_t _next;
};

class MessageQueue {
public:
    MessageQueue();
    virtual ~MessageQueue();

    void pushMessage(AbstractMessage::Ptr &msg);
    MQObserver::Ptr newObserver();
protected:
	friend class MQObserver;
    std::array<AbstractMessage::Ptr, disruptor::kDefaultRingBufferSize> _buf;
    disruptor::Sequencer<AbstractMessage::Ptr, disruptor::kDefaultRingBufferSize , disruptor::MultiThreadedStrategy<>, disruptor::YieldingStrategy<>> _sequencer;
};
}
