
/*
	2019.06.20
	Created by AcrylicShrimp.
*/

#include "Mean.h"

namespace TinNet::Node
{
	TINNET_NODE_TYPE_DEF(Mean)
	
	Mean::Mean(Core::Graph *pGraph, std::string_view sName, bool bSqueeze, const std::vector<bool> &sReduceAxis) :
		Node(pGraph, sName),
		bSqueeze{bSqueeze},
		sReduceAxis{sReduceAxis},
		sInput{this, "input", [this](const auto *pDy) { this->__backwardOpInput(pDy); }},
		nMeanFactor{.0f}
	{
		this->sNodeInputMap["input"] = &this->sInput;
	}

	void Mean::__evaluateShape()
	{
		if (!this->sInput)
			throw std::runtime_error{"no node attached at 'input'"};

		const auto &sShape{this->sInput.inputNode()->evalShape().shape()};

		if (!this->sReduceAxis.size())
		{
			this->sShape = {1};

			if (this->bSqueeze)
				this->sShape = this->sShape.squeeze();

			return;
		}

		if (this->sReduceAxis.size() == 1)
		{
			this->sShape = this->sReduceAxis[0] ? Core::Shape{1} : sShape;

			if (this->bSqueeze)
				this->sShape = this->sShape.squeeze();

			return;
		}

		if (this->sReduceAxis.size() != sShape.rank())
			throw std::runtime_error{"the length of 'reduce axis' must be equal to the rank of 'input'"};

		this->sShape = sShape;

		std::vector<std::size_t> sMultipliedShape{1};

		for (std::size_t nIndex{0}, nMaxIndex{sShape.rank() - 1}; nIndex < nMaxIndex; ++nIndex)
			sMultipliedShape.emplace_back(sMultipliedShape.back() * sShape[nIndex]);

		this->sIndexFactorList.clear();

		std::size_t nFactorSize{1};

		for (std::size_t nIndex{0}, nMaxIndex{this->sReduceAxis.size()}; nIndex < nMaxIndex; ++nIndex)
			if (this->sReduceAxis[nIndex])
			{
				this->sShape[nIndex] = 1;
				nFactorSize *= static_cast<float>(sShape[nIndex]);
			}
			else
				this->sIndexFactorList.emplace_back(sShape[nIndex], sMultipliedShape[nIndex], this->sIndexFactorList.size() ? std::get<0>(this->sIndexFactorList.back()) * std::get<2>(this->sIndexFactorList.back()) : 1);

		this->nMeanFactor = 1.f / nFactorSize;

		if (this->bSqueeze)
			this->sShape = this->sShape.squeeze();
	}

	void Mean::__evaluateOutput()
	{
		this->sInput.inputNode()->evalOutput();
		this->sOutput.span().fillZero();

		if (!this->sReduceAxis.size())
		{
			for (std::size_t nIndex{0}, nMaxIndex{this->sInput.inputNode()->output().length()}; nIndex < nMaxIndex; ++nIndex)
				this->sOutput.span()[0] += this->nMeanFactor * this->sInput.inputNode()->output()[nIndex];

			return;
		}

		if (this->sReduceAxis.size() == 1)
		{
			if (this->sReduceAxis[0])
				for (std::size_t nIndex{0}, nMaxIndex{this->sInput.inputNode()->output().length()}; nIndex < nMaxIndex; ++nIndex)
					this->sOutput.span()[0] += this->nMeanFactor * this->sInput.inputNode()->output()[nIndex];
			else
				for (std::size_t nIndex{0}, nMaxIndex{this->sInput.inputNode()->output().length()}; nIndex < nMaxIndex; ++nIndex)
					this->sOutput.span()[nIndex] += this->sInput.inputNode()->output()[nIndex];

			return;
		}

		auto fReduceIndex{[this](std::size_t nIndex)
		{
			std::size_t nResult{0};

			for (const auto &sIndexFactorTuple : this->sIndexFactorList)
				nResult += nIndex / std::get<1>(sIndexFactorTuple) % std::get<0>(sIndexFactorTuple) * std::get<2>(sIndexFactorTuple);

			return nResult;
		}};

		for (std::size_t nIndex{0}, nMaxIndex{this->sInput.inputNode()->output().length()}; nIndex < nMaxIndex; ++nIndex)
			this->sOutput.span()[fReduceIndex(nIndex)] += this->nMeanFactor * this->sInput.inputNode()->output()[nIndex];
	}

	void Mean::__backwardOpInput(const Node *pDy)
	{
		this->evalGradient(pDy);

		if (!this->sReduceAxis.size())
		{
			for (std::size_t nIndex{0}, nMaxIndex{this->sInput.inputNode()->output().length()}; nIndex < nMaxIndex; ++nIndex)
				this->sInput.inputNode()->gradient()[nIndex] += this->nMeanFactor * this->sGradient.span()[0];

			return;
		}

		if (this->sReduceAxis.size() == 1)
		{
			if (this->sReduceAxis[0])
				for (std::size_t nIndex{0}, nMaxIndex{this->sInput.inputNode()->output().length()}; nIndex < nMaxIndex; ++nIndex)
					this->sInput.inputNode()->gradient()[nIndex] += this->nMeanFactor * this->sGradient.span()[0];
			else
				for (std::size_t nIndex{0}, nMaxIndex{this->sInput.inputNode()->output().length()}; nIndex < nMaxIndex; ++nIndex)
					this->sInput.inputNode()->gradient()[nIndex] += this->sGradient.span()[nIndex];

			return;
		}

		auto fReduceIndex{[this](std::size_t nIndex)
		{
			std::size_t nResult{0};

			for (const auto &sIndexFactorTuple : this->sIndexFactorList)
				nResult += nIndex / std::get<1>(sIndexFactorTuple) % std::get<0>(sIndexFactorTuple) * std::get<2>(sIndexFactorTuple);

			return nResult;
		}};

		for (std::size_t nIndex{0}, nMaxIndex{this->sInput.inputNode()->output().length()}; nIndex < nMaxIndex; ++nIndex)
			this->sInput.inputNode()->gradient()[nIndex] += this->nMeanFactor * this->sGradient.span()[fReduceIndex(nIndex)];
	}
}