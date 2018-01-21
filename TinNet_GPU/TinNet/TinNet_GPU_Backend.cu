
/*
	2018.01.20
	Created by AcrylicShrimp.
*/

#include "TinNet_GPU_Backend.h"

#include "cuda_device_runtime_api.h"
#include "cuda_runtime.h"
#include "device_atomic_functions.h"
#include "device_functions.h"
#include "device_launch_parameters.h"
#include "math_functions.h"

#include <cstdio>

#pragma region Kernel

__global__ void kernel_mergeBatch(unsigned int nBatchSize, unsigned int nSize, const float *pInput, float *pOutput)
{
	unsigned int nUnifiedIndex = blockDim.x * blockIdx.x + threadIdx.x;

	for (unsigned int nBatch = 1; nBatch < nBatchSize; ++nBatch)
		pOutput[nUnifiedIndex] += pInput[nSize * nBatch + nUnifiedIndex];
}



__global__ void kernel_MSE_GPU_derivative(unsigned int nSize, unsigned int nOutputSize, const float *pOutput, const float *pDesiredOutput, float *pResult)
{
	unsigned int nUnifiedOut = blockDim.x * blockIdx.x + threadIdx.x;
	unsigned int vMax[2] =
	{
		nSize - 1,
		nUnifiedOut
	};

	nUnifiedOut = vMax[nUnifiedOut < nSize];
	pResult[nUnifiedOut] = 2.f / nOutputSize * (pOutput[nUnifiedOut] - pDesiredOutput[nUnifiedOut]);
}



__global__ void kernel_FullLayer_GPU_forward(unsigned int nInputSize, unsigned int nOutputSize, const float *pBias, const float *pWeight, const float *pInput, float *pOutput)
{
	extern __shared__ float pCopiedInput[];

	for (unsigned int nIn = threadIdx.x; nIn < nInputSize; nIn += blockDim.x)
		pCopiedInput[nIn] = pInput[nIn];

	__syncthreads();

	unsigned int nUnifiedOut = blockDim.x * blockIdx.x + threadIdx.x;

	if (nUnifiedOut >= nOutputSize)
		return;

	float nValue = pBias[nUnifiedOut];

	unsigned int nWeightIndex = nUnifiedOut * nInputSize;

	for (unsigned int nIn = 0; nIn < nInputSize; ++nIn)
		nValue += pCopiedInput[nIn] * pWeight[nWeightIndex + nIn];

	pOutput[nUnifiedOut] = nValue;
}

__global__ void kernel_FullLayer_GPU_forwardBatch(unsigned int nInputSize, unsigned int nOutputSize, const float *pBias, const float *pWeight, const float *pInput, float *pOutput)
{
	extern __shared__ float pCopiedInput[];

	for (unsigned int nIn = threadIdx.x; nIn < nInputSize; nIn += blockDim.x)
		pCopiedInput[nIn] = pInput[nInputSize * blockIdx.x + nIn];

	__syncthreads();
	
	unsigned int nUnifiedOut = blockDim.x * blockIdx.y + threadIdx.x;
	
	if (nUnifiedOut >= nOutputSize)
		return;
	
	float nValue = pBias[nUnifiedOut];
	
	for (unsigned int nIn = 0; nIn < nInputSize; ++nIn)
		nValue += pCopiedInput[nIn] * pWeight[nUnifiedOut * nInputSize + nIn];
	
	pOutput[nOutputSize * blockIdx.x + nUnifiedOut] = nValue;
}

