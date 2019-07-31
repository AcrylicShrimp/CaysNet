
/*
	2019.03.26
	Created by AcrylicShrimp.
*/

#include "GraphBuilder.h"

#include "Graph.h"

#include "../Node/Input.h"
#include "../Node/Parameter.h"

#include "../Node/Add.h"
#include "../Node/Subtract.h"
#include "../Node/Multiply.h"
#include "../Node/Negative.h"

#include "../Node/Log.h"
#include "../Node/ReLU.h"
#include "../Node/Sigmoid.h"
#include "../Node/Softmax.h"

#include "../Node/Sum.h"
#include "../Node/Mean.h"

#include "../Node/MM.h"

#include "../Node/Dense.h"

#include "../Node/MSE.h"
#include "../Node/SigmoidCrossEntropy.h"
#include "../Node/SoftmaxCrossEntropy.h"

#include "../Initializer/Constant.h"
#include "../Initializer/Xavier.h"

#define DEFAULT_NAME(CLASS) ((std::ostringstream{} << CLASS::typeName() << this->pGraph->nodeCount<CLASS>()).str())

namespace TinNet::Core
{
	GraphBuilder::GraphBuilder(Graph *pGraph) :
		pGraph{pGraph}
	{
		assert(this->pGraph);
	}

	void GraphBuilder::registerStandardNodeType()
	{
		this->pGraph->sNodeTypeManager.registerNode<Node::Input>();
		this->pGraph->sNodeTypeManager.registerNode<Node::Parameter>();

		this->pGraph->sNodeTypeManager.registerNode<Node::Add>();
		this->pGraph->sNodeTypeManager.registerNode<Node::Subtract>();
		this->pGraph->sNodeTypeManager.registerNode<Node::Multiply>();
		this->pGraph->sNodeTypeManager.registerNode<Node::Negative>();

		this->pGraph->sNodeTypeManager.registerNode<Node::Log>();
		this->pGraph->sNodeTypeManager.registerNode<Node::ReLU>();
		this->pGraph->sNodeTypeManager.registerNode<Node::Sigmoid>();
		this->pGraph->sNodeTypeManager.registerNode<Node::Softmax>();

		this->pGraph->sNodeTypeManager.registerNode<Node::Sum>();
		this->pGraph->sNodeTypeManager.registerNode<Node::Mean>();

		this->pGraph->sNodeTypeManager.registerNode<Node::MM>();
		
		this->pGraph->sNodeTypeManager.registerNode<Node::Dense>();

		this->pGraph->sNodeTypeManager.registerNode<Node::MSE>();
		this->pGraph->sNodeTypeManager.registerNode<Node::SigmoidCrossEntropy>();
		this->pGraph->sNodeTypeManager.registerNode<Node::SoftmaxCrossEntropy>();
	}

	NodeWrapper GraphBuilder::input()
	{
		return NodeWrapper{this->pGraph->createNode<Node::Input>(DEFAULT_NAME(Node::Input))};
	}

	NodeWrapper GraphBuilder::input(const std::string &sNodeName)
	{
		return NodeWrapper{this->pGraph->createNode<Node::Input>(sNodeName)};
	}

	NodeWrapper GraphBuilder::parameter(const Shape &sShape, InitializerWrapper sInitializer)
	{
		return NodeWrapper{this->pGraph->createNode<Node::Parameter>(DEFAULT_NAME(Node::Parameter), sShape, sInitializer)};
	}

	NodeWrapper GraphBuilder::parameter(const std::string &sNodeName, const Shape &sShape, InitializerWrapper sInitializer)
	{
		return NodeWrapper{this->pGraph->createNode<Node::Parameter>(sNodeName, sShape, sInitializer)};
	}

