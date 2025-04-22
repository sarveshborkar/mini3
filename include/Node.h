#ifndef NODE_H
#define NODE_H

#include <vector>
#include <mutex>
#include <thread>
#include <random>
#include <atomic>
#include "Network.h"


class Network;

class Node {

private:
    int nodeId;
    Network* network;

    std::vector<Task> taskQueue;
    double cpuUsage;      // 0..100, simulated
    double memoryUsage;   // 0..100, simulated

    // Weights based on CPU, Mem & Queue Length
    double wCpu;
    double wMem;
    double wQueue;

    /* Thresholds: 
        Above highThreshold = overloaded
        Below lowThreshold = underloaded
    */
    double highThreshold;
    double lowThreshold;

    // Concurrency Resoureces
    std::mutex mtx;
    std::thread worker;
    std::atomic<bool> running{false};

    // Random Generator
    std::default_random_engine rng;
    std::uniform_real_distribution<double> dist;

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

public:
    Node(int id, Network* network);
    ~Node();

    // Start and stop the node's main loop
    void start();
    void stop();

    // Compute the node's current weighted score
    double computeScore();

    // Add tasks to this node's queue
    void addTasks(const std::vector<Task>& tasks);

    // Get the Node's ID
    int getId() const { return nodeId; }
    
    // Extract task for stealing and moving to another node.
    std::vector<Task> extractTasksForSteal();

    void copyTasks(std::vector<Task>& taskList, std::vector<Task>& taskQueue, int numToShare);

    // Perform the task-stealing from the chosen donor
    void stealSomeTasks(int donorId);

    double getLowThreshold();

    double getHighThreshold();
};

#endif
