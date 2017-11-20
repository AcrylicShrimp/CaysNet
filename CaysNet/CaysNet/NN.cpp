
/*
	2017.08.20
	Created by AcrylicShrimp.
*/

#include "NN.h"

namespace CaysNet
{
	NN::NN(std::initializer_list<Layer::Layer *> sLayerList) :
		sLayerList(sLayerList)
	{
		for (const auto pLayer : this->sLayerList)
			this->sOutput.emplace_back(pLayer->fanOut(), .0f);
	}

	NN::NN(const NN &sSrc) :
		sOutput{sSrc.sOutput}
	{
		for (const auto pLayer : sSrc.sLayerList)
			this->sLayerList.emplace_back(pLayer->duplicate());
	}

	NN::NN(NN &&sSrc) :
		sLayerList{std::move(sSrc.sLayerList)},
		sOutput{std::move(sSrc.sOutput)}
	{
		//Empty.
	}

	NN::~NN()
	{
		for (auto pLayer : this->sLayerList)
			delete pLayer;

		this->sLayerList.clear();
		this->sOutput.clear();
	}

	NN &NN::operator=(const NN &sSrc)
	{
		if (&sSrc == this)
			return *this;

		this->~NN();

		this->sLayerList = sSrc.sLayerList;
		this->sOutput = sSrc.sOutput;

		return *this;
	}

	NN &NN::operator=(NN &&sSrc)
	{
		if (&sSrc == this)
			return *this;

		this->~NN();

		this->sLayerList = std::move(sSrc.sLayerList);
		this->sOutput = std::move(sSrc.sOutput);

		return *this;
	}

	void NN::forward(const float *pInput)
	{
		assert(pInput);
		assert(this->sLayerList.size());

		this->sLayerList.front()->forward(pInput, this->sOutput.front().data());

		for (std::size_t nIndex{1}, nSize{this->sLayerList.size()}; nIndex < nSize; ++nIndex)
			this->sLayerList[nIndex]->forward(this->sOutput[nIndex - 1].data(), this->sOutput[nIndex].data());
	}

	void NN::forward(const float *pInput, float *pOutput)
	{
		assert(pInput);
		assert(pOutput);

		this->forward(pInput);

		for (auto nValue : this->sOutput.back())
			*pOutput++ = nValue;
	}

	void NN::forward(
		const float *pInput,
		std::vector<std::vector<float>> &sActivationInputBuffer,
		std::vector<std::vector<float>> &sActivationOutputBuffer)
	{
		assert(pInput);
		assert(this->sLayerList.size());
		assert(this->sLayerList.size() == sActivationInputBuffer.size());
		assert(this->sLayerList.size() == sActivationOutputBuffer.size());

		this->sLayerList.front()->forward(pInput, this->sOutput.front().data(), sActivationInputBuffer.front().data(), sActivationOutputBuffer.front().data());

		for (std::size_t nIndex{1}, nSize{this->sLayerList.size()}; nIndex < nSize; ++nIndex)
			this->sLayerList[nIndex]->forward(this->sOutput[nIndex - 1].data(), this->sOutput[nIndex].data(), sActivationInputBuffer[nIndex].data(), sActivationOutputBuffer[nIndex].data());
	}

	void NN::forward(
		std::size_t nBatchSize,
		const std::vector<float> *pInput,
		std::vector<std::vector<float>> *pOutput) const
	{
		assert(pInput);
		assert(this->sLayerList.size());

		this->sLayerList.front()->forward(nBatchSize, pInput, pOutput->data());

		for (std::size_t nIndex{1}, nDepth{this->depth()}; nIndex < nDepth; ++nIndex)
			this->sLayerList[nIndex]->forward(nBatchSize, pOutput[nIndex - 1].data(), pOutput[nIndex].data());
	}

	void NN::forward(
		std::size_t nBatchSize,
		const std::vector<float> *pInput,
		std::vector<std::vector<float>> *pOutput,
		std::vector<std::vector<float>> *pActivationInput,
		std::vector<std::vector<float>> *pActivationOutput) const
	{
		assert(pInput);
		assert(this->sLayerList.size());
		assert(nBatchSize == pOutput->size());
		assert(nBatchSize == pActivationInput->size());
		assert(nBatchSize == pActivationOutput->size());

		this->sLayerList.front()->forward(nBatchSize, pInput, pOutput->data(), pActivationInput->data(), pActivationOutput->data());

		for (std::size_t nIndex{1}, nDepth{this->depth()}; nIndex < nDepth; ++nIndex)
			this->sLayerList[nIndex]->forward(nBatchSize, pOutput[nIndex - 1].data(), pOutput[nIndex].data(), pActivationInput[nIndex].data(), pActivationOutput[nIndex].data());
	}

	void NN::backward(
		const std::vector<std::vector<float>> &sActivationInputBuffer,
		const std::vector<std::vector<float>> &sActivationOutputBuffer,
		std::vector<std::vector<float>> &sBiasDeltaBuffer,
		std::vector<std::vector<float>> &sWeightDeltaBuffer,
		std::vector<std::vector<float>> &sBackwardBuffer,
		const float *pForwardInput,
		const float *pBackwardInput) const
	{
		for (std::size_t nIndex{this->sLayerList.size() - 1}; ; --nIndex)
		{
			auto pLayer{this->sLayerList[nIndex]};
			auto pLayerForwardInput{nIndex == 0 ? pForwardInput : this->sOutput[nIndex - 1].data()};
			auto pLayerBackwardInput{nIndex + 1 == this->sLayerList.size() ? pBackwardInput : sBackwardBuffer[nIndex + 1].data()};
			auto pLayerBackwardOutput{sBackwardBuffer[nIndex].data()};

			pLayer->backward(
				sActivationInputBuffer[nIndex].data(),
				sActivationOutputBuffer[nIndex].data(),
				pLayerForwardInput,
				pLayerBackwardInput,
				pLayerBackwardOutput,
				sBiasDeltaBuffer[nIndex].data(),
				sWeightDeltaBuffer[nIndex].data());

			if (!nIndex)
				break;
		}
	}