__global__ void kernel_FullLayer_GPU_backwardBatch(unsigned int nInputSize, unsigned int nOutputSize, const float *pForwardInput, const float *pBackwardInput, float *pBackwardOutput, float *pBiasDelta, float *pWeightDelta, const float *pWeight)
{
	extern __shared__ float pCopiedInput[];

	for (unsigned int nOut = threadIdx.x; nOut < nOutputSize; nOut += blockDim.x)
		pCopiedInput[nOut] = pBackwardInput[nOutputSize * blockIdx.x + nOut];

	for (unsigned int nIn = threadIdx.x; nIn < nInputSize; nIn += blockDim.x)
		pCopiedInput[nOutputSize + nIn] = pForwardInput[nInputSize * blockIdx.x + nIn];

	__syncthreads();
	
	unsigned int nUnifiedIn = blockDim.x * blockIdx.y + threadIdx.x;
	
	if (nUnifiedIn >= nInputSize)
		return;
	
	for (unsigned int nOut = threadIdx.x; nOut < nOutputSize; nOut += blockDim.x)
		pBiasDelta[nOutputSize * blockIdx.x + nOut] = pCopiedInput[nOut];
	
	float nValue = .0f;

	for (unsigned int nOut = 0; nOut < nOutputSize; ++nOut)
	{
		nValue = pCopiedInput[nOut] * pWeight[nInputSize * nOut + nUnifiedIn];
		pWeightDelta[nInputSize * nOutputSize * blockIdx.x + nInputSize * nOut + nUnifiedIn] = pCopiedInput[nOut] * pCopiedInput[nOutputSize + nUnifiedIn];
	}

	pBackwardOutput[nInputSize * blockIdx.x + nUnifiedIn] = nValue;
}

__global__ void kernel_FullLayer_GPU_update(unsigned int nParamSize, const float *pParamDelta, float *pParam)
{
	unsigned int nUnifiedOut = blockDim.x * blockIdx.x + threadIdx.x;

	if (nUnifiedOut >= nParamSize)
		return;

	pParam[nUnifiedOut] += pParamDelta[nUnifiedOut];
}

__global__ void kernel_FullLayer_GPU_updateFactor(unsigned int nParamSize, const float *pParamDelta, float *pParam, float nFactor)
{
	unsigned int nUnifiedOut = blockDim.x * blockIdx.x + threadIdx.x;

	if (nUnifiedOut >= nParamSize)
		return;

	pParam[nUnifiedOut] += nFactor * pParamDelta[nUnifiedOut];
}



__global__ void kernel_SigmoidLayer_GPU_forward(unsigned int nSize, const float *pInput, float *pOutput)
{
	unsigned int nUnifiedIndex = blockDim.x * blockIdx.x + threadIdx.x;
	unsigned int vMax[2] =
	{
		nSize - 1,
		nUnifiedIndex
	};

	nUnifiedIndex = vMax[nUnifiedIndex < nSize];
	pOutput[nUnifiedIndex] = 1.f / (expf(-pInput[nUnifiedIndex]) + 1.f);
}

__global__ void kernel_SigmoidLayer_GPU_forwardBatch(unsigned int nSize, const float *pInput, float *pOutput)
{
	unsigned int nUnifiedIndex = blockDim.x * blockIdx.y + threadIdx.x;
	unsigned int vMax[2] =
	{
		nSize - 1,
		nUnifiedIndex
	};

	nUnifiedIndex = vMax[nUnifiedIndex < nSize];
	pOutput[nSize * blockIdx.x + nUnifiedIndex] = 1.f / (expf(-pInput[nSize * blockIdx.x + nUnifiedIndex]) + 1.f);
}

__global__ void kernel_SigmoidLayer_GPU_backwardBatch(unsigned int nSize, const float *pForwardInput, const float *pBackwardInput, float *pBackwardOutput)
{
	unsigned int nUnifiedIndex = blockDim.x * blockIdx.y + threadIdx.x;

	float nValue = 1.f / (expf(-pForwardInput[nSize * blockIdx.x + nUnifiedIndex]) + 1.f);
	pBackwardOutput[nSize * blockIdx.x + nUnifiedIndex] = pBackwardInput[nSize * blockIdx.x + nUnifiedIndex] * nValue * (1.f - nValue);
}



