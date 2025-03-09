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
- **Data e ora di inizio della RUN**: Informazioni sulla data e l'ora di inizio della RUN, caricate dal file `BiPo_rate_byfile.txt`.
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

### :1234: **Dati di output per le RUN modificate**  
- **Data e ora di inizio della RUN**: Informazioni sulla data e l'ora di inizio della RUN, caricate dal file `BiPo_rate_byfile.txt`.
- **t**: Tempo totale della RUN in secondi.  
- **<delta_t>**: Tempo medio tra un evento e quello successivo in secondi.  
  - Se viene rilevato un **intervallo di tempo negativo tra due eventi**, verrà stampato un **messaggio di errore** a video e quell'intervallo sarà **ignorato** nel calcolo di <delta_t>.  
- **Rate**: Frequenza dei muoni rilevati.   
- **Numero di eventi**.
- **Distanza massima del tracciato del muone rispetto all'origine del rivelatore**: viene calcolata la distanza massima tra l'origine e la **retta** che congiunge i punti di **entrata** e di **uscita** di ogni evento.
- **Eventi nel bordo**: Numero di eventi classificati come eventi **bordo**.
- **Rate dopo il taglio**: Rate dei muoni dopo aver sottratto gli eventi di **bordo**.  

Alla fine dell'analisi viene riportato il **Tempo totale di esecuzione**: Il tempo totale impiegato per eseguire l'analisi.

---

## :page_facing_up: **error&info.txt**  
Il file **error&info.txt** contiene i messaggi di errore e le informazioni aggiuntive generate durante l'esecuzione del programma. Questo file è utile per diagnosticare eventuali problemi e per avere un registro dettagliato delle operazioni eseguite.

---

## :page_facing_up: **BiPo_rate_byfile.txt**
Il file **BiPo_rate_byfile.txt** contiene le informazioni sulla data e l'ora di inizio di ogni RUN. Le informazioni delle RUN partono dalla RUN3793.

---  

## :bar_chart: :chart_with_upwards_trend: **Grafici generati**  
Oltre all'analisi numerica, il programma produce **grafici** organizzati in diverse cartelle:  

### :bar_chart: **:file_folder: images/Distance_plot**  
Istogrammi della **distanza percorsa** dal muone (o dal bundle di muoni) per ogni file analizzato.  

### :bar_chart: **:file_folder: images/PeSum_plot**  
Istogrammi della **carica in fotoelettroni** prodotta per ogni evento.  

### :bar_chart: **:file_folder: images/PeSum_vs_Polar_Angle_plot**  
Grafici **bidimensionali** che mostrano la correlazione tra **direzione** e **energia** dei muoni, per ogni run.

### :bar_chart: **:file_folder: images/PeSum_vs_Azimuthal_Angle_plot**  
Grafici **bidimensionali** che mostrano la correlazione tra **direzione** e **energia** dei muoni, per ogni run. 

### :bar_chart: **:file_folder: images/Polar_Angle_plot**  
Istogrammi della **distribuzione angolare** dei muoni, mostrando il **coseno dell’angolo polare**. 

### :bar_chart: **:file_folder: images/PeSum_single_plot**  
Istogrammi della **carica in fotoelettroni** per i muoni singoli.  

### :bar_chart: **:file_folder: images/PeSum_bundle_plot**  
Istogrammi della **carica in fotoelettroni** per i muoni bundle.  

### :bar_chart: **:file_folder: images/Muon_Rate_plot**  
Grafici dei rate dei muoni in funzione del tempo calcolato su intervalli di tempo fissati (300 s). 

### :bar_chart: **:file_folder: images/Polar_vs_azimuth_plot**  
Grafici **bidimensionali** che mostrano la correlazione tra l'angolo **polare** e l'angolo **azimutale**.

### :bar_chart: **:file_folder: images/Azimuthal_Angle_plot**  
Istogramma della distribuzione dell'angolo **azimutale** della direzione dei muoni.

### :bar_chart: **:file_folder: images/Path_Distance_plot**
Istogramma della distanza tra l'origine degli assi cartesiani (**centro del CD**) e la **retta** passante da **entry point** e **exit point**.

### :bar_chart: **:file_folder: images/TrackID_Distribution_plot**
Istogrammi della distribuzione del **TrackID** per ogni file analizzato.

### :chart_with_upwards_trend: **:round_pushpin: images/Muon_Rate_vs_Run.png**  
Grafico del **rate dei muoni** in funzione dell'indice della run.

### :chart_with_upwards_trend: **:round_pushpin: images/Muon_Rate_edge_cut_vs_Run.png**  
Grafico del **rate dei muoni** in funzione dell'indice della run togliendo gli eventi di **bordo**.

---  

## :rocket: **Come usare il programma**  
1. Inserire i file .root da analizzare in una cartella **datasets_all_NF**.  
2. Inserire i file di classificazione totale degli eventi nella cartella **datasets_all_NF/total_wp**.
3. Eseguire il programma:  
   
```bash  
   ./main datasets_all_NF/ 
```  
 
---  

## :gear: **Dipendenze**  
Assicurarsi di avere installate le seguenti librerie e strumenti necessari per eseguire il programma:  
- CERN ROOT  
- g++   

---  

### Futuri Update e idee

1. **Barre di errore**:
    - Aggiungere le barre di errore ai grafici dei rate.

2. **Evolvere l'analisi e interpretare i risultati ottenuti**:
    - Estrarre altre informazioni dai dati forniti e analizzare in maniera critica i risultati ottenuti.

3. **Migliorare la gestione dei dati di output prodotti**:
    - Rendere i dati di output più chiari e ordinati. Lo stesso vale per le immagini generate visto che i grafici delle run modificate rientrano nelle stesse cartelle.

---

### Appunti
Ho creato una seconda struct **totalEvents** che serve ad immagazzinare le informazioni dei nuovi file forniti. Questi sono stati prodotti attraverso un software diverso e comprende tutti gli eventi (con una certa soglia) registrati. L'idea è di prendere la totalità degli eventi e confrontarli con gli eventi classificati dal software WpMuonClassifyRecTool. Dal momento che la carica di quest'ultima classificazione risulta poco attendibile ho creato un vector<muone> in cui inserisco la run modifica in cui ho tolto gli eventi ridondanti (stesso timestamp e EventID) e inserito la carica dei file nuovi.
Sono riuscito ad implementare una funzione che mi permette di modificare ogni evento che ho salvato come evento **muone** inserendo la carica corretta. In questo modo sono riuscito a stampare la distribuzione della carica dei soli eventi con tempi in comune. So che non è molto chiaro ma spero di essermi un minimo spiegato. Questo lavoro l'ho fatto solo per la run 4049 ma punto di farla per più run in quanto ho già preparato l'infrastruttura per farlo per un numero arbitrario di run. Ora sarebbe da capire se è tutto corretto.

La carica sembra molto alta rispetto a quella fornita dai file originali ma soprattutto presenta una lunghissima coda che porta la media ad essere molto alta anche se il picco è molto più basso. Ovviamente il rate dei muoni risulta più bassa in quanto (per forza di cose) ho tolto tutti gli eventi bundle. Quindi è come se analizzassi tutti gli eventi come eventi singoli. La frequenza risulta comunque troppo alta.

---
## :pencil: **Autore**  
Codice sviluppato da **Nicolò Favagrossa**.
