#include "funzioni.h"

// Prendo i dati dal TTree e li inserisco in un vector di muone
vector<muone> load_root_data(const string& filename) {
    vector<muone> eventi;
    
    TFile *file = TFile::Open(filename.c_str(), "READ");
    if (!file) {
        cerr << "Errore: impossibile aprire il file ROOT!" << endl;
        return eventi;
    }

    TTree *tree = (TTree*)file->Get("MuonEvents");
    if (!tree) {
        cerr << "Errore: TTree 'MuonEvents' non trovato!" << endl;
        file->Close();
        return eventi;
    }

    muone evento;
    tree->SetBranchAddress("EvtID", &evento.eventID);
    tree->SetBranchAddress("TrackNumber", &evento.trackID);
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
   
   Long64_t nEntries = tree->GetEntries();
    for (Long64_t i = 0; i < nEntries; i++) {
        tree->GetEntry(i);
        evento.distance = sqrt(pow(evento.exit_x - evento.entry_x, 2) + pow(evento.exit_y - evento.entry_y, 2) + pow(evento.exit_z - evento.entry_z, 2));

        eventi.push_back(evento);
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

void save_all_data_to_file(const vector<muone>& eventi, const string& filename = "output.txt"){
    ofstream file(filename);
    if (!file) {
        cerr << "Errore: impossibile aprire il file " << filename << " per la scrittura!" << endl;
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
    TCanvas *canvas = new TCanvas(("canvas_charge_" + run_name).c_str(), ("Istogramma Energia - " + run_name).c_str(), 800, 600);
    TH1F *charge = new TH1F(("Distribuzione_dell_energia_" + run_name).c_str(), ("Distribuzione dell'energia - " + run_name).c_str(), 100, 100, 100);
    charge->StatOverflows(kTRUE);
    canvas->SetGrid();
    for (const auto& ev : eventi) {
        charge->Fill(ev.PeSum);
    }
    charge->GetXaxis()->SetTitle("Charge [p.e.]");
    charge->GetYaxis()->SetTitle("Counts [a.u.]");
    charge->SetLineWidth(2);
    charge->SetFillColorAlpha(kBlue, 0.3);
    charge->Draw();
    // Salva il grafico con il nome della RUN
    string filename = "PeSum_" + run_name + ".png";
    canvas->SaveAs(filename.c_str());
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

void plot_theta_distribution(const vector<muone>& eventi, const string& run_name){
    gStyle->SetOptStat(1);  // Abilita il box delle statistiche
    TCanvas *canvas = new TCanvas(("canvas_theta_" + run_name).c_str(), ("Distribuzione angolare - " + run_name).c_str(), 800, 600);
    TH1F *zenith = new TH1F(("Distribuzione_angolare_" + run_name).c_str(), ("Distribuzione angolare - " + run_name).c_str(), 100, 100, 100);
    zenith->StatOverflows(kTRUE);
    canvas->SetGrid();
    for(const auto& e:eventi){
        double theta = acos(e.uz);
        zenith->Fill(cos(theta));
    }
    zenith->SetLineColor(kGreen);
    zenith->SetLineWidth(2);
    zenith->SetFillColorAlpha(kGreen, 0.3);
    
    zenith->GetXaxis()->SetTitle("cos(#theta)");
    zenith->GetYaxis()->SetTitle("Counts [a.u.]");
    zenith->Draw("HIST");
    string filename = "Theta_" + run_name + ".png";
    canvas->SaveAs(filename.c_str());
}

int muon_bundle(const vector<muone>& eventi){
    int bundle = 0;
    int last_entry_time = 0;
    for(const auto& e : eventi){
        if(e.trackID > 0 && e.fSec+e.fNanosec!= last_entry_time){
            last_entry_time = e.fSec+e.fNanosec;
            bundle++;// Conto il bundle una sola volta per tempo
        }
    }
    return bundle;
}

int Nevents(const vector<muone>& eventi){
    int nevents = 0;
    int last_entry_time = 0;
    for(const auto& e : eventi){
        if(e.fSec+e.fNanosec != last_entry_time){
            last_entry_time = e.fSec+e.fNanosec;
            nevents++;
        }
    }
    return nevents;
}

float mean_delta_t(const vector<muone>& eventi){
    double mean_delta_t_= 0;
    int last_entry_time = 0;
    vector<float> intervalli;
    for (size_t i = 1; i < eventi.size(); i++) {
        if(eventi[i].fSec+eventi[i].fNanosec!=last_entry_time){
            // Converti il tempo in nanosecondi e calcola la differenza
            float t1 = eventi[i - 1].fSec * 1e9 + eventi[i - 1].fNanosec;
            float t2 = eventi[i].fSec * 1e9 + eventi[i].fNanosec;
            float delta_t = t2 - t1;

            // Se l'intervallo è negativo, c'è un errore nell'ordine degli eventi
            if (delta_t < 0) {
                cerr << "Attenzione: Intervallo di tempo negativo rilevato tra due eventi!" << endl;
            } else {
                intervalli.push_back(delta_t);
            }
        }
    }

    // Calcola la media
    double somma = accumulate(intervalli.begin(), intervalli.end(), 0.0);
    double media = somma / intervalli.size();

    return media;
}

void sort_events_by_id(vector<muone>& eventi) {
    sort(eventi.begin(), eventi.end(), [](const muone& a, const muone& b) {
        return a.eventID < b.eventID;
    });
}

void PeSum_histograms(const vector<muone>& eventi) {
    TCanvas *canvasA = new TCanvas("canvasA", "Istogramma Energia - Muoni Singoli", 800, 600);
    TCanvas *canvasB = new TCanvas("canvasB", "Istogramma Energia - Muoni Bundle", 800, 600);
    canvasB->Divide(2,2);

    TH1F *one = new TH1F("Muoni singoli", "Distribuzione Energia - Muoni Singoli", 100, 100, 100);
    TH1F *two = new TH1F("Muoni doppi", "Distribuzione Energia - Muoni doppi", 100, 100, 100);
    TH1F *three = new TH1F("Muoni tripli", "Distribuzione Energia - Muoni tripli", 100, 100, 100);
    TH1F *four = new TH1F("Muoni quadrupli", "Distribuzione Energia - Muoni quadrupli", 100, 100, 100);
    TH1F *five = new TH1F("Muoni > quintupli", "Distribuzione Energia - Muoni > quintupli", 100, 100, 100);

    canvasA->SetGrid();

    size_t i = 0;
    while (i < eventi.size()) {
        // Prendiamo il tempo di riferimento del primo evento del gruppo
        int current_fSec = eventi[i].fSec;
        int current_fNanosec = eventi[i].fNanosec;
        double totalPeSum = 0.0;
        int numMuoni = 0;

        // Raggruppa tutti gli eventi con lo stesso tempo (fSec e fNanosec uguali) e somma PeSum
        while (i < eventi.size() && 
               eventi[i].fSec == current_fSec && 
               eventi[i].fNanosec == current_fNanosec) {
            totalPeSum += eventi[i].PeSum;
            numMuoni++;
            i++;
        }

        // Inserisci nei rispettivi istogrammi
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

    // Disegna gli istogrammi
    canvasA->cd();
    one->GetXaxis()->SetTitle("Charge [p.e.]");
    one->GetYaxis()->SetTitle("Counts [a.u.]");
    one->SetLineColor(kRed);
    one->SetLineWidth(2);
    one->SetFillColorAlpha(kRed, 0.3);
    one->Draw();

    // Istogrammi bundle
    canvasB->cd(1);
    gPad->SetGrid();
    two->GetXaxis()->SetTitle("Charge [p.e.]");
    two->GetYaxis()->SetTitle("Counts [a.u.]");
    two->SetLineColor(kRed);
    two->SetLineWidth(2);
    two->SetFillColorAlpha(kRed, 0.3);
    two->Draw();
    canvasB->cd(2);
    gPad->SetGrid();
    three->GetXaxis()->SetTitle("Charge [p.e.]");
    three->GetYaxis()->SetTitle("Counts [a.u.]");
    three->SetLineColor(kRed);
    three->SetLineWidth(2);
    three->SetFillColorAlpha(kRed, 0.3);
    three->Draw();
    canvasB->cd(3);
    gPad->SetGrid();
    four->GetXaxis()->SetTitle("Charge [p.e.]");
    four->GetYaxis()->SetTitle("Counts [a.u.]");
    four->SetLineColor(kRed);
    four->SetLineWidth(2);
    four->SetFillColorAlpha(kRed, 0.3);
    four->Draw();
    canvasB->cd(4);
    gPad->SetGrid();
    five->GetXaxis()->SetTitle("Charge [p.e.]");
    five->GetYaxis()->SetTitle("Counts [a.u.]");
    five->SetLineColor(kRed);
    five->SetLineWidth(2);
    five->SetFillColorAlpha(kRed, 0.3);
    five->Draw();

    // Salva i grafici
    canvasA->SaveAs("PeSum_singoli.png");
    canvasB->SaveAs("PeSum_bundle.png");
}

void Distance_histogram(const vector<muone>& eventi){
    TCanvas *canval = new TCanvas("canval", "Istogramma Distanza percorsa all'interno del CD", 800, 600);
    TH1F *dist = new TH1F("Distanza percorsa all'interno del CD", "Distanza percorsa all'interno del CD", 100, 100, 100);
    dist->StatOverflows(kTRUE);
    canval->SetGrid();
    for (const auto& ev : eventi) {
        dist->Fill(ev.distance);
    }
    dist->SetLineColor(kOrange);
    dist->GetXaxis()->SetTitle("Distance [mm]");
    dist->GetYaxis()->SetTitle("Counts [a.u.]");
    dist->SetLineWidth(2);
    dist->SetFillColorAlpha(kOrange, 0.3);
    dist->Draw();
    canval->SaveAs("Distance_plot.png");
}

int count_root_files(const string& folder_path) {
    int count = 0;
    
    // Scansiona la cartella e conta i file con estensione .root
    for (const auto& entry : fs::directory_iterator(folder_path)) {
        if (entry.path().extension() == ".root") {
            count++;
        }
    }
    return count;
}

string get_run_name(const string& filepath) {
    string filename = fs::path(filepath).filename().string();  // Ottiene solo il nome del file
    size_t pos = filename.find("_");  // Trova il primo underscore
    if (pos != string::npos) {
        filename = filename.substr(0, pos);  // Prende solo la parte prima dell'underscore
    }
    return filename;
}

vector<vector<muone>> load_multiple_root_files(const string& folder_path, vector<string>& run_names) {
    vector<vector<muone>> all_eventi;
    
    // Scansiona la cartella e carica ogni file in un vettore separato
    for (const auto& entry : fs::directory_iterator(folder_path)) {
        if (entry.path().extension() == ".root") {
            string run_name = get_run_name(entry.path().string());
            run_names.push_back(run_name);  // Salva il nome della RUN

            cout << "Caricamento file: " << run_name << endl;
            vector<muone> eventi = load_root_data(entry.path().string());
            all_eventi.push_back(eventi);
        }
    }

    cout << "Numero totale di file ROOT analizzati: " << all_eventi.size() << endl;
    return all_eventi;
}