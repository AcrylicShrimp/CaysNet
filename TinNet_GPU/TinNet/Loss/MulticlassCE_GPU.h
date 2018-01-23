
/*
	2018.01.23
	Created by AcrylicShrimp.
*/

#ifndef _CLASS_TINNET_LOSS_MULTICLASSCE_GPU_H

#define _CLASS_TINNET_LOSS_MULTICLASSCE_GPU_H

#include "../../../TinNet/TinNet/TinNetDLL.h"

#include "../TinNet_GPU_Backend.h"

#include "cuda.h"

#include <cstddef>

namespace TinNet::Loss
{
	class TINNET_DLL MulticlassCE_GPU final
	{
	public:
		MulticlassCE_GPU() = delete;
		MulticlassCE_GPU(const MulticlassCE_GPU &sSrc) = delete;
		~MulticlassCE_GPU() = delete;
		
	public:
		MulticlassCE_GPU &operator=(const MulticlassCE_GPU &sSrc) = delete;
		
	public:
		static void derivative(std::size_t nBatchSize, std::size_t nOutputSize, CUdeviceptr pOutput, CUdeviceptr pDesiredOutput, CUdeviceptr pResult);
	};
}

#endif