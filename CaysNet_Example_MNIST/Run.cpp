
/*
	2017.08.23
	Created by AcrylicShrimp.
*/

#include "../CaysNet/CaysNet/CaysNet.h"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

int32_t main()
{
	using namespace CaysNet;

	NN sNetwork;

	for (std::string sCommand;;)
	{
		std::cout << "command : ";
		std::cin >> sCommand;

		if (sCommand == "read")
		{
			std::ifstream sInput{"network.cn", std::ifstream::binary | std::ifstream::in};

			if (!sInput.is_open())
			{
				std::cout << "Cannot open." << std::endl << std::endl;

				continue;
			}

			sNetwork.deserialize(sInput);

			std::cout << "Successfully loaded." << std::endl;
			std::cout << "Number of the layers : " << sNetwork.layer().size() << std::endl;
			std::cout << "For each layer :" << std::endl;

			std::size_t nCount{0};

			for (const auto &pLayer : sNetwork.layer())
			{
				std::cout << "\tSummary of the Layer No. " << ++nCount << " :" << std::endl;
				std::cout << "\tFan in : " << pLayer->fanIn() << std::endl;
				std::cout << "\tFan out : " << pLayer->fanOut() << std::endl;
				std::cout << "\tActivation : ";
				std::wcout << pLayer->activation()->name();
				std::cout << std::endl << std::endl;
			}

			break;
		}
		else if (sCommand == "new")
		{
			sNetwork.addLayer(Layer::FullLayer::make<Activation::LReLU>(784, 50));
			sNetwork.addLayer(Layer::FullLayer::make<Activation::LReLU>(50, 50));
			sNetwork.addLayer(Layer::FullLayer::make<Activation::LReLU>(50, 50));
			sNetwork.addLayer(Layer::FullLayer::make<Activation::LReLU>(50, 50));
			sNetwork.addLayer(Layer::FullLayer::make<Activation::LReLU>(50, 50));
			sNetwork.addLayer(Layer::FullLayer::make<Activation::LReLU>(50, 50));
			sNetwork.addLayer(Layer::FullLayer::make<Activation::Softmax>(50, 10));

			std::cout << "Successfully created." << std::endl;
			std::cout << "Number of the layers : " << sNetwork.layer().size() << std::endl;
			std::cout << "For each layer :" << std::endl;

			std::size_t nCount{0};

			for (const auto &pLayer : sNetwork.layer())
			{
				std::cout << "\tSummary of the Layer No. " << ++nCount << " :" << std::endl;
				std::cout << "\tFan in : " << pLayer->fanIn() << std::endl;
				std::cout << "\tFan out : " << pLayer->fanOut() << std::endl;
				std::cout << "\tActivation : ";
				std::wcout << pLayer->activation()->name();
				std::cout << std::endl << std::endl;
			}

			sNetwork.initWeight<Initializer::He>();
			sNetwork.initBias<Initializer::Constant>(.0f);

			std::cout << "Network initialized." << std::endl;

			break;
		}
		else if (sCommand == "exit")
		{
			std::cout << "Bye" << std::endl;

			return 0;
		}
		else
			std::cout << "Wrong command." << std::endl << std::endl;
	}

	std::vector<std::vector<float>> sTrainInput(60000u);
	std::vector<std::vector<float>> sTrainOutput(60000u);

	{
		std::ifstream sInput{L"D:/Develop/Dataset/MNIST/MNIST_train_in.dat", std::ifstream::binary | std::ifstream::in};
		for (auto &sInVec : sTrainInput)
		{
			sInVec.resize(784u);
			sInput.read(reinterpret_cast<char *>(sInVec.data()), sizeof(float) * 784u);
		}
	}

	{
		std::ifstream sInput{L"D:/Develop/Dataset/MNIST/MNIST_train_out.dat", std::ifstream::binary | std::ifstream::in};
		for (auto &sInVec : sTrainOutput)
		{
			sInVec.resize(10u);
			sInput.read(reinterpret_cast<char *>(sInVec.data()), sizeof(float) * 10u);
		}
	}

	std::vector<std::vector<float>> sTestInput(10000u);
	std::vector<std::vector<float>> sTestOutput(10000u);

	{
		std::ifstream sInput{L"D:/Develop/Dataset/MNIST/MNIST_test_in.dat", std::ifstream::binary | std::ifstream::in};
		for (auto &sInVec : sTestInput)
		{
			sInVec.resize(784u);
			sInput.read(reinterpret_cast<char *>(sInVec.data()), sizeof(float) * 784u);
		}
	}

	{
		std::ifstream sInput{L"D:/Develop/Dataset/MNIST/MNIST_test_out.dat", std::ifstream::binary | std::ifstream::in};
		for (auto &sInVec : sTestOutput)
		{
			sInVec.resize(10u);
			sInput.read(reinterpret_cast<char *>(sInVec.data()), sizeof(float) * 10u);
		}
	}

	//Optimizer::Supervised::SGD sOptimizer{sNetwork, .001f};
	Optimizer::Supervised::Momentum sOptimizer{sNetwork, .9f, .001f};

	Visualizer::CSVLossExporter sExporter;

	for (;;)
	{
		float nLoss{sNetwork.classificationLoss(sTestInput, sTestOutput)};

		printf("Validation data classification loss : %f\n", nLoss);
		sExporter.accrueLoss(nLoss);
		sExporter.exportCSV(std::ofstream{"losses.csv", std::ofstream::out});

		std::cout << "Serializing network...";
		sNetwork.serialize(std::ofstream{"network.cn", std::ofstream::binary | std::ofstream::out});
		std::cout << " saved." << std::endl;

		sOptimizer.train<Loss::MulticlassCE>(sTrainInput, sTrainOutput, 32, 1);
	}

	system("pause");

	return 0;
}