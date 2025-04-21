#ifndef NETWORK_H
#define NETWORK_H

#include <map>
#include <memory>
#include <mutex>
#include <vector>
#include <string>

class Node;

class Network {
public:
    Network();
    ~Network();

    // Register a new node into this "network"
    void registerNode(std::shared_ptr<Node> node);

    // Get the current score of a node
    double getNodeScore(int nodeId);

    // Find nodes that are underloaded (score below their low threshold)
    std::vector<int> findUnderloadedNodes(int excludeId);

    // Find nodes that are overloaded (score above their high threshold)
    std::vector<int> findOverloadedNodes(int excludeId);

    // Transfer tasks from one node to another (work sharing)
    void transferTasks(int senderId, int receiverId, const std::vector<std::string>& tasks);

    // Request tasks from donor (work stealing)
    void requestTasks(int donorId, int receiverId);

    // For convenience, get the pointer to a node by ID
    std::shared_ptr<Node> getNode(int nodeId);

private:
    std::mutex mtx;
    // nodeId -> Node
    std::map<int, std::shared_ptr<Node>> nodes;
};

#endif
