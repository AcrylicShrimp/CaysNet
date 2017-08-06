
/*
	2017.08.04
	Created by AcrylicShrimp.
*/

#include "Layer.h"

#include "Activation.h"

namespace CaysNet
{
	Layer::Layer(std::size_t nFanIn, std::size_t nFanOut, Activation::Activation *pNewActivation) :
		sWeight(nFanOut, std::vector<float>(nFanIn, .0f)),
		sBias(nFanOut, .0f),
		sOutput(nFanOut, .0f),
		pActivation{pNewActivation}
	{
		//Empty.
	}

	Layer::Layer(const Layer &sSrc) :
		sWeight{sSrc.sWeight},
		sBias{sSrc.sBias},
		sOutput{sSrc.sOutput},
		pActivation{sSrc.pActivation->duplicate()}
	{
		//Empty.
	}

	Layer::Layer(Layer &&sSrc) :
		sWeight{std::move(sSrc.sWeight)},
		sBias{std::move(sSrc.sBias)},
		sOutput{std::move(sSrc.sOutput)},
		pActivation{sSrc.pActivation}
	{
		sSrc.pActivation = nullptr;
	}

	Layer::~Layer()
	{
		if (this->pActivation)
			delete this->pActivation;

		this->pActivation = nullptr;
	}

	Layer &Layer::operator=(const Layer &sSrc)
	{
		if (&sSrc == this)
			return *this;

		this->~Layer();

		this->sWeight = sSrc.sWeight;
		this->sBias = sSrc.sBias;
		this->sOutput = sSrc.sOutput;
		this->pActivation = sSrc.pActivation->duplicate();

		return *this;
	}

	Layer &Layer::operator=(Layer &&sSrc)
	{
		if (&sSrc == this)
			return *this;

		this->~Layer();

		this->sWeight = std::move(sSrc.sWeight);
		this->sBias = std::move(sSrc.sBias);
		this->sOutput = std::move(sSrc.sOutput);
		this->pActivation = sSrc.pActivation;

		sSrc.~Layer();

		return *this;
	}

	void Layer::forward(const float *pInput, float *pOutput)
	{
		assert(this->pActivation);

		//z = X * W + b
		for (std::size_t nOut{0}, nOutSize{this->fanOut()}; nOut < nOutSize; ++nOut)
		{
			auto &nDestination{pOutput[nOut] = this->sBias[nOut]};

			for (std::size_t nIn{0}, nInSize{this->fanIn()}; nIn < nInSize; ++nIn)
				nDestination += pInput[nIn] * this->sWeight[nOut][nIn];

			this->sOutput[nOut] = nDestination;
		}

		//a = f(z)
		this->pActivation->activate(this, pOutput);
	}

	void Layer::backward(const float *pFrontOutput, const float *pBackInput, float *pBackOutput) const
	{
		assert(this->pActivation);

		//Backprop the sum of the differentials.
		for (std::size_t nIn{0}, nInSize{this->fanIn()}; nIn < nInSize; ++nIn)
			for (std::size_t nOut{0}, nOutSize{this->fanOut()}; nOut < nOutSize; ++nOut)	//SUM (W * derivation * derivation_before)
				pBackOutput[nIn] += this->sWeight[nOut][nIn] * this->pActivation->derivative(this->sOutput[nOut], pFrontOutput[nOut]) * pBackInput[nOut];
	}
}