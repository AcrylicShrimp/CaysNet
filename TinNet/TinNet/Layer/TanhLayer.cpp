
/*
	2018.01.14
	Created by AcrylicShrimp.
*/

#include "TanhLayer.h"

namespace TinNet::Layer
{
	TanhLayer::TanhLayer(std::size_t nFanIn) :
		Layer(nFanIn, nFanIn)
	{
		//Empty.
	}
	
	TanhLayer::TanhLayer(const TanhLayer &sSrc) :
		Layer(sSrc)
	{
		//Empty.
	}

	TanhLayer &TanhLayer::operator=(const TanhLayer &sSrc)
	{
		if (&sSrc == this)
			return *this;
		
		this->Layer::operator=(sSrc);
		
		return *this;
	}

	const char *TanhLayer::name() const
	{
		return "TanhLayer";
	}

	std::unique_ptr<Layer> TanhLayer::duplicate() const
	{
		return std::make_unique<TanhLayer>(*this);
	}

	void TanhLayer::initBias(std::function<float()> sGenerator)
	{
		//Empty.
	}

	void TanhLayer::initWeight(std::function<float()> sGenerator)
	{
		//Empty.
	}

	void TanhLayer::specifySize(std::size_t &nBiasDeltaSize, std::size_t &nWeightDeltaSize) const
	{
		nBiasDeltaSize = nWeightDeltaSize = 0;
	}

	void TanhLayer::forward(const float *pInput, float *pOutput) const
	{
		for (std::size_t nIndex{0}; nIndex < this->nFanIn; ++nIndex)
			pOutput[nIndex] = std::tanh(pInput[nIndex]);
	}

	void TanhLayer::forward(std::size_t nBatchSize, const std::vector<float> *pInput, std::vector<float> *pOutput, bool bTrainingPhase) const
	{
		for (std::size_t nBatch{0}; nBatch < nBatchSize; ++nBatch)
			for (std::size_t nIndex{0}; nIndex < this->nFanIn; ++nIndex)
				pOutput[nBatch][nIndex] = std::tanh(pInput[nBatch][nIndex]);
	}

	void TanhLayer::backward(std::size_t nBatchSize, const std::vector<float> *pForwardInput, const std::vector<float> *pBackwardInput, std::vector<float> *pBackwardOutput, float *pBiasDelta, float *pWeightDelta) const
	{
		for (std::size_t nBatch{0}; nBatch < nBatchSize; ++nBatch)
			for (std::size_t nIndex{0}; nIndex < this->nFanIn; ++nIndex)
			{
				auto nValue{std::tanh(pForwardInput[nBatch][nIndex])};
				pBackwardOutput[nBatch][nIndex] = pBackwardInput[nBatch][nIndex] * (1.f - nValue * nValue);
			}
	}

	void TanhLayer::backward(std::size_t nBatchSize, const std::vector<float> *pForwardInput, const std::vector<float> *pBackwardInput, std::vector<float> *pBackwardOutput, float *pBiasDelta, float *pWeightDelta, const float *pFactor) const
	{
		for (std::size_t nBatch{0}; nBatch < nBatchSize; ++nBatch)
			for (std::size_t nIndex{0}; nIndex < this->nFanIn; ++nIndex)
			{
				auto nValue{std::tanh(pForwardInput[nBatch][nIndex])};
				pBackwardOutput[nBatch][nIndex] = pBackwardInput[nBatch][nIndex] * (1.f - nValue * nValue);
			}
	}

	void TanhLayer::update(const float *pBiasDelta, const float *pWeightDelta)
	{
		//Empty.
	}

	void TanhLayer::update(float nFactor, const float *pBiasDelta, const float *pWeightDelta)
	{
		//Empty.
	}
}