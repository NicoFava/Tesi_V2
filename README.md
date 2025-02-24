# **Tesi_V2**  

📌 **Tesi_V2** è l'evoluzione dei progetti **Tesi_V0** e **Tesi_V1**, con miglioramenti nel codice per supportare l'analisi di **più file contemporaneamente** e la generazione automatica di immagini organizzate in apposite cartelle.  

## 📄 **output.txt**  
Il file **output.txt** contiene una sintesi dei risultati principali dell'analisi:  

### ✅ **Stampe iniziali**  
- Il programma stampa a video i **file caricati** e segnala eventuali errori se non viene trovato il **"TTree MuonseEents"** o se questo è vuoto.  
- Stampa il **numero totale di file caricati**.  
- Per ogni **run**, viene stampato il **numero di eventi** contenuti al suo interno.  

### 📊 **Dati di output**  
- **t**: Tempo totale della RUN in secondi.  
- **<delta_t>**: Tempo medio tra un evento e quello successivo in secondi.  
  - Se viene rilevato un **intervallo di tempo negativo tra due eventi**, verrà stampato un **messaggio di errore** a video e quell'intervallo sarà **ignorato** nel calcolo di <delta_t>.  
- **Rate**: Frequenza dei muoni rilevati.  
- **Numero di eventi univoci**: Conteggio degli **EventID** registrati per ogni run (utile per calcolare la percentuale di eventi bundle).  
- **Numero di eventi bundle**: Eventi con **più tracce** (ossia più di un muone per singolo EventID).  
- **Percentuale di eventi bundle**: Percentuale di eventi multipli rispetto al totale.  

🎨 Inoltre, il file di output include i **messaggi di creazione delle immagini** generate automaticamente durante l'analisi.  

---  

## 📊 **Grafici generati**  
Oltre all'analisi numerica, il programma produce **grafici** organizzati in diverse cartelle:  

### 1️⃣ **📂 Distance_plot**  
Istogrammi della **distanza percorsa** dal muone (o dal bundle di muoni) per ogni file analizzato.  

### 2️⃣ **📂 PeSum_plot**  
Istogrammi della **carica in fotoelettroni** prodotta per ogni evento.  

### 3️⃣ **📂 PeSum_vs_Angle_plot**  
Grafici **bidimensionali** che mostrano la correlazione tra **direzione** e **energia** dei muoni, per ogni run.  

### 4️⃣ **📂 Theta_plot**  
Istogrammi della **distribuzione angolare** dei muoni, mostrando il **coseno dell’angolo** della direzione di propagazione dei muoni rispetto all'asse \( z \).  

---  

## 🚀 **Come usare il programma**  
1. Inserire i file .root da analizzare in una cartella **<nome_cartella>**.  
2. Eseguire il programma:  
   

```bash  
   ./main <nome_cartella>  
```
