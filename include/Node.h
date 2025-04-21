#ifndef NODE_H
#define NODE_H

#include <vector>
#include <string>
#include <mutex>
#include <thread>
#include <random>
#include <atomic>
#include "Network.h"

class Network; // Forward declaration to avoid circular includes

class Node {
public:
    Node(int id, Network* network);
    ~Node();

    // Start and stop the node's main loop
    void start();
    void stop();

    // Compute the node's current weighted score
    double computeScore();

    // Add tasks to this node's queue
    void addTasks(const std::vector<std::string>& tasks);

    // Get the node's ID
    int getId() const { return nodeId; }

    std::mutex& getMutex();

    // Actually perform the task-stealing from the chosen donor
    void stealSomeTasks(int donorId);

private:
    // Main loop for simulating load and performing share/steal decisions
    void run();

    // Simulate random CPU/memory usage changes
    void simulateResourceUsage();

    // Try to share tasks if overloaded
    void attemptShareTasks(double currentScore);

    // Try to steal tasks if underloaded
    void attemptStealTasks(double currentScore);

    // Actually perform the task-sharing with the chosen receiver
    void shareSomeTasks(int receiverId);

private:
    int nodeId;
    Network* network;

    // For simplicity, tasks are strings
    std::vector<std::string> taskQueue;
    double cpuUsage;      // 0..100, simulated
    double memoryUsage;   // 0..100, simulated
    double wCpu;
    double wMem;
    double wQueue;

    // Thresholds: above highThreshold = overloaded, below lowThreshold = underloaded
    double highThreshold;
    double lowThreshold;

    // Concurrency
    std::mutex mtx;
    std::thread worker;
    std::atomic<bool> running{false};

    // Random generator
    std::default_random_engine rng;
    std::uniform_real_distribution<double> dist;
};

#endif
