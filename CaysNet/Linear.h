
/*
	2017.08.04
	Created by AcrylicShrimp.
*/

#ifndef _CLASS_CAYSNET_ACTIVATION_LINEAR_H

#define _CLASS_CAYSNET_ACTIVATION_LINEAR_H

#include "Activation.h"

namespace CaysNet::Activation
{
	class Linear final : public Activation
	{
	public:
		Linear() = default;
		Linear(const Linear &sSrc) = default;
		Linear(Linear &&sSrc) = default;
		~Linear() = default;
		
	public:
		Linear &operator=(const Linear &sSrc) = default;
		Linear &operator=(Linear &&sSrc) = default;
		
	public:
		virtual Activation *duplicate() const override;
		virtual void activate(const Layer *pLayer, float *pOutput) const override;
		virtual void derivative(
			std::size_t nLength,
			const float *pActivationInput,
			const float *pActivationOutput,
			const float *pDerivativeInput,
			float *pResult) const override;
	};
}

#endif