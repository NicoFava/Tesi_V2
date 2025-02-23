# **Tesi_V2**  

📌 **Tesi_V2** è l'evoluzione dei progetti **Tesi_V0** e **Tesi_V1**, con miglioramenti nel codice per supportare l'analisi di **più file contemporaneamente** e la generazione automatica di immagini organizzate in apposite cartelle.  

## 📄 **output.txt**  
Il file **output.txt** contiene una sintesi dei risultati principali dell'analisi:  

- **t**: Tempo medio tra un evento e quello successivo.
- **Rate**: Frequenza dei muoni rilevati.
-  **Numero di eventi univoci**: Conteggio degli **EventID** registrati per ogni run (utile per calcolare la percentuale di eventi bundle).  
- **Numero di eventi bundle**: Eventi con **più tracce** (ossia più di un muone per singolo EventID).  
- **Percentuale di eventi bundle**: Percentuale di eventi multipli rispetto al totale.  

---

## 📊 **Grafici generati**  
Oltre all'analisi numerica, il programma produce **grafici dettagliati** organizzati in diverse cartelle:  

### 1️⃣ **📂 Distance_plot**  
Istogrammi della **distanza percorsa** dal muone (o dal bundle di muoni) per ogni file analizzato.  

### 2️⃣ **📂 PeSum_plot**  
Istogrammi della **carica in fotoelettroni** prodotta per ogni evento.  

### 3️⃣ **📂 PeSum_vs_Angle_plot**  
Grafici **bidimensionali** che mostrano la correlazione tra **direzione** e **energia** dei muoni, per ogni run.  

### 4️⃣ **📂 Theta_plot**  
Istogrammi della **distribuzione angolare** dei muoni, mostrando il **coseno dell’angolo** rispetto all'asse \( z \).  

---

## 🚀 **Come usare il programma**  
1. Inserire i file .root da analizzare in una cartella **<nome_cartella>**.  
2. Eseguire il programma:  
   ```bash
   ./main <nome_cartella>
