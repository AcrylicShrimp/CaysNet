
/*
	2018.01.13
	Created by AcrylicShrimp.
*/

#include "SigmoidLayer.h"

namespace TinNet::Layer
{
	SigmoidLayer::SigmoidLayer(std::size_t nFanIn) :
		Layer(nFanIn, nFanIn)
	{
		//Empty.
	}

	SigmoidLayer::SigmoidLayer(const SigmoidLayer &sSrc) :
		Layer(sSrc)
	{
		//Empty.
	}

	SigmoidLayer &SigmoidLayer::operator=(const SigmoidLayer &sSrc)
	{
		if (&sSrc == this)
			return *this;

		this->Layer::operator=(sSrc);

		return *this;
	}

	const char *SigmoidLayer::name() const
	{
		return "SigmoidLayer";
	}

	std::unique_ptr<Layer> SigmoidLayer::duplicate() const
	{
		return std::make_unique<SigmoidLayer>(*this);
	}

	void SigmoidLayer::initBias(std::function<float()> sGenerator)
	{
		//Empty.
	}

	void SigmoidLayer::initWeight(std::function<float()> sGenerator)
	{
		//Empty.
	}

	void SigmoidLayer::specifySize(std::size_t &nBiasDeltaSize, std::size_t &nWeightDeltaSize) const
	{
		nBiasDeltaSize = nWeightDeltaSize = 0;
	}

	void SigmoidLayer::forward(const float *pInput, float *pOutput) const
	{
		for (std::size_t nIndex{0}; nIndex < this->nFanIn; ++nIndex)
			pOutput[nIndex] = 1.f / (std::exp(-pInput[nIndex]) + 1.f);
	}

	void SigmoidLayer::forward(std::size_t nBatchSize, const std::vector<float> *pInput, std::vector<float> *pOutput, bool bTrainingPhase) const
	{
		for (std::size_t nBatch{0}; nBatch < nBatchSize; ++nBatch)
			for (std::size_t nIndex{0}; nIndex < this->nFanIn; ++nIndex)
				pOutput[nBatch][nIndex] = 1.f / (std::exp(-pInput[nBatch][nIndex]) + 1.f);
	}

	void SigmoidLayer::backward(std::size_t nBatchSize, const std::vector<float> *pForwardInput, const std::vector<float> *pBackwardInput, std::vector<float> *pBackwardOutput, float *pWeightDelta) const
	{
		for (std::size_t nBatch{0}; nBatch < nBatchSize; ++nBatch)
			for (std::size_t nIndex{0}; nIndex < this->nFanIn; ++nIndex)
			{
				auto nValue{1.f / (std::exp(-pForwardInput[nBatch][nIndex]) + 1.f)};
				pBackwardOutput[nBatch][nIndex] = pBackwardInput[nBatch][nIndex] * nValue * (1.f - nValue);
			}
	}

	void SigmoidLayer::update(const float *pBiasDelta, const float *pWeightDelta)
	{
		//Empty.
	}

	void SigmoidLayer::update(float nFactor, const float *pBiasDelta, const float *pWeightDelta)
	{
		//Empty.
	}
}