__global__ void kernel_TanhLayer_GPU_forward(unsigned int nSize, const float *pInput, float *pOutput)
{
	unsigned int nUnifiedIndex = blockDim.x * blockIdx.x + threadIdx.x;
	unsigned int vMax[2] =
	{
		nSize - 1,
		nUnifiedIndex
	};

	nUnifiedIndex = vMax[nUnifiedIndex < nSize];
	pOutput[nUnifiedIndex] = tanhf(pInput[nUnifiedIndex]);
}

__global__ void kernel_TanhLayer_GPU_forwardBatch(unsigned int nSize, const float *pInput, float *pOutput)
{
	unsigned int nUnifiedIndex = blockDim.x * blockIdx.y + threadIdx.x;
	unsigned int vMax[2] =
	{
		nSize - 1,
		nUnifiedIndex
	};

	nUnifiedIndex = vMax[nUnifiedIndex < nSize];
	pOutput[nSize * blockIdx.x + nUnifiedIndex] = tanhf(pInput[nSize * blockIdx.x + nUnifiedIndex]);
}

__global__ void kernel_TanhLayer_GPU_backwardBatch(unsigned int nSize, const float *pForwardInput, const float *pBackwardInput, float *pBackwardOutput)
{
	unsigned int nUnifiedIndex = blockDim.x * blockIdx.y + threadIdx.x;

	float nValue = tanhf(pForwardInput[nSize * blockIdx.x + nUnifiedIndex]);
	pBackwardOutput[nSize * blockIdx.x + nUnifiedIndex] = pBackwardInput[nSize * blockIdx.x + nUnifiedIndex] * (1.f - nValue * nValue);
}

#pragma endregion

void mergeBatch(std::size_t nBatchSize, std::size_t nSize, CUdeviceptr pInput, CUdeviceptr pOutput)
{
	uint3 sDimGrid;
	uint3 sDimBlock;

	sDimGrid.x = (nSize - 1) / 1024 + 1;
	sDimGrid.y = 1;
	sDimGrid.z = 1;

	sDimBlock.x = nSize < 1024 ? nSize : 1024;
	sDimBlock.y = 1;
	sDimBlock.z = 1;

	kernel_mergeBatch<<<sDimGrid, sDimBlock>>>(nBatchSize, nSize, (const float *)pInput, (float *)pOutput);

	cudaError_t nError = cudaGetLastError();

	if (nError != cudaError_t::cudaSuccess)
		printf("mergeBatch : %d\n", nError);
}

#pragma region MSE_GPU

void MSE_GPU_derivative(std::size_t nBatchSize, std::size_t nOutputSize, CUdeviceptr pOutput, CUdeviceptr pDesiredOutput, CUdeviceptr pResult)
{
	std::size_t nSize = nBatchSize * nOutputSize;

	uint3 sDimGrid;
	uint3 sDimBlock;

	sDimGrid.x = (nSize - 1) / 1024 + 1;
	sDimGrid.y = 1;
	sDimGrid.z = 1;

	sDimBlock.x = nSize < 1024 ? nSize : 1024;
	sDimBlock.y = 1;
	sDimBlock.z = 1;

	kernel_MSE_GPU_derivative<<<sDimGrid, sDimBlock>>>(nSize, nOutputSize, (const float *)pOutput, (const float *)pDesiredOutput, (float *)pResult);

	cudaError_t nError = cudaGetLastError();

	if (nError != cudaError_t::cudaSuccess)
		printf("MSE_GPU_derivative : %d\n", nError);
}

#pragma endregion

#pragma region Layer_GPU

