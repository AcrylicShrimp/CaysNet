
/*
	2017.08.04
	Created by AcrylicShrimp.
*/

#ifndef _CLASS_CAYSNET_OPTIMIZER_SUPERVISED_SGD_H

#define _CLASS_CAYSNET_OPTIMIZER_SUPERVISED_SGD_H

#include "../../CaysNetDLL.h"

#include "../../Layer/Layer.h"
#include "../../Layer/FullLayer.h"
#include "../../NN.h"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <random>
#include <utility>
#include <vector>

namespace CaysNet::Optimizer::Supervised
{
	class CAYSNET_DLL SGD final
	{
	private:
		NN &sNN;
		std::size_t nBatchSize;
		float nLearningRate;
		std::vector<std::vector<float>> sBiasDelta;
		std::vector<std::vector<float>> sWeightDelta;
		std::vector<std::vector<float>> sBiasDeltaBuffer;
		std::vector<std::vector<std::vector<float>>> sForwardOutput;
		std::vector<std::vector<std::vector<float>>> sBackwardOutput;
		std::vector<std::vector<std::vector<float>>> sActivationInput;
		std::vector<std::vector<std::vector<float>>> sActivationOutput;
		std::mt19937_64 sEngine;

	public:
		SGD(NN &sNN, std::size_t nNewBatchSize, float nNewLearningRate);
		SGD(const SGD &sSrc) = delete;
		SGD(SGD &&sSrc);
		~SGD() = default;

	public:
		SGD &operator=(const SGD &sSrc) = delete;
		SGD &operator=(SGD &&sSrc) = delete;

	public:
		inline float &learningRate();
		inline float learningRate() const;
		template<class LossFunc> std::pair<float, float> calcNumericalGradient(const std::vector<float> &sInput, const std::vector<float> &sOutput, std::size_t nLayerIndex, std::size_t nOutIndex, std::size_t nInIndex);
		template<class LossFunc> void train(std::size_t nEpoch, std::size_t nSize, std::vector<float> *pInput, std::vector<float> *pOutput);
	};

	inline float &SGD::learningRate()
	{
		return this->nLearningRate;
	}

	inline float SGD::learningRate() const
	{
		return this->nLearningRate;
	}
}

#include "SGD.hpp"

#endif