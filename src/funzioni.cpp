#include "funzioni.h"

vector<muone> load_root_data(const string& filename) {
    vector<muone> eventi;
    string run_name = get_run_name(filename);
    
    TFile *file = TFile::Open(filename.c_str(), "READ");
    if (!file) {
        cerr << "Errore: impossibile aprire il file "<< run_name << "!" << endl;
        return eventi;
    }

    TTree *tree = (TTree*)file->Get("MuonEvents");
    if (!tree) {
        cerr << "Errore: TTree 'MuonEvents' non trovato nel file "<< run_name << "! " << endl;
        file->Close();
        return eventi;
    }

    if (tree->GetEntries() == 0) {
        cerr << "Errore: TTree 'MuonEvents' nel file " << run_name <<" è vuoto!" << endl;
        file->Close();
        return eventi;
    }

    muone evento;
    tree->SetBranchAddress("EvtID", &evento.eventID);
    tree->SetBranchAddress("fSec", &evento.fSec);
    tree->SetBranchAddress("fNanoSec", &evento.fNanosec);
    tree->SetBranchAddress("PeSum", &evento.PeSum);
    tree->SetBranchAddress("inPos._0", &evento.entry_x);
    tree->SetBranchAddress("inPos._1", &evento.entry_y);
    tree->SetBranchAddress("inPos._2", &evento.entry_z);
    tree->SetBranchAddress("outPos._0", &evento.exit_x);
    tree->SetBranchAddress("outPos._1", &evento.exit_y);
    tree->SetBranchAddress("outPos._2", &evento.exit_z);
    tree->SetBranchAddress("Direction._0", &evento.ux);
    tree->SetBranchAddress("Direction._1", &evento.uy);
    tree->SetBranchAddress("Direction._2", &evento.uz);
    tree->SetBranchAddress("PosQuality", &evento.quality);
   // tree->SetBranchAddress("InTime", &evento.entry_time);
   // tree->SetBranchAddress("OutTime", &evento.exit_time);
   
    // Controllo se esiste il branch "TrackNumber"
    bool hasTrackNumber = (tree->GetBranch("TrackNumber") != nullptr);
    if (hasTrackNumber) {
     tree->SetBranchAddress("TrackNumber", &evento.trackID);
    }

    Long64_t nEntries = tree->GetEntries();
    for (Long64_t i = 0; i < nEntries; i++) {
        tree->GetEntry(i);
        evento.distance = sqrt(pow(evento.exit_x - evento.entry_x, 2) + pow(evento.exit_y - evento.entry_y, 2) + pow(evento.exit_z - evento.entry_z, 2));

     // Se il file ha il trackNumber, lo considero normalmente
        if (hasTrackNumber) {
            eventi.push_back(evento);
        } else {
            evento.trackID = -1;
            eventi.push_back(evento);
        }
    }

    file->Close();
    return eventi;
}

void print_all_data(const vector<muone>& eventi) {
    cout << "============================================" << endl;
    cout << " STAMPA COMPLETA DI TUTTI GLI EVENTI " << endl;
    cout << "============================================" << endl;

    for (const auto& ev : eventi) {
        cout << "Evento ID: " << ev.eventID << endl;
        cout << "Traccia: " << ev.trackID << endl;
        cout << "Tempo Secondi: " << ev.fSec << " | Nanosecondi: " << ev.fNanosec << endl;
        cout << "Energia PeSum: " << ev.PeSum << endl;
        cout << "Ingresso (x,y,z): (" << ev.entry_x << ", " << ev.entry_y << ", " << ev.entry_z << ")" << endl;
        cout << "Uscita (x,y,z): (" << ev.exit_x << ", " << ev.exit_y << ", " << ev.exit_z << ")" << endl;
        cout << "Direzione (ux,uy,uz): (" << ev.ux << ", " << ev.uy << ", " << ev.uz << ")" << endl;
        cout << "Qualità Posizione: " << ev.quality << endl;
     // cout << "Tempo Entrata: " << ev.entry_time << " | Tempo Uscita: " << ev.exit_time << endl;
        cout << "Distanza percorsa: "<< ev.distance << " mm" << endl;
        cout << "--------------------------------------------" << endl;
    }
}

void save_all_data_to_file(const vector<muone>& eventi, const string& filename){
    string full_filename = filename + ".txt"; // Aggiunge l'estensione .txt
    ofstream file(full_filename);
    
    if (!file) {
        cerr << "Errore: impossibile aprire il file " << full_filename << " per la scrittura!" << endl;
        return;
    }

    file << "============================================\n";
    file << " STAMPA COMPLETA DI TUTTI GLI EVENTI \n";
    file << "============================================\n";

    for (const auto& ev : eventi) {
        file << "Evento ID: " << ev.eventID << "\n";
        file << "Traccia: " << ev.trackID << "\n";
        file << "Tempo Secondi: " << ev.fSec << " | Nanosecondi: " << ev.fNanosec << "\n";
        file << "Energia PeSum: " << ev.PeSum << "\n";
        file << "Ingresso (x,y,z): (" << ev.entry_x << ", " << ev.entry_y << ", " << ev.entry_z << ")\n";
        file << "Uscita (x,y,z): (" << ev.exit_x << ", " << ev.exit_y << ", " << ev.exit_z << ")\n";
        file << "Direzione (ux,uy,uz): (" << ev.ux << ", " << ev.uy << ", " << ev.uz << ")\n";
        file << "Qualità Posizione: " << ev.quality << "\n";
     // file << "Tempo Entrata: " << ev.entry_time << " | Tempo Uscita: " << ev.exit_time << "\n";
        file << "Distanza percorsa: " << ev.distance << " mm\n";
        file << "--------------------------------------------\n";
    }
    file.close();
    cout << "Dati salvati con successo in " << filename << "!" << endl;
}

