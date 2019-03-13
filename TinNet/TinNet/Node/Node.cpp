
/*
	2018.10.21
	Created by AcrylicShrimp.
*/

#include "Node.h"

namespace TinNet::Node
{
	//Node::Node(Graph *pGraph, std::string_view sName) :
	//	pGraph{pGraph},
	//	sName{sName},
	//	sShape{0}
	//{
	//	//Empty.
	//}

	Node::Node(std::string_view sName) :
		sName{sName},
		bShapeDirty{true},
		bOutputDirty{true},
		pGradientDirty{nullptr}
	{
		//Empty.
	}

	NodeInput *Node::operator[](std::string_view sNodeInputName)
	{
		auto iIndex{this->sNodeInputMap.find(std::string{sNodeInputName})};

		return iIndex == this->sNodeInputMap.cend() ? nullptr : iIndex->second;
	}

	const NodeInput *Node::operator[](std::string_view sNodeInputName) const
	{
		auto iIndex{this->sNodeInputMap.find(std::string{sNodeInputName})};

		return iIndex == this->sNodeInputMap.cend() ? nullptr : iIndex->second;
	}

	Node &Node::markDirty()
	{
		this->bShapeDirty = true;
		this->bOutputDirty = true;
		this->pGradientDirty = nullptr;

		for (auto *pNode : this->sRevDeps)
		{
			pNode->bShapeDirty = true;
			pNode->bOutputDirty = true;
			pNode->pGradientDirty = nullptr;
		}

		return *this;
	}

	Node &Node::evalShape()
	{
		if (!this->bShapeDirty)
			return *this;

		for (const auto sPair : this->sNodeInputMap)
			sPair.second->inputNode()->evalShape();

		this->__evaluateShape();
		this->bShapeDirty = false;

		return *this;
	}

	Node &Node::evalOutput()
	{
		if (!this->bOutputDirty)
			return *this;

		this->sOutput.resize(this->evalShape().sShape.size());

		this->__evaluateOutput();
		this->bOutputDirty = false;

		return *this;
	}

	Node &Node::evalGradient(const Node *pDy)
	{
		if (this->pGradientDirty == pDy)
			return *this;

		this->sGradient.resize(this->evalShape().sShape.size());

		if (pDy == this)
		{
			this->sGradient.span().fillOne();
			this->pGradientDirty = pDy;

			return *this;
		}

		this->sGradient.span().fillZero();

		for (const auto *pRevNodeInput : this->sRevNodeInputList)
			if (pRevNodeInput->pNode == pDy || pRevNodeInput->pNode->hasRevDeps(pDy))
				pRevNodeInput->fBackwardOp(pDy);

		this->pGradientDirty = pDy;

		return *this;
	}
}