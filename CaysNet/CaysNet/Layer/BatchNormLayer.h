
/*
	2018.01.15
	Created by AcrylicShrimp.
*/

#ifndef _CLASS_CAYSNET_OPTIMIZER_SUPERVISED_BATCHNORMLAYER_H

#define _CLASS_CAYSNET_OPTIMIZER_SUPERVISED_BATCHNORMLAYER_H

#include "../CaysNetDLL.h"

#include "../Layer/Layer.h"
#include "../IO/Serializable.h"
#include "../Vector.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

namespace CaysNet::Layer
{
	class CAYSNET_DLL BatchNormLayer final : public Layer
	{
	protected:
		float nMomentum;
		mutable std::vector<float> sShift;
		mutable std::vector<float> sScale;
		mutable std::vector<float> sMean;
		mutable std::vector<float> sVariance;
		mutable std::vector<float> sMeanBuffer;
		mutable std::vector<float> sVarianceBuffer;
		mutable std::vector<float> sVarianceInversed;

	public:
		BatchNormLayer(std::size_t nFanIn, float nNewMomentum);
		BatchNormLayer(const BatchNormLayer &sSrc);
		BatchNormLayer(BatchNormLayer &&sSrc);
		~BatchNormLayer() = default;
		
	public:
		BatchNormLayer &operator=(const BatchNormLayer &sSrc);
		BatchNormLayer &operator=(BatchNormLayer &&sSrc);
		
	public:
		virtual std::unique_ptr<Layer> duplicate() const override;
		virtual void initBias(std::function<float()> sGenerator) override;
		virtual void initWeight(std::function<float()> sGenerator) override;
		virtual void specifySize(std::size_t &nBiasDeltaSize, std::size_t &nWeightDeltaSize) const override;
		virtual void forward(const float *pInput, float *pOutput) const override;
		virtual void forward(std::size_t nBatchSize, const std::vector<float> *pInput, std::vector<float> *pOutput) const override;
		virtual void backward(std::size_t nBatchSize, const std::vector<float> *pForwardInput, const std::vector<float> *pBackwardInput, std::vector<float> *pBackwardOutput, float *pWeightDelta) const override;
		virtual void update(const float *pBiasDelta, const float *pWeightDelta) override;
		virtual void update(float nFactor, const float *pBiasDelta, const float *pWeightDelta) override;
		virtual void serialize(std::ofstream &sOutput) const override;
		virtual void deserialize(std::ifstream &sInput) override;
	};
}

#endif