
/*
	2018.06.17
	Created by AcrylicShrimp.
*/

#ifndef _CLASS_TINNET_GRAPHNODE_INPUT_H

#define _CLASS_TINNET_GRAPHNODE_INPUT_H

#include "../TinNetDLL.h"

#include "../Cache.h"
#include "../Graph.h"
#include "../Feedable.h"
#include "../Shape.h"
#include "../ShapedCache.h"
#include "BackpropNode.h"

#include <cstddef>
#include <string>
#include <vector>

namespace TinNet::GraphNode
{
	class TINNET_DLL Input final : public BackpropNode, public Feedable
	{
	protected:
		ShapedCache sShapedCache;
		
	public:
		Input(Graph *pGraph, const std::string &sName, const Shape &sShape);
		Input(const Input &sSrc) = delete;
		~Input() = default;
		
	public:
		Input &operator=(const Input &sSrc) = delete;
		
	public:
		virtual Shape shape() const override;
		virtual std::string type() const override;
		virtual Cache forward() override;
		virtual void feed(const ShapedCache &sShapedCache) override;
		inline static std::string typeName();

	protected:
		virtual void forwardPass(Cache sDestination) override;
		virtual void backwardPass(Cache sDestination, NodePtr pInput) override;
	};

	inline std::string Input::typeName()
	{
		return "input";
	}
}

#endif