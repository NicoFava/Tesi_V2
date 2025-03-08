#include "funzioni.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Uso: " << argv[0] << " <folder_name>" << endl;
        return 1;
    }

    // Reindirizzo stdout verso un file e stderr verso un altro file
    freopen("output.txt", "w", stdout);
    freopen("error&info.txt", "w", stderr);

    // Misuro il tempo di inizio
    auto start = std::chrono::high_resolution_clock::now();

    TApplication app("app", 0, 0);
    string folder_name = argv[1];

    // Abilito la modalità batch di ROOT
    gROOT->SetBatch(kTRUE);

    // Carico i dati di Elisa
    vector<string> elisa_run_names;
    vector<vector<elisaEvents>> elisa_eventi_per_file = load_multiple_elisa_files(folder_name + "/elisa_wp", elisa_run_names);
    cout << "============================================" << endl;

    // Contiene i nomi delle RUN
    vector<string> run_names;

    string run_info_file = "BiPo_rate_byfile.txt";
    
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

    // Stampo le RUN comuni
    // Trovo le RUN comuni
    vector<string> common_runs = find_common_runs(run_names, elisa_run_names);
    cout << "RUN comuni trovate:" << endl;
    for (const auto& run : common_runs) {
        cout << run << endl;
    }
    
    // Conto gli eventi comuni per ogni run comune
    for (const auto& run_name : common_runs) {
        size_t muone_index = std::distance(run_names.begin(), std::find(run_names.begin(), run_names.end(), run_name));
        size_t elisa_index = std::distance(elisa_run_names.begin(), std::find(elisa_run_names.begin(), elisa_run_names.end(), run_name));

        cout << "Analizzando la run: " << run_name << endl;
        cout << "Numero di eventi muone: " << eventi_per_file[muone_index].size() << endl;
        cout << "Numero di eventi elisa: " << elisa_eventi_per_file[elisa_index].size() << endl;

        int common_event_count = count_common_events(eventi_per_file[muone_index], elisa_eventi_per_file[elisa_index]);

        // Stampo il numero di eventi comuni
        cout << "Run: " << run_name << " - Numero di eventi comuni: " << common_event_count << endl;
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
        } else {
            cout << i+1 << ") " << run_names[i] << ": " << endl;
        }
        
        // Trovo le informazioni sulla RUN
        bool found = false;
        for (const auto& info : run_info_list) {
            if (info.run_name == run_names[i]) {
                cout << "Data di inizio: " << info.date << " | Ora di inizio: " << info.time <<" | Durata: "<< info.duration << " s. " << endl;
                found = true;
                break;
            }
        }
        if (!found) {
            cout << "Informazioni sulla RUN non trovate." << endl;
        }
        
        cout << "Tempo totale della RUN: t = " << total_run_time(eventi_per_file[i]) << " s" << endl;
        cout << "Numero di eventi totali registrati: " << eventi_per_file[i].size() << endl;
        delta_t = mean_delta_t(eventi_per_file[i], run_names[i])*1e-9;
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
        
        vector<double> path_distances; // Inizializzo il vettore per ogni run
        for (const auto& e : eventi_per_file[i]) {
            double distance = distance_point_to_line(e);
            path_distances.push_back(distance);
        }

        // Calcolo e stampo la distanza massima
        double max_dist;
        max_dist = *max_element(path_distances.begin(), path_distances.end());
        max_path_distances.push_back(max_dist);
        cout << "La distanza massima tra l'origine e il tracciato dei muoni è: " << max_dist << " mm" << endl;
        cout << "I muoni che passano nel bordo (cut = " << cut_distance << " mm ) sono: " << edge_events(eventi_per_file[i], cut_distance) << endl;
        cout << "La frequenza togliendo gli eventi di bordo è: " << ((double )eventi_per_file[i].size() - (double)edge_events(eventi_per_file[i], cut_distance)) / total_run_time(eventi_per_file[i]) << " Hz" << endl;
        //total_PeSum_histogram(eventi_per_file[i], run_names[i]);
        plot_polar_angle_distribution(eventi_per_file[i], run_names[i]);
        plot_azimuthal_angle_distribution(eventi_per_file[i], run_names[i]);
        Distance_histogram(eventi_per_file[i], run_names[i]);
        //PeSum_vs_polar_angle(eventi_per_file[i], run_names[i]);
        //PeSum_histograms(eventi_per_file[i], run_names[i]);
        plot_trackID_distribution(eventi_per_file[i], run_names[i]);
        plot_muon_rate(eventi_per_file[i], run_names[i], 300);   
        Polar_vs_Azimuthal_angle(eventi_per_file[i], run_names[i]);
        path_distance_histogram(eventi_per_file[i], run_names[i]);
        // Provo a stampare tutte le informazioni di una run per verificare le varie quantità
        if(run_names[i] == "RUN3990"){
            save_all_data_to_file(eventi_per_file[i], run_names[i]);
        }
        cout << "--------------------------------------------" << endl;
    }
    plot_muon_rate_vs_run(eventi_per_file, run_names);
    plot_muon_rate_with_edge_cut_vs_run(eventi_per_file, run_names, cut_distance);
    cout << "La distanza del tracciato con l'origine massima registrata tra tutti i file è: " << *max_element(max_path_distances.begin(), max_path_distances.end()) << " mm" << endl;
    cout << "============================================" << endl;
    cout << "FINE ANALISI" << endl;
    cout << "============================================" << endl;

    // Misuro il tempo di fine
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    cout << "Tempo totale di esecuzione: " << elapsed.count() << " secondi" << endl;

    app.Run();
    return 0;
}