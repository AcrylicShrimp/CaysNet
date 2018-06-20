
/*
	2018.06.19
	Created by AcrylicShrimp.
*/

#ifndef _CLASS_TINNET_GRAPHNODE_SQUARE_H

#define _CLASS_TINNET_GRAPHNODE_SQUARE_H

#include "../TinNetDLL.h"

#include "../Cache.h"
#include "../Graph.h"
#include "../Shape.h"
#include "FullNode.h"

#include <cstddef>
#include <string>

namespace TinNet::GraphNode
{
	class TINNET_DLL Square final : public FullNode
	{
	public:
		Square(Graph *pGraph, const std::string &sName);
		Square(const Square &sSrc) = delete;
		~Square() = default;
		
	public:
		Square &operator=(const Square &sSrc) = delete;
		
	public:
		virtual const Shape &shape() const override;
		virtual std::string type() const override;
		inline static std::string typeName();

	protected:
		virtual void forwardPass(Cache sDestination) override;
		virtual void backwardPass(Cache sDestination, NodePtr pInput) override;
	};

	inline std::string Square::typeName()
	{
		return "square";
	}
}

#endif