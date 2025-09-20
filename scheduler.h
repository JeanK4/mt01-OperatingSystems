#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <set>
#include <algorithm>
#include <iomanip>

using namespace std;

struct ExecutionLog {
    int start;
    int end;
};

struct Process {
    string label;
    int burst;
    int arrival;
    int queue;
    int remaining;
    int start_time;
    int completion_time;
    int waiting_time;
    int turnaround_time;
    int response_time;
    vector<ExecutionLog> execution_log;

    Process(string lbl, int bt, int at, int q)
        : label(lbl), burst(bt), arrival(at), queue(q),
          remaining(bt), start_time(-1), completion_time(-1),
          waiting_time(0), turnaround_time(0), response_time(-1) {}
};

class QueuePolicy {
public:
    int quantum;
    QueuePolicy(int q=0) : quantum(q) {}
    virtual ~QueuePolicy() {}
    virtual Process* schedule(vector<Process*>& q, int current_time, int& run_q) = 0;
};

class RRPolicy : public QueuePolicy {
public:
    RRPolicy(int q) : QueuePolicy(q) {}
    Process* schedule(vector<Process*>& q, int current_time, int& run_q) override;
};

class SJFPolicy : public QueuePolicy {
public:
    SJFPolicy() : QueuePolicy(0) {}
    Process* schedule(vector<Process*>& q, int current_time, int& run_q) override;
};

class STCFPolicy : public QueuePolicy {
public:
    STCFPolicy() : QueuePolicy(1) {}
    Process* schedule(vector<Process*>& q, int current_time, int& run_q) override;
};

class MLFQScheduler {
    vector<Process> processes;
    int scheme;
    vector<unique_ptr<QueuePolicy>> queues;
    map<string,double> metrics;
public:
    MLFQScheduler(vector<Process> procs, int sch);
    static MLFQScheduler from_file(const string& filename, int scheme);
    vector<Process> run();
    void print_results(const vector<Process>& results);
};

#endif
