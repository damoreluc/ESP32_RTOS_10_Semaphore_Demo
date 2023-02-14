/*
 * FreeRTOS Esempio 10: Semaphore demo
 *
 * Passaggio di un parametro ad un task con segnalazione tramite semaforo
 * la setup viene arrestata fino a quando il task non ha ricevuto il parametro
 *
 * Nota: nel file soc.h sono definiti i riferimenti ai due core della ESP32:
 *   #define PRO_CPU_NUM (0)
 *   #define APP_CPU_NUM (1)
 *
 * Qui viene adoperata la APP_CPU
 *
 */

#include <Arduino.h>

// pin driver del Led
#define pinLed GPIO_NUM_23

// variabili globali
// dichiarazione di un semaforo (binario)
static SemaphoreHandle_t binSem = NULL;

// variabile globale condivisa
static int shared_var = 0;


//**********************************************************************
// Tasks

// lampeggio del led con periodo passato come parametro
void blinkLED(void *parameters)
{
  // copia il parametro in una variabile locale
  int num = *(int *)parameters;

  // rilascia il semaforo binario affinché la funzione setup possa terminare
  xSemaphoreGive(binSem);

  // stampa il valore del parametro
  Serial.print("Ricevuto: ");
  Serial.println(num);

  // configura il pin del led
  pinMode(pinLed, OUTPUT);

  // loop infinito per il lampeggio
  while (1)
  {
    digitalWrite(pinLed, HIGH);
    vTaskDelay(num / portTICK_PERIOD_MS);
    digitalWrite(pinLed, LOW);
    vTaskDelay(num / portTICK_PERIOD_MS);
  }
}

//**********************************************************************
// Main (sul core 1, con priorità 1)

// configurazione del sistema
void setup()
{
 long int delay_arg;

  // Configurazione della seriale
  Serial.begin(115200);

  // breve pausa
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("FreeRTOS semaphore demo");
  Serial.println("Inserire il numero di millisecondi per il ritardo");

  // attende un input dalla seriale
  while( Serial.available() <= 0);

  // lettura del valore intero:
  delay_arg = Serial.parseInt();
  Serial.print("Inviato: ");
  Serial.println(delay_arg);

  // creazione del semaforo binario prima di avviare il task
  binSem = xSemaphoreCreateBinary();

  // creazione e avvio del Task1
  xTaskCreatePinnedToCore(
      blinkLED,           // funzione da richiamare nel task
      "Led Task",         // nome del task (etichetta utile per debug)
      1500,               // dimensione in byte dello stack per le variabili locali del task (minimo 768 byte)
      (void*)&delay_arg,  // puntatore agli eventuali parametri da passare al task
      1,                  // priorità del task
      NULL,               // eventuale task handle per gestire il task da un altro task
      APP_CPU_NUM         // core su cui far girare il task
  );

  // resta in attesa che il semaforo binario venga restituito
  xSemaphoreTake(binSem, portMAX_DELAY);

  // segnalazione che il dato è stato inviato al task 1
  Serial.println("Valore inviato");

}

void loop()
{
  // put your main code here, to run repeatedly:
}