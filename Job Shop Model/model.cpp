#include "event.h"
#include "server.h"
#include "scheduler.h"
#include <iostream>
#include <vector>
#include <queue>

struct Job {
    int id;
    std::vector<int> routing;
    std::vector<double> serviceTimes;
};

void Event::activate(double t) {
    Scheduler &s = Scheduler::getInstance();
    s.scheduleEvent(this, t);
}


class JobShopModel {
public:
    std::vector<Job> jobs;
    std::vector<Server> workstations;
    Scheduler scheduler;

    JobShopModel() {
        initializeJobs();
        initializeWorkstations();
    }

    void initializeJobs() {

        jobs.push_back({1, {3,1,2,5}, {0.50, 1.10, 1.20, 0.60}});
        jobs.push_back({2, {4,1,3}, {0.80, 0.25, 0.85}});
        jobs.push_back({3, {2,5,1,4,3}, {0.75, 0.70, 0.50, 0.90, 1.00}});
    }

    void initializeWorkstations() {
        workstations.resize(5); 
    }

    void startSimulation() {
        for (auto &job : jobs) {
            scheduler.scheduleJob(job);
        }
        scheduler.run();
    }
};

int main() {
    JobShopModel model;
    model.startSimulation();
    return 0;
}
