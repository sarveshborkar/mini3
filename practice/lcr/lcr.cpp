#include <queue>
#include <vector>
#include <iostream>

// TODO : Broken as hell but almost there!

enum Status {UNKNOWN , CHOSEN, REPORTED};

class State{

private:
    int uid;
    std::queue<int> send_queue;
    Status status;

public:
    State(int id) : uid(id), status(UNKNOWN) {}

    void send(std::vector<State>& ring, int i){
        if(!send_queue.empty()){
            int message = send_queue.front();
            send_queue.pop();
            int j = (i+1) % ring.size();
            ring[j].push_to_queue(message);
        }
    }

    void receive(std::vector<State>& ring, int i){
        if(!send_queue.empty()){
            int message = send_queue.front();
            if (status == UNKNOWN){
                if(message > uid){
                    send_queue.pop();
                    int j = (i+1) % ring.size();
                    ring[j].push_to_queue(message);
                }
                else if(message == uid){
                    status = CHOSEN;
                    send_queue.pop();
                    std::cout << "CHOSEN:" << uid << std::endl;
                }
                else{
                    send_queue.pop();
                }
            }
            else if (status == CHOSEN || status == REPORTED){
                if (message == uid && status == CHOSEN){
                    send_queue.pop();
                }
                else{
                    send_queue.pop();
                    if (status == UNKNOWN){
                        status = REPORTED;
                    }
                    int j = (i+1) % ring.size();
                    ring[j].send_queue.push(message);
                }
                std::cout << status << ":" << std::endl;
            }
        }
        
    }

    void set_status(Status& nextStatus){
        status = nextStatus;
    }

    Status get_status(){
        return status;
    }

    int get_uid(){
        return uid;
    }

    void push_to_queue(int message){
        send_queue.push(message);
    }

};

bool isLeaderElected(std::vector<State>& ring){
    for (State& state : ring){
        if(state.get_status() != CHOSEN && state.get_status() != REPORTED){
            return false;
        }
    }
    return true;
}

int main(int argc, char* argv[]){
    std::vector<int> ids = {3, 1 ,4 ,2};
    std::vector<State> ring;

    for(int id : ids){
        State state(id);
        ring.push_back(state);
    }

    for (State& state : ring){
        state.push_to_queue(state.get_uid());
    }

    while(!isLeaderElected(ring)){
        for(int i = 0; i < ring.size(); ++i){
            ring[i].send(ring,i);
        }
        for(int i = 0;i < ring.size(); ++i){
            ring[i].receive(ring, i);
        }
    }

    return 0;
}