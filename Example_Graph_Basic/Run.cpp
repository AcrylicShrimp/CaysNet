
/*
	2018.05.27
	Created by AcrylicShrimp.
*/

#include <TinNet/TinNet.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <utility>
#include <vector>

std::int32_t main()
{
	std::vector<float> train_x(60000 * 784);
	std::vector<float> train_y(60000 * 10);
	std::vector<float> test_x(10000 * 784);
	std::vector<float> test_y(10000 * 10);

	{
		std::ifstream sInput{L"train_input.dat", std::ifstream::binary | std::ifstream::in};
		sInput.read(reinterpret_cast<char *>(train_x.data()), sizeof(float) * train_x.size());
	}

	{
		std::ifstream sInput{L"train_label.dat", std::ifstream::binary | std::ifstream::in};
		sInput.read(reinterpret_cast<char *>(train_y.data()), sizeof(float) * train_y.size());
	}

	{
		std::ifstream sInput{L"test_input.dat", std::ifstream::binary | std::ifstream::in};
		sInput.read(reinterpret_cast<char *>(test_x.data()), sizeof(float) * test_x.size());
	}

	{
		std::ifstream sInput{L"test_label.dat", std::ifstream::binary | std::ifstream::in};
		sInput.read(reinterpret_cast<char *>(test_y.data()), sizeof(float) * test_y.size());
	}

	using namespace TinNet;

	Core::Graph graph;

	auto x = graph.builder().input("x");
	auto y = graph.builder().input("y");

	auto w1 = graph.builder().parameter("w1", Core::Shape{300, 764}, graph.builder().initXavier(764, 300));
	auto b1 = graph.builder().parameter("b1", Core::Shape{300}, graph.builder().initConstant());
	auto o1 = graph.builder().relu(graph.builder().dense(x, w1, b1));

	auto w2 = graph.builder().parameter("w2", Core::Shape{10, 300}, graph.builder().initXavier(300, 10));
	auto b2 = graph.builder().parameter("b2", Core::Shape{10}, graph.builder().initConstant());
	auto o2 = graph.builder().softmax(graph.builder().dense(o1, w2, b2), {true, false});

	auto loss = graph.builder().mean(-graph.builder().sum(y * graph.builder().log(o2), true, {true, false}), true);

	Optimizer::Momentum optimizer
	{
		.9f,
		{
			graph.node<Node::Parameter>("w1"),
			graph.node<Node::Parameter>("b1"),
			graph.node<Node::Parameter>("w2"),
			graph.node<Node::Parameter>("b2")
		}
	};
	
	std::mt19937_64 sEngine{std::random_device{}()};
	std::vector<std::size_t> sShuffledIndexList;

	for (std::size_t nIndex{0}; nIndex < 60000; ++nIndex)
		sShuffledIndexList.emplace_back(nIndex);

	for (;;)
	{
		graph.feed(
			{
				{"x", Core::Shape{764, 32}, Core::Span<float>{train_x.begin(), train_x.begin() + 764 * 32}},
				{"y", Core::Shape{10, 32}, Core::Span<float>{train_y.begin(), train_y.begin() + 10 * 32}}
			});

		std::cout << "Training Loss: " << loss.evalOutput().output()[0] << std::endl;

		graph.feed(
			{
				{"x", Core::Shape{764, 32}, Core::Span<float>{test_x.begin(), test_x.begin() + 764 * 32}},
				{"y", Core::Shape{10, 32}, Core::Span<float>{test_y.begin(), test_y.begin() + 10 * 32}}
			});

		std::cout << "Test Loss: " << loss.evalOutput().output()[0] << std::endl;

		for (std::size_t nIndex{1}; nIndex < 60000; ++nIndex)
			std::swap(sShuffledIndexList[nIndex - 1], sShuffledIndexList[std::uniform_int_distribution<std::size_t>{nIndex, 60000 - 1}(sEngine)]);

		for (std::size_t nIndex{0}; nIndex < 60000; nIndex += 32)
		{
			std::size_t nActualBatchSize{std::min<std::size_t>(60000 - nIndex, 32)};

			graph.feed(
				{
					{"x", Core::Shape{764, nActualBatchSize}, Core::Span<float>{train_x.begin() + nIndex / 32 * nActualBatchSize * 764, train_x.begin() + (nIndex / 32 + 1) * nActualBatchSize * 764}},
					{"y", Core::Shape{10, nActualBatchSize}, Core::Span<float>{train_y.begin() + nIndex / 32 * nActualBatchSize * 10, train_y.begin() + (nIndex / 32 + 1) * nActualBatchSize * 10}}
				});

			optimizer.reduce(.001f, loss);
		}
	}

	return 0;
}