void total_PeSum_histogram(const vector<muone>& eventi, const string& run_name) {
    string main_folder = "images";
    string folder_name = main_folder + "/PeSum_plot";
    if (!fs::exists(main_folder)) {
        fs::create_directory(main_folder);
    }
    if (!fs::exists(folder_name)) {
        fs::create_directory(folder_name);
    }
    
    TCanvas *canvas = new TCanvas(("canvas_charge_" + run_name).c_str(), ("Istogramma Energia - " + run_name).c_str(), 800, 600);
    TH1F *charge = new TH1F(("Distribuzione_dell_energia_" + run_name).c_str(), ("Distribuzione dell'energia - " + run_name).c_str(), 100, 100, 100);
    gPad->SetLeftMargin(0.12);
    charge->StatOverflows(kTRUE);
    canvas->SetGrid();
    
    for (const auto& ev : eventi) {
        charge->Fill(ev.PeSum);
    }
    
    charge->GetXaxis()->SetTitle("Charge [p.e.]");
    charge->GetYaxis()->SetTitle("Counts [a.u.]");
    charge->SetLineWidth(2);
    charge->SetFillColorAlpha(kBlue, 1);
    charge->Draw();
    
    string filename = folder_name + "/PeSum_" + run_name + ".png";
    canvas->SaveAs(filename.c_str());
    delete canvas;
    delete charge;
}

void plot_3D_distribution(const vector<muone>& eventi){
    TCanvas *c3D = new TCanvas("c3D", "Distribuzione 3D dei punti di entrata", 800, 600);
    TGraph2D *g3D = new TGraph2D();

    for (const auto &ev : eventi) {
        double x = ev.entry_x;
        double y = ev.entry_y;
        double z = ev.entry_z;
        g3D->SetPoint(g3D->GetN(), x, y, z);
    }

    gStyle->SetPalette(kRainBow);
    gStyle->SetOptStat(0);
    g3D->SetTitle("Distribuzione 3D dei punti di entrata");
    g3D->SetMarkerStyle(21);
    g3D->SetMarkerSize(1.1);
    g3D->Draw("PCOL");

    g3D->GetXaxis()->SetTitle("X [mm]");
    g3D->GetYaxis()->SetTitle("Y [mm]");
    g3D->GetZaxis()->SetTitle("Z [mm]");

    c3D->Update();
}

void plot_polar_angle_distribution(const vector<muone>& eventi, const string& run_name){
    string main_folder = "images";
    string folder_name = main_folder + "/Polar_Angle_plot";
    if (!fs::exists(main_folder)) {
        fs::create_directory(main_folder);
    }
    if (!fs::exists(folder_name)) {
        fs::create_directory(folder_name);
    }
    
    gStyle->SetOptStat(1);
    TCanvas *canvas = new TCanvas(("canvas_polar_angle_" + run_name).c_str(), ("Distribuzione dell'angolo polare - " + run_name).c_str(), 800, 600);
    TH1F *polar = new TH1F(run_name.c_str(), ("Distribuzione dell'angolo polare - " + run_name).c_str(), 100, 100, 100);
    gPad->SetLeftMargin(0.12);
    polar->StatOverflows(kTRUE);
    canvas->SetGrid();
    
    for(const auto& e:eventi){
        //double r = sqrt(pow(e.entry_x, 2) + pow(e.entry_y, 2) + pow(e.entry_z, 2));
        //double theta = acos(e.entry_z/r);
        double theta = acos(e.uz);
        polar->Fill(cos(theta));
    }
    
    polar->SetLineColor(kGreen);
    polar->SetLineWidth(2);
    polar->SetFillColorAlpha(kGreen, 1);
    polar->GetXaxis()->SetTitle("cos(#theta)");
    polar->GetYaxis()->SetTitle("Counts [a.u.]");
    polar->Draw("HIST");
    string filename = folder_name + "/Theta_" + run_name + ".png";
    canvas->SaveAs(filename.c_str());
    delete canvas;
    delete polar;
}

int Nevents(const vector<muone>& eventi) {
    int nevents = 0;
    int last_event_id = -1;
    for (const auto& e : eventi) {
        if (e.eventID != last_event_id) {
            last_event_id = e.eventID;
            nevents++;
        }
    }
    return nevents;
}

int muon_bundle(const vector<muone>& eventi) {
    int bundle_count = 0;

    // Conto il numero di righe per ogni eventID
    for (size_t i = 0; i < eventi.size(); ) {
        int current_eventID = eventi[i].eventID;
        int count = 0;
        while (i < eventi.size() && eventi[i].eventID == current_eventID) {
            count++;
            i++;
        }
        if (count > 1) {
            bundle_count++;
        }
    }

    return bundle_count;
}

float mean_delta_t(const vector<muone>& eventi, const string& run_names){
    int last_entry_time = 0;
    vector<float> intervalli;
    for (size_t i = 1; i < eventi.size(); i++) {
        last_entry_time = eventi[i - 1].fSec + eventi[i - 1].fNanosec;
        if(eventi[i].fSec+eventi[i].fNanosec!=last_entry_time){
            // Converto il tempo in nanosecondi e calcola la differenza
            float t1 = eventi[i - 1].fSec * 1e9 + eventi[i - 1].fNanosec;
            float t2 = eventi[i].fSec * 1e9 + eventi[i].fNanosec;
            float delta_t = t2 - t1;

            // Se l'intervallo è negativo, c'è un errore nell'ordine degli eventi
            if (delta_t < 0) {
                cerr << "Attenzione: Intervallo di tempo negativo rilevato tra due eventi nel file " << run_names << "! " << endl;
            } else {
                intervalli.push_back(delta_t);
            }
        }
    }

    double somma = accumulate(intervalli.begin(), intervalli.end(), 0.0);
    double media = somma / intervalli.size();
    return media;
}

