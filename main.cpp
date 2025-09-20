#include "scheduler.h"
#include <iostream>
#include <fstream>

using namespace std;

int main() {
    string input = "mlfq001.txt";

    for (int scheme = 1; scheme <= 3; scheme++) {
        try {
            auto scheduler = MLFQScheduler::from_file(input, scheme);
            auto results = scheduler.run();
            string out_name = "scheme" + to_string(scheme) + "output.txt";
            ofstream out(out_name);
            if (!out.is_open()) {
                cerr << "No se pudo abrir " << out_name << " para escribir.\n";
                continue;
            }

            streambuf* coutbuf = cout.rdbuf();
            cout.rdbuf(out.rdbuf());

            scheduler.print_results(results);

            cout.rdbuf(coutbuf);

            cout << "Esquema " << scheme << " -> resultados guardados en " << out_name << "\n";

        } catch (const exception& e) {
            cerr << "Error en esquema " << scheme << ": " << e.what() << "\n";
        }
    }

    return 0;
}
