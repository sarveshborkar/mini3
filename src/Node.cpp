#include "Node.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <algorithm>
#include <cmath>

#include "Network.h"

Node::Node(int id, Network* net)
    : nodeId(id),
      network(net),
      cpuUsage(0.0),
      memoryUsage(0.0),
      wCpu(0.4),
      wMem(0.3),
      wQueue(0.3),
      highThreshold(60.0),
      lowThreshold(40.0),
      rng(std::random_device{}()),
      dist(-5.0, 5.0)
{}

Node::~Node() {
    stop(); // Ensure the thread is joined if user forgot
}

void Node::start() {
    running = true;
    worker = std::thread(&Node::run, this);
}

void Node::stop() {
    running = false;
    if (worker.joinable()) {
        worker.join();
    }
}

double Node::computeScore() {
    std::lock_guard<std::mutex> lock(mtx);
    // Weighted sum of CPU usage, memory usage, and queue length
    double score = wCpu   * cpuUsage
                 + wMem   * memoryUsage
                 + wQueue * static_cast<double>(taskQueue.size());
    return score;
}

void Node::addTasks(const std::vector<std::string>& tasks) {
    std::lock_guard<std::mutex> lock(mtx);
    for (auto& t : tasks) {
        taskQueue.push_back(t);
    }
}

void Node::run() {
    while (running) {
        // Simulate resource usage (random CPU / memory changes)
        simulateResourceUsage();

        // Compute current node score
        double currentScore = computeScore();

        // Decide to share or steal
        if (currentScore > highThreshold) {
            attemptShareTasks(currentScore);
        } else if (currentScore < lowThreshold) {
            attemptStealTasks(currentScore);
        }

        // Sleep for a short time
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void Node::simulateResourceUsage() {
    std::lock_guard<std::mutex> lock(mtx);

    // Add a small random fluctuation in CPU and memory usage
    cpuUsage    = std::clamp(cpuUsage    + dist(rng), 0.0, 100.0);
    memoryUsage = std::clamp(memoryUsage + dist(rng), 0.0, 100.0);
}

void Node::attemptShareTasks(double currentScore) {
    // Find potential receivers (nodes with a score < their low threshold)
    std::vector<int> receivers = network->findUnderloadedNodes(nodeId);
    if (receivers.empty()) {
        // No underloaded nodes found
        return;
    }

    // Choose a "best" receiver, e.g., the one with the smallest score
    double bestScore = std::numeric_limits<double>::infinity();
    int bestReceiverId = -1;

    for (int rId : receivers) {
        double rScore = network->getNodeScore(rId);
        if (rScore < bestScore) {
            bestScore = rScore;
            bestReceiverId = rId;
        }
    }

    if (bestReceiverId >= 0) {
        shareSomeTasks(bestReceiverId);
    }
}

void Node::attemptStealTasks(double currentScore) {
    // Find overloaded nodes (score > their high threshold)
    std::vector<int> donors = network->findOverloadedNodes(nodeId);
    if (donors.empty()) {
        // No overloaded donors found
        return;
    }

    // Pick the donor with the highest score
    double bestScore = -1.0;
    int bestDonorId = -1;

    for (int dId : donors) {
        double dScore = network->getNodeScore(dId);
        if (dScore > bestScore) {
            bestScore = dScore;
            bestDonorId = dId;
        }
    }

    if (bestDonorId >= 0) {
        stealSomeTasks(bestDonorId);
    }
}

void Node::shareSomeTasks(int receiverId) {
    std::vector<std::string> tasksToShare;
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (taskQueue.empty()) {
            return;
        }

        // Example: share half of the tasks
        int numToShare = static_cast<int>(taskQueue.size() / 2);
        if (numToShare == 0) {
            return;
        }

        // Copy tasks to a temporary vector
        tasksToShare.insert(tasksToShare.end(), taskQueue.begin(), taskQueue.begin() + numToShare);

        // Erase them from local queue
        taskQueue.erase(taskQueue.begin(), taskQueue.begin() + numToShare);
    }
    // Perform the actual "transfer" via the network
    network->transferTasks(nodeId, receiverId, tasksToShare);
}

void Node::stealSomeTasks(int donorId) {
    // Request tasks from the donor
    network->requestTasks(donorId, nodeId);
}

std::mutex& Node::getMutex(){
    return mtx;
}