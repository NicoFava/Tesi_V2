<p align="center">
  <a href="#it">🇮🇹 Italiano</a> •
  <a href="#en">🇬🇧 English</a>
</p>

---

<a id="it"></a>
# 🌌 JUNO Cosmic Muon Analysis

**JUNO Cosmic Muon Analysis** è l'evoluzione finale e ottimizzata dei progetti di analisi dati sviluppati per il mio lavoro di tesi triennale in Fisica (precedentemente *Tesi_V0* e *Tesi_V1*). Questo applicativo C++/ROOT supporta l'analisi di **più file ROOT contemporaneamente**, garantendo un'elaborazione efficiente e la generazione automatica di plot organizzati in apposite directory.

## 📖 Contesto del Lavoro di Tesi

L'esperimento **JUNO (Jiangmen Underground Neutrino Observatory)** è un sofisticato rivelatore sotterraneo situato in Cina, progettato principalmente per determinare la gerarchia di massa dei neutrini. Per poter misurare con estrema precisione il segnale neutrinico, è essenziale minimizzare il fondo (background). Una delle principali fonti di rumore è dovuta agli isotopi radioattivi cosmogenici, prodotti dal passaggio dei **muoni cosmici** all'interno del rivelatore. Per questo motivo, una ricostruzione e classificazione accurata delle tracce muoniniche è vitale per le strategie di veto.

Attualmente, JUNO si trova nella **fase di riempimento** del *Central Detector* (CD) con lo scintillatore liquido. Già in questa fase è possibile caratterizzare il flusso di muoni sfruttando l'abbondante produzione di luce Cherenkov. 
Questo progetto software è stato sviluppato per analizzare i dati reali provenienti dai primi mesi di presa dati in fase di riempimento. Si occupa di calcolare rate, distribuzioni spaziali ed energetiche, di gestire le molteplicità dei muoni (eventi singoli vs bundle) e di confrontare le efficienze di classificazione tra diversi sottosistemi (es. Water Pool e Central Detector).

---

## 📄 File di Output Principali

Il programma analizza i dataset e sintetizza i risultati in alcuni file di testo:

### 1. `output.txt`
Contiene la sintesi dei risultati principali per ogni RUN analizzata:
* **Check di caricamento**: Numero di file analizzati, eventi contenuti per run ed eventuali segnalazioni di assenza del tree `MuonEvents`.
* **Timestamp e Tempo di Presa Dati**: Data/ora di inizio RUN (lette da `BiPo_rate_byfile.txt`) e tempo totale t misurato in secondi.
* **Analisi Temporale (<$\Delta t$>)**: Tempo medio tra un evento e il successivo. Qualora si verifichi un $\Delta t$ negativo (errore di acquisizione/timestamp), l'intervallo viene ignorato e segnalato per evitare distorsioni.
* **Calcolo del Rate**: Frequenza dei muoni in Hz. Viene calcolato sia sugli **eventi univoci** (singolo `EventID`, trattando i bundle come un'unica traccia), sia sulla totalità dei muoni registrati (includendo la molteplicità).
* **Muoni Singoli e Bundle**: Conteggio e percentuale degli eventi multipli (più tracce per un singolo `EventID`). *Nota: in assenza di `TrackID` valido, le grandezze legate ai bundle vengono omesse.*
* **Distanza dall'Origine e Tagli di Bordo**: Calcolo della distanza minima tra il centro del rivelatore e la traiettoria del muone (retta interpolata tra `entry_point` e `exit_point`). Gli eventi troppo periferici vengono contati come *eventi di bordo* e viene calcolato il **Rate dopo il taglio**.

*(Un procedimento analogo e riassuntivo viene stampato per le RUN modificate che integrano l'analisi incrociata tra WP e CD).*

### 2. `error&info.txt`
Il log di diagnostica. Raccoglie messaggi d'errore (es. $\Delta t$ negativi, problemi di apertura file ROOT) e informazioni sullo stato di esecuzione, vitale per il debug dell'infrastruttura dati.

### 3. `BiPo_rate_byfile.txt`
Tabella di input ausiliaria che fornisce le associazioni tra i nomi delle RUN (dalla `RUN3793` in poi) e i rispettivi orari e date di inizio acquisizione.

---

## 📊 Grafici Generati

L'eseguibile automatizza la produzione di istogrammi e scatter plot, salvati nella cartella `images/`:

* 📂 `Distance_plot/`: Istogrammi della distanza percorsa dai muoni/bundle nel rivelatore.
* 📂 `PeSum_plot/`: Distribuzione della carica prodotta (Photoelectrons Sum) per evento.
* 📂 `PeSum_single_plot/` & `PeSum_bundle_plot/`: Carica (PeSum) suddivisa rispettivamente per muoni singoli e per muoni bundle.
* 📂 `Polar_Angle_plot/` & `Azimuthal_Angle_plot/`: Distribuzioni angolari (coseno dell'angolo polare e angolo azimutale) delle tracce.
* 📂 `PeSum_vs_Polar_Angle_plot/` & `PeSum_vs_Azimuthal_Angle_plot/`: Mappe 2D che mettono in correlazione l'energia del muone con la sua direzione d'arrivo.
* 📂 `Polar_vs_azimuth_plot/`: Distribuzione direzionale 2D (Zenith vs Azimuth).
* 📂 `Path_Distance_plot/`: Istogrammi della distanza minima al centro del Central Detector.
* 📂 `TrackID_Distribution_plot/`: Distribuzioni delle molteplicità di traccia per file.
* 📂 `Muon_Rate_plot/`: Andamento temporale del rate in bin da 300 secondi.
* 📈 `Muon_Rate_vs_Run.png` & `Muon_Rate_edge_cut_vs_Run.png`: Andamento del Rate medio complessivo in funzione dell'indice della RUN, prima e dopo i tagli di bordo.

---

## 🔬 Note Tecniche: Integrazione Dati WP e CD

Un focus centrale di questo codice è stato l'incrocio dei dati provenienti da dataset differenti: la classificazione tramite il software *WpMuonClassifyRecTool* e la totalità degli eventi catturati con una soglia differente.
* **La struttura `totalEvents`**: Creata per immagazzinare la totalità degli eventi non ancora filtrati. L'obiettivo è confrontarli con l'output di classificazione.
* **Correzione della Carica**: Essendo la carica stimata dalla classificazione WPMuonClassify poco attendibile, è stata implementata una funzione di correzione. Per le tracce che mostrano sovrapposizione temporale (stesso timestamp ed `EventID`), il codice sovrascrive l'evento rimuovendo le repliche ridondanti e iniettando la carica esatta dei nuovi dataset.
* **Considerazioni sui risultati**: Avendo escluso gli eventi bundle (a causa del merging) stiamo approssimando ogni interazione come un muone singolo, portando fisiologicamente a un abbassamento del rate assoluto, anche se i valori restano elevati. L'istogramma di carica così ricalcolato evidenzia valori decisamente superiori ai dataset base e mostra una lunghissima coda asimmetrica, caratteristica che richiederà ulteriori indagini fisiche. Attualmente questa pipeline ibrida è stata validata a fondo sulla `RUN4049`, ma l'architettura supporta già un set arbitrario di RUN.

---

## 🚀 Come usare il programma

1. Clona la repository.
2. Inserisci i file dati `.root` all'interno di una cartella (ad esempio `datasets_all_NF`).
3. Inserisci i file di classificazione totale degli eventi in una sottocartella ad hoc, es. `datasets_all_NF/total_wp`.
4. Compila il codice (es. tramite Makefile o `g++` includendo le flag di ROOT).
5. Lancia l'eseguibile passando il path della cartella madre:
   ```bash
   ./main datasets_all_NF/
   ```

### ⚙️ Dipendenze
* Compilatore C++ (`g++`)
* [CERN ROOT](https://root.cern/) (Framework per l'analisi dati)

---

## 🔮 Sviluppi Futuri
1. **Barre di errore**: Integrare il calcolo formale delle incertezze statistiche nei grafici dei rate.
2. **Interpretazione Fisica**: Estrarre distribuzioni più complesse per comprendere la "coda" anomala di carica.
3. **Gestione Output**: Rendere l'esportazione di testo e immagini ancora più modulare, instradando i grafici delle RUN modificate in sub-directory dedicate per non confonderle con l'analisi standard.
4. **Ottimizzazione del Merging**: Raffinare l'algoritmo di matching temporale WP/CD oltre alla RUN 4049, generalizzando il taglio energetico e spaziale.

---

**Autore:** Nicolò Favagrossa 🎓 *(Lavoro di Tesi di Laurea Triennale in Fisica - Università degli Studi di Milano)*

<br><br>

---
---

<a id="en"></a>
# 🌌 JUNO Cosmic Muon Analysis

**JUNO Cosmic Muon Analysis** is the final, optimized iteration of the data analysis tools developed for my Bachelor's Thesis in Physics (formerly *Tesi_V0* and *Tesi_V1*). This C++/ROOT application processes **multiple ROOT files simultaneously**, ensuring efficient data analysis and automatic generation of structured plots.

## 📖 Thesis Context

The **JUNO (Jiangmen Underground Neutrino Observatory)** experiment is a cutting-edge underground neutrino detector in China, primarily designed to resolve the Neutrino Mass Ordering. To achieve extremely precise neutrino signal measurements, background mitigation is strictly required. One of the most significant backgrounds comes from cosmogenic radioactive isotopes, which are generated when **cosmic muons** traverse the liquid scintillator. Therefore, accurately reconstructing and classifying muon tracks is vital for implementing effective veto strategies.

Currently, the detector is undergoing the **filling phase** of its Central Detector (CD) with liquid scintillator. During this phase, it is already possible to characterize the cosmic muon flux by exploiting the abundant Cherenkov light produced. 
This software project analyzes real data from the very first months of the filling phase. It computes event rates, spatial/angular tracking, energy distributions, evaluates muon multiplicity (single vs. bundle events), and compares classification efficiencies across different detector subsystems (e.g., Water Pool vs. Central Detector).

---

## 📄 Main Output Files

The software processes the input datasets and summarizes the outputs into text logs:

### 1. `output.txt`
Contains the core analytical results for each analyzed RUN:
* **Initialization Check**: Total loaded files, events per RUN, and warnings if the `MuonEvents` tree is missing or empty.
* **Timestamp & Uptime**: Start date and time (read from `BiPo_rate_byfile.txt`) and total RUN duration t in seconds.
* **Timing Analysis (<$\Delta t$>)**: Average time difference between consecutive events. If a negative time delta is detected (likely a timestamp sync error), the interval is skipped and logged to prevent statistical biases.
* **Muon Rate**: The event frequency (in Hz). It is computed in two ways: based on **unique events** (by `EventID`, treating bundles as a single trigger) and based on total recorded muons (factoring in bundle multiplicity).
* **Single vs Bundle Muons**: Total count and percentage of multiple-track events (more than one muon per `EventID`). *Note: Without a valid `TrackID`, bundle-related metrics are bypassed.*
* **Impact Parameter & Edge Cuts**: Calculation of the shortest distance between the detector's center and the extrapolated muon trajectory. Events running too close to the detector boundaries are counted as *edge events*, leading to the computation of a cleaner **Rate after edge cut**.

*(A similar statistical breakdown is printed for the modified RUNs that cross-analyze WP and CD data).*

### 2. `error&info.txt`
Diagnostic logging. Captures system warnings, negative $\Delta t$ occurrences, and missing file paths. Extremely useful for pipeline debugging.

### 3. `BiPo_rate_byfile.txt`
A supporting configuration file linking RUN names (starting from `RUN3793`) to their exact temporal start timestamps.

---

## 📊 Generated Plots

The program automatically produces root canvases and exports them as standard images in the `images/` directory:

* 📂 `Distance_plot/`: Histograms of the path lengths traveled by muons through the detector volume.
* 📂 `PeSum_plot/`: Event-by-event charge distribution (Photoelectrons Sum).
* 📂 `PeSum_single_plot/` & `PeSum_bundle_plot/`: PeSum differentiated by single muons and muon bundles.
* 📂 `Polar_Angle_plot/` & `Azimuthal_Angle_plot/`: Track direction distributions (cosine of polar angle and azimuthal angle).
* 📂 `PeSum_vs_Polar_Angle_plot/` & `PeSum_vs_Azimuthal_Angle_plot/`: 2D maps correlating detected charge (energy proxy) against the incoming track angles.
* 📂 `Polar_vs_azimuth_plot/`: 2D Zenith-Azimuth directional heatmaps.
* 📂 `Path_Distance_plot/`: Histograms of the shortest distance from the track line to the detector origin.
* 📂 `TrackID_Distribution_plot/`: Number of tracks per event ID.
* 📂 `Muon_Rate_plot/`: Fluctuation of muon rates binned over 300-second windows.
* 📈 `Muon_Rate_vs_Run.png` & `Muon_Rate_edge_cut_vs_Run.png`: Overall mean rate across successive runs, before and after applying the boundary veto cuts.

---

## 🔬 Technical Notes: WP and CD Data Integration

A heavy focus of this software version is data matching between the standard *WpMuonClassifyRecTool* output and the raw totality of events captured under a different physical threshold.
* **The `totalEvents` structure**: Specifically designed to parse completely unfiltered root files for cross-referencing against the classified data.
* **Charge Value Correction**: Since the original WP classification provided unreliable charge measurements, a dynamic correction function was built. For time-matched events (identical timestamp and `EventID`), redundant overlaps are pruned, and the software directly injects the correct charge metric from the parallel dataset into our primary `muone` structure.
* **Physical Interpretations**: By inherently stripping out bundled events during the merging phase, we effectively analyze all matched data as single muons. This causes a physiological drop in the absolute rate. The corrected charge distribution highlights significantly higher Photoelectron values than previous iterations and presents an extremely long asymmetrical tail—a feature that demands future physical investigation. Currently, this hybrid pipeline is thoroughly validated on `RUN4049`, though the codebase inherently scales to arbitrary multi-run lists.

---

## 🚀 How to Run

1. Clone the repository.
2. Place your raw `.root` datasets into a designated folder (e.g., `datasets_all_NF`).
3. Place the total event classification files into a dedicated subfolder, e.g., `datasets_all_NF/total_wp`.
4. Compile the application (ensure ROOT flags are properly linked in your Makefile or via `g++`).
5. Execute the binary, passing the root dataset folder as an argument:
   ```bash
   ./main datasets_all_NF/
   ```

### ⚙️ Dependencies
* C++ Compiler (`g++`)
* [CERN ROOT](https://root.cern/) (Data analysis framework)

---

## 🔮 Future Updates
1. **Error Bars**: Implement strict statistical uncertainties into rate trend plots.
2. **Physics Interpretation**: Perform deeper studies on the high-charge distribution tails uncovered during WP/CD event matching.
3. **Output Management**: Decouple the modified/merged RUN plot generation into separated sub-directories to prevent cluttering standard analysis output.
4. **Merging Optimization**: Expand and refine the exact spatial and time-coincidence algorithms beyond RUN 4049 for the entire dataset.

---

**Author:** Nicolò Favagrossa 🎓 *(BSc Physics Thesis Project - University of Milan)*