	void NN::backward(
		std::size_t nBatchSize,
		const std::vector<float> *pForwardInput,
		const std::vector<float> *pBackwardInput,
		const std::vector<std::vector<float>> *pForwardOutput,
		std::vector<std::vector<float>> *pBackwardOutput,
		const std::vector<std::vector<float>> *pActivationInput,
		const std::vector<std::vector<float>> *pActivationOutput,
		std::vector<float> *pBiasDelta,
		std::vector<float> *pWeightDelta,
		std::vector<float> *pBiasDeltaBuffer) const
	{
		for (std::size_t nIndex{this->sLayerList.size() - 1}; ; --nIndex)
		{
			auto pLayer{this->sLayerList[nIndex]};
			auto pLayerForwardInput{nIndex == 0 ? pForwardInput : pForwardOutput[nIndex - 1].data()};
			auto pLayerBackwardInput{nIndex + 1 == this->depth() ? pBackwardInput : pBackwardOutput[nIndex + 1].data()};
			auto pLayerBackwardOutput{pBackwardOutput[nIndex].data()};

			pLayer->backward(
				nBatchSize,
				pActivationInput[nIndex].data(),
				pActivationOutput[nIndex].data(),
				pLayerForwardInput,
				pLayerBackwardInput,
				pLayerBackwardOutput,
				pBiasDelta[nIndex].data(),
				pWeightDelta[nIndex].data(),
				pBiasDeltaBuffer[nIndex].data());

			if (!nIndex)
				break;
		}
	}

	std::size_t NN::classify(const float *pInput)
	{
		assert(this->sLayerList.size());

		this->forward(pInput);

		std::size_t nMaxIndex{0};
		auto nMaxValue{this->sOutput.back().front()};

		for (std::size_t nOut{1}, nOutSize{this->sOutput.back().size()}; nOut < nOutSize; ++nOut)
			if (nMaxValue < this->sOutput.back()[nOut])
				nMaxValue = this->sOutput.back()[nMaxIndex = nOut];

		return nMaxIndex;
	}

	void NN::classify(std::size_t nBatchSize, const std::vector<float> *pInput, std::size_t *pOutput)
	{
		assert(this->sLayerList.size());

		std::size_t nIndex{0};
		std::vector<std::vector<std::vector<float>>> sOutput(this->depth());

		for (auto &sLayerBuffer : sOutput)
		{
			sLayerBuffer.resize(nBatchSize);

			for (auto &sOutputBuffer : sLayerBuffer)
				sOutputBuffer.resize(this->sLayerList[nIndex]->fanOut());

			++nIndex;
		}

		this->forward(nBatchSize, pInput, sOutput.data());

		for (std::size_t nBatch{0}; nBatch < nBatchSize; ++nBatch)
		{
			std::size_t nMaxIndex{0};
			auto nMaxValue{sOutput.back()[nBatch].front()};

			for (std::size_t nOut{1}, nOutSize{sOutput.back()[nBatch].size()}; nOut < nOutSize; ++nOut)
				if (nMaxValue < sOutput.back()[nBatch][nOut])
					nMaxValue = sOutput.back()[nBatch][nMaxIndex = nOut];

			*pOutput++ = nMaxIndex;
		}
	}

	float NN::classificationLoss(std::size_t nBatchSize, const std::vector<float> *pInput, const std::vector<float> *pOutput)
	{
		assert(this->sLayerList.size());

		std::vector<std::size_t> sResult(nBatchSize);
		this->classify(nBatchSize, pInput, sResult.data());

		auto nResult{.0f};

		for (std::size_t nIndex{0u}; nIndex < nBatchSize; ++nIndex)
			nResult += pOutput[nIndex][sResult[nIndex]];

		return 1.f - nResult / nBatchSize;
	}

	void NN::serialize(std::ofstream &sOutput) const
	{
		//IO::Serializable::write(sOutput, this->sLayerList.size());
		//
		//for (auto &pLayer : this->sLayerList)
		//	pLayer->serialize(sOutput);
	}

	void NN::deserialize(std::ifstream &sInput)
	{
		//this->sLayerList.clear();
		//this->sOutput.clear();
		//
		//for (std::size_t nIndex{0}, nSize{IO::Serializable::read<std::size_t>(sInput)}; nIndex < nSize; ++nIndex)
		//{
		//	auto nFanIn{IO::Serializable::read<std::size_t>(sInput)};
		//	auto nFanOut{IO::Serializable::read<std::size_t>(sInput)};
		//
		//	this->sLayerList.emplace_back(nFanIn, nFanOut, Activation::Activations::createByName(IO::Serializable::readWideString(sInput), sInput));
		//	this->sOutput.emplace_back(nFanOut, .0f);
		//
		//	this->sLayerList.back().deserialize(sInput);
		//}
	}
}