void FullLayer_GPU_forward(std::size_t nInputSize, std::size_t nOutputSize, CUdeviceptr pInput, CUdeviceptr pOutput, CUdeviceptr pBias, CUdeviceptr pWeight)
{
	uint3 sDimGrid;
	uint3 sDimBlock;

	sDimGrid.x = (nOutputSize - 1) / 1024 + 1;
	sDimGrid.y = 1;
	sDimGrid.z = 1;

	sDimBlock.x = nOutputSize < 1024 ? nOutputSize : 1024;
	sDimBlock.y = 1;
	sDimBlock.z = 1;

	kernel_FullLayer_GPU_forward<<<sDimGrid, sDimBlock, sizeof(float) * nInputSize>>>(nInputSize, nOutputSize, (const float *)pBias, (const float *)pWeight, (const float *)pInput, (float *)pOutput);

	cudaError_t nError = cudaGetLastError();

	if (nError != cudaError_t::cudaSuccess)
		printf("FullLayer_GPU_forward : %d\n", nError);
}

void FullLayer_GPU_forwardBatch(std::size_t nBatchSize, std::size_t nInputSize, std::size_t nOutputSize, CUdeviceptr pInput, CUdeviceptr pOutput, CUdeviceptr pBias, CUdeviceptr pWeight)
{
	uint3 sDimGrid;
	uint3 sDimBlock;

	sDimGrid.x = nBatchSize;
	sDimGrid.y = (nOutputSize - 1) / 1024 + 1;
	sDimGrid.z = 1;

	sDimBlock.x = nOutputSize < 1024 ? nOutputSize : 1024;
	sDimBlock.y = 1;
	sDimBlock.z = 1;

	kernel_FullLayer_GPU_forwardBatch<<<sDimGrid, sDimBlock, sizeof(float) * nInputSize>>>(nInputSize, nOutputSize, (const float *)pBias, (const float *)pWeight, (const float *)pInput, (float *)pOutput);

	cudaError_t nError = cudaGetLastError();

	if (nError != cudaError_t::cudaSuccess)
		printf("FullLayer_GPU_forwardBatch : %d\n", nError);
}

void FullLayer_GPU_backwardBatch(std::size_t nBatchSize, std::size_t nInputSize, std::size_t nOutputSize, CUdeviceptr pForwardInput, CUdeviceptr pBackwardInput, CUdeviceptr pBackwardOutput, CUdeviceptr pBiasDelta, CUdeviceptr pWeightDelta, CUdeviceptr pWeight)
{
	uint3 sDimGrid;
	uint3 sDimBlock;

	sDimGrid.x = nBatchSize;
	sDimGrid.y = (nInputSize - 1) / 1024 + 1;
	sDimGrid.z = 1;

	sDimBlock.x = nInputSize < 1024 ? nInputSize : 1024;
	sDimBlock.y = 1;
	sDimBlock.z = 1;

	kernel_FullLayer_GPU_backwardBatch<<<sDimGrid, sDimBlock, sizeof(float) * (nInputSize + nOutputSize)>>>(nInputSize, nOutputSize, (const float *)pForwardInput, (const float *)pBackwardInput, (float *)pBackwardOutput, (float *)pBiasDelta, (float *)pWeightDelta, (const float *)pWeight);

	cudaError_t nError = cudaGetLastError();

	if (nError != cudaError_t::cudaSuccess)
		printf("FullLayer_GPU_backwardBatch : %d\n", nError);
}

void FullLayer_GPU_updateParam(std::size_t nBiasSize, std::size_t nWeightSize, CUdeviceptr pBias, CUdeviceptr pWeight, CUdeviceptr pBiasDelta, CUdeviceptr pWeightDelta)
{
	uint3 sDimGrid;
	uint3 sDimBlock;

	sDimGrid.x = (nBiasSize - 1) / 1024 + 1;
	sDimGrid.y = 1;
	sDimGrid.z = 1;

	sDimBlock.x = nBiasSize < 1024 ? nBiasSize : 1024;
	sDimBlock.y = 1;
	sDimBlock.z = 1;

	kernel_FullLayer_GPU_update << <sDimGrid, sDimBlock >> > (nBiasSize, (const float *)pBiasDelta, (float *)pBias);

	cudaError_t nError = cudaGetLastError();

	if (nError != cudaError_t::cudaSuccess)
		printf("FullLayer_GPU_updateParam__BIAS : %d\n", nError);

	sDimGrid.x = (nWeightSize - 1) / 1024 + 1;
	sDimGrid.y = 1;
	sDimGrid.z = 1;

	sDimBlock.x = nWeightSize < 1024 ? nWeightSize : 1024;
	sDimBlock.y = 1;
	sDimBlock.z = 1;

	kernel_FullLayer_GPU_update<<<sDimGrid, sDimBlock>>>(nWeightSize, (const float *)pWeightDelta, (float *)pWeight);

	nError = cudaGetLastError();

	if (nError != cudaError_t::cudaSuccess)
		printf("FullLayer_GPU_updateParam__WEIGHT : %d\n", nError);
}