	NodeWrapper GraphBuilder::add(NodeWrapper sLeft, NodeWrapper sRight)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::Add>(DEFAULT_NAME(Node::Add))};

		sNode["left"]->attach(sLeft);
		sNode["right"]->attach(sRight);

		return sNode;
	}

	NodeWrapper GraphBuilder::add(const std::string &sNodeName, NodeWrapper sLeft, NodeWrapper sRight)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::Add>(sNodeName)};

		sNode["left"]->attach(sLeft);
		sNode["right"]->attach(sRight);

		return sNode;
	}

	NodeWrapper GraphBuilder::subtract(NodeWrapper sLeft, NodeWrapper sRight)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::Subtract>(DEFAULT_NAME(Node::Subtract))};

		sNode["left"]->attach(sLeft);
		sNode["right"]->attach(sRight);

		return sNode;
	}

	NodeWrapper GraphBuilder::subtract(const std::string &sNodeName, NodeWrapper sLeft, NodeWrapper sRight)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::Subtract>(sNodeName)};

		sNode["left"]->attach(sLeft);
		sNode["right"]->attach(sRight);

		return sNode;
	}

	NodeWrapper GraphBuilder::multiply(NodeWrapper sLeft, NodeWrapper sRight)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::Multiply>(DEFAULT_NAME(Node::Multiply))};

		sNode["left"]->attach(sLeft);
		sNode["right"]->attach(sRight);

		return sNode;
	}

	NodeWrapper GraphBuilder::multiply(const std::string &sNodeName, NodeWrapper sLeft, NodeWrapper sRight)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::Multiply>(sNodeName)};

		sNode["left"]->attach(sLeft);
		sNode["right"]->attach(sRight);

		return sNode;
	}

	NodeWrapper GraphBuilder::negative(NodeWrapper sInput)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::Negative>(DEFAULT_NAME(Node::Negative))};

		sNode["input"]->attach(sInput);

		return sNode;
	}

	NodeWrapper GraphBuilder::negative(const std::string &sNodeName, NodeWrapper sInput)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::Negative>(sNodeName)};

		sNode["input"]->attach(sInput);

		return sNode;
	}

	NodeWrapper GraphBuilder::log(NodeWrapper sLogit)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::Log>(DEFAULT_NAME(Node::Log))};

		sNode["input"]->attach(sLogit);

		return sNode;
	}

	NodeWrapper GraphBuilder::log(const std::string &sNodeName, NodeWrapper sLogit)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::Log>(sNodeName)};

		sNode["input"]->attach(sLogit);

		return sNode;
	}

	NodeWrapper GraphBuilder::relu(NodeWrapper sLogit, float nAlpha)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::ReLU>(DEFAULT_NAME(Node::ReLU), nAlpha)};

		sNode["logit"]->attach(sLogit);

		return sNode;
	}

	NodeWrapper GraphBuilder::relu(const std::string &sNodeName, NodeWrapper sLogit, float nAlpha)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::ReLU>(sNodeName, nAlpha)};

		sNode["logit"]->attach(sLogit);

		return sNode;
	}

	NodeWrapper GraphBuilder::sigmoid(NodeWrapper sLogit)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::Sigmoid>(DEFAULT_NAME(Node::Sigmoid))};

		sNode["logit"]->attach(sLogit);

		return sNode;
	}

	NodeWrapper GraphBuilder::sigmoid(const std::string &sNodeName, NodeWrapper sLogit)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::Sigmoid>(sNodeName)};

		sNode["logit"]->attach(sLogit);

		return sNode;
	}

	NodeWrapper GraphBuilder::softmax(NodeWrapper sLogit)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::Softmax>(DEFAULT_NAME(Node::Softmax), std::vector<bool>{})};

		sNode["logit"]->attach(sLogit);

		return sNode;
	}

	NodeWrapper GraphBuilder::softmax(NodeWrapper sLogit, const std::vector<bool> &sReduceAxis)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::Softmax>(DEFAULT_NAME(Node::Softmax), sReduceAxis)};

		sNode["logit"]->attach(sLogit);

		return sNode;
	}

	NodeWrapper GraphBuilder::softmax(const std::string &sNodeName, NodeWrapper sLogit)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::Softmax>(sNodeName, std::vector<bool>{})};

		sNode["logit"]->attach(sLogit);

		return sNode;
	}

	NodeWrapper GraphBuilder::softmax(const std::string &sNodeName, NodeWrapper sLogit, const std::vector<bool> &sReduceAxis)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::Softmax>(sNodeName, sReduceAxis)};

		sNode["logit"]->attach(sLogit);

		return sNode;
	}

	NodeWrapper GraphBuilder::sum(NodeWrapper sInput, bool bSqueeze)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::Sum>(DEFAULT_NAME(Node::Sum), bSqueeze, std::vector<bool>{})};

		sNode["input"]->attach(sInput);

		return sNode;
	}

	NodeWrapper GraphBuilder::sum(NodeWrapper sInput, bool bSqueeze, const std::vector<bool> &sReduceAxis)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::Sum>(DEFAULT_NAME(Node::Sum), bSqueeze, sReduceAxis)};

		sNode["input"]->attach(sInput);

		return sNode;
	}

	NodeWrapper GraphBuilder::sum(const std::string &sNodeName, NodeWrapper sInput, bool bSqueeze)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::Sum>(sNodeName, bSqueeze, std::vector<bool>{})};

		sNode["input"]->attach(sInput);

		return sNode;
	}

	NodeWrapper GraphBuilder::sum(const std::string &sNodeName, NodeWrapper sInput, bool bSqueeze, const std::vector<bool> &sReduceAxis)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::Sum>(sNodeName, bSqueeze, sReduceAxis)};

		sNode["input"]->attach(sInput);

		return sNode;
	}

	NodeWrapper GraphBuilder::mean(NodeWrapper sInput, bool bSqueeze)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::Mean>(DEFAULT_NAME(Node::Mean), bSqueeze, std::vector<bool>{})};

		sNode["input"]->attach(sInput);

		return sNode;
	}

	NodeWrapper GraphBuilder::mean(NodeWrapper sInput, bool bSqueeze, const std::vector<bool> &sReduceAxis)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::Mean>(DEFAULT_NAME(Node::Mean), bSqueeze, sReduceAxis)};

		sNode["input"]->attach(sInput);

		return sNode;
	}

	NodeWrapper GraphBuilder::mean(const std::string &sNodeName, NodeWrapper sInput, bool bSqueeze)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::Mean>(sNodeName, bSqueeze, std::vector<bool>{})};

		sNode["input"]->attach(sInput);

		return sNode;
	}

	NodeWrapper GraphBuilder::mean(const std::string &sNodeName, NodeWrapper sInput, bool bSqueeze, const std::vector<bool> &sReduceAxis)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::Mean>(sNodeName, bSqueeze, sReduceAxis)};

		sNode["input"]->attach(sInput);

		return sNode;
	}

	NodeWrapper GraphBuilder::mm(NodeWrapper sLeft, NodeWrapper sRight)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::MM>(DEFAULT_NAME(Node::MM))};

		sNode["left"]->attach(sLeft);
		sNode["right"]->attach(sRight);

		return sNode;
	}

	NodeWrapper GraphBuilder::mm(const std::string &sNodeName, NodeWrapper sLeft, NodeWrapper sRight)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::MM>(sNodeName)};

		sNode["left"]->attach(sLeft);
		sNode["right"]->attach(sRight);

		return sNode;
	}

	NodeWrapper GraphBuilder::dense(NodeWrapper sInput, NodeWrapper sWeight)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::Dense>(DEFAULT_NAME(Node::Dense))};

		sNode["input"]->attach(sInput);
		sNode["weight"]->attach(sWeight);

		return sNode;
	}

	NodeWrapper GraphBuilder::dense(const std::string &sNodeName, NodeWrapper sInput, NodeWrapper sWeight)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::Dense>(sNodeName)};

		sNode["input"]->attach(sInput);
		sNode["weight"]->attach(sWeight);

		return sNode;
	}

	NodeWrapper GraphBuilder::dense(NodeWrapper sInput, NodeWrapper sWeight, NodeWrapper sBias)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::Dense>(DEFAULT_NAME(Node::Dense))};

		sNode["input"]->attach(sInput);
		sNode["weight"]->attach(sWeight);
		sNode["bias"]->attach(sBias);

		return sNode;
	}

	NodeWrapper GraphBuilder::dense(const std::string &sNodeName, NodeWrapper sInput, NodeWrapper sWeight, NodeWrapper sBias)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::Dense>(sNodeName)};

		sNode["input"]->attach(sInput);
		sNode["weight"]->attach(sWeight);
		sNode["bias"]->attach(sBias);

		return sNode;
	}

	NodeWrapper GraphBuilder::mse(NodeWrapper sLabel, NodeWrapper sPred)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::MSE>(DEFAULT_NAME(Node::MSE))};

		sNode["label"]->attach(sLabel);
		sNode["pred"]->attach(sPred);

		return sNode;
	}

	NodeWrapper GraphBuilder::mse(const std::string &sNodeName, NodeWrapper sLabel, NodeWrapper sPred)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::MSE>(sNodeName)};

		sNode["label"]->attach(sLabel);
		sNode["pred"]->attach(sPred);

		return sNode;
	}

	NodeWrapper GraphBuilder::sigmoidCE(NodeWrapper sLabel, NodeWrapper sProb)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::SigmoidCrossEntropy>(DEFAULT_NAME(Node::SigmoidCrossEntropy))};

		sNode["label"]->attach(sLabel);
		sNode["prob"]->attach(sProb);

		return sNode;
	}

	NodeWrapper GraphBuilder::sigmoidCE(const std::string &sNodeName, NodeWrapper sLabel, NodeWrapper sProb)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::SigmoidCrossEntropy>(sNodeName)};

		sNode["label"]->attach(sLabel);
		sNode["prob"]->attach(sProb);

		return sNode;
	}

	NodeWrapper GraphBuilder::softmaxCE(NodeWrapper sLabel, NodeWrapper sProb)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::SoftmaxCrossEntropy>(DEFAULT_NAME(Node::SoftmaxCrossEntropy))};

		sNode["label"]->attach(sLabel);
		sNode["prob"]->attach(sProb);

		return sNode;
	}

	NodeWrapper GraphBuilder::softmaxCE(const std::string &sNodeName, NodeWrapper sLabel, NodeWrapper sProb)
	{
		NodeWrapper sNode{this->pGraph->createNode<Node::SoftmaxCrossEntropy>(sNodeName)};

		sNode["label"]->attach(sLabel);
		sNode["prob"]->attach(sProb);

		return sNode;
	}

	InitializerWrapper GraphBuilder::initConstant(float nConstant)
	{
		return InitializerWrapper{this->pGraph->createInitializer<Initializer::Constant>(nConstant)};
	}

	InitializerWrapper GraphBuilder::initXavier(std::size_t nFanIn, std::size_t nFanOut)
	{
		return InitializerWrapper{this->pGraph->createInitializer<Initializer::Xavier>(nFanIn, nFanOut)};
	}

	NodeWrapper operator+(NodeWrapper sLeft, NodeWrapper sRight)
	{
		assert(sLeft.pNode);
		assert(sRight.pNode);
		assert(sLeft.pNode->pGraph);
		assert(sRight.pNode->pGraph);
		assert(sLeft.pNode->pGraph == sRight.pNode->pGraph);

		return sLeft.pNode->pGraph->builder().add(sLeft, sRight);
	}

	NodeWrapper operator-(NodeWrapper sLeft, NodeWrapper sRight)
	{
		assert(sLeft.pNode);
		assert(sRight.pNode);
		assert(sLeft.pNode->pGraph);
		assert(sRight.pNode->pGraph);
		assert(sLeft.pNode->pGraph == sRight.pNode->pGraph);

		return sLeft.pNode->pGraph->builder().subtract(sLeft, sRight);
	}

	NodeWrapper operator*(NodeWrapper sLeft, NodeWrapper sRight)
	{
		assert(sLeft.pNode);
		assert(sRight.pNode);
		assert(sLeft.pNode->pGraph);
		assert(sRight.pNode->pGraph);
		assert(sLeft.pNode->pGraph == sRight.pNode->pGraph);

		return sLeft.pNode->pGraph->builder().multiply(sLeft, sRight);
	}

	NodeWrapper operator-(NodeWrapper sInput)
	{
		assert(sInput.pNode);
		assert(sInput.pNode->pGraph);

		return sInput.pNode->pGraph->builder().negative(sInput);
	}
}