#include "funzioni.h"

using namespace std;

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
    cout << "============================================" << endl;
    cout << " CARICAMENTO DATI WPMUONCLASSIFY " << endl;
    cout << "============================================" << endl;
    vector<vector<muone>> eventi_per_file = load_multiple_root_files(folder_name, run_names);
    cout << "============================================" << endl;

    // Stampo il numero di eventi caricati per ciascun file con il nome della RUN
    for (size_t i = 0; i < eventi_per_file.size(); i++) {
        cout << run_names[i] << ": " << eventi_per_file[i].size() << " eventi caricati" << endl;
    }
    
    // Carico i dati totalEvents
    cout << "============================================" << endl;
    cout << " CARICAMENTO DATI TOTALI DA WP " << endl;
    cout << "============================================" << endl;
    vector<string> total_run_names_wp;
    vector<vector<totalEvents>> total_eventi_per_file_wp = load_multiple_totalEvents_files(folder_name + "/total_wp", total_run_names_wp);
    cout << "============================================" << endl;
    
    // Stampo il numero di eventi caricati per ciascun file con il nome della RUN
    for (size_t i = 0; i < total_eventi_per_file_wp.size(); i++) {
        cout << total_run_names_wp[i] << ": " << total_eventi_per_file_wp[i].size() << " eventi caricati" << endl;
    }    
    
    cout << "============================================" << endl;
    cout << " CARICAMENTO DATI TOTALI DA CD " << endl;
    cout << "============================================" << endl;
    vector<string> total_run_names_cd;
    vector<vector<totalEvents>> total_eventi_per_file_cd = load_multiple_totalEvents_files(folder_name + "/total_cd", total_run_names_cd);
    cout << "============================================" << endl;
    
    // Stampo il numero di eventi caricati per ciascun file con il nome della RUN
    for (size_t i = 0; i < total_eventi_per_file_cd.size(); i++) {
        cout << total_run_names_cd[i] << ": " << total_eventi_per_file_cd[i].size() << " eventi caricati" << endl;
    }
    cout << "============================================" << endl;
    
    // Trovo le run comuni e salvo gli indici
    vector<pair<size_t, size_t>> common_run_indices_wp = find_common_runs(run_names, total_run_names_wp);
    vector<pair<size_t, size_t>> common_run_indices_cd = find_common_runs(run_names, total_run_names_cd);

    // Stampo le RUN comuni
    cout << "RUN comuni tra wpclassify e wp_total:" << endl;
    for (size_t i = 0; i < common_run_indices_wp.size(); i++) {
        cout << i+1 << ") " << run_names[common_run_indices_wp[i].first] << endl;
    }
    cout << "--------------------------------------------" << endl;
    cout << "RUN comuni tra wpclassify e cd_total:" << endl;
    for (size_t i = 0; i < common_run_indices_cd.size(); i++) {
        cout << i+1 << ") " << run_names[common_run_indices_cd[i].first] << endl;
    }
    
    cout << "============================================" << endl;
    cout << " ALCUNI DATI PER OGNI RUN " << endl;
    cout << "============================================" << endl;
    
    vector<double> run_indices;
    vector<double> muon_rates;
    vector<double> path_distances;
    vector<double> max_path_distances;
    long double delta_t = 0;
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
        cout << "La frequenza calcolata solo con gli eventi univoci è: " << Nevents(eventi_per_file[i])/total_run_time(eventi_per_file[i]) << " Hz" << endl;
        
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
        PeSum_histogram(eventi_per_file[i], run_names[i]);
        plot_polar_angle_distribution(eventi_per_file[i], run_names[i]);
        plot_azimuthal_angle_distribution(eventi_per_file[i], run_names[i]);
        Distance_histogram(eventi_per_file[i], run_names[i]);
        PeSum_vs_polar_angle(eventi_per_file[i], run_names[i]);
        PeSum_vs_azimuthal_angle(eventi_per_file[i], run_names[i]);
        PeSum_histograms(eventi_per_file[i], run_names[i]);
        plot_trackID_distribution(eventi_per_file[i], run_names[i]);
        plot_muon_rate(eventi_per_file[i], run_names[i], 300);   
        Polar_vs_Azimuthal_angle(eventi_per_file[i], run_names[i]);
        path_distance_histogram(eventi_per_file[i], run_names[i]);
        // Provo a stampare tutte le informazioni di una run per verificare le varie quantità
        if(run_names[i] == "RUN4049"){
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
    
    // Analisi dei file modificati
    cout << "============================================" << endl;
    cout << " ALCUNI DATI PER OGNI RUN MODIFICATA " << endl;
    cout << "============================================" << endl;
    // Creo un nuovo vector di vector muone con gli eventi aggiornati
    vector<string> run_names_mod;
    vector<vector<muone>> updated_eventi_per_file = create_updated_events_vector(eventi_per_file, total_eventi_per_file_wp, common_run_indices_wp, run_names, run_names_mod);
    for (size_t i = 0; i < updated_eventi_per_file.size(); i++){
        if (updated_eventi_per_file[i].empty()) {
            cout << i+1 << ") " << run_names_mod[i] << ": RUN modificata vuota, salto l'analisi." << endl;
            cout << "--------------------------------------------" << endl;
            continue;
        } else {
            cout << i+1 << ") " << run_names_mod[i] << ": " << endl;
        }
        // Trovo le informazioni sulla RUN
        bool found = false;
        for (const auto& info : run_info_list) {
            if (info.run_name == run_names[common_run_indices_wp[i].first]) {
                cout << "Data di inizio: " << info.date << " | Ora di inizio: " << info.time <<" | Durata: "<< info.duration << " s. " << endl;
                found = true;
                break;
            }
        }
        if (!found) {
            cout << "Informazioni sulla RUN non trovate." << endl;
        }

        cout << "Tempo totale della RUN: t = " << total_run_time(updated_eventi_per_file[i]) << " s" << endl;
        cout << "Numero di eventi totali registrati: " << updated_eventi_per_file[i].size() << endl;
        delta_t = mean_delta_t(updated_eventi_per_file[i], run_names[common_run_indices_wp[i].first])*1e-9;
        rate = 1.0/(delta_t);
        rates = updated_eventi_per_file[i].size()/total_run_time(updated_eventi_per_file[i]);
        cout <<"<delta_t> = " << delta_t << " s | rate (contando gli eventi bundle come unici 1/delta_t) = "<< 1.0/(delta_t) << " Hz" << endl;
        cout << "Il rate dei muoni (righe/t_tot) è: " << rates << " Hz." <<  endl;
        vector<double> path_distances; // Inizializzo il vettore per ogni run
        for (const auto& e : updated_eventi_per_file[i]) {
            double distance = distance_point_to_line(e);
            path_distances.push_back(distance);
        }
        // Calcolo e stampo la distanza massima
        double max_dist;
        max_dist = *max_element(path_distances.begin(), path_distances.end());
        cout << "La distanza massima tra l'origine e il tracciato dei muoni è: " << max_dist << " mm" << endl;
        cout << "I muoni che passano nel bordo (cut = " << cut_distance << " mm ) sono: " << edge_events(updated_eventi_per_file[i], cut_distance) << endl;
        cout << "La frequenza togliendo gli eventi di bordo è: " << ((double )updated_eventi_per_file[i].size() - (double)edge_events(updated_eventi_per_file[i], cut_distance)) / total_run_time(updated_eventi_per_file[i]) << " Hz" << endl;
        PeSum_histogram_log(updated_eventi_per_file[i], run_names_mod[i]);
        PeSum_vs_polar_angle(updated_eventi_per_file[i], run_names_mod[i]);
        PeSum_vs_azimuthal_angle(updated_eventi_per_file[i], run_names_mod[i]);
        plot_muon_rate(updated_eventi_per_file[i], run_names_mod[i], 300);   
        plot_time_difference_vs_charge(total_eventi_per_file_wp[i], total_run_names_wp[i]);
        total_PeSum_histogram_log(total_eventi_per_file_wp[i], updated_eventi_per_file[i], total_run_names_wp[i] , run_names_mod[i]);
        total_PeSum_histogram_log_complementary(total_eventi_per_file_wp[i], updated_eventi_per_file[i], total_run_names_wp[i], run_names_mod[i]);
        count_high_energy_events(total_eventi_per_file_wp[i], updated_eventi_per_file[i], total_run_names_wp[i], run_names_mod[i]);
        // Provo a stampare tutte le informazioni di una run per verificare le varie quantità
        if(run_names[i] == "RUN4049mod"){
            save_all_data_to_file(updated_eventi_per_file[i], run_names_mod[i]);
        }
        total_PeSum_histogram_log_divided(total_eventi_per_file_wp[i], updated_eventi_per_file[i], total_run_names_wp[i] , run_names_mod[i]);
        total_PeSum_histogram_log_divided_track(total_eventi_per_file_wp[i], updated_eventi_per_file[i], total_run_names_wp[i] , run_names_mod[i]);
        path_distance_histogram(updated_eventi_per_file[i], run_names_mod[i]);
    }
    cout << "============================================" << endl;
    cout << "FINE ANALISI DELLE RUN MODIFICATE" << endl;
    cout << "============================================" << endl;
    for (size_t i = 0; i < total_eventi_per_file_wp.size(); i++){
        total_PeSum_histogram_log(total_eventi_per_file_wp[i], total_run_names_wp[i]);
    }
    
    //Fino a qui dovrebbe funzionare per un numero arbitrario di file di eventi totali WP.
    //Da qui in poi invece avendo aggiunto anche i file CD non è automatico analizzare più file WP WPmuonclassifytool e CD
    //Per ora so che c'è solo una run (la RUN4049) in comune tra WPmuonclassifytool WP e CD
    //Quindi per ora analizzo solo quella run
    // Una volta ottenuti diversi file potrei tenere solo quelli "in comune" ed eliminare gli altri tenendo magari i grafici essenziali
    // Oppure creo una seconda cartella inserendo i datasets wpclassify, CD e WP in maniera analoga ad ora ma mettendo solo quelli che hanno run in comune e le analizzo in maniera autonoma
    cout << "============================================" << endl;
    cout << "CONFRONTO TRA TOTALEVENTS WP E CD" << endl;
    cout << "============================================" << endl;
    
    analyze_total_wp_cd(total_eventi_per_file_wp, total_eventi_per_file_cd, updated_eventi_per_file, total_run_names_wp, total_run_names_cd, run_names_mod);

    cout << "============================================" << endl;
    cout << "FINE CONFRONTO WP e CD" << endl;
    cout << "============================================" << endl;
    // Misuro il tempo di fine
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    cout << "Tempo totale di esecuzione: " << elapsed.count() << " secondi" << endl;

    app.Run();
    return 0;
}