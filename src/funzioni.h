#ifndef FUNZIONI_H
#define FUNZIONI_H

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <cmath>
#include <numeric>
#include <filesystem>
#include <algorithm>
#include <set>
#include <chrono>
#include <unordered_set>
#include <iomanip>

// Librerie di ROOT
#include "TH1F.h"
#include "TCanvas.h"
#include "TApplication.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TPaveStats.h"
#include "TAxis.h"
#include "TH2.h"
#include "TF1.h"
#include "TGraph2D.h"
#include "TStyle.h"
#include "TColor.h"
#include "TFile.h"
#include "TTree.h"
#include "TGraphErrors.h"
#include "TString.h"
#include "TRandom.h"
#include "TROOT.h"

using namespace std;
namespace fs = std::filesystem; // Alias per filesystem

struct muone {
    int eventID;
    int trackID;
    int fSec;
    int fNanosec;
    float PeSum;
    float entry_x, entry_y, entry_z;
    float exit_x, exit_y, exit_z;
    float ux, uy, uz;
    float quality;
   // float entry_time, exit_time;
    double distance;
};

struct totalEvents {
    float NPE;
    Int_t fSec;
    Int_t fNanoSec;
    float Recox, Recoy, Recoz;
    Int_t RunNumber;
};

struct RunInfo {
    string run_name;
    int file;
    double counts;
    double error;
    string date;
    string time;
    double duration;
    double volume;
    double rate;
    double err_rate;
};

// Funzione per caricare i dati ROOT in un vettore
vector<muone> load_root_data(const string&);

// Funzione per stampare tutti i dati
void print_all_data(const vector<muone>&);

// Funzione per salvare i dati in un file
void save_all_data_to_file(const vector<muone>&, const string&);

// Funzione per creare un istogramma della carica
void PeSum_histogram(const vector<muone>&, const string&);

// Funzione per creare plot tridimensionale dei punti di entrata
void plot_3D_distribution(const vector<muone>&);

// Funzione per creare istrogramma distribuzione angolo polare
void plot_polar_angle_distribution(const vector<muone>&, const string&);

// Funzione per calcolare il numero di bundle di muoni
int muon_bundle(const vector<muone>&);

// Funzione per contare il numero di eventi singolarmente
int Nevents(const vector<muone>&);

// Funzione per calcolare il tempo medio tra un evento e il successivo (inverso della frequenza)
float mean_delta_t(const vector<muone>&, const string&);

// Funzione che mi ordina in maniera crescente in ID
void sort_events_by_id(vector<muone>&);

// Funzione per creare istogramma di energia per muoni singoli e bundle
void PeSum_histograms(const vector<muone>& eventi, const string& run_name);

// Funzione per creare un istogramma delle distanze percorse all'interno del rivelatore
void Distance_histogram(const vector<muone>&, const string&);

// Funzione per contare il numero di file da analizzare (.root)
int count_root_files(const string&);

// Funzione per estrarre il nome della RUN
string get_run_name(const string&);

// Funzione per caricare più file ROOT e salvare i nomi delle RUN
vector<vector<muone>> load_multiple_root_files(const string&, vector<string>&);

// Funzione per studiare la correlazione tra la direzione dei muoni e la loro energia
void PeSum_vs_polar_angle(const vector<muone>&, const string&);

// Funzione per studiare la correlazione tra la direzione dei muoni e la loro energia
void PeSum_vs_azimuthal_angle(const vector<muone>&, const string&);

// Funzione per calcolare il tempo totale della run
double total_run_time(const vector<muone>&);

// Funzione per estrarre le informazioni sulla run
vector<RunInfo> load_run_info(const string&);

// Funzione per calcolare il rate dei muoni nel tempo
void plot_muon_rate(const vector<muone>& , const string&, double interval_sec);

// Funzione per calcolare il rate dei muoni nel tempo per più file
void plot_muon_rate_vs_run(const vector<vector<muone>>&, const vector<string>&);

// Funzione per creare un istogramma della distribuzione dell'angolo azimutale
void plot_azimuthal_angle_distribution(const vector<muone>&, const string&);

// Funzione per confrontare angolo polare e azimutale
void Polar_vs_Azimuthal_angle(const vector<muone>&, const string&);

