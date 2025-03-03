# **Tesi_V2**  

:pushpin: **Tesi_V2** è l'evoluzione dei progetti **Tesi_V0** e **Tesi_V1**, con miglioramenti nel codice per supportare l'analisi di **più file contemporaneamente** e la generazione automatica di immagini organizzate in apposite cartelle.  

---  

## :page_facing_up: **output.txt**  
Il file **output.txt** contiene una sintesi dei risultati principali dell'analisi:  

### :white_check_mark: **Stampe iniziali**  
- Il programma stampa a video i **file caricati** e segnala eventuali errori se non viene trovato il **"TTree MuonsEvents"** o se questo è vuoto.  
- Stampa il **numero totale di file caricati**.  
- Per ogni **run**, viene stampato il **numero di eventi** contenuti al suo interno.  

### :1234: **Dati di output**  
- **Data e ora di inizio della RUN**: Informazioni sulla data e l'ora di inizio della RUN, caricate dal file `run_info.txt`.
- **t**: Tempo totale della RUN in secondi.  
- **<delta_t>**: Tempo medio tra un evento e quello successivo in secondi.  
  - Se viene rilevato un **intervallo di tempo negativo tra due eventi**, verrà stampato un **messaggio di errore** a video e quell'intervallo sarà **ignorato** nel calcolo di <delta_t>.  
- **Rate**: Frequenza dei muoni rilevati.  
  - Il rate viene calcolato sia contando gli eventi bundle una singola volta (eventi univocamente determinati dall'**EventID**), sia contando la totalità dei muoni (tenendo conto della **moltiplicità** dei muoni bundle).  
- **Numero di eventi univoci**: Conteggio degli **EventID** registrati per ogni run (utile per calcolare la percentuale di eventi bundle).  
- **Numero di eventi bundle**: Eventi con **più tracce** (ossia più di un muone per singolo EventID).
- **Percentuale di eventi bundle**: Percentuale di eventi multipli rispetto al totale.  
  - In caso di **TrackID inesistente** tutte le quantità legate ai muoni bundle non verranno calcolate o rappresentate in un grafico. 
- **Distanza massima del tracciato del muone rispetto all'origine del rivelatore**: viene calcolata la distanza massima tra l'origine e la **retta** che congiunge i punti di **entrata** e di **uscita** di ogni evento.
- **Eventi nel bordo**: Numero di eventi classificati come eventi **bordo**.   
- **Rate dopo il taglio**: Rate dei muoni dopo aver sottratto gli eventi di **bordo**.  

Inoltre, il file di output include i **messaggi di creazione delle immagini** generate automaticamente durante l'analisi.  

---

## :page_facing_up: **run_info.txt**  
Il file **run_info.txt** contiene le informazioni sulla data e l'ora di inizio di ogni RUN. Ogni riga del file ha il seguente formato:

```txt
<nome_run> <data> <ora>
```

---  

## :bar_chart: **Grafici generati**  
Oltre all'analisi numerica, il programma produce **grafici** organizzati in diverse cartelle:  

### :one: **:file_folder: Distance_plot**  
Istogrammi della **distanza percorsa** dal muone (o dal bundle di muoni) per ogni file analizzato.  

### :two: **:file_folder: PeSum_plot**  
Istogrammi della **carica in fotoelettroni** prodotta per ogni evento.  

### :three: **:file_folder: PeSum_vs_Polar_Angle_plot**  
Grafici **bidimensionali** che mostrano la correlazione tra **direzione** e **energia** dei muoni, per ogni run.  

### :four: **:file_folder: Polar_Angle_plot**  
Istogrammi della **distribuzione angolare** dei muoni, mostrando il **coseno dell’angolo polare**. 

### :five: **:file_folder: PeSum_single_plot**  
Istogrammi della **carica in fotoelettroni** per i muoni singoli.  

### :six: **:file_folder: PeSum_bundle_plot**  
Istogrammi della **carica in fotoelettroni** per i muoni bundle.  

### :seven: **:file_folder: Muon_Rate_plot**  
Grafici dei rate dei muoni in funzione del tempo calcolato su intervalli di tempo fissati (300 s). 

### :eight: **:file_folder: Polar_vs_azimuth_plot**  
Grafici **bidimensionali** che mostrano la correlazione tra l'angolo **polare** e l'angolo **azimutale**.

### :nine: **:file_folder: Azimuthal_Angle_plot**  
Istogramma della distribuzione dell'angolo **azimutale** della direzione dei muoni.

### :one::zero: **:file_folder: Path_Distance_plot**
Istogramma della distanza tra l'origine degli assi cartesiani (**centro del CD**) e la **retta** passante da **entry point** e **exit point**.

### :one::one: **:file_folder: TrackID_Distribution_plot**
Istogrammi della distribuzione del **TrackID** per ogni file analizzato.

### :one::two: **:eight_pointed_black_star: Muon_Rate_vs_Run**  
Grafico del **rate dei muoni** in funzione dell'indice della run.

### :one::three: **:eight_pointed_black_star: Muon_Rate_edge_cut_vs_Run**  
Grafico del **rate dei muoni** in funzione dell'indice della run togliendo gli eventi di **bordo**.

---  

## :rocket: **Come usare il programma**  
1. Inserire i file .root da analizzare in una cartella **<nome_cartella>**.  
2. Creare un file **run_info.txt** con le informazioni sulla data e l'ora di inizio di ogni RUN.  
3. Eseguire il programma:  
   
```bash  
   ./main <nome_cartella>  
```  
 
---  

## :gear: **Dipendenze**  
Assicurarsi di avere installate le seguenti librerie e strumenti necessari per eseguire il programma:  
- CERN ROOT  
- g++  
- CMake  

---  

### Futuri Update e idee

1. **Barre di errore**:
    - Aggiungere le barre di errore ai grafici dei rate.

2. **Evolvere l'analisi e intepretare i risultati ottenuti**:
    - Estrarre altre informazioni dai dati forniti e analizzare in maniera critica i risultati ottenuti.

---

## :pencil: **Autore**  
Codice sviluppato da **Nicolò Favagrossa**.
