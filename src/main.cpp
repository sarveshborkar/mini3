#include <iostream>
#include <vector>
#include <memory>
#include "Network.h"
#include "Node.h"
#include <thread>
#include <chrono>
#include <random>

int main() {
    // Create a shared Network
    std::unique_ptr<Network> network = std::make_unique<Network>();

    // Create several nodes
    const int nodeCount = 5;
    std::vector<std::shared_ptr<Node>> nodes;

    for (int i = 0; i < nodeCount; ++i) {
        auto node = std::make_shared<Node>(i, network.get());
        network->registerNode(node);
        nodes.push_back(node);
    }

    // Initialize tasks
    // Let's overload Node 0 with a bunch of tasks
    std::vector<std::string> heavyTasks;
    for (int j = 0; j < 50; ++j) {
        heavyTasks.push_back("task-" + std::to_string(j));
    }
    nodes[0]->addTasks(heavyTasks);

    // Light tasks for others
    std::default_random_engine rng(std::random_device{}());
    std::uniform_int_distribution<int> distTasks(0, 5);

    for (int i = 1; i < nodeCount; ++i) {
        int numTasks = distTasks(rng);
        std::vector<std::string> smallTasks;
        for (int j = 0; j < numTasks; ++j) {
            smallTasks.push_back("lightTask-" + std::to_string(i) + "-" + std::to_string(j));
        }
        nodes[i]->addTasks(smallTasks);
    }

    // Randomly initialize CPU and memory usage in range [0..100]
    std::uniform_real_distribution<double> distUsage(0.0, 100.0);
    for (auto& n : nodes) {
        // We'll cast to Node* to access fields if we had a public setter, or do it differently.
        // For demo, let's assume we do it by actually simulating once in Node.
    }

    // Start each node's thread
    for (auto& n : nodes) {
        n->start();
    }

    // Let the simulation run for ~10 seconds
    std::this_thread::sleep_for(std::chrono::seconds(10));

    // Stop all nodes
    for (auto& n : nodes) {
        n->stop();
    }

    // Print final queue sizes
    for (auto& n : nodes) {
        std::cout << "Node " << n->getId()
                  << " final queue size: " << n->computeScore() / 1.0 // or just queue size if you prefer
                  << " (Score: " << n->computeScore() << ")\n";
    }

    std::cout << "Done.\n";
    return 0;
}
