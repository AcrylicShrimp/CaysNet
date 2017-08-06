
/*
	2017.08.04
	Created by AcrylicShrimp.
*/

#include "Linear.h"

namespace CaysNet::Activation
{
	void Linear::activate(const Layer *pLayer, float *pOutput) const
	{
		//Empty.
	}

	float Linear::derivative(float nZ, float nY) const
	{
		return 1.f;
	}

	Activation *Linear::duplicate() const
	{
		return new Linear();
	}
}