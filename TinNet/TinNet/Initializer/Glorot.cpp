
/*
	2017.08.05
	Created by AcrylicShrimp.
*/

#include "Glorot.h"

namespace TinNet::Initializer
{
	Glorot::Glorot() :
		sEngine{static_cast<std::mt19937_64::result_type>(std::chrono::system_clock::now().time_since_epoch().count())}
	{
		//Empty.
	}

	Glorot::Glorot(std::mt19937_64::result_type nSeed) :
		sEngine{nSeed}
	{
		//Empty.
	}

	Glorot::Glorot(const Glorot &sSrc) :
		sEngine{sSrc.sEngine}
	{
		//Empty.
	}

	Glorot::Glorot(Glorot &&sSrc) :
		sEngine{std::move(sSrc.sEngine)}
	{
		//Empty.
	}

	Glorot &Glorot::operator=(const Glorot &sSrc)
	{
		if (&sSrc == this)
			return *this;

		this->sEngine = sSrc.sEngine;

		return *this;
	}

	Glorot &Glorot::operator=(Glorot &&sSrc)
	{
		if (&sSrc == this)
			return *this;

		this->sEngine = std::move(sSrc.sEngine);

		return *this;
	}

	void Glorot::initializeBias(Layer::LayerBase &sLayer)
	{
		std::normal_distribution<double> sDist{.0, std::sqrt(2. / (sLayer.fanIn() + sLayer.fanOut()))};

		sLayer.initBias([this, &sDist]()
		{
			return static_cast<float>(sDist(this->sEngine));
		});
	}

	void Glorot::initializeWeight(Layer::LayerBase &sLayer)
	{
		std::normal_distribution<double> sDist{.0, std::sqrt(2. / (sLayer.fanIn() + sLayer.fanOut()))};

		sLayer.initWeight([this, &sDist]()
		{
			return static_cast<float>(sDist(this->sEngine));
		});
	}
}