
/*
	2018.05.27
	Created by AcrylicShrimp.
*/

#include "DivideBP.h"

namespace TinNet::Graph::Node
{
	DivideBP::DivideBP(const std::string &sName, const GraphNodeBP *pLeftNodeBP, const GraphNodeBP *pRightNodeBP) :
		GraphNodeBP(sName)
	{
		this->asBackwardNode(pLeftNodeBP);
		this->asBackwardNode(pRightNodeBP);
	}

	std::unique_ptr<GraphNode> DivideBP::make(class Graph *pGraph) const
	{
		return std::make_unique<Divide>(this->sName, pGraph);
	}
}