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

void PeSum_histogram(const vector<muone>& eventi, const string& run_name) {
    string main_folder = "images";
    string folder_name = main_folder + "/PeSum_plot";
    if (!fs::exists(main_folder)) {
        fs::create_directory(main_folder);
    }
    if (!fs::exists(folder_name)) {
        fs::create_directory(folder_name);
    }
    
    TCanvas *canvas = new TCanvas(("canvas_charge_" + run_name).c_str(), ("Istogramma Carica - " + run_name).c_str(), 800, 600);
    TH1F *charge = new TH1F(("Distribuzione_della_carica_" + run_name).c_str(), ("Distribuzione della Carica - " + run_name).c_str(), 100, 100, 100);
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

long double mean_delta_t(const vector<muone>& eventi, const string& run_names){
    int last_entry_time = 0;
    vector<long double> intervalli;
    for (size_t i = 1; i < eventi.size(); i++) {
        last_entry_time = eventi[i - 1].fSec + eventi[i - 1].fNanosec;
        if(eventi[i].fSec+eventi[i].fNanosec!=last_entry_time){
            // Converto il tempo in nanosecondi e calcola la differenza
            long double t1 = (long double) eventi[i - 1].fSec * 1e9 + (long double) eventi[i - 1].fNanosec;
            long double t2 = (long double) eventi[i].fSec * 1e9 + (long double) eventi[i].fNanosec;
            long double delta_t = t2 - t1;

            // Se l'intervallo è negativo, c'è un errore nell'ordine degli eventi
            if (delta_t < 0) {
                cerr << "Attenzione: Intervallo di tempo negativo rilevato tra due eventi nel file " << run_names << "! " << endl;
            } else {
                intervalli.push_back(delta_t);
            }
        }
    }

    long double somma = accumulate(intervalli.begin(), intervalli.end(), 0.0);
    long double media = somma / intervalli.size();
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

    TCanvas *canvasA = new TCanvas(("canvasA_" + run_name).c_str(), ("Istogramma Carica - Muoni Singoli - " + run_name).c_str(), 800, 600);
    TCanvas *canvasB = new TCanvas(("canvasB_" + run_name).c_str(), ("Istogramma Carica - Muoni Bundle - " + run_name).c_str(), 800, 600);
    canvasB->Divide(2, 2);

    TH1F *one = new TH1F(("1_" + run_name).c_str(), ("Distribuzione Carica - Muoni Singoli - " + run_name).c_str(), 100, 0, 1000);
    TH1F *two = new TH1F(("2_" + run_name).c_str(), ("Distribuzione Carica - Muoni Doppi - " + run_name).c_str(), 100, 0, 1000);
    TH1F *three = new TH1F(("3_" + run_name).c_str(), ("Distribuzione Carica - Muoni Tripli - " + run_name).c_str(), 100, 0, 1000);
    TH1F *four = new TH1F(("4_" + run_name).c_str(), ("Distribuzione Carica - Muoni Quadrupli - " + run_name).c_str(), 100, 0, 1000);
    TH1F *five = new TH1F(("gt4_" + run_name).c_str(), ("Distribuzione caroca - Muoni gt Quadrupli - " + run_name).c_str(), 100, 0, 1000);

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
    TCanvas *canvas = new TCanvas(("canvas_2D_" + run_name).c_str(), ("Heatmap Carica vs Angolo Polare - " + run_name).c_str(), 800, 600);
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

void PeSum_vs_azimuthal_angle(const vector<muone>& eventi, const string& run_name) {
    string main_folder = "images";
    string folder_name = main_folder + "/PeSum_vs_Azimuthal_Angle_plot";
    if (!fs::exists(main_folder)) {
        fs::create_directory(main_folder);
    }
    if (!fs::exists(folder_name)) {
        fs::create_directory(folder_name);
    }
    TCanvas *canvas = new TCanvas(("canvas_2D_" + run_name).c_str(), ("Heatmap Carica vs Angolo Azimutale - " + run_name).c_str(), 800, 600);
    gPad->SetRightMargin(0.12);
    canvas->SetGrid();

    TH2F *hist2D = new TH2F( run_name.c_str(), ("Carica vs Angolo Azimutale - " + run_name).c_str(), 100, 100, 100, 100, 100, 100);

    for (const auto& ev : eventi) {
        hist2D->Fill(atan2(ev.uy, ev.ux), ev.PeSum);
    }

    hist2D->GetXaxis()->SetTitle("#phi [rad]");
    hist2D->GetYaxis()->SetTitle("PeSum [p.e.]");
    hist2D->Draw("COLZ");


    string filename = folder_name + "/PeSum_vs_Azimuthal_Angle_" + run_name + ".png";
    canvas->SaveAs(filename.c_str());
    delete canvas;
    delete hist2D;
}

long double total_run_time(const vector<muone>& eventi) {
    if (eventi.empty()) {
        return 0.0;
    }

    long double start_time = (long double) eventi.front().fSec + (long double) eventi.front().fNanosec * 1e-9;
    long double end_time = (long double) eventi.back().fSec + (long double) eventi.back().fNanosec * 1e-9;
    long double total_time = end_time - start_time;

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
    // Salto la prima riga
    getline(file, line);

    RunInfo last_info;
    string last_run_name;
    bool first_line = true;

    while (getline(file, line)) {
        istringstream iss(line);
        RunInfo info;
        string date_str;
        iss >> info.run_name >> info.file >> info.counts >> info.error >> date_str >> info.time >> info.duration >> info.volume >> info.rate >> info.err_rate;

        // Converto la data in formato AAAA-MM-GG
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

    // Aggiungo l'ultima riga memorizzata
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
    long double t_start = (long double) eventi.front().fSec + (long double) eventi.front().fNanosec * 1e-9;
    long double t_end = (long double) eventi.back().fSec + (long double) eventi.back().fNanosec * 1e-9;
    long double duration = (long double) t_end - (long double) t_start;

    if (duration <= 0) {
        cerr << "Errore: Tempo totale del file " << run_name << " non valido!" << endl;
        return;
    }

    // Numero di bin = numero di intervalli di tempo approssimando per eccesso
    int num_bins = ceil(duration / interval_sec);

    vector<double> times(num_bins);
    vector<double> rates(num_bins, 0.0);
    vector<double> errors(num_bins, 0.0);
    for (const auto& ev : eventi) {
        long double event_time = ev.fSec + ev.fNanosec * 1e-9;
        // Calcolo l'indice del bin in cui l'evento deve essere inserito
        int bin = (event_time - t_start) / interval_sec;
        if (bin >= 0 && bin < num_bins) {
            rates[bin]++;
        }
    }

    // Calcolo i tempi centrali dei bin e gli errori
    for (int i = 0; i < num_bins; i++) {
        // Per ottenere il punto medio dell'intervallo
        times[i] = t_start + (i + 0.5) * interval_sec;
        // Verifico se il bin corrente è l'ultimo bin
        if (i == num_bins - 1) {
            // Calcolo il rate per l'ultimo bin in base alla durata effettiva
            long double last_bin_duration = duration - (num_bins - 1) * interval_sec;
            rates[i] /= last_bin_duration;
            errors[i] = sqrt(rates[i] * last_bin_duration) / last_bin_duration;
        } else {
            rates[i] /= interval_sec;
            errors[i] = sqrt(rates[i] * interval_sec) / interval_sec;
        }
    }

    TGraphErrors *graph = new TGraphErrors(num_bins, &times[0], &rates[0], nullptr, &errors[0]);

    TCanvas *c_rate = new TCanvas(("RateCanvas_" + run_name).c_str(), "Rate Muoni vs Tempo", 800, 600);
    c_rate->SetGrid();
    stringstream title;
    title << "Rate dei muoni nel tempo - " << run_name << " (Intervallo: " << fixed << setprecision(1) << interval_sec << " s)";
    graph->SetTitle(title.str().c_str());
    graph->GetXaxis()->SetTitle("Tempo [s]");
    graph->GetYaxis()->SetTitle("Rate [Hz]");
    graph->GetYaxis()->SetRangeUser(0, *max_element(rates.begin(), rates.end()) * 1.1); // Imposta la scala dell'asse Y per partire da 0
    graph->SetMarkerStyle(20);
    graph->SetMarkerSize(1.4);
    graph->Draw("AP");

    string filename = folder_name + "/MuonRate_" + run_name + ".png";
    c_rate->SaveAs(filename.c_str());

    delete c_rate;
    delete graph;
}

void plot_muon_rate_vs_run(const vector<vector<muone>>& eventi_per_file, const vector<string>& run_names) {
    vector<double> run_indices;
    vector<double> muon_rates;
    vector<double> errors;

    for (size_t i = 0; i < eventi_per_file.size(); i++) {
        if (eventi_per_file[i].empty()) {
            continue;
        }

        long double total_time = total_run_time(eventi_per_file[i]);
        double rate = eventi_per_file[i].size() / (long double) total_time;
        double error = sqrt(eventi_per_file[i].size()) / total_time;

        run_indices.push_back(i + 1);
        muon_rates.push_back(rate);
        errors.push_back(error);
    }

    string main_folder = "images";
    if (!fs::exists(main_folder)) {
        fs::create_directory(main_folder);
    }
    if (!fs::exists(main_folder)) {
        fs::create_directory(main_folder);
    }

    TCanvas *canvas = new TCanvas("canvas", "Rate dei Muoni in Funzione della Run", 800, 600);
    TGraphErrors *graph = new TGraphErrors(run_indices.size(), &run_indices[0], &muon_rates[0], nullptr, &errors[0]);
    canvas->SetGrid();

    graph->SetTitle("Rate dei Muoni in Funzione della Run;Indice della Run;Rate [Hz]");
    graph->SetMarkerStyle(20);
    graph->SetMarkerSize(1.4);
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
    azimuthal->GetXaxis()->SetTitle("#phi [rad]");
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

    TCanvas *canvas = new TCanvas(("canvas_distance_" + run_name).c_str(), ("Distribuzione delle distanze dall'origine al tracciato del muone - " + run_name).c_str(), 800, 600);
    TH1F *distance_hist = new TH1F(run_name.c_str(), ("Distribuzione delle distanze dall'origine al tracciato del muone - " + run_name).c_str(), 100, 100, 100); // 100 bins, range 0-100 mm
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
    vector<double> errors;

    for (size_t i = 0; i < eventi_per_file.size(); i++) {
        if (eventi_per_file[i].empty()) {
            continue;
        }

        long double total_time = total_run_time(eventi_per_file[i]);
        int total_events = eventi_per_file[i].size();
        int edge_event_count = edge_events(eventi_per_file[i], cut_distance);
        double rate = (total_events - edge_event_count) / total_time;
        double error = sqrt(total_events - edge_event_count) / total_time;

        run_indices.push_back(i + 1);
        muon_rates.push_back(rate);
        errors.push_back(error);
    }

    string main_folder = "images";
    if (!fs::exists(main_folder)) {
        fs::create_directory(main_folder);
    }
    if (!fs::exists(main_folder)) {
        fs::create_directory(main_folder);
    }
    
    TCanvas *canvas = new TCanvas("canvas", "Rate dei Muoni in Funzione della Run", 800, 600);
    TGraphErrors *graph = new TGraphErrors(run_indices.size(), &run_indices[0], &muon_rates[0], nullptr, &errors[0]);
    canvas->SetGrid();

    string title = "Rate dei Muoni in Funzione della Run dopo il Taglio (" + to_string(cut_distance) + " mm);Indice della Run;Rate [Hz]";
    graph->SetTitle(title.c_str());
    graph->SetMarkerStyle(20);
    graph->SetMarkerSize(1.4);
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
    delete canvas;
    delete trackID_hist;
    delete pave;
}

vector<totalEvents> load_totalEvents_data(const string& filename) {
    vector<totalEvents> eventi;

    string run_name = get_run_name(filename);
    
    TFile *file = TFile::Open(filename.c_str(), "READ");
    if (!file) {
        cerr << "Errore: impossibile aprire il file "<< run_name << "!" << endl;
        return eventi;
    }

    // Ottiengo l'albero (TTree) dal file ROOT
    TTree *tree = (TTree*)file->Get("Events");
    if (!tree) {
        cerr << "Errore nell'ottenere l'albero dal file ROOT: " << filename << endl;
        file->Close();
        return eventi;
    }

    // Definisco le variabili per leggere i dati dall'albero
    totalEvents event;
    tree->SetBranchAddress("NPE", &event.NPE);
    tree->SetBranchAddress("fSec", &event.fSec);
    tree->SetBranchAddress("fNanoSec", &event.fNanoSec);
    tree->SetBranchAddress("Recox", &event.Recox);
    tree->SetBranchAddress("Recoy", &event.Recoy);
    tree->SetBranchAddress("Recoz", &event.Recoz);
    tree->SetBranchAddress("RunNumber", &event.RunNumber);

    // Leggo i dati dall'albero e salvali nel vettore
    Long64_t nentries = tree->GetEntries();
    for (Long64_t i = 0; i < nentries; i++) {
        tree->GetEntry(i);
        eventi.push_back(event);
    }

    // Chiudo il file ROOT
    file->Close();
    return eventi;
}

vector<vector<totalEvents>> load_multiple_totalEvents_files(const string& folder_path, vector<string>& run_names) {
    vector<vector<totalEvents>> all_eventi;
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
        vector<totalEvents> eventi = load_totalEvents_data(file_path);
        all_eventi.push_back(eventi);
    }

    cout << "Numero di file di eventi totali analizzati: " << all_eventi.size() << endl;
    return all_eventi;
}

vector<pair<size_t, size_t>> find_common_runs(const vector<string>& run_names, const vector<string>& total_run_names) {
    vector<pair<size_t, size_t>> common_run_indices;

    for (size_t i = 0; i < run_names.size(); i++) {
        for (size_t j = 0; j < total_run_names.size(); j++) {
            if (run_names[i] == total_run_names[j]) {
                common_run_indices.push_back(make_pair(i, j));
            }
        }
    }

    return common_run_indices;
}

vector<vector<muone>> create_updated_events_vector(const vector<vector<muone>>& eventi_per_file, const vector<vector<totalEvents>>& total_eventi_per_file, const vector<pair<size_t, size_t>>& common_run_indices, const vector<string>& run_names, vector<string>& run_names_mod) {
    vector<vector<muone>> updated_eventi_per_file(common_run_indices.size());
    run_names_mod.clear();

    for (size_t k = 0; k < common_run_indices.size(); ++k) {
        size_t run_index = common_run_indices[k].first;
        size_t total_run_index = common_run_indices[k].second;

        const auto& eventi = eventi_per_file[run_index];
        const auto& total_eventi = total_eventi_per_file[total_run_index];

        size_t i = 0, j = 0;

        while (i < eventi.size() && j < total_eventi.size()) {
            int ev_time_sec = eventi[i].fSec;
            int ev_time_nsec = eventi[i].fNanosec;
            int total_ev_time_sec = total_eventi[j].fSec;
            int total_ev_time_nsec = total_eventi[j].fNanoSec;

            if (ev_time_sec == total_ev_time_sec && ev_time_nsec == total_ev_time_nsec) {
                // Trovo l'ultimo trackID per lo stesso eventID
                int last_trackID = eventi[i].trackID;
                size_t temp_i = i;
                while (temp_i < eventi.size() && eventi[temp_i].eventID == eventi[i].eventID) {
                    last_trackID = eventi[temp_i].trackID;
                    temp_i++;
                }

                // Creo una nuova struct muone con la carica e il trackID aggiornati
                muone updated_event = eventi[i];
                updated_event.PeSum = total_eventi[j].NPE;
                updated_event.trackID = last_trackID;
                
                updated_eventi_per_file[k].push_back(updated_event);
                i++;
                j++;
            } else if (ev_time_sec < total_ev_time_sec || (ev_time_sec == total_ev_time_sec && ev_time_nsec < total_ev_time_nsec)) {
                i++;
            } else {
                j++;
            }
        }
        run_names_mod.push_back(run_names[run_index] + "mod");
    }
    return updated_eventi_per_file;
}

void plot_time_difference_vs_charge(const vector<totalEvents>& eventi, const string& run_name) {
    string main_folder = "images";
    string folder_name = main_folder + "/Time_Difference_vs_Charge_plot";
    if (!fs::exists(main_folder)) {
        fs::create_directory(main_folder);
    }
    if (!fs::exists(folder_name)) {
        fs::create_directory(folder_name);
    }

    TCanvas *canvas = new TCanvas(run_name.c_str(), ("Time Difference vs Charge - " + run_name).c_str(), 800, 600);
    gPad->SetRightMargin(0.12);
    canvas->SetGrid();
    gStyle->SetPalette(kRainBow);

    // Creazione dell'istogramma bidimensionale con limiti degli assi più appropriati
    //TH2F *hist2D = new TH2F(run_name.c_str(), ("Time Difference vs Charge - " + run_name).c_str(), 100, 0, 0.06, 100, 0, 3000000);
    TH2F *hist2D = new TH2F(run_name.c_str(), ("Time Difference vs Charge - " + run_name).c_str(), 100, 100, 100, 100, 100, 100);

    int count = 0;
    for (size_t i = 1; i < eventi.size(); i++) {
        double delta_t = ((double)eventi[i].fSec - (double)eventi[i-1].fSec) + ((double)eventi[i].fNanoSec - (double)eventi[i-1].fNanoSec) * 1e-9;
        double energy = eventi[i-1].NPE; // Energia del primo evento
        hist2D->Fill(delta_t, energy);
        count++;
    }

    hist2D->GetXaxis()->SetTitle("Time Difference [s]");
    hist2D->GetYaxis()->SetTitle("Charge (first event) [p.e.]");
    hist2D->Draw("COLZ");

    string filename = folder_name + "/Time_Difference_vs_Charge_" + run_name + ".png";
    canvas->SaveAs(filename.c_str());

    delete canvas;
    delete hist2D;
}

void PeSum_histogram_log(const vector<muone>& eventi, const string& run_name) {
    string main_folder = "images";
    string folder_name = main_folder + "/PeSum_log_plot";
    if (!fs::exists(main_folder)) {
        fs::create_directory(main_folder);
    }
    if (!fs::exists(folder_name)) {
        fs::create_directory(folder_name);
    }
    
    TCanvas *canvas = new TCanvas(("canvas_charge_" + run_name).c_str(), ("Istogramma Energia - " + run_name).c_str(), 800, 600);
    TH1F *charge = new TH1F(run_name.c_str(), ("Distribuzione dell'energia - " + run_name).c_str(), 100, 0, 7); // Binning logaritmico da 10^0 a 10^7
    // Applico il binning logaritmico sugli assi X e Y
    BinLogX(charge);
    BinLogY(charge);
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
    
    // Imposto la scala logaritmica sugli assi X e Y
    canvas->SetLogx();
    canvas->SetLogy();

    string filename = folder_name + "/PeSum_log_" + run_name + ".png";
    canvas->SaveAs(filename.c_str());
    delete canvas;
    delete charge;
}

void BinLogX(TH1* h) {
    TAxis *axis = h->GetXaxis();
    int bins = axis->GetNbins();

    Axis_t from = axis->GetXmin();
    Axis_t to = axis->GetXmax();
    Axis_t width = (to - from) / bins;
    Axis_t *new_bins = new Axis_t[bins + 1];

    for (int i = 0; i <= bins; i++) {
        new_bins[i] = TMath::Power(10, from + i * width);
    }
    axis->Set(bins, new_bins);
    delete new_bins;
}

void BinLogY(TH1* h) {
    TAxis *axis = h->GetYaxis();
    int bins = axis->GetNbins();

    Axis_t from = axis->GetXmin();
    Axis_t to = axis->GetXmax();
    Axis_t width = (to - from) / bins;
    Axis_t *new_bins = new Axis_t[bins + 1];

    for (int i = 0; i <= bins; i++) {
        new_bins[i] = TMath::Power(10, from + i * width);
    }
    axis->Set(bins, new_bins);
    delete new_bins;
}

void total_PeSum_histogram_log(const vector<totalEvents>& eventi, const string& run_name) {
    string main_folder = "images";
    string folder_name = main_folder + "/Total_PeSum_log_plot";
    if (!fs::exists(main_folder)) {
        fs::create_directory(main_folder);
    }
    if (!fs::exists(folder_name)) {
        fs::create_directory(folder_name);
    }
    
    TCanvas *canvas = new TCanvas(("canvas_charge_" + run_name).c_str(), ("Istogramma Energia - " + run_name).c_str(), 800, 600);
    TH1F *charge = new TH1F(run_name.c_str(), ("Distribuzione dell'energia - " + run_name).c_str(), 100, 0, 7); // Binning logaritmico da 10^0 a 10^7
    // Applico il binning logaritmico sugli assi X e Y
    BinLogX(charge);
    BinLogY(charge);
    gPad->SetLeftMargin(0.12);

    charge->StatOverflows(kTRUE);
    canvas->SetGrid();

    for (const auto& ev : eventi) {
        charge->Fill(ev.NPE);
    }

    charge->GetXaxis()->SetTitle("Charge [p.e.]");
    charge->GetYaxis()->SetTitle("Counts [a.u.]");
    charge->SetLineWidth(2);
    charge->SetFillColorAlpha(kBlue, 1);
    charge->Draw();
    
    // Imposto la scala logaritmica sugli assi X e Y
    canvas->SetLogx();
    canvas->SetLogy();

    string filename = folder_name + "/total_PeSum_log_" + run_name + ".png";
    canvas->SaveAs(filename.c_str());
    delete canvas;
    delete charge;
}

void total_PeSum_histogram_log(const vector<totalEvents>& eventi1, const vector<muone>& eventi2, const string& run_name1, const string& run_name2) {
    string main_folder = "images";
    string folder_name = main_folder + "/Total_PeSum_overlap_log_plot";
    if (!fs::exists(main_folder)) {
        fs::create_directory(main_folder);
    }
    if (!fs::exists(folder_name)) {
        fs::create_directory(folder_name);
    }
    
    TCanvas *canvas = new TCanvas(("canvas_charge_" + run_name1 + "_vs_" + run_name2).c_str(), ("Istogramma Energia - " + run_name1 + " vs " + run_name2).c_str(), 800, 600);
    TH1F *charge1 = new TH1F(("Distribuzione_dell_energia_" + run_name1).c_str(), ("Distribuzione dell'energia - " + run_name1).c_str(), 100, 0, 7); // Binning logaritmico da 10^0 a 10^7
    TH1F *charge2 = new TH1F(("Distribuzione_dell_energia_" + run_name2).c_str(), ("Distribuzione dell'energia - " + run_name2).c_str(), 100, 0, 7); // Binning logaritmico da 10^0 a 10^7
    
    // Applico il binning logaritmico sugli assi X e Y
    BinLogX(charge1);
    BinLogX(charge2);
    BinLogY(charge1);
    BinLogY(charge2);
    gPad->SetLeftMargin(0.12);

    charge1->StatOverflows(kTRUE);
    charge2->StatOverflows(kTRUE);
    canvas->SetGrid();

    for (const auto& ev : eventi1) {
        charge1->Fill(ev.NPE);
    }

    for (const auto& ev : eventi2) {
        charge2->Fill(ev.PeSum);
    }

    charge1->GetXaxis()->SetTitle("Charge [p.e.]");
    charge1->GetYaxis()->SetTitle("Counts [a.u.]");
    charge1->SetLineWidth(2);
    charge1->SetLineColor(kBlue);
    charge1->SetFillColorAlpha(kBlue, 0.5);
    charge1->SetStats(kFALSE);
    charge1->Draw();

    charge2->SetLineWidth(2);
    charge2->SetLineColor(kRed);
    charge2->SetFillColorAlpha(kRed, 0.5);
    charge2->SetStats(kFALSE);
    charge2->Draw("SAME");

    // Imposto la scala logaritmica sugli assi X e Y
    canvas->SetLogx();
    canvas->SetLogy();

    TLegend *legend = new TLegend(0.7, 0.7, 0.9, 0.9);
    legend->AddEntry(charge1, run_name1.c_str(), "f");
    string run_name2_legend = run_name1 + " muonclassify";
    legend->AddEntry(charge2, run_name2_legend.c_str(), "f");
    legend->Draw();

    string filename = folder_name + "/total_PeSum_log_" + run_name1 + "_vs_" + run_name2 + ".png";
    canvas->SaveAs(filename.c_str());
    delete canvas;
    delete charge1;
    delete charge2;
    delete legend;
}

void total_PeSum_histogram_log_complementary(const vector<totalEvents>& eventi1, const vector<muone>& eventi2, const string& run_name1, const string& run_name2) {
    string main_folder = "images";
    string folder_name = main_folder + "/Total_PeSum_complementary_log_plot";
    if (!fs::exists(main_folder)) {
        fs::create_directory(main_folder);
    }
    if (!fs::exists(folder_name)) {
        fs::create_directory(folder_name);
    }
    
    TCanvas *canvas = new TCanvas(("canvas_charge_complementary_" + run_name1 + "_vs_" + run_name2).c_str(), ("Istogramma Complementare Energia - " + run_name1 + " vs " + run_name2).c_str(), 800, 600);
    TH1F *charge1 = new TH1F(run_name1.c_str(), ("Distribuzione della carica 'complementare' - " + run_name1).c_str(), 100, 0, 7); // Binning logaritmico da 10^0 a 10^7
    
    // Applico il binning logaritmico sugli assi X e Y
    BinLogX(charge1);
    BinLogY(charge1);
    gPad->SetLeftMargin(0.12);

    charge1->StatOverflows(kTRUE);
    canvas->SetGrid();

    // Creo un set di tempi (secondi e nanosecondi) per gli eventi di eventi2
    set<pair<int, int>> eventi2_times;
    for (const auto& ev : eventi2) {
        eventi2_times.insert(make_pair(ev.fSec, ev.fNanosec));
    }

    // Riempio l'istogramma con gli eventi di eventi1 che non sono in eventi2
    for (const auto& ev : eventi1) {
        if (eventi2_times.find(make_pair(ev.fSec, ev.fNanoSec)) == eventi2_times.end()) {
            charge1->Fill(ev.NPE);
        }
    }

    charge1->GetXaxis()->SetTitle("Charge [p.e.]");
    charge1->GetYaxis()->SetTitle("Counts [a.u.]");
    charge1->SetLineWidth(2);
    charge1->SetLineColor(kGreen);
    charge1->SetFillColorAlpha(kGreen, 1);
    charge1->Draw();

    // Imposto la scala logaritmica sugli assi X e Y
    canvas->SetLogx();
    canvas->SetLogy();

    string filename = folder_name + "/total_PeSum_complementary_log_" + run_name1 + "_vs_" + run_name2 + ".png";
    canvas->SaveAs(filename.c_str());
    delete canvas;
    delete charge1;
}

void count_high_energy_events(const vector<totalEvents>& eventi1, const vector<muone>& eventi2, const string& run_name1, const string& run_name2) {
    const double energy_threshold = 1.5e4; // Soglia di energia

    int count_eventi1 = 0;
    int count_eventi2 = 0;
    int count_bundle = 0;
    int count_trackID_0 = 0;
    int count_trackID_1 = 0;
    int count_trackID_2 = 0;
    int count_trackID_3 = 0;
    int count_trackID_4 = 0;
    int count_trackID_gt4 = 0;

    // Conto gli eventi di eventi1 con energia superiore alla soglia
    for (const auto& ev : eventi1) {
        if (ev.NPE > energy_threshold) {
            count_eventi1++;
        }
    }

    // Conto gli eventi di eventi2 con energia superiore alla soglia e divisi per trackID
    for (const auto& ev : eventi2) {
        if (ev.PeSum > energy_threshold) {
            count_eventi2++;
            if (ev.trackID == 0) {
                count_trackID_0++;
            } else if (ev.trackID == 1) {
                count_trackID_1++;
            } else if (ev.trackID == 2) {
                count_trackID_2++;
            } else if (ev.trackID == 3) {
                count_trackID_3++;
            } else if (ev.trackID == 4) {
                count_trackID_4++;
            } else if (ev.trackID > 4) {
                count_trackID_gt4++;
            }
        }
    }

    double perc_trackID_0 = (double)count_trackID_0 / count_eventi1 * 100;
    double perc_trackID_1 = (double)count_trackID_1 / count_eventi1 * 100;
    double perc_trackID_2 = (double)count_trackID_2 / count_eventi1 * 100;
    double perc_trackID_3 = (double)count_trackID_3 / count_eventi1 * 100;
    double perc_trackID_4 = (double)count_trackID_4 / count_eventi1 * 100;
    double perc_trackID_gt4 = (double)count_trackID_gt4 / count_eventi1 * 100;
    double perc_bundle = (double)count_bundle / count_eventi1 * 100;

    // Stampa i risultati
    cout << "Numero di eventi in " << run_name1 << " con energia superiore a " << energy_threshold << " p.e.: " << count_eventi1 << endl;
    cout << "Numero di eventi in " << run_name2 << " con energia superiore a " << energy_threshold << " p.e.: " << count_eventi2 << endl;
    cout << "Numero di eventi con trackID = 0 in " << run_name2 << " con energia superiore a " << energy_threshold << " p.e.: " << count_trackID_0 << " (" << perc_trackID_0 << "%)" << endl;
    cout << "Numero di eventi con trackID = 1 in " << run_name2 << " con energia superiore a " << energy_threshold << " p.e.: " << count_trackID_1 << " (" << perc_trackID_1 << "%)" << endl;
    cout << "Numero di eventi con trackID = 2 in " << run_name2 << " con energia superiore a " << energy_threshold << " p.e.: " << count_trackID_2 << " (" << perc_trackID_2 << "%)" << endl;
    cout << "Numero di eventi con trackID = 3 in " << run_name2 << " con energia superiore a " << energy_threshold << " p.e.: " << count_trackID_3 << " (" << perc_trackID_3 << "%)" << endl;
    cout << "Numero di eventi con trackID = 4 in " << run_name2 << " con energia superiore a " << energy_threshold << " p.e.: " << count_trackID_4 << " (" << perc_trackID_4 << "%)" << endl;
    cout << "Numero di eventi con trackID > 4 in " << run_name2 << " con energia superiore a " << energy_threshold << " p.e.: " << count_trackID_gt4 << " (" << perc_trackID_gt4 << "%)" << endl;
}

void total_PeSum_histogram_log_divided(const vector<totalEvents>& eventi1, const vector<muone>& eventi2, const string& run_name1, const string& run_name2) {
    string main_folder = "images";
    string folder_name = main_folder + "/Total_PeSum_overlap_log_divided_plot";
    if (!fs::exists(main_folder)) {
        fs::create_directory(main_folder);
    }
    if (!fs::exists(folder_name)) {
        fs::create_directory(folder_name);
    }
    
    TCanvas *canvas = new TCanvas(("canvas_charge_" + run_name1 + "_vs_" + run_name2).c_str(), ("Istogramma Carica - " + run_name1 + " vs " + run_name2).c_str(), 800, 600);
    TH1F *charge1_total = new TH1F(("Distribuzione_dell_energia_totale_" + run_name1).c_str(), ("Distribuzione dell'energia totale - " + run_name1).c_str(), 100, 0, 7);
    TH1F *charge2_single = new TH1F(("Distribuzione_dell_energia_singoli_" + run_name2).c_str(), ("Distribuzione dell'energia singoli - " + run_name2).c_str(), 100, 0, 7);
    TH1F *charge2_bundle = new TH1F(("Distribuzione_dell_energia_bundle_" + run_name2).c_str(), ("Distribuzione dell'energia bundle - " + run_name2).c_str(), 100, 0, 7);
    
    // Applico il binning logaritmico sugli assi X e Y
    BinLogX(charge1_total);
    BinLogX(charge2_single);
    BinLogX(charge2_bundle);
    BinLogY(charge1_total);
    BinLogY(charge2_single);
    BinLogY(charge2_bundle);
    gPad->SetLeftMargin(0.12);

    charge1_total->StatOverflows(kTRUE);
    charge2_single->StatOverflows(kTRUE);
    charge2_bundle->StatOverflows(kTRUE);
    canvas->SetGrid();

    // Riempio l'istogramma con gli eventi totali
    for (const auto& ev : eventi1) {
        charge1_total->Fill(ev.NPE);
    }

    // Riempio gli istogrammi con gli eventi singoli e bundle
    for (const auto& ev : eventi2) {
        if (ev.trackID == 0) {
            charge2_single->Fill(ev.PeSum);
        } else {
            charge2_bundle->Fill(ev.PeSum);
        }
    }

    // Disegno l'istogramma degli eventi totali
    charge1_total->GetXaxis()->SetTitle("Charge [p.e.]");
    charge1_total->GetYaxis()->SetTitle("Counts [a.u.]");
    charge1_total->SetLineWidth(2);
    charge1_total->SetLineColor(kBlue);
    charge1_total->SetFillColorAlpha(kBlue, 0.5);
    charge1_total->SetStats(kFALSE);
    charge1_total->Draw();

    // Disegno l'istogramma degli eventi singoli
    charge2_single->SetLineWidth(2);
    charge2_single->SetLineColor(kRed);
    charge2_single->SetFillColorAlpha(kRed, 0.5);
    charge2_single->SetStats(kFALSE);
    charge2_single->Draw("SAME");

    // Disegno l'istogramma degli eventi bundle
    charge2_bundle->SetLineWidth(2);
    charge2_bundle->SetLineColor(kGreen);
    charge2_bundle->SetFillColorAlpha(kGreen, 0.5);
    charge2_bundle->SetStats(kFALSE);
    charge2_bundle->Draw("SAME");

    // Imposto la scala logaritmica sugli assi X e Y
    canvas->SetLogx();
    canvas->SetLogy();

    TLegend *legend = new TLegend(0.7, 0.7, 0.9, 0.9);
    legend->AddEntry(charge1_total, (run_name1 + " Totale").c_str(), "f");
    legend->AddEntry(charge2_single, (run_name2 + " Singoli").c_str(), "f");
    legend->AddEntry(charge2_bundle, (run_name2 + " Bundle").c_str(), "f");
    legend->Draw();

    string filename = folder_name + "/total_PeSum_log_divided_" + run_name1 + "_vs_" + run_name2 + ".png";
    canvas->SaveAs(filename.c_str());
    delete canvas;
    delete charge1_total;
    delete charge2_single;
    delete charge2_bundle;
    delete legend;
}

void total_PeSum_histogram_log_divided_track(const vector<totalEvents>& eventi1, const vector<muone>& eventi2, const string& run_name1, const string& run_name2) {
    string main_folder = "images";
    string folder_name = main_folder + "/Total_PeSum_overlap_log_divided_track_plot";
    if (!fs::exists(main_folder)) {
        fs::create_directory(main_folder);
    }
    if (!fs::exists(folder_name)) {
        fs::create_directory(folder_name);
    }
    
    TCanvas *canvas = new TCanvas(("canvas_charge_" + run_name1 + "_vs_" + run_name2).c_str(), ("Istogramma Carica - " + run_name1 + " vs " + run_name2).c_str(), 800, 600);
    TH1F *charge1_total = new TH1F(("Distribuzione_dell_energia_totale_" + run_name1).c_str(), ("Distribuzione dell'energia totale - " + run_name1).c_str(), 100, 0, 7);
    TH1F *charge2_single = new TH1F(("Distribuzione_dell_energia_singoli_" + run_name2).c_str(), ("Distribuzione dell'energia singoli - " + run_name2).c_str(), 100, 0, 7);
    TH1F *charge2_track1 = new TH1F(("Distribuzione_dell_energia_track1_" + run_name2).c_str(), ("Distribuzione dell'energia trackID = 1 - " + run_name2).c_str(), 100, 0, 7);
    TH1F *charge2_track2 = new TH1F(("Distribuzione_dell_energia_track2_" + run_name2).c_str(), ("Distribuzione dell'energia trackID = 2 - " + run_name2).c_str(), 100, 0, 7);
    TH1F *charge2_track3 = new TH1F(("Distribuzione_dell_energia_track3_" + run_name2).c_str(), ("Distribuzione dell'energia trackID = 3 - " + run_name2).c_str(), 100, 0, 7);
    TH1F *charge2_track4 = new TH1F(("Distribuzione_dell_energia_track4_" + run_name2).c_str(), ("Distribuzione dell'energia trackID = 4 - " + run_name2).c_str(), 100, 0, 7);
    TH1F *charge2_track_gt4 = new TH1F(("Distribuzione_dell_energia_track_gt4_" + run_name2).c_str(), ("Distribuzione dell'energia trackID gt 4 - " + run_name2).c_str(), 100, 0, 7); 
    
    // Applico il binning logaritmico sugli assi X e Y
    BinLogX(charge1_total);
    BinLogX(charge2_single);
    BinLogX(charge2_track1);
    BinLogX(charge2_track2);
    BinLogX(charge2_track3);
    BinLogX(charge2_track4);
    BinLogX(charge2_track_gt4);
    BinLogY(charge1_total);
    BinLogY(charge2_single);
    BinLogY(charge2_track1);
    BinLogY(charge2_track2);
    BinLogY(charge2_track3);
    BinLogY(charge2_track4);
    BinLogY(charge2_track_gt4);
    gPad->SetLeftMargin(0.12);

    charge1_total->StatOverflows(kTRUE);
    charge2_single->StatOverflows(kTRUE);
    charge2_track1->StatOverflows(kTRUE);
    charge2_track2->StatOverflows(kTRUE);
    charge2_track3->StatOverflows(kTRUE);
    charge2_track4->StatOverflows(kTRUE);
    charge2_track_gt4->StatOverflows(kTRUE);
    canvas->SetGrid();

    // Riempie l'istogramma con gli eventi totali
    for (const auto& ev : eventi1) {
        charge1_total->Fill(ev.NPE);
    }

    // Riempie gli istogrammi con gli eventi singoli e bundle
    for (const auto& ev : eventi2) {
        if (ev.trackID == 0) {
            charge2_single->Fill(ev.PeSum);
        } else if (ev.trackID == 1) {
            charge2_track1->Fill(ev.PeSum);
        } else if (ev.trackID == 2) {
            charge2_track2->Fill(ev.PeSum);
        } else if (ev.trackID == 3) {
            charge2_track3->Fill(ev.PeSum);
        } else if (ev.trackID == 4) {
            charge2_track4->Fill(ev.PeSum);
        } else {
            charge2_track_gt4->Fill(ev.PeSum);
        }
    }

    // Disegno l'istogramma degli eventi totali
    charge1_total->GetXaxis()->SetTitle("Charge [p.e.]");
    charge1_total->GetYaxis()->SetTitle("Counts [a.u.]");
    charge1_total->SetLineWidth(2);
    charge1_total->SetLineColor(kBlue);
    charge1_total->SetFillColorAlpha(kBlue, 0.5);
    charge1_total->SetStats(kFALSE); // Disabilito la casella delle statistiche
    charge1_total->Draw();

    // Disegno l'istogramma degli eventi singoli
    charge2_single->SetLineWidth(2);
    charge2_single->SetLineColor(kRed);
    charge2_single->SetFillColorAlpha(kRed, 0.5);
    charge2_single->SetStats(kFALSE); // Disabilito la casella delle statistiche
    charge2_single->Draw("SAME");

    // Disegno l'istogramma degli eventi trackID = 1
    charge2_track1->SetLineWidth(2);
    charge2_track1->SetLineColor(kGreen);
    charge2_track1->SetFillColorAlpha(kGreen, 0.5);
    charge2_track1->SetStats(kFALSE); // Disabilito la casella delle statistiche
    charge2_track1->Draw("SAME");

    // Disegno l'istogramma degli eventi trackID = 2
    charge2_track2->SetLineWidth(2);
    charge2_track2->SetLineColor(kMagenta);
    charge2_track2->SetFillColorAlpha(kMagenta, 0.5);
    charge2_track2->SetStats(kFALSE); // Disabilito la casella delle statistiche
    charge2_track2->Draw("SAME");

    // Disegno l'istogramma degli eventi trackID = 3
    charge2_track3->SetLineWidth(2);
    charge2_track3->SetLineColor(kCyan);
    charge2_track3->SetFillColorAlpha(kCyan, 0.5);
    charge2_track3->SetStats(kFALSE); // Disabilito la casella delle statistiche
    charge2_track3->Draw("SAME");

    // Disegno l'istogramma degli eventi trackID = 4
    charge2_track4->SetLineWidth(2);
    charge2_track4->SetLineColor(kOrange);
    charge2_track4->SetFillColorAlpha(kOrange, 0.5);
    charge2_track4->SetStats(kFALSE); // Disabilito la casella delle statistiche
    charge2_track4->Draw("SAME");

    // Disegno l'istogramma degli eventi trackID > 4
    charge2_track_gt4->SetLineWidth(2);
    charge2_track_gt4->SetLineColor(kOrange+10);
    charge2_track_gt4->SetFillColorAlpha(kOrange+10, 0.5);
    charge2_track_gt4->SetStats(kFALSE); // Disabilito la casella delle statistiche
    charge2_track_gt4->Draw("SAME");

    // Imposto la scala logaritmica sugli assi X e Y
    canvas->SetLogx();
    canvas->SetLogy();

    TLegend *legend = new TLegend(0.7, 0.7, 0.9, 0.9);
    legend->AddEntry(charge1_total, (run_name1 + " Totale").c_str(), "f");
    legend->AddEntry(charge2_single, (run_name2 + " Singoli").c_str(), "f");
    legend->AddEntry(charge2_track1, (run_name2 + " trackID = 1").c_str(), "f");
    legend->AddEntry(charge2_track2, (run_name2 + " trackID = 2").c_str(), "f");
    legend->AddEntry(charge2_track3, (run_name2 + " trackID = 3").c_str(), "f");
    legend->AddEntry(charge2_track4, (run_name2 + " trackID = 4").c_str(), "f");
    legend->AddEntry(charge2_track_gt4, (run_name2 + " trackID > 4").c_str(), "f");
    legend->Draw();

    string filename = folder_name + "/total_PeSum_log_divided_" + run_name1 + "_vs_" + run_name2 + ".png";
    canvas->SaveAs(filename.c_str());
    delete canvas;
    delete charge1_total;
    delete charge2_single;
    delete charge2_track1;
    delete charge2_track2;
    delete charge2_track3;
    delete charge2_track4;
    delete charge2_track_gt4;
    delete legend;
}

// Restituisce il numero di eventi in comune (entro una certa tolleranza in nanoseconti) tra WP (eventi1) e CD (eventi 2)
int count_common_events(const vector<totalEvents>& eventi1, const vector<totalEvents>& eventi2, double tolerance_ns) {
    int common_event_count = 0;
    size_t i = 0, j = 0;

    while (i < eventi1.size() && j < eventi2.size()) {
        long double ev_time1 = (long double) eventi1[i].fSec * 1e9 + (long double) eventi1[i].fNanoSec;
        long double ev_time2 = (long double) eventi2[j].fSec * 1e9 + (long double) eventi2[j].fNanoSec;

        if (abs(ev_time1 - ev_time2) <= tolerance_ns) {
            common_event_count++;
            i++;
            j++;
        } else if (ev_time1 < ev_time2) {
            i++;
        } else {
            j++;
        }
    }
    return common_event_count;
}

void plot_common_events_NPE(const vector<totalEvents>& eventi1, const vector<totalEvents>& eventi2, double tolerance_ns, const string& run_name1, const string& run_name2) {
    vector<double> common_NPE1;
    vector<double> common_NPE2;

    size_t i = 0, j = 0;
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << tolerance_ns;
    while (i < eventi1.size() && j < eventi2.size()) {
        long double ev_time1 = (long double) eventi1[i].fSec * 1e9 + (long double) eventi1[i].fNanoSec;
        long double ev_time2 = (long double) eventi2[j].fSec * 1e9 + (long double) eventi2[j].fNanoSec;

        if (abs(ev_time1 - ev_time2) <= tolerance_ns) {
            common_NPE1.push_back(eventi1[i].NPE);
            common_NPE2.push_back(eventi2[j].NPE);
            i++;
            j++;
        } else if (ev_time1 < ev_time2) {
            i++;
        } else {
            j++;
        }
    }

    string main_folder = "images";
    string folder_name = main_folder + "/Time_CD_WP_Events_NPE_plot";
    string wp_folder = folder_name + "/WPplot";
    string cd_folder = folder_name + "/CDplot";
    if (!fs::exists(main_folder)) {
        fs::create_directory(main_folder);
    }
    if (!fs::exists(folder_name)) {
        fs::create_directory(folder_name);
    }
    if (!fs::exists(wp_folder)) {
        fs::create_directory(wp_folder);
    }
    if (!fs::exists(cd_folder)) {
        fs::create_directory(cd_folder);
    }

    // Istogramma per l'energia del WP
    TCanvas *canvas1 = new TCanvas(run_name1.c_str(), "Istogramma Carica WP", 800, 600);
    TH1F *hist1 = new TH1F(run_name1.c_str(), ("Istogramma Carica WP (Tolleranza: " + oss.str() + " ns)").c_str(), 100, 0, 7);
    canvas1->SetGrid();
    BinLogX(hist1);
    BinLogY(hist1);

    for (const auto& npe : common_NPE1) {
        hist1->Fill(npe);
    }
    // Imposto la scala logaritmica sugli assi X e Y
    canvas1->SetLogx();
    canvas1->SetLogy();

    hist1->GetXaxis()->SetTitle("Charge [p.e.]");
    hist1->GetYaxis()->SetTitle("Counts [a.u.]");
    hist1->SetLineWidth(2);
    hist1->SetLineColor(kBlue);
    hist1->SetFillColorAlpha(kBlue, 1);
    hist1->Draw();

    string filename1 = wp_folder + "/WP_" + run_name1 + "_tolerance_" + oss.str() + ".png";
    canvas1->SaveAs(filename1.c_str());

    delete canvas1;
    delete hist1;

    // Istogramma per l'energia del CD
    TCanvas *canvas2 = new TCanvas(run_name1.c_str(), "Istogramma Carica CD", 800, 600);
    TH1F *hist2 = new TH1F(run_name1.c_str(), ("Istogramma Carica CD (Tolleranza: " + oss.str() + " ns)").c_str(), 100, 0, 8);
    canvas2->SetGrid();
    BinLogX(hist2);
    BinLogY(hist2);

    for (const auto& npe : common_NPE2) {
        hist2->Fill(npe);
    }
    
    // Imposto la scala logaritmica sugli assi X e Y
    canvas2->SetLogx();
    canvas2->SetLogy();

    hist2->GetXaxis()->SetTitle("Charge [p.e.]");
    hist2->GetYaxis()->SetTitle("Counts [a.u.]");
    hist2->SetLineWidth(2);
    hist2->SetLineColor(kRed);
    hist2->SetFillColorAlpha(kRed, 1);
    hist2->Draw();

    string filename2 = cd_folder + "/CD_" + run_name1 + "_tolerance_" + oss.str() + ".png";
    canvas2->SaveAs(filename2.c_str());

    delete canvas2;
    delete hist2;
}

void plot_common_events_NPE_all(const vector<totalEvents>& eventi1, const vector<totalEvents>& eventi2, double tolerance_ns, const string& run_name1, const string& run_name2) {
    vector<double> common_NPE1;
    vector<double> common_NPE2;
    vector<double> all_NPE1;
    vector<double> all_NPE2;

    size_t i = 0, j = 0;
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << tolerance_ns;

    // Riempio i vettori con tutti gli eventi
    for (const auto& ev : eventi1) {
        all_NPE1.push_back(ev.NPE);
    }
    for (const auto& ev : eventi2) {
        all_NPE2.push_back(ev.NPE);
    }

    // Riempio i vettori con gli eventi comuni entro la tolleranza
    while (i < eventi1.size() && j < eventi2.size()) {
        long double ev_time1 = (long double) eventi1[i].fSec * 1e9 + (long double) eventi1[i].fNanoSec;
        long double ev_time2 = (long double) eventi2[j].fSec * 1e9 + (long double) eventi2[j].fNanoSec;

        if (abs(ev_time1 - ev_time2) <= tolerance_ns) {
            common_NPE1.push_back(eventi1[i].NPE);
            common_NPE2.push_back(eventi2[j].NPE);
            i++;
            j++;
        } else if (ev_time1 < ev_time2) {
            i++;
        } else {
            j++;
        }
    }

    string main_folder = "images";
    string folder_name = main_folder + "/Time_CD_WP_Events_comparison_NPE_plot";
    string wp_folder = folder_name + "/WPplot";
    string cd_folder = folder_name + "/CDplot";
    if (!fs::exists(main_folder)) {
        fs::create_directory(main_folder);
    }
    if (!fs::exists(folder_name)) {
        fs::create_directory(folder_name);
    }
    if (!fs::exists(wp_folder)) {
        fs::create_directory(wp_folder);
    }
    if (!fs::exists(cd_folder)) {
        fs::create_directory(cd_folder);
    }

    // Istogramma per l'energia del CD
    TCanvas *canvas1 = new TCanvas(run_name1.c_str(), "Istogramma Carica WP", 800, 600);
    TH1F *hist1 = new TH1F(run_name1.c_str(), ("Istogramma Carica WP (Tolleranza: " + oss.str() + " ns)").c_str(), 100, 0, 7);
    TH1F *hist_all1 = new TH1F((run_name1 + "_all").c_str(), ("Istogramma Carica WP (tutti gli eventi vs eventi accoppiati) Tolleranza: " + oss.str() + " ns").c_str(), 100, 0, 7);
    canvas1->SetGrid();
    BinLogX(hist1);
    BinLogY(hist1);
    BinLogX(hist_all1);
    BinLogY(hist_all1);

    for (const auto& npe : all_NPE1) {
        hist_all1->Fill(npe);
    }
    
    for (const auto& npe : common_NPE1) {
        hist1->Fill(npe);
    }

    // Imposto la scala logaritmica sugli assi X e Y
    canvas1->SetLogx();
    canvas1->SetLogy();

    hist_all1->SetLineWidth(2);
    hist_all1->SetLineColor(kBlue);
    hist_all1->SetFillColorAlpha(kBlue, 0.5);
    hist_all1->SetStats(kFALSE);
    hist_all1->Draw();

    hist1->GetXaxis()->SetTitle("Charge [p.e.]");
    hist1->GetYaxis()->SetTitle("Counts [a.u.]");
    hist1->SetLineWidth(2);
    hist1->SetStats(kFALSE);
    hist1->SetLineColor(kRed);
    hist1->SetFillColorAlpha(kRed, 0.5);
    hist1->Draw("SAME");

    TLegend *legend1 = new TLegend(0.7, 0.7, 0.9, 0.9);
    legend1->AddEntry(hist_all1, "Eventi totali", "f");
    legend1->AddEntry(hist1, "Eventi accoppiati", "f");
    legend1->Draw();

    string filename1 = wp_folder + "/WP_" + run_name1 + "_tolerance_" + oss.str() + ".png";
    canvas1->SaveAs(filename1.c_str());

    delete canvas1;
    delete hist1;
    delete hist_all1;
    delete legend1;

    // Istogramma per l'energia del WP
    TCanvas *canvas2 = new TCanvas(run_name1.c_str(), "Istogramma Carica CD", 800, 600);
    TH1F *hist2 = new TH1F(run_name1.c_str(), ("Istogramma Carica CD (Tolleranza: " + oss.str() + " ns)").c_str(), 100, 0, 8);
    TH1F *hist_all2 = new TH1F((run_name1 + "_all").c_str(), ("Istogramma Carica CD (tutti gli eventi vs eventi accoppiati) Tolleranza: " + oss.str() + " ns").c_str(), 100, 0, 8);
    canvas2->SetGrid();
    BinLogX(hist2);
    BinLogY(hist2);
    BinLogX(hist_all2);
    BinLogY(hist_all2);

    for (const auto& npe : all_NPE2) {
        hist_all2->Fill(npe);
    }
    
    for (const auto& npe : common_NPE2) {
        hist2->Fill(npe);
    }

    // Imposto la scala logaritmica sugli assi X e Y
    canvas2->SetLogx();
    canvas2->SetLogy();

    hist_all2->SetLineWidth(2);
    hist_all2->SetStats(kFALSE);
    hist_all2->SetLineColor(kBlue);
    hist_all2->SetFillColorAlpha(kBlue, 0.5);
    hist_all2->Draw();

    hist2->GetXaxis()->SetTitle("Charge [p.e.]");
    hist2->GetYaxis()->SetTitle("Counts [a.u.]");
    hist2->SetLineWidth(2);
    hist2->SetStats(kFALSE);
    hist2->SetLineColor(kRed);
    hist2->SetFillColorAlpha(kRed, 0.5);
    hist2->Draw("SAME");

    TLegend *legend2 = new TLegend(0.7, 0.7, 0.9, 0.9);
    legend2->AddEntry(hist_all2, "Eventi totali", "f");
    legend2->AddEntry(hist2, "Eventi accoppiati", "f");
    legend2->Draw();

    string filename2 = cd_folder + "/CD_" + run_name1 + "_tolerance_" + oss.str() + ".png";
    canvas2->SaveAs(filename2.c_str());

    delete canvas2;
    delete hist2;
    delete hist_all2;
    delete legend2;
}

void plot_common_events_NPE_muon(const vector<totalEvents>& eventi1, const vector<totalEvents>& eventi2, const vector<muone>& eventi_muone, double tolerance_ns, const string& run_name1, const string& run_name2, double& overlap_area) {
    vector<double> common_NPE1;
    vector<double> common_NPE2;
    vector<double> all_NPE1;
    vector<double> all_NPE2;
    vector<double> all_NPE_muone;

    size_t i = 0, j = 0;
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << tolerance_ns;

    // Riempio i vettori con tutti gli eventi
    for (const auto& ev : eventi1) {
        all_NPE1.push_back(ev.NPE);
    }
    for (const auto& ev : eventi2) {
        all_NPE2.push_back(ev.NPE);
    }
    for (const auto& ev : eventi_muone) {
        all_NPE_muone.push_back(ev.PeSum);
    }

    // Riempio i vettori con gli eventi comuni entro la tolleranza
    while (i < eventi1.size() && j < eventi2.size()) {
        long double ev_time1 = (long double) eventi1[i].fSec * 1e9 + (long double) eventi1[i].fNanoSec;
        long double ev_time2 = (long double) eventi2[j].fSec * 1e9 + (long double) eventi2[j].fNanoSec;

        if (abs(ev_time1 - ev_time2) <= tolerance_ns) {
            common_NPE1.push_back(eventi1[i].NPE);
            common_NPE2.push_back(eventi2[j].NPE);
            i++;
            j++;
        } else if (ev_time1 < ev_time2) {
            i++;
        } else {
            j++;
        }
    }

    string main_folder = "images";
    string folder_name = main_folder + "/Time_CD_WP_Muon_Events_comparison_NPE_plot";
    string wp_folder = folder_name + "/WPplot";
    string cd_folder = folder_name + "/CDplot";
    if (!fs::exists(main_folder)) {
        fs::create_directory(main_folder);
    }
    if (!fs::exists(folder_name)) {
        fs::create_directory(folder_name);
    }
    if (!fs::exists(wp_folder)) {
        fs::create_directory(wp_folder);
    }
    if (!fs::exists(cd_folder)) {
        fs::create_directory(cd_folder);
    }

    // Istogramma per l'energia del WP
    TCanvas *canvas1 = new TCanvas(run_name1.c_str(), "Istogramma Carica WP", 800, 600);
    TH1F *hist1 = new TH1F(run_name1.c_str(), ("Istogramma Carica WP (Tolleranza: " + oss.str() + " ns)").c_str(), 100, 0, 7);
    TH1F *hist_all1 = new TH1F((run_name1 + "_all").c_str(), ("Istogramma Carica WP (tutti gli eventi vs eventi accoppiati) Tolleranza: " + oss.str() + " ns").c_str(), 100, 0, 7);
    TH1F *hist_muone1 = new TH1F((run_name1 + "_muone").c_str(), "Istogramma Carica WP (muone)", 100, 0, 7);
    canvas1->SetGrid();
    BinLogX(hist1);
    BinLogY(hist1);
    BinLogX(hist_all1);
    BinLogY(hist_all1);
    BinLogX(hist_muone1);
    BinLogY(hist_muone1);

    for (const auto& npe : all_NPE1) {
        hist_all1->Fill(npe);
    }
    
    for (const auto& npe : common_NPE1) {
        hist1->Fill(npe);
    }

    for (const auto& npe : all_NPE_muone) {
        hist_muone1->Fill(npe);
    }

    // Imposto la scala logaritmica sugli assi X e Y
    canvas1->SetLogx();
    canvas1->SetLogy();

    hist_all1->GetXaxis()->SetTitle("Charge [p.e.]");
    hist_all1->GetYaxis()->SetTitle("Counts [a.u.]");
    hist_all1->SetLineWidth(2);
    hist_all1->SetLineColor(kBlue);
    hist_all1->SetFillColorAlpha(kBlue, 0.5);
    hist_all1->SetStats(kFALSE);
    hist_all1->Draw();

    hist1->GetXaxis()->SetTitle("Charge [p.e.]");
    hist1->GetYaxis()->SetTitle("Counts [a.u.]");
    hist1->SetLineWidth(2);
    hist1->SetStats(kFALSE);
    hist1->SetLineColor(kRed);
    hist1->SetFillColorAlpha(kRed, 0.5);
    hist1->Draw("SAME");

    hist_muone1->SetLineWidth(2);
    hist_muone1->SetLineColor(kGreen);
    hist_muone1->SetFillColorAlpha(kGreen, 0.5);
    hist_muone1->SetStats(kFALSE);
    hist_muone1->Draw("SAME");

    TLegend *legend1 = new TLegend(0.7, 0.7, 0.9, 0.9);
    legend1->AddEntry(hist_all1, "Eventi totali WP", "f");
    legend1->AddEntry(hist1, "Eventi comuni WP", "f");
    legend1->AddEntry(hist_muone1, "Eventi wp-classifytool", "f");
    legend1->Draw();

    string filename1 = wp_folder + "/WP_" + run_name1 + "_tolerance_" + oss.str() + ".png";
    canvas1->SaveAs(filename1.c_str());

    // Calcolo l'area di sovrapposizione tra gli istogrammi WP e muone
    overlap_area = 0.0;
    double total_muon_area = 0.0;
    for (int bin = 1; bin <= hist1->GetNbinsX(); ++bin) {
        double bin_content1 = hist1->GetBinContent(bin);
        double bin_content2 = hist_muone1->GetBinContent(bin);
        overlap_area += std::min(bin_content1, bin_content2);
        total_muon_area += bin_content2;
    }

    // Normalizzo l'area di sovrapposizione all'area totale degli eventi muoni
    if (total_muon_area > 0) {
        overlap_area /= total_muon_area;
    }

    delete canvas1;
    delete hist1;
    delete hist_all1;
    delete hist_muone1;
    delete legend1;

    // Istogramma per l'energia del CD
    TCanvas *canvas2 = new TCanvas(run_name1.c_str(), "Istogramma Carica CD", 800, 600);
    TH1F *hist2 = new TH1F(run_name1.c_str(), ("Istogramma Carica CD (Tolleranza: " + oss.str() + " ns)").c_str(), 100, 0, 8);
    TH1F *hist_all2 = new TH1F((run_name1 + "_all").c_str(), ("Istogramma Carica CD (tutti gli eventi vs eventi accoppiati) Tolleranza: " + oss.str() + " ns").c_str(), 100, 0, 8);
    canvas2->SetGrid();
    BinLogX(hist2);
    BinLogY(hist2);
    BinLogX(hist_all2);
    BinLogY(hist_all2);

    for (const auto& npe : all_NPE2) {
        hist_all2->Fill(npe);
    }
    
    for (const auto& npe : common_NPE2) {
        hist2->Fill(npe);
    }

    // Imposto la scala logaritmica sugli assi X e Y
    canvas2->SetLogx();
    canvas2->SetLogy();

    hist_all2->GetXaxis()->SetTitle("Charge [p.e.]");
    hist_all2->GetYaxis()->SetTitle("Counts [a.u.]");
    hist_all2->SetLineWidth(2);
    hist_all2->SetStats(kFALSE);
    hist_all2->SetLineColor(kBlue);
    hist_all2->SetFillColorAlpha(kBlue, 0.5);
    hist_all2->Draw();

    hist2->GetXaxis()->SetTitle("Charge [p.e.]");
    hist2->GetYaxis()->SetTitle("Counts [a.u.]");
    hist2->SetLineWidth(2);
    hist2->SetStats(kFALSE);
    hist2->SetLineColor(kRed);
    hist2->SetFillColorAlpha(kRed, 0.5);
    hist2->Draw("SAME");

    TLegend *legend2 = new TLegend(0.7, 0.7, 0.9, 0.9);
    legend2->AddEntry(hist_all2, "Eventi totali CD", "f");
    legend2->AddEntry(hist2, "Eventi comuni CD", "f");
    legend2->Draw();

    string filename2 = cd_folder + "/CD_" + run_name1 + "_tolerance_" + oss.str() + ".png";
    canvas2->SaveAs(filename2.c_str());

    delete canvas2;
    delete hist2;
    delete hist_all2;
    delete legend2;
}

void analyze_total_wp_cd(const vector<vector<totalEvents>>& total_eventi_per_file_wp, const vector<vector<totalEvents>>& total_eventi_per_file_cd, const vector<vector<muone>>& updated_eventi_per_file, const vector<string>& total_run_names_wp, const vector<string>& total_run_names_cd, const vector<string>& run_names_mod) {
    vector<double> tolerances;
    vector<double> overlap_areas;

    for (size_t j = 0; j < total_eventi_per_file_wp.size(); j++) {
        cout << "Analisi del file " << total_run_names_wp[j] << endl;
        for (double tolerance_ns = 1.0; tolerance_ns <= 1000000000.0; tolerance_ns *= 10) {
            double overlap_area;
            plot_common_events_NPE(total_eventi_per_file_wp[j], total_eventi_per_file_cd[j], tolerance_ns, total_run_names_wp[j], total_run_names_cd[j]);
            plot_common_events_NPE_all(total_eventi_per_file_wp[j], total_eventi_per_file_cd[j], tolerance_ns, total_run_names_wp[j], total_run_names_cd[j]);
            plot_common_events_NPE_muon(total_eventi_per_file_wp[j], total_eventi_per_file_cd[j], updated_eventi_per_file[j], tolerance_ns, total_run_names_wp[j], total_run_names_cd[j], overlap_area);
            tolerances.push_back(tolerance_ns);
            overlap_areas.push_back(overlap_area);
        }
        analyze_common_events(total_eventi_per_file_wp[j], total_eventi_per_file_cd[j], total_run_names_wp[j], total_run_names_cd[j]);
        analyze_common_events_with_energy_cut_cd(total_eventi_per_file_wp[j], total_eventi_per_file_cd[j], total_run_names_wp[j], total_run_names_cd[j], 5e4);
        analyze_common_events_with_energy_cut_wp(total_eventi_per_file_wp[j], total_eventi_per_file_cd[j], total_run_names_wp[j], total_run_names_cd[j], 5e3);
        // Creazione del grafico dell'area di sovrapposizione in funzione della tolleranza
        TCanvas *c_overlap = new TCanvas("c_overlap", "Overlap Area totalWP & WP-classify vs Tolerance", 800, 600);
        TGraph *graph_overlap = new TGraph(tolerances.size(), &tolerances[0], &overlap_areas[0]);
        c_overlap->SetGrid();
        // Aggiungi margini al canvas
        c_overlap->SetLeftMargin(0.15);
        c_overlap->SetBottomMargin(0.15);
        graph_overlap->SetTitle("Normalized Overlap Area totalWP & WP-classify Ev. vs Tolerance;Tolerance [ns];Normalized Overlap Area");
        graph_overlap->GetXaxis()->SetTitleOffset(1.4); // Sposta il titolo dell'asse X
        graph_overlap->GetYaxis()->SetTitleOffset(1.6); // Sposta il titolo dell'asse Y
        graph_overlap->SetMarkerStyle(20);
        graph_overlap->SetMarkerSize(1.4);
        graph_overlap->Draw("AP");
        c_overlap->SetLogx();

        string main_folder = "images";
        string folder_im2= main_folder + "/Overlap_Area_vs_Tolerance_plot";
        if (!fs::exists(main_folder)) {
            fs::create_directory(main_folder);
        }
        if (!fs::exists(folder_im2)) {
            fs::create_directory(folder_im2);
        }

        string filename_overlap2 = folder_im2 + "/Overlap_Area_vs_Tolerance_"+ total_run_names_wp[j] + "_plot.png";
        c_overlap->SaveAs(filename_overlap2.c_str());

        delete c_overlap;
        delete graph_overlap;



    }
    cout << "=============================================" << endl;
}

void analyze_common_events(const vector<totalEvents>& eventi1, const vector<totalEvents>& eventi2, const string& run_name1, const string& run_name2) {
    vector<double> tolerances;
    vector<double> common_event_frequencies;
    vector<double> errors;

    // Calcolo la durata della run
    long double start_time1 = (long double) eventi1.front().fSec + (long double) eventi1.front().fNanoSec * 1e-9;
    long double end_time1 = (long double) eventi1.back().fSec + (long double) eventi1.back().fNanoSec * 1e-9;
    long double start_time2 = (long double) eventi2.front().fSec + (long double) eventi2.front().fNanoSec * 1e-9;
    long double end_time2 = (long double) eventi2.back().fSec + (long double) eventi2.back().fNanoSec * 1e-9;
    long double run_duration1 = end_time1 - start_time1;
    long double run_duration2 = end_time2 - start_time2;
    long double run_duration = max(run_duration1, run_duration2);

    for (double tolerance_ns = 1.0; tolerance_ns <= 100000000.0; tolerance_ns *= 5.0) {
        int common_event_count = count_common_events(eventi1, eventi2, tolerance_ns);
        double common_event_frequency = common_event_count / (long double) run_duration;
        double error = sqrt(common_event_count) / run_duration;
        tolerances.push_back(tolerance_ns);
        common_event_frequencies.push_back(common_event_frequency);
        errors.push_back(error);
    }

    // Creazione del grafico della frequenza degli eventi comuni in funzione della tolleranza
    TCanvas *c_common_events = new TCanvas("c_common_events", "Common Events Rate vs Tolerance", 800, 600);
    int n = tolerances.size();
    TGraphErrors *graph_common_events = new TGraphErrors(n, &tolerances[0], &common_event_frequencies[0], nullptr, &errors[0]);

    c_common_events->SetGrid();
    c_common_events->SetLeftMargin(0.15);
    c_common_events->SetBottomMargin(0.15);
    graph_common_events->SetTitle("Common Events Rate vs Tolerance;Tolerance [ns];Rate [Hz]");
    graph_common_events->GetXaxis()->SetTitleOffset(1.4);
    graph_common_events->GetYaxis()->SetTitleOffset(1.6);
    graph_common_events->SetMarkerStyle(20);
    graph_common_events->SetMarkerSize(1.4);
    graph_common_events->Draw("AP");
    c_common_events->SetLogx();

    string main_folder = "images";
    string folder_common_events = main_folder + "/Common_Events_vs_Tolerance_plot";
    if (!fs::exists(main_folder)) {
        fs::create_directory(main_folder);
    }
    if (!fs::exists(folder_common_events)) {
        fs::create_directory(folder_common_events);
    }

    string filename_common_events = folder_common_events + "/Common_Events_vs_Tolerance_" + run_name1 + "_rate_plot.png";
    c_common_events->SaveAs(filename_common_events.c_str());

    delete c_common_events;
    delete graph_common_events;

    plot_common_events_vs_tolerance(eventi1, eventi2, run_name1, run_name2);
}

void analyze_common_events_with_energy_cut_cd(const vector<totalEvents>& eventi1, const vector<totalEvents>& eventi2, const string& run_name1, const string& run_name2, double energy_cut) {
    vector<double> tolerances;
    vector<double> common_event_frequencies;
    vector<double> errors;

    // Calcolo la durata della run
    long double start_time1 = (long double) eventi1.front().fSec + (long double) eventi1.front().fNanoSec * 1e-9;
    long double end_time1 = (long double) eventi1.back().fSec + (long double) eventi1.back().fNanoSec * 1e-9;
    long double start_time2 = (long double) eventi2.front().fSec + (long double) eventi2.front().fNanoSec * 1e-9;
    long double end_time2 = (long double) eventi2.back().fSec + (long double) eventi2.back().fNanoSec * 1e-9;
    long double run_duration1 = end_time1 - start_time1;
    long double run_duration2 = end_time2 - start_time2;
    long double run_duration = max(run_duration1, run_duration2);

    for (double tolerance_ns = 1.0; tolerance_ns <= 100000000.0; tolerance_ns *= 5.0) {
        int common_event_count = count_common_events_with_energy_cut_cd(eventi1, eventi2, tolerance_ns, energy_cut);
        double common_event_frequency = common_event_count / (long double) run_duration;
        double error = sqrt(common_event_count) / run_duration;
        tolerances.push_back(tolerance_ns);
        common_event_frequencies.push_back(common_event_frequency);
        errors.push_back(error);
    }

    // Creazione del grafico della frequenza degli eventi comuni in funzione della tolleranza
    TCanvas *c_common_events = new TCanvas("c_common_events", "Common Events Rate vs Tolerance", 800, 600);
    int n = tolerances.size();
    TGraphErrors *graph_common_events = new TGraphErrors(n, &tolerances[0], &common_event_frequencies[0], nullptr, &errors[0]);
    c_common_events->SetGrid();
    c_common_events->SetLeftMargin(0.15);
    c_common_events->SetBottomMargin(0.15);
    graph_common_events->SetTitle("Common Events Rate vs Tolerance with charge cut on CD;Tolerance [ns];Rate [Hz]");
    graph_common_events->GetXaxis()->SetTitleOffset(1.4);
    graph_common_events->GetYaxis()->SetTitleOffset(1.6);
    graph_common_events->SetMarkerStyle(20);
    graph_common_events->SetMarkerSize(1.4);
    graph_common_events->Draw("AP");
    c_common_events->SetLogx();

    string main_folder = "images";
    string folder_common_events = main_folder + "/Common_Events_vs_Tolerance_with_cut_plot";
    if (!fs::exists(main_folder)) {
        fs::create_directory(main_folder);
    }
    if (!fs::exists(folder_common_events)) {
        fs::create_directory(folder_common_events);
    }

    string filename_common_events = folder_common_events + "/Common_Events_vs_Tolerance_" + run_name1 + "_rate__cut_CD_plot.png";
    c_common_events->SaveAs(filename_common_events.c_str());

    delete c_common_events;
    delete graph_common_events;
}

int count_common_events_with_energy_cut_cd(const vector<totalEvents>& eventi1, const vector<totalEvents>& eventi2, double tolerance_ns, double energy_cut) {
    int common_event_count = 0;
    size_t i = 0, j = 0;

    while (i < eventi1.size() && j < eventi2.size()) {
        long double ev_time1 = (long double) eventi1[i].fSec * 1e9 + (long double) eventi1[i].fNanoSec;
        long double ev_time2 = (long double) eventi2[j].fSec * 1e9 + (long double) eventi2[j].fNanoSec;

        if (abs(ev_time1 - ev_time2) <= tolerance_ns && eventi2[j].NPE >= energy_cut) {
            common_event_count++;
            i++;
            j++;
        } else if (ev_time1 < ev_time2) {
            i++;
        } else {
            j++;
        }
    }
    return common_event_count;
}

void analyze_common_events_with_energy_cut_wp(const vector<totalEvents>& eventi1, const vector<totalEvents>& eventi2, const string& run_name1, const string& run_name2, double energy_cut) {
    vector<double> tolerances;
    vector<double> common_event_frequencies;
    vector<double> errors;

    // Calcolo la durata della run
    long double start_time1 = (long double) eventi1.front().fSec + (long double) eventi1.front().fNanoSec * 1e-9;
    long double end_time1 = (long double) eventi1.back().fSec + (long double) eventi1.back().fNanoSec * 1e-9;
    long double start_time2 = (long double) eventi2.front().fSec + (long double) eventi2.front().fNanoSec * 1e-9;
    long double end_time2 = (long double) eventi2.back().fSec + (long double) eventi2.back().fNanoSec * 1e-9;
    long double run_duration1 = end_time1 - start_time1;
    long double run_duration2 = end_time2 - start_time2;
    long double run_duration = max(run_duration1, run_duration2);

    for (double tolerance_ns = 1.0; tolerance_ns <= 100000000.0; tolerance_ns *= 5.0) {
        int common_event_count = count_common_events_with_energy_cut_wp(eventi1, eventi2, tolerance_ns, energy_cut);
        double common_event_frequency = common_event_count / (long double) run_duration;
        double error = sqrt(common_event_count) / run_duration;
        tolerances.push_back(tolerance_ns);
        common_event_frequencies.push_back(common_event_frequency);
        errors.push_back(error);
    }

    // Creazione del grafico della frequenza degli eventi comuni in funzione della tolleranza
    TCanvas *c_common_events = new TCanvas("c_common_events", "Common Events Rate vs Tolerance", 800, 600);
    int n = tolerances.size();
    TGraphErrors *graph_common_events = new TGraphErrors(n, &tolerances[0], &common_event_frequencies[0], nullptr, &errors[0]);
    c_common_events->SetGrid();
    c_common_events->SetLeftMargin(0.15);
    c_common_events->SetBottomMargin(0.15);
    graph_common_events->SetTitle("Common Events Rate vs Tolerance with charge cut on WP;Tolerance [ns];Rate [Hz]");
    graph_common_events->GetXaxis()->SetTitleOffset(1.4);
    graph_common_events->GetYaxis()->SetTitleOffset(1.6);
    graph_common_events->SetMarkerStyle(20);
    graph_common_events->SetMarkerSize(1.4);
    graph_common_events->Draw("AP");
    c_common_events->SetLogx();

    string main_folder = "images";
    string folder_common_events = main_folder + "/Common_Events_vs_Tolerance_with_cut_plot";
    if (!fs::exists(main_folder)) {
        fs::create_directory(main_folder);
    }
    if (!fs::exists(folder_common_events)) {
        fs::create_directory(folder_common_events);
    }

    string filename_common_events = folder_common_events + "/Common_Events_vs_Tolerance_" + run_name1 + "_rate__cut_WP_plot.png";
    c_common_events->SaveAs(filename_common_events.c_str());

    delete c_common_events;
    delete graph_common_events;
}

int count_common_events_with_energy_cut_wp(const vector<totalEvents>& eventi1, const vector<totalEvents>& eventi2, double tolerance_ns, double energy_cut) {
    int common_event_count = 0;
    size_t i = 0, j = 0;

    while (i < eventi1.size() && j < eventi2.size()) {
        long double ev_time1 = eventi1[i].fSec * 1e9 + eventi1[i].fNanoSec;
        long double ev_time2 = eventi2[j].fSec * 1e9 + eventi2[j].fNanoSec;
        if (abs(ev_time1 - ev_time2) <= tolerance_ns && eventi1[i].NPE >= energy_cut) {
            common_event_count++;
            i++;
            j++;
        } else if (ev_time1 < ev_time2) {
            i++;
        } else {
            j++;
        }
    }
    return common_event_count;
}

void plot_common_events_vs_tolerance(const vector<totalEvents>& eventi1, const vector<totalEvents>& eventi2, const string& run_name1, const string& run_name2) {
    vector<double> tolerances;
    vector<int> common_event_counts;

    // Definisci i valori di tolleranza da analizzare
    for (double tolerance_ns = 1.0; tolerance_ns <= 100000000.0; tolerance_ns *= 10.0) {
        tolerances.push_back(tolerance_ns);
    }

    // Calcola il numero di eventi in comune per ciascuna tolleranza
    for (double tolerance_ns : tolerances) {
        int common_event_count = count_common_events(eventi1, eventi2, tolerance_ns);
        common_event_counts.push_back(common_event_count);
    }

    // Creazione dell'istogramma
    TCanvas *c_common_events = new TCanvas("c_common_events", "Common Events vs Tolerance", 800, 600);
    TH1F *hist_common_events = new TH1F("hist_common_events", "Common Events vs Tolerance;Tolerance [ns];Number of Common Events", tolerances.size(), 0, tolerances.size());

    for (size_t i = 0; i < tolerances.size(); ++i) {
        hist_common_events->SetBinContent(i + 1, common_event_counts[i]);
        std::ostringstream oss;
        oss << "10^" << static_cast<int>(log10(tolerances[i]));
        hist_common_events->GetXaxis()->SetBinLabel(i + 1, oss.str().c_str());
    }

    // Rimuovi il riquadro delle informazioni
    hist_common_events->SetStats(0);

    // Riempie di colore l'istogramma
    hist_common_events->SetFillColor(kBlue);
    hist_common_events->SetFillStyle(3001);

    c_common_events->SetGrid();
    c_common_events->SetLeftMargin(0.15);
    c_common_events->SetBottomMargin(0.15);
    hist_common_events->GetXaxis()->SetTitleOffset(1.4);
    hist_common_events->GetYaxis()->SetTitleOffset(1.6);
    hist_common_events->SetMarkerStyle(21);
    hist_common_events->SetMarkerSize(1.5);
    hist_common_events->Draw("HIST");

    string main_folder = "images";
    string folder_common_events = main_folder + "/Common_Events_vs_Tolerance_hist";
    if (!fs::exists(main_folder)) {
        fs::create_directory(main_folder);
    }
    if (!fs::exists(folder_common_events)) {
        fs::create_directory(folder_common_events);
    }

    string filename_common_events = folder_common_events + "/Common_Events_vs_Tolerance_" + run_name1 + "_plot.png";
    c_common_events->SaveAs(filename_common_events.c_str());

    delete c_common_events;
    delete hist_common_events;
}