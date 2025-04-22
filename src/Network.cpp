#include "Network.h"
#include "Node.h"
#include <iostream>
#include <algorithm>

Network::Network() {}
Network::~Network() {}

// Register Node to the network
void Network::registerNode(std::shared_ptr<Node> node) {
    std::lock_guard<std::mutex> lock(mtx);
    nodes[node->getId()] = node;
}

// Obtains score for a NodeId
double Network::getNodeScore(int nodeId) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = nodes.find(nodeId);
    if (it != nodes.end()) {
        return it->second->computeScore();
    }
    return -1.0;
}

std::vector<int> Network::findUnderloadedNodes(int currentNodeId) {
    std::shared_ptr<Node> currentNode = getNode(currentNodeId); 
    std::vector<int> underloaded;
    std::lock_guard<std::mutex> lock(mtx);

    for (auto& kv : nodes) {
        int nid = kv.first;
        if (nid == currentNodeId) continue;
        
        double score = kv.second->computeScore();

        if (score < currentNode->getLowThreshold()) {
            underloaded.push_back(nid);
        }
    }
    return underloaded;
}

std::vector<int> Network::findOverloadedNodes(int currentNodeId) {
    std::shared_ptr<Node> currentNode = getNode(currentNodeId); 
    std::vector<int> overloaded;
    std::lock_guard<std::mutex> lock(mtx);

    for (auto& kv : nodes) {
        int nid = kv.first;
        if (nid == currentNodeId) continue;

        double score = kv.second->computeScore();

        if (score > currentNode->getHighThreshold()) {
            overloaded.push_back(nid);
        }
    }
    return overloaded;
}

void Network::transferTasks(int senderId, int receiverId, const std::vector<std::string>& tasks) {
    auto receiver = getNode(receiverId);
    if (!receiver) return;

    receiver->addTasks(tasks);

    std::cout << "[NETWORK] Node " << senderId << " transferred "
              << tasks.size() << " tasks to Node " << receiverId << std::endl;
}

void Network::requestTasks(int donorId, int receiverId) {
    auto donor = getNode(donorId);
    auto receiver = getNode(receiverId);
    if (!donor || !receiver) {
        return;
    }

    std::vector<std::string> stolen = donor->extractTasksForSteal();

    if (!stolen.empty()) {
        receiver->addTasks(stolen);
        std::cout << "[NETWORK] Node " << receiverId
                  << " stole " << stolen.size() << " tasks from Node "
                  << donorId << std::endl;
    } else {
        std::cout << "[NETWORK] Node " << receiverId
                  << " attempted to steal but donor " << donorId
                  << " had none to spare.\n";
    }
}



std::shared_ptr<Node> Network::getNode(int nodeId) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = nodes.find(nodeId);
    if (it != nodes.end()) {
        return it->second;
    }
    return nullptr;
}
