#include "scheduler.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <numeric>

using namespace std;

// ----------------- POLICIES -----------------
Process* RRPolicy::schedule(vector<Process*>& q, int current_time, int& run_q) {
    for (auto* p : q) {
        if (p->arrival <= current_time && p->remaining > 0) {
            run_q = quantum;
            return p;
        }
    }
    return nullptr;
}

Process* SJFPolicy::schedule(vector<Process*>& q, int current_time, int& run_q) {
    vector<Process*> ready;
    for (auto* p : q)
        if (p->arrival <= current_time && p->remaining > 0) ready.push_back(p);
    if (ready.empty()) return nullptr;
    auto* proc = *min_element(ready.begin(), ready.end(),
                                   [](Process* a, Process* b){ return a->remaining < b->remaining; });
    run_q = proc->remaining;
    return proc;
}

Process* STCFPolicy::schedule(vector<Process*>& q, int current_time, int& run_q) {
    vector<Process*> ready;
    for (auto* p : q)
        if (p->arrival <= current_time && p->remaining > 0) ready.push_back(p);
    if (ready.empty()) return nullptr;
    auto* proc = *min_element(ready.begin(), ready.end(),
                                   [](Process* a, Process* b){ return a->remaining < b->remaining; });
    run_q = 1;
    return proc;
}

// ----------------- SCHEDULER -----------------
MLFQScheduler::MLFQScheduler(vector<Process> procs, int sch) : processes(procs), scheme(sch) {
    if (scheme == 1) {
        queues.emplace_back(new RRPolicy(1));
        queues.emplace_back(new RRPolicy(3));
        queues.emplace_back(new RRPolicy(4));
        queues.emplace_back(new SJFPolicy());
    } else if (scheme == 2) {
        queues.emplace_back(new RRPolicy(2));
        queues.emplace_back(new RRPolicy(3));
        queues.emplace_back(new RRPolicy(4));
        queues.emplace_back(new STCFPolicy());
    } else if (scheme == 3) {
        queues.emplace_back(new RRPolicy(3));
        queues.emplace_back(new RRPolicy(5));
        queues.emplace_back(new RRPolicy(6));
        queues.emplace_back(new RRPolicy(20));
    } else {
        throw invalid_argument("Esquema no soportado");
    }
}

MLFQScheduler MLFQScheduler::from_file(const string& filename, int scheme) {
    ifstream file(filename);
    if (!file.is_open()) throw runtime_error("No se pudo abrir el archivo");

    vector<Process> procs;
    string line;
    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        stringstream ss(line);
        string part;
        vector<string> parts;

        // separar por ';'
        while (getline(ss, part, ';')) {
            // quitar espacios alrededor
            part.erase(0, part.find_first_not_of(" \t"));
            part.erase(part.find_last_not_of(" \t")+1);
            parts.push_back(part);
        }

        if (parts.size() >= 4) {
            string label = parts[0];
            int bt = stoi(parts[1]);
            int at = stoi(parts[2]);
            int q  = stoi(parts[3]);
            procs.emplace_back(label, bt, at, q);
        }

    }
    return MLFQScheduler(procs, scheme);
}


vector<Process> MLFQScheduler::run() {
    int time = 0, finished = 0;
    auto n = processes.size();
    vector<vector<Process*>> ready_queues(queues.size());
    set<string> arrived;

    while (finished < n) {
        for (auto& p : processes) {
            if (p.arrival <= time && !arrived.count(p.label)) {
                ready_queues[p.queue-1].push_back(&p);
                arrived.insert(p.label);
            }
        }

        Process* selected = nullptr;
        int q_idx = -1, run_q = 0;
        for (size_t i=0; i<queues.size(); i++) {
            Process* p = queues[i]->schedule(ready_queues[i], time, run_q);
            if (p) { selected = p; q_idx = i; break; }
        }

        if (!selected) { time++; continue; }

        int run_for = min(selected->remaining, run_q);
        if (selected->start_time == -1) {
            selected->start_time = time;
            selected->response_time = time - selected->arrival;
        }

        selected->execution_log.push_back({time, time+run_for});
        time += run_for;
        selected->remaining -= run_for;

        if (selected->remaining == 0) {
            selected->completion_time = time;
            selected->turnaround_time = selected->completion_time - selected->arrival;
            selected->waiting_time = selected->turnaround_time - selected->burst;
            finished++;
            ready_queues[q_idx].erase(remove(ready_queues[q_idx].begin(), ready_queues[q_idx].end(), selected), ready_queues[q_idx].end());
        } else {
            if (dynamic_cast<RRPolicy*>(queues[q_idx].get()) && q_idx+1 < (int)queues.size()) {
                ready_queues[q_idx].erase(remove(ready_queues[q_idx].begin(), ready_queues[q_idx].end(), selected), ready_queues[q_idx].end());
                ready_queues[q_idx+1].push_back(selected);
            }
        }
    }

    metrics["WT"] = accumulate(processes.begin(), processes.end(), 0.0,
        [](double sum, const Process& p){ return sum + p.waiting_time; })/n;
    metrics["CT"] = accumulate(processes.begin(), processes.end(), 0.0,
        [](double sum, const Process& p){ return sum + p.completion_time; })/n;
    metrics["RT"] = accumulate(processes.begin(), processes.end(), 0.0,
        [](double sum, const Process& p){ return sum + p.response_time; })/n;
    metrics["TAT"] = accumulate(processes.begin(), processes.end(), 0.0,
        [](double sum, const Process& p){ return sum + p.turnaround_time; })/n;

    return processes;
}

void MLFQScheduler::print_results(const vector<Process>& results) {
    cout << "\n" << string(60, '=') 
              << "\nRESULTADOS DE LA SIMULACION\n" 
              << string(60, '=') << "\n\n";

    cout << setw(8) << "Proceso" 
              << setw(4) << "BT" 
              << setw(4) << "AT" 
              << setw(3) << "Q"  
              << setw(4) << "WT" 
              << setw(4) << "CT"
              << setw(4) << "RT" 
              << setw(4) << "TAT" << "\n";
    cout << string(60, '-') << "\n";

    for (const auto& p : results) {
        cout << setw(8) << p.label 
                  << setw(4) << p.burst 
                  << setw(4) << p.arrival 
                  << setw(3) << p.queue 
                  << setw(4) << p.waiting_time 
                  << setw(4) << p.completion_time
                  << setw(4) << p.response_time 
                  << setw(4) << p.turnaround_time << "\n";
    }

    cout << string(60, '-') << "\n";
    cout << "Promedios:\n";
    cout << "  Tiempo de Espera (WT):     " << metrics["WT"] << "\n";
    cout << "  Tiempo de Finalizacion (CT): " << metrics["CT"] << "\n";
    cout << "  Tiempo de Respuesta (RT):  " << metrics["RT"] << "\n";
    cout << "  Tiempo de Retorno (TAT):   " << metrics["TAT"] << "\n";
    int total_time = 0;
    for (const auto& p : results) 
        total_time = max(total_time, p.completion_time);
    cout << "  Tiempo Total de Simulacion: " << total_time << "\n";
}
