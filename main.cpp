#include "funzioni.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Uso: " << argv[0] << " <folder_name>" << endl;
        return 1;
    }
    TApplication app("app", 0, 0);
    string folder_name = argv[1];

    // Contiene i nomi delle RUN
    vector<string> run_names;

    // Carica i dati in più vettori e salva i nomi delle RUN
    vector<vector<muone>> eventi_per_file = load_multiple_root_files(folder_name, run_names);
    cout << "============================================" << endl;

    // Stampa il numero di eventi caricati per ciascun file con il nome della RUN
    for (size_t i = 0; i < eventi_per_file.size(); i++) {
        cout << run_names[i] << ": " << eventi_per_file[i].size() << " eventi caricati" << endl;
    }
    cout << "============================================" << endl;
    cout << " ALCUNI DATI PER OGNI RUN " << endl;
    cout << "============================================" << endl;
    
    vector<double> run_indices;
    vector<double> muon_rates;
    double delta_t = 0;
    double rate = 0;
    for (size_t i = 0; i < eventi_per_file.size(); i++) {
        if (eventi_per_file[i].empty()) {
            cout << i+1 << ") " << run_names[i] << ": RUN vuota, salto l'analisi." << endl;
            continue;
        }else{
            cout << i+1 << ") " << run_names[i] << ": "<< endl;
        }
        cout << "Tempo totale della RUN: t = " << total_run_time(eventi_per_file[i]) << " s" << endl;
        delta_t = mean_delta_t(eventi_per_file[i])*1e-9;
        rate = 1.0/(delta_t);
        cout <<"<delta_t> = " << delta_t << " s | rate = "<< 1.0/(delta_t) << " Hz" << endl;
        cout << "Il numero di eventi singoli registrati è: " << Nevents(eventi_per_file[i]) << endl;
        run_indices.push_back(i+1);
        muon_rates.push_back(rate);
        
        bool TrackID = false;
        for (const auto& e : eventi_per_file[i]) {
            if (e.trackID != -1) {
                TrackID = true;
                break;
            }
        }
        if (TrackID) {
            cout << "Il numero di eventi bundle (muoni per traccia > 1) registrati è: " << muon_bundle(eventi_per_file[i]) << " e rappresentano il: " << (double) muon_bundle(eventi_per_file[i])/((double)Nevents(eventi_per_file[i]))*100 << "%. " << endl;
        } else {
            cout << "In questo file non è presente il TrackID quindi non si può calcolare il numero di eventi bundle." << endl;
        }
        total_PeSum_histogram(eventi_per_file[i], run_names[i]);
        plot_theta_distribution(eventi_per_file[i], run_names[i]);
        Distance_histogram(eventi_per_file[i], run_names[i]);
        PeSum_vs_Angle(eventi_per_file[i], run_names[i]);
        PeSum_histograms(eventi_per_file[i], run_names[i]);
        cout << "--------------------------------------------" << endl;
    }
    TCanvas *canvas = new TCanvas("canvas", "Rate dei Muoni in Funzione della Run", 800, 600);
    TGraph *graph = new TGraph(run_indices.size(), &run_indices[0], &muon_rates[0]);
    canvas->SetGrid(); // Aggiungi la griglia

    graph->SetTitle("Rate dei Muoni in Funzione della Run;Indice della Run;Rate [Hz]");
    graph->SetMarkerStyle(21); // Usa quadratini come marker
    graph->SetMarkerSize(1.5); // Aumenta la dimensione dei marker
    graph->Draw("AP");

    canvas->SaveAs("Muon_Rate_vs_Run.png");
    app.Run();
    return 0;
}