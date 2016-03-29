#pragma once

#include <cstdint>
#include <array>
#include <algorithm>
#include "disruptor/sequencer.h"

namespace mse {
template<size_t N>
class PCMCache
{
public:
	PCMCache(): _sequencer(_buf), _nextSeq(0)
	{
		_barrier = _sequencer.NewBarrier(_track);
	}

	~PCMCache()
	{
		if (_barrier) delete _barrier;
	}

	size_t write(int16_t pcm[], size_t size)
	{
		size_t writed = 0;
		if (size <= N) {
			int64_t begin = _sequencer.GetCursor();
			int64_t end = _sequencer.Claim(size);
			writed = end - begin;
			for (size_t i = end, j = writed - 1; i > begin; i--, j--){
				_sequencer[i] = pcm[j];
			}
			_sequencer.Publish(end);
		}

		return writed;
	}

	size_t read(int16_t pcm[], size_t size)
	{
		size_t readNum = 0;
		int64_t cursor = _sequencer.GetCursor();
		if (size < cursor - _nextSeq) {
			for (size_t i = 0; i < size; i++){
				_barrier->WaitFor(_nextSeq);
				pcm[i] = _sequencer[_nextSeq++];
			}
		}

		return readNum;
	}
protected:
	std::array<int16_t, N> _buf;
	disruptor::Sequencer<int16_t, N, disruptor::kDefaultClaimStrategy, disruptor::YieldingStrategy<>> _sequencer;
	disruptor::SequenceBarrier<disruptor::YieldingStrategy<>> *_barrier;
	std::vector<disruptor::Sequence*> _track;
	int64_t _nextSeq;
};
}