// Inutile ma ho verificato che per la RUN3677 alcuni eventID non erano in ordine 
void sort_events_by_id(vector<muone>& eventi) {
    sort(eventi.begin(), eventi.end(), [](const muone& a, const muone& b) {
        return a.eventID < b.eventID;
    });
}

void PeSum_histograms(const vector<muone>& eventi, const string& run_name) {
    // Controllo se c'è almeno un trackID uguale a -1
    for (const auto& e : eventi) {
        if (e.trackID == -1) {
            cout << "Trovato trackID = -1, salto la creazione degli istogrammi bundle per la run " << run_name << endl;
            return;
        }
    }

    string main_folder = "images";
    string folder_name_singoli = main_folder + "/PeSum_single_plot";
    string folder_name_bundle = main_folder + "/PeSum_bundle_plot";

    if (!fs::exists(main_folder)) {
        fs::create_directory(main_folder);
    }
    if (!fs::exists(folder_name_singoli)) {
        fs::create_directory(folder_name_singoli);
    }
    if (!fs::exists(folder_name_bundle)) {
        fs::create_directory(folder_name_bundle);
    }

    TCanvas *canvasA = new TCanvas(("canvasA_" + run_name).c_str(), ("Istogramma Energia - Muoni Singoli - " + run_name).c_str(), 800, 600);
    TCanvas *canvasB = new TCanvas(("canvasB_" + run_name).c_str(), ("Istogramma Energia - Muoni Bundle - " + run_name).c_str(), 800, 600);
    canvasB->Divide(2, 2);

    TH1F *one = new TH1F(("1_" + run_name).c_str(), ("Distribuzione Energia - Muoni Singoli - " + run_name).c_str(), 100, 0, 1000);
    TH1F *two = new TH1F(("2_" + run_name).c_str(), ("Distribuzione Energia - Muoni Doppi - " + run_name).c_str(), 100, 0, 1000);
    TH1F *three = new TH1F(("3_" + run_name).c_str(), ("Distribuzione Energia - Muoni Tripli - " + run_name).c_str(), 100, 0, 1000);
    TH1F *four = new TH1F(("4_" + run_name).c_str(), ("Distribuzione Energia - Muoni Quadrupli - " + run_name).c_str(), 100, 0, 1000);
    TH1F *five = new TH1F(("4>_" + run_name).c_str(), ("Distribuzione Energia - Muoni >= Quintupli - " + run_name).c_str(), 100, 0, 1000);

    canvasA->SetGrid();
    // Itero attraverso gli eventi, raggruppandoli per eventID
    size_t i = 0;
    while (i < eventi.size()) {
        int current_eventID = eventi[i].eventID;
        double totalPeSum = 0.0;
        int numMuoni = 0;
        // Calcolo la somma totale di PeSum per ogni eventID e conto il numero di tracce
        while (i < eventi.size() && eventi[i].eventID == current_eventID) {
            totalPeSum += eventi[i].PeSum;
            numMuoni++;
            i++;
        }
        // Riempo l'istrogramma appropriato in base al numero di tracce
        if (numMuoni == 1) {
            one->Fill(totalPeSum);
        } else if (numMuoni == 2) {
            two->Fill(totalPeSum);
        } else if (numMuoni == 3) {
            three->Fill(totalPeSum);
        } else if (numMuoni == 4) {
            four->Fill(totalPeSum);
        } else if (numMuoni >= 5) {
            five->Fill(totalPeSum);
        }
    }

    canvasA->cd();
    one->GetXaxis()->SetTitle("Charge [p.e.]");
    one->GetYaxis()->SetTitle("Counts [a.u.]");
    one->SetLineColor(kRed);
    one->SetLineWidth(2);
    one->SetFillColorAlpha(kRed, 1);
    one->Draw();

    canvasB->cd(1);
    gPad->SetGrid();
    two->GetXaxis()->SetTitle("Charge [p.e.]");
    two->GetYaxis()->SetTitle("Counts [a.u.]");
    two->SetLineColor(kRed);
    two->SetLineWidth(2);
    two->SetFillColorAlpha(kRed, 1);
    two->Draw();
    canvasB->cd(2);
    gPad->SetGrid();
    three->GetXaxis()->SetTitle("Charge [p.e.]");
    three->GetYaxis()->SetTitle("Counts [a.u.]");
    three->SetLineColor(kRed);
    three->SetLineWidth(2);
    three->SetFillColorAlpha(kRed, 1);
    three->Draw();
    canvasB->cd(3);
    gPad->SetGrid();
    four->GetXaxis()->SetTitle("Charge [p.e.]");
    four->GetYaxis()->SetTitle("Counts [a.u.]");
    four->SetLineColor(kRed);
    four->SetLineWidth(2);
    four->SetFillColorAlpha(kRed, 1);
    four->Draw();
    canvasB->cd(4);
    gPad->SetGrid();
    five->GetXaxis()->SetTitle("Charge [p.e.]");
    five->GetYaxis()->SetTitle("Counts [a.u.]");
    five->SetLineColor(kRed);
    five->SetLineWidth(2);
    five->SetFillColorAlpha(kRed, 1);
    five->Draw();

    string filenameA = folder_name_singoli + "/PeSum_singoli_" + run_name + ".png";
    string filenameB = folder_name_bundle + "/PeSum_bundle_" + run_name + ".png";
    canvasA->SaveAs(filenameA.c_str());
    canvasB->SaveAs(filenameB.c_str());
    delete canvasA;
    delete canvasB;
    delete one;
    delete two;
    delete three;
    delete four;
    delete five;
}

