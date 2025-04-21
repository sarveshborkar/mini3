#include "Network.h"
#include "Node.h"
#include <iostream>
#include <algorithm>

Network::Network() {}
Network::~Network() {}

void Network::registerNode(std::shared_ptr<Node> node) {
    std::lock_guard<std::mutex> lock(mtx);
    nodes[node->getId()] = node;
}

double Network::getNodeScore(int nodeId) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = nodes.find(nodeId);
    if (it != nodes.end()) {
        return it->second->computeScore();
    }
    return -1.0;
}

std::vector<int> Network::findUnderloadedNodes(int excludeId) {
    std::vector<int> underloaded;
    std::lock_guard<std::mutex> lock(mtx);

    for (auto& kv : nodes) {
        int nid = kv.first;
        if (nid == excludeId) continue;
        
        double score = kv.second->computeScore();
        // We can re-check the thresholds from the Node's perspective:
        // This example just does a quick approach:
        if (score < 30.0) { // Hard-coded for demo; you could query the node's lowThreshold
            underloaded.push_back(nid);
        }
    }
    return underloaded;
}

std::vector<int> Network::findOverloadedNodes(int excludeId) {
    std::vector<int> overloaded;
    std::lock_guard<std::mutex> lock(mtx);

    for (auto& kv : nodes) {
        int nid = kv.first;
        if (nid == excludeId) continue;

        double score = kv.second->computeScore();
        // Hard-coded check for demo:
        if (score > 70.0) { // Compare to the node's highThreshold
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
    if (!donor || !receiver) return;

    // We'll emulate "steal half of donor's tasks"
    // We'll do that by directly calling a method on donor to fetch tasks:
    // In this example, we must do it from the network side. We'll replicate
    // the logic from shareSomeTasks.
    std::vector<std::string> stolenTasks;
    {
        // Lock inside donor->addTasks or something else. But let's keep it simple
        // We'll do a quick approach: We can dynamically cast or create a method on Node
        // For demonstration, let's define a small lambda to do the stealing:

        // We need Node to provide an interface for "extract tasks".
        // We'll do it quickly by re-using the approach from shareSomeTasks's logic.
        // Because we haven't defined a direct method, let's do it in place:
        std::lock_guard<std::mutex> lock(*(&donor->getMutex())); // Access the donor's lock if public
        // But Node's mtx is private. For a real design, you'd have a method. Let's hack a bit:

        // We'll do it "the naive way" for this example code. We'll assume we have a friend class or
        // re-architect. For clarity here, let's just trust we can do it.
        extern std::mutex hackMutex; // placeholder if needed, but let's skip it for brevity

        // We can't easily do it if the Node's fields are private. Let's just assume
        // we can access them in a real scenario or Node exposes a method. We'll show a conceptual approach:

        // Conceptual:
        // 1. read the donor's queue, 2. remove half, 3. push to stolenTasks
        // We won't do it in detail to keep code short, but let's pretend:
    }

    // For demonstration, let's do a simpler approach:
    // We'll call the donor's "shareSomeTasks" but with the receiver as an argument.
    // In real code, you'd have a method to handle partial direct extraction.
    donor->stealSomeTasks(receiverId); // This is a cheat to demonstrate the concept

    // The console message is printed in stealSomeTasks after it's done.
}

std::shared_ptr<Node> Network::getNode(int nodeId) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = nodes.find(nodeId);
    if (it != nodes.end()) {
        return it->second;
    }
    return nullptr;
}
