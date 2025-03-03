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

    string run_info_file = "run_info.txt";
    
    // Carico le informazioni sulle RUN
    vector<RunInfo> run_info_list = load_run_info(run_info_file);
    if (run_info_list.empty()) {
        cerr << "Errore: Nessuna informazione sulle RUN caricata." << endl;
        return 1;
    }

    // Carico i dati in più vettori e salva i nomi delle RUN
    vector<vector<muone>> eventi_per_file = load_multiple_root_files(folder_name, run_names);
    cout << "============================================" << endl;

    // Stampo il numero di eventi caricati per ciascun file con il nome della RUN
    for (size_t i = 0; i < eventi_per_file.size(); i++) {
        cout << run_names[i] << ": " << eventi_per_file[i].size() << " eventi caricati" << endl;
    }
    cout << "============================================" << endl;
    cout << " ALCUNI DATI PER OGNI RUN " << endl;
    cout << "============================================" << endl;
    
    vector<double> run_indices;
    vector<double> muon_rates;
    vector<double> path_distances;
    vector<double> max_path_distances;
    double delta_t = 0;
    double rate = 0;
    double rates = 0;
    double cut_distance = 2800; // Distanza di taglio per gli eventi di bordo
    for (size_t i = 0; i < eventi_per_file.size(); i++) {
        if (eventi_per_file[i].empty()) {
            cout << i+1 << ") " << run_names[i] << ": RUN vuota, salto l'analisi." << endl;
            cout << "--------------------------------------------" << endl;
            continue;
        }else{
            cout << i+1 << ") " << run_names[i] << ": "<< endl;
        }
        
         // Trovo le informazioni sulla RUN
         bool found = false;
         for (const auto& info : run_info_list) {
             if (info.run_name == run_names[i]) {
                 cout << "Data di inizio: " << info.date << " Ora di inizio: " << info.time << endl;
                 found = true;
                 break;
             }
         }
         if (!found) {
             cout << "Informazioni sulla RUN non trovate." << endl;
         }
        
        cout << "Tempo totale della RUN: t = " << total_run_time(eventi_per_file[i]) << " s" << endl;
        cout << "Numero di eventi totali registrati: " << eventi_per_file[i].size() << endl;
        delta_t = mean_delta_t(eventi_per_file[i])*1e-9;
        rate = 1.0/(delta_t);
        rates = eventi_per_file[i].size()/total_run_time(eventi_per_file[i]);
        cout <<"<delta_t> = " << delta_t << " s | rate (contando gli eventi bundle come unici 1/delta_t) = "<< 1.0/(delta_t) << " Hz" << endl;
        cout << "Il rate dei muoni (righe/t_tot) è: " << rates << " Hz." <<  endl;
        cout << "Il numero di eventi univoci per EventID: " << Nevents(eventi_per_file[i]) << endl;
        
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
        
        vector<double> path_distances; // Inizializza il vettore per ogni run
        for (const auto& e : eventi_per_file[i]) {
            double distance = distance_point_to_line(e);
            path_distances.push_back(distance);
        }

        // Calcola e stampa la distanza massima
        double max_dist;
        max_dist = *max_element(path_distances.begin(), path_distances.end());
        max_path_distances.push_back(max_dist);
        cout << "La distanza massima tra l'origine e il tracciato dei muoni è: " << max_dist << " mm" << endl;
        cout << "I muoni che passano nel bordo (cut = " << cut_distance << " mm ) sono: " << edge_events(eventi_per_file[i], cut_distance) << endl;
        cout << "La frequenza togliendo gli eventi di bordo è: " << ((double )eventi_per_file[i].size() - (double)edge_events(eventi_per_file[i], cut_distance)) / total_run_time(eventi_per_file[i]) << " Hz" << endl;
        total_PeSum_histogram(eventi_per_file[i], run_names[i]);
        plot_polar_angle_distribution(eventi_per_file[i], run_names[i]);
        plot_azimuthal_angle_distribution(eventi_per_file[i], run_names[i]);
        Distance_histogram(eventi_per_file[i], run_names[i]);
        PeSum_vs_polar_angle(eventi_per_file[i], run_names[i]);
        PeSum_histograms(eventi_per_file[i], run_names[i]);
        plot_trackID_distribution(eventi_per_file[i], run_names[i]);
        plot_muon_rate(eventi_per_file[i], run_names[i], 300);   
        Polar_vs_Azimuthal_angle(eventi_per_file[i], run_names[i]);
        path_distance_histogram(eventi_per_file[i], run_names[i]);
        cout << "--------------------------------------------" << endl;
    }
    plot_muon_rate_vs_run(eventi_per_file, run_names);
    plot_muon_rate_with_edge_cut_vs_run(eventi_per_file, run_names, cut_distance);
    cout << "La distanza del tracciato con l'origine massima registrata tra tutti i file è: " << *max_element(max_path_distances.begin(), max_path_distances.end()) << " mm" << endl;
    cout << "============================================" << endl;
    cout << "FINE ANALISI" << endl;
    cout << "============================================" << endl;
    app.Run();
    return 0;
}