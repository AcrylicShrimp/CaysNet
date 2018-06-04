
/*
	2018.06.03
	Created by AcrylicShrimp.
*/

#ifndef _CLASS_TINNET_GRAPH_NODE_REDUCEMEAN_H

#define _CLASS_TINNET_GRAPH_NODE_REDUCEMEAN_H

#include "../../TinNetDLL.h"

#include "../../Accessor.h"
#include "../../Iterator.h"
#include "../../Shape.h"
#include "../Graph.h"
#include "../FullCachedGraphNode.h"

#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

namespace TinNet::Graph::Node
{
	class TINNET_DLL ReduceMean final : public FullCachedGraphNode
	{
	protected:
		Shape sShape;
		Shape sUnsqueezedShape;
		Iterator<Accessor, Accessor> sIterator;
		float nFactor;
		std::vector<bool> sAxis;
		bool bSqueeze;
		
	public:
		ReduceMean(const std::string &sName, Graph *pGraph, const std::vector<bool> &sAxis, bool bSqueeze);
		ReduceMean(const ReduceMean &sSrc) = default;
		~ReduceMean() = default;
		
	public:
		ReduceMean &operator=(const ReduceMean &sSrc) = default;
		
	public:
		virtual const Shape &shape() const override;
		virtual std::size_t maxBackwardNodeCount() const override;
		virtual void initNode() override;
		virtual void forwardPass(Cache sDestination) override;
		virtual void backwardPass(GraphNode *pBackward, Cache sDestination) override;
	};
}

#endif