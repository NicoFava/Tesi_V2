#include "funzioni.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Uso: " << argv[0] << " <folder_name>" << endl;
        return 1;
    }
    TApplication app("app", 0, 0);
    string folder_path = argv[1];

    // Contiene i nomi delle RUN
    vector<string> run_names;

    // Carica i dati in più vettori e salva i nomi delle RUN
    vector<vector<muone>> eventi_per_file = load_multiple_root_files(folder_path, run_names);

    // Stampa il numero di eventi caricati per ciascun file con il nome della RUN
    for (size_t i = 0; i < eventi_per_file.size(); i++) {
        cout << run_names[i] << ": " << eventi_per_file[i].size() << " eventi caricati" << endl;
    }
    cout <<"Tempo tra due eventi successivi e rate:" << endl;
    double delta_t = 0;
    for (size_t i = 0; i < eventi_per_file.size(); i++) {
        cout << run_names[i] << ": "<< endl;
        delta_t = mean_delta_t(eventi_per_file[i])*1e-9;
        cout <<"t = " << delta_t << "s | rate = "<< 1.0/(delta_t) << " Hz" << endl;
        total_PeSum_histogram(eventi_per_file[i], run_names[i]);
        plot_theta_distribution(eventi_per_file[i], run_names[i]);
        Distance_histogram(eventi_per_file[i], run_names[i]);
        cout << "--------------------------------------------" << endl;
    }
    app.Run();
    return 0;
}