void Distance_histogram(const vector<muone>& eventi, const string& run_name){
    string main_folder = "images";
    string folder_name = main_folder + "/Distance_plot";
    if (!fs::exists(main_folder)) {
        fs::create_directory(main_folder);
    }
    if (!fs::exists(folder_name)) {
        fs::create_directory(folder_name);
    }
    
    TCanvas *canvas = new TCanvas(("canvas_dist_" + run_name).c_str(), ("Distanza percorsa - " + run_name).c_str(), 800, 600);
    TH1F *dist = new TH1F(run_name.c_str(), ("Distribuzione distanza percorsa - " + run_name).c_str(), 100, 100, 100);
    gPad->SetLeftMargin(0.12);
    dist->StatOverflows(kTRUE);
    canvas->SetGrid();
    for (const auto& ev : eventi) {
        dist->Fill(ev.distance);
    }
    dist->SetLineColor(kOrange);
    dist->GetXaxis()->SetTitle("Distance [mm]");
    dist->GetYaxis()->SetTitle("Counts [a.u.]");
    dist->SetLineWidth(2);
    dist->SetFillColorAlpha(kOrange, 1);
    dist->Draw();
    string filename = folder_name + "/Distance_" + run_name + ".png";
    canvas->SaveAs(filename.c_str());
    delete canvas;
    delete dist;
}

int count_root_files(const string& folder_path) {
    int count = 0;
    
    // Scansiono la cartella e conto i file con estensione .root
    for (const auto& entry : fs::directory_iterator(folder_path)) {
        if (entry.path().extension() == ".root") {
            count++;
        }
    }
    return count;
}

string get_run_name(const string& filepath) {
    // Creo un oggetto path per estrarre il nome del file
    string filename = fs::path(filepath).filename().string();
    // Cerco la posizione del primo carattere underscore nel nome del file e reastituisco la posizione (indice) del carattere
    size_t pos = filename.find("_");
    // Verifico se l'underscore è stato trovato nel nome del file
    if (pos != string::npos) {
        // Estraggo una sottoriga del nome del file che inizia all'indice 0 e ha una lunghezza di pos caratteri
        filename = filename.substr(0, pos);
    }
    return filename;
}

vector<vector<muone>> load_multiple_root_files(const string& folder_path, vector<string>& run_names) {
    vector<vector<muone>> all_eventi;
    vector<string> file_paths;

    // Scansiono la cartella e memorizzo i percorsi dei file con estensione .root
    for (const auto& entry : fs::directory_iterator(folder_path)) {
        if (entry.path().extension() == ".root") {
            file_paths.push_back(entry.path().string());
        }
    }

    // Ordino i percorsi dei file
    sort(file_paths.begin(), file_paths.end());

    // Carico i file ordinati
    for (const auto& file_path : file_paths) {
        string run_name = get_run_name(file_path);
        run_names.push_back(run_name);

        cout << "Caricamento file: " << run_name << endl;
        vector<muone> eventi = load_root_data(file_path);
        all_eventi.push_back(eventi);
    }

    cout << "Numero totale di file ROOT analizzati: " << all_eventi.size() << endl;
    return all_eventi;
}

void PeSum_vs_polar_angle(const vector<muone>& eventi, const string& run_name) {
    string main_folder = "images";
    string folder_name = main_folder + "/PeSum_vs_Polar_Angle_plot";
    if (!fs::exists(main_folder)) {
        fs::create_directory(main_folder);
    }
    if (!fs::exists(folder_name)) {
        fs::create_directory(folder_name);
    }
    TCanvas *canvas = new TCanvas(("canvas_2D_" + run_name).c_str(), ("Heatmap Energia vs Angolo Polare - " + run_name).c_str(), 800, 600);
    gPad->SetRightMargin(0.12);
    canvas->SetGrid();

    TH2F *hist2D = new TH2F( run_name.c_str(), ("Carica vs Angolo Polare - " + run_name).c_str(), 100, 100, 100, 100, 100, 100);

    for (const auto& ev : eventi) {
        hist2D->Fill(cos(acos(ev.uz)), ev.PeSum);
    }

    hist2D->GetXaxis()->SetTitle("cos(#theta)");
    hist2D->GetYaxis()->SetTitle("PeSum [p.e.]");
    hist2D->Draw("COLZ");


    string filename = folder_name + "/PeSum_vs_Polar_Angle_" + run_name + ".png";
    canvas->SaveAs(filename.c_str());
    delete canvas;
    delete hist2D;
}

double total_run_time(const vector<muone>& eventi) {
    if (eventi.empty()) {
        return 0.0;
    }

    double start_time = eventi.front().fSec + eventi.front().fNanosec * 1e-9;
    double end_time = eventi.back().fSec + eventi.back().fNanosec * 1e-9;
    double total_time = end_time - start_time;

    return total_time;
}