// Funzione per calcolare la distanza tra l'origine e la retta del muone
double distance_point_to_line(const muone&);

// Funzione che plotta la distanza della retta di muoni dall'origine
void path_distance_histogram(const vector<muone>&, const string&);

// Funzione per calcoare gli eventi di bordo
double edge_events(const vector<muone>&, double);

// Funzione per plottare il rate dei muoni con taglio sugli eventi di bordo
void plot_muon_rate_with_edge_cut_vs_run(const vector<vector<muone>>&, const vector<string>&, double);

// Funzione per plottare il numero di trackID presenti
void plot_trackID_distribution(const vector<muone>&, const string&);

// Funzione per caricare i dati ROOT di total in un vettore
vector<totalEvents> load_totalEvents_data(const string&);

// Funzione per caricare più file ROOT di total e salvare i nomi delle RUN
vector<vector<totalEvents>> load_multiple_totalEvents_files(const string&, vector<string>&);

// Funzione per trovare le RUN comuni tra i due set di dati
vector<pair<size_t, size_t>> find_common_runs(const vector<string>&, const vector<string>&);

// Funzione per creare un nuovo vettore di eventi aggiornato
vector<vector<muone>> create_updated_events_vector(const vector<vector<muone>>&, const vector<vector<totalEvents>>&, const vector<pair<size_t, size_t>>&, const vector<string>&, vector<string>&);

// Funzione per plottare la differenza di tempo con l'energia dell'envento
void plot_time_difference_vs_charge(const vector<totalEvents>&, const string&);

// Funzione per creare un istogramma della carica
void PeSum_histogram_log(const vector<muone>&, const string&);

// Dichiarazione della funzione BinLogX
void BinLogX(TH1*);

// Dichiarazione della funzione BinLogX
void BinLogY(TH1*);

// Funzione per creare un istogramma della carica in scala logaritmica degli eventi totali
void total_PeSum_histogram_log(const vector<totalEvents>&, const string&);

// Funzione per creare un istogramma in cui vengono sovrapposti i due istogrammi
void total_PeSum_histogram_log(const vector<totalEvents>&, const vector<muone>&, const string&, const string&);


void total_PeSum_histogram_log_complementary(const vector<totalEvents>&, const vector<muone>&, const string&, const string&);


void count_high_energy_events(const vector<totalEvents>&, const vector<muone>&, const string&, const string&);


void total_PeSum_histogram_log_divided(const vector<totalEvents>&, const vector<muone>&, const string&, const string&);


void total_PeSum_histogram_log_divided_track(const vector<totalEvents>&, const vector<muone>&, const string&, const string&);


int count_common_events(const vector<totalEvents>&, const vector<totalEvents>&, double);


void plot_common_events_NPE(const vector<totalEvents>&, const vector<totalEvents>&, double, const string&, const string&);


void plot_common_events_NPE_all(const vector<totalEvents>&, const vector<totalEvents>&, double, const string&, const string&);


void plot_common_events_NPE_muon(const vector<totalEvents>&, const vector<totalEvents>&, const vector<muone>&, double, const string&, const string&, double&);


void analyze_total_wp_cd(const vector<vector<totalEvents>>&, const vector<vector<totalEvents>>&, const vector<vector<muone>>&, const vector<string>&, const vector<string>&, const vector<string>&);


void plot_common_events_histogram(const vector<totalEvents>&, const vector<totalEvents>&, const string&, const string&);


void analyze_common_events(const vector<totalEvents>&, const vector<totalEvents>&, const string&, const string&);


void analyze_common_events_with_energy_cut_cd(const vector<totalEvents>&, const vector<totalEvents>&, const string&, const string&, double);
int count_common_events_with_energy_cut_cd(const vector<totalEvents>&, const vector<totalEvents>&, double, double);

void analyze_common_events_with_energy_cut_wp(const vector<totalEvents>&, const vector<totalEvents>&, const string&, const string&, double);
int count_common_events_with_energy_cut_wp(const vector<totalEvents>&, const vector<totalEvents>&, double, double);

void plot_common_events_vs_tolerance(const vector<totalEvents>&, const vector<totalEvents>&, const string&, const string&);

void plot_frequency_vs_tolerance_and_charge_cut_cd(const vector<totalEvents>&, const vector<totalEvents>&, const string&, const string&);

#endif