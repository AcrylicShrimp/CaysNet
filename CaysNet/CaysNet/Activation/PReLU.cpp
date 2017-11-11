
/*
	2017.08.04
	Created by AcrylicShrimp.
*/

#include "PReLU.h"

namespace CaysNet::Activation
{
	PReLU::PReLU(float nNewFactor) :
		nFactor{nNewFactor}
	{
		//Empty.
	}

	PReLU::PReLU(const PReLU &sSrc) :
		nFactor{sSrc.nFactor}
	{
		//Empty.
	}

	PReLU &PReLU::operator=(const PReLU &sSrc)
	{
		if (&sSrc == this)
			return *this;

		this->nFactor = sSrc.nFactor;

		return *this;
	}

	PReLU &PReLU::operator=(float nNewFactor)
	{
		this->nFactor = nNewFactor;

		return *this;
	}

	const wchar_t *PReLU::name() const
	{
		return L"PReLU";
	}

	Activation *PReLU::duplicate() const
	{
		return new PReLU(this->nFactor);
	}

	void PReLU::activate(std::size_t nFanOut, float *pOutput) const
	{
		float vDesk[2]{this->nFactor, 1.f};

		for (std::size_t nIndex{0}; nIndex < nFanOut; ++nIndex)
			pOutput[nIndex] = vDesk[pOutput[nIndex] > .0f] * pOutput[nIndex];
	}

	void PReLU::derivative(
		std::size_t nLength,
		const float *pActivationInput,
		const float *pActivationOutput,
		const float *pDerivativeInput,
		float *pResult) const
	{
		for (std::size_t nIndex{0}; nIndex < nLength; ++nIndex)
		{
			float vDesk[2]{this->nFactor, 1.f};

			pResult[nIndex] = pDerivativeInput[nIndex] * vDesk[pActivationInput[nIndex] > .0f];
		}
	}

	void PReLU::serialize(std::ofstream &sOutput) const
	{
		IO::Serializable::write(sOutput, this->nFactor);
	}

	void PReLU::deserialize(std::ifstream &sInput)
	{
		this->nFactor = IO::Serializable::read<float>(sInput);
	}
}