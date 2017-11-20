
/*
	2017.11.03
	Created by AcrylicShrimp.
*/

#include "FullLayer.h"

namespace CaysNet::Layer
{
	FullLayer::FullLayer(Activation::Activation *pActivation, std::size_t nFanIn, std::size_t nFanOut) :
		Layer(pActivation, nFanIn, nFanOut),
		sWeight(nFanOut, std::vector<float>(nFanIn, .0f)),
		sBias(nFanOut, .0f)
	{
		//Empty.
	}

	FullLayer::FullLayer(const FullLayer &sSrc) :
		Layer(sSrc),
		sWeight{sSrc.sWeight},
		sBias{sSrc.sBias}
	{
		//Empty.
	}

	FullLayer::FullLayer(FullLayer &&sSrc) :
		Layer(sSrc),
		sWeight{std::move(sSrc.sWeight)},
		sBias{std::move(sSrc.sBias)}
	{
		//Empty.
	}

	FullLayer &FullLayer::operator=(const FullLayer &sSrc)
	{
		if (&sSrc == this)
			return *this;

		this->sWeight = sSrc.sWeight;
		this->sBias = sSrc.sBias;

		this->Layer::operator=(sSrc);

		return *this;
	}

	FullLayer &FullLayer::operator=(FullLayer &&sSrc)
	{
		if (&sSrc == this)
			return *this;

		this->sWeight = std::move(sSrc.sWeight);
		this->sBias = std::move(sSrc.sBias);

		this->Layer::operator=(std::move(sSrc));

		return *this;
	}

	Layer *FullLayer::duplicate() const
	{
		return new FullLayer(*this);
	}

	void FullLayer::initBias(std::function<float()> sGenerator)
	{
		for (auto &nBias : this->sBias)
			nBias = sGenerator();
	}

	void FullLayer::initWeight(std::function<float()> sGenerator)
	{
		for (auto &sWeightBuffer : this->sWeight)
			for (auto &nWeight : sWeightBuffer)
				nWeight = sGenerator();
	}

	void FullLayer::specifySize(std::size_t &nActivationInputSize, std::size_t &nActivationOutputSize, std::size_t &nBiasDeltaSize, std::size_t &nWeightDeltaSize) const
	{
		nWeightDeltaSize = this->nFanIn * (nActivationInputSize = nActivationOutputSize = nBiasDeltaSize = this->nFanOut);
	}

	void FullLayer::forward(const float *pInput, float *pOutput) const
	{
		assert(this->pActivation);

		//z = X * W + b
		for (std::size_t nOut{0}; nOut < this->nFanOut; ++nOut)
		{
			auto &nDestination{pOutput[nOut] = this->sBias[nOut]};

			for (std::size_t nIn{0}; nIn < this->nFanIn; ++nIn)
				nDestination += pInput[nIn] * this->sWeight[nOut][nIn];
		}

		//a = f(z)
		this->pActivation->activate(this->nFanOut, pOutput);
	}

	void FullLayer::forward(const float *pInput, float *pOutput, float *pActivationInput, float *pActivationOutput) const
	{
		assert(this->pActivation);

		//z = X * W + b
		for (std::size_t nOut{0}; nOut < this->nFanOut; ++nOut)
		{
			auto &nDestination{pOutput[nOut] = this->sBias[nOut]};

			for (std::size_t nIn{0}; nIn < this->nFanIn; ++nIn)
				nDestination += pInput[nIn] * this->sWeight[nOut][nIn];

			//Fill the activation input.
			pActivationInput[nOut] = nDestination;
		}

		//a = f(z)
		this->pActivation->activate(this->nFanOut, pOutput);

		//Fill the activation output.
		for (std::size_t nOut{0}; nOut < this->nFanOut; ++nOut)
			pActivationOutput[nOut] = pOutput[nOut];
	}

	void FullLayer::forward(std::size_t nBatchSize, const std::vector<float> *pInput, std::vector<float> *pOutput) const
	{
		assert(this->pActivation);

		//z = b
		for (std::size_t nBatch{0}; nBatch < nBatchSize; ++nBatch)
			std::copy(this->sBias.cbegin(), this->sBias.cend(), pOutput[nBatch].begin());

		//z += X * W
		for (std::size_t nBatch{0}; nBatch < nBatchSize; ++nBatch)
			for (std::size_t nOut{0}; nOut < this->nFanOut; ++nOut)
				for (std::size_t nIn{0}; nIn < this->nFanIn; ++nIn)
					pOutput[nBatch][nOut] += pInput[nBatch][nIn] * this->sWeight[nOut][nIn];

			//a = f(z)
		for (std::size_t nBatch{0}; nBatch < nBatchSize; ++nBatch)
			this->pActivation->activate(this->nFanOut, pOutput[nBatch].data());
	}

