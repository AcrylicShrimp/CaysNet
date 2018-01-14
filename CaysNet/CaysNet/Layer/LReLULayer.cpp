
/*
	2018.01.14
	Created by AcrylicShrimp.
*/

#include "LReLULayer.h"

namespace CaysNet::Layer
{
	LReLULayer::LReLULayer(std::size_t nFanIn) :
		Layer(nFanIn, nFanIn)
	{
		//Empty.
	}

	LReLULayer::LReLULayer(const LReLULayer &sSrc) :
		Layer(sSrc)
	{
		//Empty.
	}

	LReLULayer &LReLULayer::operator=(const LReLULayer &sSrc)
	{
		if (&sSrc == this)
			return *this;

		this->Layer::operator=(sSrc);

		return *this;
	}

	std::unique_ptr<Layer> LReLULayer::duplicate() const
	{
		return std::make_unique<LReLULayer>(this->nFanIn);
	}

	void LReLULayer::initBias(std::function<float()> sGenerator)
	{
		//Empty.
	}

	void LReLULayer::initWeight(std::function<float()> sGenerator)
	{
		//Empty.
	}

	void LReLULayer::specifySize(std::size_t &nBiasDeltaSize, std::size_t &nWeightDeltaSize) const
	{
		nBiasDeltaSize = nWeightDeltaSize = 0;
	}

	void LReLULayer::forward(const float *pInput, float *pOutput) const
	{
		static constexpr float vDesk[2]{.01f, 1.f};

		for (std::size_t nIndex{0}; nIndex < this->nFanIn; ++nIndex)
			pOutput[nIndex] = vDesk[pInput[nIndex] > .0f] * pInput[nIndex];
	}

	void LReLULayer::forward(std::size_t nBatchSize, const std::vector<float> *pInput, std::vector<float> *pOutput) const
	{
		static constexpr float vDesk[2]{.01f, 1.f};

		for (std::size_t nBatch{0}; nBatch < nBatchSize; ++nBatch)
			for (std::size_t nIndex{0}; nIndex < this->nFanIn; ++nIndex)
				pOutput[nBatch][nIndex] = vDesk[pInput[nBatch][nIndex] > .0f] * pInput[nBatch][nIndex];
	}

	void LReLULayer::backward(const float *pForwardInput, const float *pBackwardInput, float *pBackwardOutput, float *pWeightDelta) const
	{
		static constexpr float vDesk[2]{.01f, 1.f};

		for (std::size_t nIndex{0}; nIndex < this->nFanIn; ++nIndex)
			pBackwardOutput[nIndex] = pBackwardInput[nIndex] * vDesk[pForwardInput[nIndex] > .0f];
	}

	void LReLULayer::backward(std::size_t nBatchSize, const std::vector<float> *pForwardInput, const std::vector<float> *pBackwardInput, std::vector<float> *pBackwardOutput, float *pWeightDelta) const
	{
		static constexpr float vDesk[2]{.01f, 1.f};

		for (std::size_t nBatch{0}; nBatch < nBatchSize; ++nBatch)
			for (std::size_t nIndex{0}; nIndex < this->nFanIn; ++nIndex)
				pBackwardOutput[nBatch][nIndex] = pBackwardInput[nBatch][nIndex] * vDesk[pForwardInput[nBatch][nIndex] > .0f];
	}

	void LReLULayer::update(const float *pBiasDelta, const float *pWeightDelta)
	{
		//Empty.
	}

	void LReLULayer::update(float nFactor, const float *pBiasDelta, const float *pWeightDelta)
	{
		//Empty.
	}

	void LReLULayer::serialize(std::ofstream &sOutput) const
	{
		IO::Serializable::write(sOutput, this->nFanIn);
	}

	void LReLULayer::deserialize(std::ifstream &sInput)
	{
		this->nFanIn = this->nFanOut = IO::Serializable::read<std::size_t>(sInput);
	}
}