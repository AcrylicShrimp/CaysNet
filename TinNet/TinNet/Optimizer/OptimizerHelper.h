
/*
	2018.01.14
	Created by AcrylicShrimp.
*/

#ifndef _CLASS_TINNET_OPTIMIZER_OPTIMIZERHELPER_H

#define _CLASS_TINNET_OPTIMIZER_OPTIMIZERHELPER_H

#include "../TinNetDLL.h"

#include "../Layer/Layer.h"
#include "../NN.h"

#include <algorithm>
#include <chrono>
#include <random>
#include <vector>

namespace TinNet::Optimizer
{
	class TINNET_DLL OptimizerHelper
	{
	public:
		OptimizerHelper() = delete;
		OptimizerHelper(const OptimizerHelper &sSrc) = delete;
		~OptimizerHelper() = delete;
		
	public:
		OptimizerHelper &operator=(const OptimizerHelper &sSrc) = delete;
		
	public:
		static void shuffleTrainingSet(std::size_t nSize, std::vector<float> *pInput, std::vector<float> *pOutput);
		static void zeros(std::vector<float> &sDst);
		static void zeros(std::vector<std::vector<float>> &sDst);
		static void zeros(std::vector<std::vector<std::vector<float>>> &sDst);
	};
}

#endif