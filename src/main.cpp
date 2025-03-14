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

    cout << "============================================" << endl;
    cout << "CONFRONTO TRA TOTALEVENTS WP E CD" << endl;
    cout << "============================================" << endl;

    vector<double> tolerances;
    vector<double> overlap_areas;
    //Infatti per rendere tutto automatico questo for rischia di fare tanti casini! Assolutamente da togliere.
    for (size_t i = 0; i < updated_eventi_per_file.size(); i++) {
        for (size_t j = 0; j < total_eventi_per_file_wp.size(); j++) {
            cout << "RUN: " << total_run_names_wp[j] << endl;
            cout << "Numero di eventi totali registrati da WP: " << total_eventi_per_file_wp[j].size() << endl;
            cout << "Numero di eventi totali registrati da CD: " << total_eventi_per_file_cd[j].size() << endl;
            // Aggiungo il ciclo per la tolleranza
            for (double tolerance_ns = 1.0; tolerance_ns <= 10000000.0; tolerance_ns *= 10) {
                cout << "Prendendo un intervallo di tolleranza di " << tolerance_ns << " ns" << endl;
                cout << "Il numero di eventi con lo stesso tempo registrati da WP e CD: " << count_common_events(total_eventi_per_file_wp[j], total_eventi_per_file_cd[j], tolerance_ns) << endl;
            
                double overlap_area;
                plot_common_events_NPE(total_eventi_per_file_wp[j], total_eventi_per_file_cd[j], tolerance_ns, total_run_names_wp[j], total_run_names_cd[j]);
                plot_common_events_NPE_all(total_eventi_per_file_wp[j], total_eventi_per_file_cd[j], tolerance_ns, total_run_names_wp[j], total_run_names_cd[j]);
                plot_common_events_NPE_muon(total_eventi_per_file_wp[j], total_eventi_per_file_cd[j], updated_eventi_per_file[i], tolerance_ns, total_run_names_wp[j], total_run_names_cd[j], overlap_area);

                tolerances.push_back(tolerance_ns);
                overlap_areas.push_back(overlap_area);
            }
        }
    }

    cout << "============================================" << endl;
    cout << "FINE CONFRONTO WP e CD" << endl;
    cout << "============================================" << endl;

    // Creazione del grafico dell'area di sovrapposizione in funzione della tolleranza
    TCanvas *c_overlap = new TCanvas("c_overlap", "Overlap Area vs Tolerance", 800, 600);
    TGraph *graph_overlap = new TGraph(tolerances.size(), &tolerances[0], &overlap_areas[0]);
    c_overlap->SetGrid();
    // Aggiungi margini al canvas
    c_overlap->SetLeftMargin(0.15);
    c_overlap->SetBottomMargin(0.15);
    graph_overlap->SetTitle("Overlap Area vs Tolerance;Tolerance [ns];Overlap Area");
    graph_overlap->GetXaxis()->SetTitleOffset(1.4); // Sposta il titolo dell'asse X
    graph_overlap->GetYaxis()->SetTitleOffset(1.6); // Sposta il titolo dell'asse Y
    graph_overlap->SetMarkerStyle(21);
    graph_overlap->SetMarkerSize(1.5);
    graph_overlap->Draw("AP");
    c_overlap->SetLogx();

    string main_folder = "images";
    string folder_im= main_folder + "/Overlap_Area_vs_Tolerance_plot";
    if (!fs::exists(main_folder)) {
        fs::create_directory(main_folder);
    }
    if (!fs::exists(folder_im)) {
        fs::create_directory(folder_im);
    }

    string filename_overlap = folder_im + "/Overlap_Area_vs_Tolerance.png";
    c_overlap->SaveAs(filename_overlap.c_str());

    delete c_overlap;
    delete graph_overlap;

    // Misuro il tempo di fine
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    cout << "Tempo totale di esecuzione: " << elapsed.count() << " secondi" << endl;

    app.Run();
    return 0;
}