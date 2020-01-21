
#include "tinnet/includes/node/Builder.h"

#include "tinnet/includes/node/kernel/Add.h"

#include <algorithm>
#include <cstdint>
#include <utility>
#include <vector>

namespace tinnet::node {
	// std::unique_ptr<Node> Builder::wrap(Type eType, Shape &&sShape, bool bGradientEnabled, std::uint8_t *pOutput)
	// {
	// 	return std::make_unique<Node>(
	// 		eType,
	// 		std::move(sShape),
	// 		bGradientEnabled,
	// 		pOutput,
	// 		std::vector<Node *>{},
	// 		std::vector<Node::GFunc>{});
	// }

	std::unique_ptr<Node> Builder::memory(Shape &&sShape, const float *pSource, bool bGradientEnabled)
	{
		memory::ScopedStorage sOutput{sizeof(float) * sShape.size()};
		std::copy(pSource, pSource + sShape.size(), sOutput.aligned<float>());

		return std::make_unique<Node>(
			Type::F32,
			std::move(sShape),
			bGradientEnabled,
			std::move(sOutput),
			std::vector<Node *>{},
			std::vector<Node::GFunc>{});
	}

	std::unique_ptr<Node>
		Builder::add(const std::unique_ptr<Node> &sLeft, const std::unique_ptr<Node> &sRight, bool bGradientEnabled)
	{
		return std::make_unique<Node>(
			Type::F32,
			Shape{sLeft->sShape},
			bGradientEnabled,
			kernel::__kernel__add(sLeft.get(), sRight.get()),
			std::vector<Node *>{sLeft.get(), sRight.get()},
			std::vector<Node::GFunc>{&kernel::__kernel__addGradient, &kernel::__kernel__addGradient});
	}
}	 // namespace tinnet::node