#include <queue>
#include <vector>
#include <iostream>

enum Status { UNKNOWN, CHOSEN, REPORTED };

struct Message {
    int uid;
    bool is_report;
};

class State {
private:
    int uid;
    std::queue<Message> send_queue;
    Status status;

public:
    State(int id) : uid(id), status(UNKNOWN) {}

    void send(std::vector<State>& ring, int i) {
        if (!send_queue.empty()) {
            Message msg = send_queue.front();
            send_queue.pop();
            int j = (i + 1) % ring.size();
            ring[j].push_to_queue(msg);
        }
    }

    void receive(std::vector<State>& ring, int i) {
        if (send_queue.empty()) return;

        Message msg = send_queue.front();

        if (!msg.is_report) {
            // Electioneering
            if (status == UNKNOWN) {
                if (msg.uid > uid) {
                    send_queue.pop();
                    int j = (i + 1) % ring.size();
                    ring[j].push_to_queue(msg);
                } else if (msg.uid == uid) {
                    send_queue.pop();
                    status = CHOSEN;
                    std::cout << "CHOSEN: " << uid << std::endl;
                    Message report = { uid, true };
                    int j = (i + 1) % ring.size();
                    ring[j].push_to_queue(report);
                } else {
                    send_queue.pop(); 
                }
            } else {
                send_queue.pop();
            }
        } else {
            // Reporting
            send_queue.pop();
            if (msg.uid == uid && status == CHOSEN) {
                return;
            }
            if (status == UNKNOWN) {
                status = REPORTED;
                std::cout << "REPORTED: " << uid << " (Leader: " << msg.uid << ")" << std::endl;
            }
            int j = (i + 1) % ring.size();
            ring[j].push_to_queue(msg);
        }
    }

    void push_to_queue(Message message) {
        send_queue.push(message);
    }

    Status get_status() {
        return status;
    }

    int get_uid() {
        return uid;
    }
};

bool isLeaderElected(std::vector<State>& ring) {
    for (State& state : ring) {
        if (state.get_status() != CHOSEN && state.get_status() != REPORTED) {
            return false;
        }
    }
    return true;
}

int main() {
    std::vector<int> ids = {2, 7 , 1, 8, 3}; 
    std::vector<State> ring;

    for (int id : ids) {
        ring.emplace_back(id);
    }

    for (State& state : ring) {
        // Setting initial queue with uid and message
        state.push_to_queue({ state.get_uid(), false });  
    }

    int iteration = 1;
    while (!isLeaderElected(ring)) {
        std::cout << "Iteration: " << iteration++ << std::endl;

        for (int i = 0; i < ring.size(); i++) {
            ring[i].send(ring, i);
        }
        for (int i = 0; i < ring.size(); i++) {
            ring[i].receive(ring, i);
        }
    }

    std::cout << "LCR complete." << std::endl;
    return 0;
}