void FullLayer_GPU_updateParamFactor(std::size_t nBiasSize, std::size_t nWeightSize, CUdeviceptr pBias, CUdeviceptr pWeight, CUdeviceptr pBiasDelta, CUdeviceptr pWeightDelta, float nFactor)
{
	uint3 sDimGrid;
	uint3 sDimBlock;

	sDimGrid.x = (nBiasSize - 1) / 1024 + 1;
	sDimGrid.y = 1;
	sDimGrid.z = 1;

	sDimBlock.x = nBiasSize < 1024 ? nBiasSize : 1024;
	sDimBlock.y = 1;
	sDimBlock.z = 1;

	kernel_FullLayer_GPU_updateFactor<<<sDimGrid, sDimBlock>>>(nBiasSize, (const float *)pBiasDelta, (float *)pBias, nFactor);

	cudaError_t nError = cudaGetLastError();

	if (nError != cudaError_t::cudaSuccess)
		printf("FullLayer_GPU_updateParamFactor__BIAS : %d\n", nError);

	sDimGrid.x = (nWeightSize - 1) / 1024 + 1;
	sDimGrid.y = 1;
	sDimGrid.z = 1;

	sDimBlock.x = nWeightSize < 1024 ? nWeightSize : 1024;
	sDimBlock.y = 1;
	sDimBlock.z = 1;

	kernel_FullLayer_GPU_updateFactor<<<sDimGrid, sDimBlock>>>(nWeightSize, (const float *)pWeightDelta, (float *)pWeight, nFactor);

	nError = cudaGetLastError();

	if (nError != cudaError_t::cudaSuccess)
		printf("FullLayer_GPU_updateParamFactor__WEIGHT : %d\n", nError);
}



void SigmoidLayer_GPU_forward(std::size_t nSize, CUdeviceptr pInput, CUdeviceptr pOutput)
{
	uint3 sDimGrid;
	uint3 sDimBlock;

	sDimGrid.x = (nSize - 1) / 1024 + 1;
	sDimGrid.y = 1;
	sDimGrid.z = 1;

	sDimBlock.x = nSize < 1024 ? nSize : 1024;
	sDimBlock.y = 1;
	sDimBlock.z = 1;

	kernel_SigmoidLayer_GPU_forward<<<sDimGrid, sDimBlock>>>(nSize, (const float *)pInput, (float *)pOutput);

	cudaError_t nError = cudaGetLastError();

	if (nError != cudaError_t::cudaSuccess)
		printf("SigmoidLayer_GPU_forward : %d\n", nError);
}

void SigmoidLayer_GPU_forwardBatch(std::size_t nBatchSize, std::size_t nSize, CUdeviceptr pInput, CUdeviceptr pOutput)
{
	uint3 sDimGrid;
	uint3 sDimBlock;

	sDimGrid.x = nBatchSize;
	sDimGrid.y = (nSize - 1) / 1024 + 1;
	sDimGrid.z = 1;

	sDimBlock.x = nSize < 1024 ? nSize : 1024;
	sDimBlock.y = 1;
	sDimBlock.z = 1;

	kernel_SigmoidLayer_GPU_forwardBatch<<<sDimGrid, sDimBlock>>>(nSize, (const float *)pInput, (float *)pOutput);

	cudaError_t nError = cudaGetLastError();

	if (nError != cudaError_t::cudaSuccess)
		printf("SigmoidLayer_GPU_forwardBatch : %d\n", nError);
}