vector<RunInfo> load_run_info(const string& filename) {
    vector<RunInfo> run_info;
    ifstream file(filename);
    if (!file) {
        cerr << "Errore: impossibile aprire il file " << filename << " per la lettura!" << endl;
        return run_info;
    }

    string line;
    // Salta la prima riga
    getline(file, line);

    RunInfo last_info;
    string last_run_name;
    bool first_line = true;

    while (getline(file, line)) {
        istringstream iss(line);
        RunInfo info;
        string date_str;
        iss >> info.run_name >> info.file >> info.counts >> info.error >> date_str >> info.time >> info.duration >> info.volume >> info.rate >> info.err_rate;

        // Converti la data in formato AAAA-MM-GG
        stringstream date_ss;
        date_ss << date_str.substr(0, 4) << "-" << date_str.substr(4, 2) << "-" << date_str.substr(6, 2);
        info.date = date_ss.str();

        if (first_line) {
            last_info = info;
            last_run_name = info.run_name;
            first_line = false;
        } else if (info.run_name != last_run_name) {
            run_info.push_back(last_info);
            last_info = info;
            last_run_name = info.run_name;
        } else {
            last_info = info;
        }
    }

    // Aggiungi l'ultima riga memorizzata
    run_info.push_back(last_info);

    file.close();
    return run_info;
}

void plot_muon_rate(const vector<muone>& eventi, const string& run_name, double interval_sec) {
    string main_folder = "images";
    string folder_name = main_folder + "/MuonRate_plot";
    if (!fs::exists(main_folder)) {
        fs::create_directory(main_folder);
    }
    if (!fs::exists(folder_name)) {
        fs::create_directory(folder_name);
    }
    if (eventi.empty()) {
        cerr << "Errore: Nessun evento disponibile all'interno del file " << run_name << " per il calcolo del rate!" << endl;
        return;
    }

    // Trovo il tempo iniziale e finale della RUN
    double t_start = eventi.front().fSec + eventi.front().fNanosec * 1e-9;
    double t_end = eventi.back().fSec + eventi.back().fNanosec * 1e-9;
    double duration = t_end - t_start;

    if (duration <= 0) {
        cerr << "Errore: Tempo totale del file " << run_name << " non valido!" << endl;
        return;
    }

    // Numero di bin = numero di intervalli di tempo approssimando per eccesso
    int num_bins = ceil(duration / interval_sec);

    vector<double> times(num_bins);
    vector<double> rates(num_bins, 0.0);

    for (const auto& ev : eventi) {
        double event_time = ev.fSec + ev.fNanosec * 1e-9;
        // Calcolo l'indice del bin in cui l'evento deve essere inserito
        int bin = (event_time - t_start) / interval_sec;
        if (bin >= 0 && bin < num_bins) {
            rates[bin]++;
        }
    }

    // Calcolo i tempi centrali dei bin
    for (int i = 0; i < num_bins; i++) {
        // Per ottenere il punto medio dell'intervallo
        times[i] = t_start + (i + 0.5) * interval_sec;
        // Verifico se il bin corrente è l'ultimo bin
        if (i == num_bins - 1) {
            // Calcolo il rate per l'ultimo bin in base alla durata effettiva
            double last_bin_duration = duration - (num_bins - 1) * interval_sec;
            rates[i] /= last_bin_duration;
        } else {
            rates[i] /= interval_sec;
        }
    }

    TGraph *graph = new TGraph(num_bins, &times[0], &rates[0]);

    TCanvas *c_rate = new TCanvas(("RateCanvas_" + run_name).c_str(), "Rate Muoni vs Tempo", 800, 600);
    c_rate->SetGrid();
    string title = "Rate dei muoni nel tempo - " + run_name + " (Intervallo: " + to_string(interval_sec) + " s)";
    graph->SetTitle(title.c_str());
    graph->GetXaxis()->SetTitle("Tempo [s]");
    graph->GetYaxis()->SetTitle("Rate [Hz]");
    graph->GetYaxis()->SetRangeUser(0, *max_element(rates.begin(), rates.end()) * 1.1); // Imposta la scala dell'asse Y per partire da 0
    graph->SetMarkerStyle(21);
    graph->SetMarkerSize(1.5);
    graph->SetLineColor(kRed);
    graph->SetLineWidth(2);
    graph->Draw("AP");

    string filename = folder_name + "/MuonRate_" + run_name + ".png";
    c_rate->SaveAs(filename.c_str());

    delete c_rate;
    delete graph;
}

void plot_muon_rate_vs_run(const vector<vector<muone>>& eventi_per_file, const vector<string>& run_names) {
    vector<double> run_indices;
    vector<double> muon_rates;

    for (size_t i = 0; i < eventi_per_file.size(); i++) {
        if (eventi_per_file[i].empty()) {
            continue;
        }

        double total_time = total_run_time(eventi_per_file[i]);
        double rate = eventi_per_file[i].size() / total_time;

        run_indices.push_back(i + 1);
        muon_rates.push_back(rate);
    }

    string main_folder = "images";
    if (!fs::exists(main_folder)) {
        fs::create_directory(main_folder);
    }
    if (!fs::exists(main_folder)) {
        fs::create_directory(main_folder);
    }

    TCanvas *canvas = new TCanvas("canvas", "Rate dei Muoni in Funzione della Run", 800, 600);
    TGraph *graph = new TGraph(run_indices.size(), &run_indices[0], &muon_rates[0]);
    canvas->SetGrid();

    graph->SetTitle("Rate dei Muoni in Funzione della Run;Indice della Run;Rate [Hz]");
    graph->SetMarkerStyle(21);
    graph->SetMarkerSize(1.5);
    graph->Draw("AP");

    string filename = main_folder + "/Muon_Rate_vs_Run.png";
    canvas->SaveAs(filename.c_str());

    delete canvas;
    delete graph;
}