	void FullLayer::forward(std::size_t nBatchSize, const std::vector<float> *pInput, std::vector<float> *pOutput, std::vector<float> *pActivationInput, std::vector<float> *pActivationOutput) const
	{
		assert(this->pActivation);

		//z = b
		for (std::size_t nBatch{0}; nBatch < nBatchSize; ++nBatch)
			std::copy(this->sBias.cbegin(), this->sBias.cend(), pOutput[nBatch].begin());

		//z += X * W
		for (std::size_t nBatch{0}; nBatch < nBatchSize; ++nBatch)
			for (std::size_t nOut{0}; nOut < this->nFanOut; ++nOut)
				for (std::size_t nIn{0}; nIn < this->nFanIn; ++nIn)
					pOutput[nBatch][nOut] += pInput[nBatch][nIn] * this->sWeight[nOut][nIn];

		for (std::size_t nBatch{0}; nBatch < nBatchSize; ++nBatch)
			std::copy(pOutput[nBatch].cbegin(), pOutput[nBatch].cend(), pActivationInput[nBatch].begin());

		//a = f(z)
		for (std::size_t nBatch{0}; nBatch < nBatchSize; ++nBatch)
			this->pActivation->activate(this->nFanOut, pOutput[nBatch].data());

		for (std::size_t nBatch{0}; nBatch < nBatchSize; ++nBatch)
			std::copy(pOutput[nBatch].cbegin(), pOutput[nBatch].cend(), pActivationOutput[nBatch].begin());
	}

	void FullLayer::backward(const float *pActivationInput, const float *pActivationOutput, const float *pForwardInput, const float *pBackwardInput, float *pBackwardOutput, float *pBiasDelta, float *pWeightDelta) const
	{
		assert(this->pActivation);

		//Take the derivation of the activation function.
		this->pActivation->derivative(this->nFanOut, pActivationInput, pActivationOutput, pBackwardInput, pBiasDelta);

		for (std::size_t nOut{0}; nOut < this->nFanOut; ++nOut)
			for (std::size_t nIn{0}; nIn < this->nFanIn; ++nIn)
				pWeightDelta[nOut * this->nFanIn + nIn] = pBiasDelta[nOut] * pForwardInput[nIn];

		for (std::size_t nIn{0}; nIn < this->nFanIn; ++nIn)
			pBackwardOutput[nIn] = .0f;

		for (std::size_t nOut{0}; nOut < this->nFanOut; ++nOut)
			for (std::size_t nIn{0}; nIn < this->nFanIn; ++nIn)
				pBackwardOutput[nIn] += pBiasDelta[nOut] * this->sWeight[nOut][nIn];
	}

	void FullLayer::backward(std::size_t nBatchSize, const std::vector<float> *pActivationInput, const std::vector<float> *pActivationOutput, const std::vector<float> *pForwardInput, const std::vector<float> *pBackwardInput, std::vector<float> *pBackwardOutput, float *pBiasDelta, float *pWeightDelta, float *pBiasDeltaBuffer) const
	{
		assert(this->pActivation);

		for (std::size_t nBatch{0}; nBatch < nBatchSize; ++nBatch)
			std::fill(pBackwardOutput[nBatch].begin(), pBackwardOutput[nBatch].end(), .0f);

		//Take the derivation of the activation function.
		for (std::size_t nBatch{0}; nBatch < nBatchSize; ++nBatch)
		{
			this->pActivation->derivative(this->nFanOut, pActivationInput[nBatch].data(), pActivationOutput[nBatch].data(), pBackwardInput[nBatch].data(), pBiasDeltaBuffer);

			for (std::size_t nOut{0}; nOut < this->nFanOut; ++nOut)
				pBiasDelta[nOut] += pBiasDeltaBuffer[nOut];

			for (std::size_t nOut{0}; nOut < this->nFanOut; ++nOut)
				for (std::size_t nIn{0}; nIn < this->nFanIn; ++nIn)
					pWeightDelta[nOut * this->nFanIn + nIn] += pBiasDeltaBuffer[nOut] * pForwardInput[nBatch][nIn];

			for (std::size_t nOut{0}; nOut < this->nFanOut; ++nOut)
				for (std::size_t nIn{0}; nIn < this->nFanIn; ++nIn)
					pBackwardOutput[nBatch][nIn] += pBiasDeltaBuffer[nOut] * this->sWeight[nOut][nIn];
		}
	}

	void FullLayer::update(const float *pBiasDelta, const float *pWeightDelta)
	{
		for (std::size_t nOut{0}; nOut < this->nFanOut; ++nOut)
			this->sBias[nOut] += pBiasDelta[nOut];

		for (std::size_t nOut{0}; nOut < this->nFanOut; ++nOut)
			for (std::size_t nIn{0}; nIn < this->nFanIn; ++nIn)
				this->sWeight[nOut][nIn] += pWeightDelta[nOut * this->nFanIn + nIn];
	}

	void FullLayer::update(float nFactor, const float *pBiasDelta, const float *pWeightDelta)
	{
		for (std::size_t nOut{0}; nOut < this->nFanOut; ++nOut)
			this->sBias[nOut] += nFactor * pBiasDelta[nOut];

		for (std::size_t nOut{0}; nOut < this->nFanOut; ++nOut)
			for (std::size_t nIn{0}; nIn < this->nFanIn; ++nIn)
				this->sWeight[nOut][nIn] += nFactor * pWeightDelta[nOut * this->nFanIn + nIn];
	}

	void FullLayer::serialize(std::ofstream &sOutput) const
	{
		for (auto &sWeight : this->sWeight)
			for (auto nWeight : sWeight)
				IO::Serializable::write(sOutput, nWeight);

		for (auto nBias : this->sBias)
			IO::Serializable::write(sOutput, nBias);

		this->Layer::serialize(sOutput);
	}

	void FullLayer::deserialize(std::ifstream &sInput)
	{
		for (auto &sWeight : this->sWeight)
			for (auto &nWeight : sWeight)
				nWeight = IO::Serializable::read<float>(sInput);

		for (auto &nBias : this->sBias)
			nBias = IO::Serializable::read<float>(sInput);

		this->Layer::deserialize(sInput);
	}
}