void SigmoidLayer_GPU_backwardBatch(std::size_t nBatchSize, std::size_t nSize, CUdeviceptr pForwardInput, CUdeviceptr pBackwardInput, CUdeviceptr pBackwardOutput)
{
	uint3 sDimGrid;
	uint3 sDimBlock;

	sDimGrid.x = nBatchSize;
	sDimGrid.y = (nSize - 1) / 1024 + 1;
	sDimGrid.z = 1;

	sDimBlock.x = nSize < 1024 ? nSize : 1024;
	sDimBlock.y = 1;
	sDimBlock.z = 1;

	kernel_SigmoidLayer_GPU_backwardBatch<<<sDimGrid, sDimBlock>>>(nSize, (const float *)pForwardInput, (const float *)pBackwardInput, (float *)pBackwardOutput);

	cudaError_t nError = cudaGetLastError();

	if (nError != cudaError_t::cudaSuccess)
		printf("SigmoidLayer_GPU_backwardBatch : %d\n", nError);
}



void TanhLayer_GPU_forward(std::size_t nSize, CUdeviceptr pInput, CUdeviceptr pOutput)
{
	uint3 sDimGrid;
	uint3 sDimBlock;

	sDimGrid.x = (nSize - 1) / 1024 + 1;
	sDimGrid.y = 1;
	sDimGrid.z = 1;

	sDimBlock.x = nSize < 1024 ? nSize : 1024;
	sDimBlock.y = 1;
	sDimBlock.z = 1;

	kernel_TanhLayer_GPU_forward<<<sDimGrid, sDimBlock>>>(nSize, (const float *)pInput, (float *)pOutput);

	cudaError_t nError = cudaGetLastError();

	if (nError != cudaError_t::cudaSuccess)
		printf("TanhLayer_GPU_forward : %d\n", nError);
}

void TanhLayer_GPU_forwardBatch(std::size_t nBatchSize, std::size_t nSize, CUdeviceptr pInput, CUdeviceptr pOutput)
{
	uint3 sDimGrid;
	uint3 sDimBlock;

	sDimGrid.x = nBatchSize;
	sDimGrid.y = (nSize - 1) / 1024 + 1;
	sDimGrid.z = 1;

	sDimBlock.x = nSize < 1024 ? nSize : 1024;
	sDimBlock.y = 1;
	sDimBlock.z = 1;

	kernel_TanhLayer_GPU_forwardBatch<<<sDimGrid, sDimBlock>>>(nSize, (const float *)pInput, (float *)pOutput);

	cudaError_t nError = cudaGetLastError();

	if (nError != cudaError_t::cudaSuccess)
		printf("TanhLayer_GPU_forwardBatch : %d\n", nError);
}

void TanhLayer_GPU_backwardBatch(std::size_t nBatchSize, std::size_t nSize, CUdeviceptr pForwardInput, CUdeviceptr pBackwardInput, CUdeviceptr pBackwardOutput)
{
	uint3 sDimGrid;
	uint3 sDimBlock;

	sDimGrid.x = nBatchSize;
	sDimGrid.y = (nSize - 1) / 1024 + 1;
	sDimGrid.z = 1;

	sDimBlock.x = nSize < 1024 ? nSize : 1024;
	sDimBlock.y = 1;
	sDimBlock.z = 1;

	kernel_TanhLayer_GPU_backwardBatch<<<sDimGrid, sDimBlock>>>(nSize, (const float *)pForwardInput, (const float *)pBackwardInput, (float *)pBackwardOutput);

	cudaError_t nError = cudaGetLastError();

	if (nError != cudaError_t::cudaSuccess)
		printf("TanhLayer_GPU_backwardBatch : %d\n", nError);
}

#pragma endregion