void plot_azimuthal_angle_distribution(const vector<muone>& eventi, const string& run_name) {
    string main_folder = "images";
    string folder_name = main_folder + "/Azimuthal_Angle_plot";
    if (!fs::exists(main_folder)) {
        fs::create_directory(main_folder);
    }
    if (!fs::exists(folder_name)) {
        fs::create_directory(folder_name);
    }
    
    gStyle->SetOptStat(1);
    TCanvas *canvas = new TCanvas(("canvas_azimuthal_angle_" + run_name).c_str(), ("Distribuzione dell'angolo azimutale - " + run_name).c_str(), 800, 600);
    TH1F *azimuthal = new TH1F(run_name.c_str(), ("Distribuzione dell'angolo azimutale - " + run_name).c_str(), 100, -M_PI, M_PI);
    gPad->SetLeftMargin(0.12);
    azimuthal->StatOverflows(kTRUE);
    canvas->SetGrid();
    
    for(const auto& e : eventi){
        double phi = atan2(e.uy, e.ux);
        azimuthal->Fill(phi);
    }
    
    azimuthal->SetLineColor(kBlue);
    azimuthal->SetLineWidth(2);
    azimuthal->SetFillColorAlpha(kBlue, 1);
    azimuthal->GetXaxis()->SetTitle("Azimuthal Angle [rad]");
    azimuthal->GetYaxis()->SetTitle("Counts [a.u.]");
    azimuthal->SetMinimum(0); // Imposta l'asse Y per partire da 0
    azimuthal->Draw("HIST");
    string filename = folder_name + "/Azimuthal_" + run_name + ".png";
    canvas->SaveAs(filename.c_str());
    delete canvas;
    delete azimuthal;
} 

void Polar_vs_Azimuthal_angle(const vector<muone>& eventi, const string& run_name) {
    string main_folder = "images";
    string folder_name = main_folder + "/Polar_vs_Azimuthal_Angle_plot";
    if (!fs::exists(main_folder)) {
        fs::create_directory(main_folder);
    }
    if (!fs::exists(folder_name)) {
        fs::create_directory(folder_name);
    }
    TCanvas *canvas = new TCanvas(("canvas_2D_" + run_name).c_str(), ("Heatmap Angolo Polare vs Angolo Azimutale - " + run_name).c_str(), 800, 600);
    gPad->SetRightMargin(0.12);
    canvas->SetGrid();
    gStyle->SetPalette(kRainBow);

    TH2F *hist2D = new TH2F(run_name.c_str(), ("Angolo Polare vs Angolo Azimutale - " + run_name).c_str(), 100, 100, 100, 100, 100, 100);

    for (const auto& ev : eventi) {
        hist2D->Fill(atan2(ev.uy, ev.ux), cos(acos(ev.uz)));
    }

    hist2D->GetXaxis()->SetTitle("#phi [rad]");
    hist2D->GetYaxis()->SetTitle("cos(#theta)");
    hist2D->Draw("COLZ");

    string filename = folder_name + "/Polar_vs_Azimuthal_Angle_" + run_name + ".png";
    canvas->SaveAs(filename.c_str());
    delete canvas;
    delete hist2D;
}

// Funzione più generale possibile per modifiche future
double distance_point_to_line(const muone& evento){
     // Punto di entrata
     double x1 = evento.entry_x;
     double y1 = evento.entry_y;
     double z1 = evento.entry_z;
 
     // Punto di uscita
     double x2 = evento.exit_x;
     double y2 = evento.exit_y;
     double z2 = evento.exit_z;
 
     // Centro del rivelatore (origine)
     double x0 = 0.0;
     double y0 = 0.0;
     double z0 = 0.0;
 
     // Vettore direzionale della retta (P2 - P1)
     double ux = x2 - x1;
     double uy = y2 - y1;
     double uz = z2 - z1;
 
     // Vettore dal centro del rivelatore al punto di entrata (P1 - P0)
     double vx = x1 - x0;
     double vy = y1 - y0;
     double vz = z1 - z0;
 
     // Prodotto vettoriale u x v
     double cross_x = uy * vz - uz * vy;
     double cross_y = uz * vx - ux * vz;
     double cross_z = ux * vy - uy * vx;
 
     // Norma del prodotto vettoriale |u x v|
     double cross_norm = sqrt(cross_x * cross_x + cross_y * cross_y + cross_z * cross_z);
 
     // Norma del vettore direzionale |u|
     double u_norm = sqrt(ux * ux + uy * uy + uz * uz);
 
     // Distanza punto-retta
     double distance = cross_norm / u_norm;
 
     return distance;
}

void path_distance_histogram(const vector<muone>& eventi, const string& run_name) {
    string main_folder = "images";
    string folder_name = main_folder + "/Path_Distance_plot";
    if (!fs::exists(main_folder)) {
        fs::create_directory(main_folder);
    }
    if (!fs::exists(folder_name)) {
        fs::create_directory(folder_name);
    }

    TCanvas *canvas = new TCanvas(("canvas_distance_" + run_name).c_str(), ("Distribuzione delle distanze origine-retta muone - " + run_name).c_str(), 800, 600);
    TH1F *distance_hist = new TH1F(run_name.c_str(), ("Distribuzione della distanza origine-retta muone - " + run_name).c_str(), 100, 100, 100); // 100 bins, range 0-100 mm
    gPad->SetLeftMargin(0.12);
    canvas->SetGrid();
    for (const auto& e : eventi) {
        double distance = distance_point_to_line(e);
        distance_hist->Fill(distance);
    }
    distance_hist->SetLineColor(kViolet);
    distance_hist->SetLineWidth(2);
    distance_hist->SetFillColorAlpha(kViolet, 1);
    distance_hist->GetXaxis()->SetTitle("Distanza [mm]");
    distance_hist->GetYaxis()->SetTitle("Conteggio");
    distance_hist->Draw("HIST");
    string filename = folder_name + "/Path_Distance_" + run_name + ".png";
    canvas->SaveAs(filename.c_str());
    delete canvas;
    delete distance_hist;
}

