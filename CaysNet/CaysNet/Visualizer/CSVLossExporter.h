
/*
	2017.08.20
	Created by AcrylicShrimp.
*/

#ifndef _CLASS_CAYSNET_VISUALIZER_CSV_LOSS_EXPORTER_H

#define _CLASS_CAYSNET_VISUALIZER_CSV_LOSS_EXPORTER_H

#include "../CaysNetDLL.h"

#include <chrono>
#include <cstddef>
#include <ctime>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

namespace CaysNet::Visualizer
{
	class CAYSNET_DLL CSVLossExporter final
	{
	private:
		std::vector<float> sLoss;
		std::vector<std::chrono::system_clock::time_point> sTimepoint;
		
	public:
		CSVLossExporter() = default;
		CSVLossExporter(const CSVLossExporter &sSrc) = delete;
		CSVLossExporter(CSVLossExporter &&sSrc);
		~CSVLossExporter() = default;
		
	public:
		CSVLossExporter &operator=(const CSVLossExporter &sSrc) = delete;
		CSVLossExporter &operator=(CSVLossExporter &&sSrc) = delete;
		
	public:
		inline void accrueLoss(float nLoss);
		void exportCSV(std::ofstream &sOutput) const;
	};

	inline void CSVLossExporter::accrueLoss(float nLoss)
	{
		this->sLoss.emplace_back(nLoss);
		this->sTimepoint.emplace_back(std::chrono::system_clock::now());
	}
}

#endif