double edge_events(const vector<muone>& eventi, double cut_distance){
    int count = 0;
    double threshold = 20548.1 - cut_distance; // 20548.1 mm rappresentanta la distanza massima misurata tra origine e tracciato di un muone tra tutti i file
    for (const auto& e : eventi) {
        double distance = distance_point_to_line(e);
        if(distance >= threshold){ // Se tolgo 2800 mm circa ottengo le dimensioni del CD (circa 35.5 m di diametro)
            count ++;
        } // Conto quanti eventi di muoni passano al bordo (devo capire ancora come definire il bordo)
    }
    return count;
}

void plot_muon_rate_with_edge_cut_vs_run(const vector<vector<muone>>& eventi_per_file, const vector<string>& run_names, double cut_distance) {
    vector<double> run_indices;
    vector<double> muon_rates;

    for (size_t i = 0; i < eventi_per_file.size(); i++) {
        if (eventi_per_file[i].empty()) {
            continue;
        }

        double total_time = total_run_time(eventi_per_file[i]);
        double rate = ((double )eventi_per_file[i].size() - (double)edge_events(eventi_per_file[i], cut_distance)) / total_time;

        run_indices.push_back(i + 1);
        muon_rates.push_back(rate);
    }

    string main_folder = "images";
    if (!fs::exists(main_folder)) {
        fs::create_directory(main_folder);
    }
    if (!fs::exists(main_folder)) {
        fs::create_directory(main_folder);
    }
    
    TCanvas *canvas = new TCanvas("canvas", "Rate dei Muoni in Funzione della Run", 800, 600);
    TGraph *graph = new TGraph(run_indices.size(), &run_indices[0], &muon_rates[0]);
    canvas->SetGrid();

    string title = "Rate dei Muoni in Funzione della Run dopo il Taglio (" + to_string(cut_distance) + " mm);Indice della Run;Rate [Hz]";
    graph->SetTitle(title.c_str());
    graph->SetMarkerStyle(21);
    graph->SetMarkerSize(1.5);
    graph->Draw("AP");

    string filename = main_folder + "/Muon_Rate_edge_cut_vs_Run.png";
    canvas->SaveAs(filename.c_str());

    delete canvas;
    delete graph;
}

void plot_trackID_distribution(const vector<muone>& eventi, const string& run_name) {
    // Controllo se c'è almeno un trackID uguale a -1
    for (const auto& e : eventi) {
        if (e.trackID == -1) {
            cout << "Trovato trackID = -1, salto la creazione dell'istrogramma TrackID per la " << run_name << endl;
            return;
        }
    }
    
    // Trovo il trackID massimo presente negli eventi
    int trackID_max = -1;
    for (const auto& e : eventi) {
        if (e.trackID > trackID_max) {
            trackID_max = e.trackID;
        }
    }

    // Creo un vettore per contare le occorrenze di ciascun trackID
    vector<int> trackID_count(trackID_max + 1, 0);

    // Conto il numero di righe per ogni eventID
    for (size_t i = 0; i < eventi.size(); ) {
        int current_eventID = eventi[i].eventID;
        int count = 0;
        while (i < eventi.size() && eventi[i].eventID == current_eventID) {
            count++;
            i++;
        }
        if (count <= trackID_max + 1) {
            trackID_count[count - 1]++;
        }
    }

    string main_folder = "images";
    string folder_name = main_folder + "/TrackID_Distribution_plot";
    if (!fs::exists(main_folder)) {
        fs::create_directory(main_folder);
    }
    if (!fs::exists(folder_name)) {
        fs::create_directory(folder_name);
    }

    TCanvas *canvas = new TCanvas(("canvas_trackID_" + run_name).c_str(), ("Distribuzione trackID - " + run_name).c_str(), 800, 600);
    TH1F *trackID_hist = new TH1F(run_name.c_str(), ("Distribuzione trackID - " + run_name).c_str(), trackID_max + 1, 0, trackID_max + 1);
    gPad->SetLeftMargin(0.12);
    canvas->SetGrid();

    for (int i = 0; i <= trackID_max; i++) {
        trackID_hist->SetBinContent(i + 1, trackID_count[i]);
        trackID_hist->GetXaxis()->SetBinLabel(i + 1, to_string(i).c_str());
    }

    trackID_hist->SetLineColor(kBlue);
    trackID_hist->SetLineWidth(2);
    trackID_hist->SetFillColorAlpha(kBlue, 1);
    trackID_hist->GetXaxis()->SetTitle("TrackID");
    trackID_hist->GetYaxis()->SetTitle("Conteggio");
    trackID_hist->SetStats(kFALSE); // Disabilito la casella delle statistiche
    trackID_hist->Draw("HIST");

    // Creo una casella di testo in alto a destra per visualizzare il conteggio di ciascun trackID
    TPaveText *pave = new TPaveText(0.7, 0.7, 0.9, 0.9, "NDC");
    pave->SetFillColor(0);
    pave->SetTextAlign(12);
    pave->SetTextSize(0.03);
    pave->SetBorderSize(1); // Imposto la dimensione del bordo
    pave->SetFillStyle(1001); // Imposto lo stile di riempimento
    for (int i = 0; i <= trackID_max; i++) {
        if (trackID_count[i] > 0) {
            pave->AddText(("TrackID " + to_string(i) + ": " + to_string(trackID_count[i])).c_str());
        }
    }
    pave->Draw();

    string filename = folder_name + "/TrackID_Distribution_" + run_name + ".png";
    canvas->SaveAs(filename.c_str());
    delete trackID_hist;
    delete pave;
}

vector<elisaEvents> load_elisa_data(const string& filename) {
    vector<elisaEvents> eventi;

    string run_name = get_run_name(filename);
    
    TFile *file = TFile::Open(filename.c_str(), "READ");
    if (!file) {
        cerr << "Errore: impossibile aprire il file "<< run_name << "!" << endl;
        return eventi;
    }

    // Ottieni l'albero (TTree) dal file ROOT
    TTree *tree = (TTree*)file->Get("Events"); // Sostituisci "tree" con il nome effettivo dell'albero nel file ROOT
    if (!tree) {
        cerr << "Errore nell'ottenere l'albero dal file ROOT: " << filename << endl;
        file->Close();
        return eventi;
    }

    // Definisci le variabili per leggere i dati dall'albero
    elisaEvents event;
    tree->SetBranchAddress("NPE", &event.NPE);
    tree->SetBranchAddress("fSec", &event.fSec);
    tree->SetBranchAddress("fNanoSec", &event.fNanoSec);
    tree->SetBranchAddress("Recox", &event.Recox);
    tree->SetBranchAddress("Recoy", &event.Recoy);
    tree->SetBranchAddress("Recoz", &event.Recoz);
    tree->SetBranchAddress("RunNumber", &event.RunNumber);

    // Leggi i dati dall'albero e salvali nel vettore
    Long64_t nentries = tree->GetEntries();
    for (Long64_t i = 0; i < nentries; i++) {
        tree->GetEntry(i);
        eventi.push_back(event);
    }

    // Chiudi il file ROOT
    file->Close();
    return eventi;
}

vector<vector<elisaEvents>> load_multiple_elisa_files(const string& folder_path, vector<string>& run_names) {
    vector<vector<elisaEvents>> all_eventi;
    vector<string> file_paths;

    // Scansiono la cartella e memorizzo i percorsi dei file con estensione .root
    for (const auto& entry : fs::directory_iterator(folder_path)) {
        if (entry.path().extension() == ".root") {
            file_paths.push_back(entry.path().string());
        }
    }

    // Ordino i percorsi dei file
    sort(file_paths.begin(), file_paths.end());

    // Carico i file ordinati
    for (const auto& file_path : file_paths) {
        // Estraggo il numero della run dal nome del file e aggiungo il prefisso "RUN"
        string filename = fs::path(file_path).filename().string();
        size_t pos1 = filename.find("_");
        size_t pos2 = filename.find("_", pos1 + 1);
        string run_number = filename.substr(pos1 + 1, pos2 - pos1 - 1);
        string run_name = "RUN" + run_number;
        run_names.push_back(run_name);

        cout << "Caricamento file: " << run_name << endl;
        vector<elisaEvents> eventi = load_elisa_data(file_path);
        all_eventi.push_back(eventi);
    }

    cout << "Numero totale di file di Elisa .root analizzati: " << all_eventi.size() << endl;
    return all_eventi;
}

vector<string> find_common_runs(const vector<string>& run_names, const vector<string>& elisa_run_names) {
    vector<string> common_runs;

    for (const auto& run : run_names) {
        if (std::find(elisa_run_names.begin(), elisa_run_names.end(), run) != elisa_run_names.end()) {
            common_runs.push_back(run);
        }
    }

    return common_runs;
}

int count_common_events(const vector<muone>& muone_events, const vector<elisaEvents>& elisa_events) {
    int common_event_count = 0;
    size_t muone_index = 0;
    size_t elisa_index = 0;

    while (muone_index < muone_events.size() && elisa_index < elisa_events.size()) {
        const auto& muone_event = muone_events[muone_index];
        const auto& elisa_event = elisa_events[elisa_index];

        if (muone_event.fSec == elisa_event.fSec) {
            if (muone_event.fNanosec == elisa_event.fNanoSec) {
                common_event_count++;
                muone_index++;
                elisa_index++;
            } else if (muone_event.fNanosec < elisa_event.fNanoSec) {
                muone_index++;
            } else {
                elisa_index++;
            }
        } else if (muone_event.fSec < elisa_event.fSec) {
            muone_index++;
        } else {
            elisa_index++;
        }
    }

    return common_event_count;
}

void save_elisa_data_to_file(const vector<elisaEvents>& eventi, const string& run_name) {
    string full_filename = run_name + "_elisa.txt"; // Aggiunge l'estensione .txt
    ofstream file(full_filename);
    
    if (!file) {
        cerr << "Errore: impossibile aprire il file " << full_filename << " per la scrittura!" << endl;
        return;
    }

    file << "============================================\n";
    file << " STAMPA COMPLETA DEI PRIMI 100.000 EVENTI DI ELISA \n";
    file << "============================================\n";

    size_t max_events = 1000000;
    size_t event_count = 0;

    for (const auto& ev : eventi) {
        if (event_count >= max_events) {
            break;
        }
        file << "Run Number: " << ev.RunNumber << "\n";
        file << "Tempo Secondi: " << ev.fSec << " | Nanosecondi: " << ev.fNanoSec << "\n";
        file << "NPE: " << ev.NPE << "\n";
        file << "Reco (x,y,z): (" << ev.Recox << ", " << ev.Recoy << ", " << ev.Recoz << ")\n";
        file << "--------------------------------------------\n";
        event_count++;
    }
    file.close();
    cout << "Dati di Elisa salvati con successo in " << full_filename << "!" << endl;
}