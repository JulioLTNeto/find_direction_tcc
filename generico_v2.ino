#include <SoftwareSerial.h>
#include <EEPROM.h>

struct t_sensores {
    int valor_sensor;
    int setor;
    char direcao;
};

typedef struct t_sensores TSensores;

SoftwareSerial bluetooth(2, 3); // Pinos RX, TX do módulo HC-05
int sensor_leste = A5;
int sensor_norte = A4;
int sensor_oeste = A3;
int sensor_sul = A2;

t_sensores valores[4];

unsigned int valor_sensor_leste = 0;
unsigned int valor_sensor_norte = 0;
unsigned int valor_sensor_oeste = 0;
unsigned int valor_sensor_sul = 0;

unsigned long int tempo_atual = 0;
unsigned long int tempo_inicial = 0;
unsigned long int tempo_decorrido = 0;

float valor_medio_leste = 0;
float valor_medio_norte = 0;
float valor_medio_oeste = 0;
float valor_medio_sul = 0;

float desvio_padrao_leste = 0;
float desvio_padrao_norte = 0;
float desvio_padrao_oeste = 0;
float desvio_padrao_sul = 0;

long unsigned int somatorio_leste = 0;
long unsigned int somatorio_norte = 0;
long unsigned int somatorio_oeste = 0;
long unsigned int somatorio_sul = 0;

int quadrante = 0;
int contador = 0;
int endereco = 0;

int contagem_direcao = 0;

int sensibilidade = 10;
int esta_ligado = 1; 
int mostrar_sensor = 0;

void setup() {
    Serial.begin(9600);
    bluetooth.begin(9600);
    pinMode(sensor_leste, INPUT);
    pinMode(sensor_norte, INPUT);
    pinMode(sensor_oeste, INPUT);
    pinMode(sensor_sul, INPUT);

    sensibilidade = EEPROM.read(endereco); // Lê o valor da EEPROM
    Serial.print("Sensibilidade: ");
    Serial.println(sensibilidade);

    tempo_inicial = millis();
}

// Função de troca
void swap(TSensores *a, TSensores *b) {
    TSensores temp = *a;
    *a = *b;
    *b = temp;
}

// Função de partição
int partition(TSensores arr[], int low, int high) {
    int pivot = arr[high].valor_sensor; // escolhe o último elemento como pivô
    int i = (low - 1); // índice do menor elemento

    for (int j = low; j <= high - 1; j++) {
        // Se o elemento atual é menor ou igual ao pivô
        if (arr[j].valor_sensor >= pivot) {
            i++; // incrementa o índice do menor elemento
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

// Função Quick Sort
void quickSort(TSensores arr[], int low, int high) {
    if (low < high) {
        // Encontra o índice do pivô, onde o arr[pi] está no lugar correto
        int pi = partition(arr, low, high);

        // Ordena os elementos separadamente antes e depois do pivô
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

// Função para imprimir o array de sensores
void printSensores(TSensores arr[], int size) {
    for (int i = 0; i < size; i++) {
        Serial.print("Valor: ");
        Serial.print(arr[i].valor_sensor);
        Serial.print(", Direcao: ");
        Serial.println(arr[i].direcao);
    }
    Serial.println();
}

void verificar_posicao(){
  valores[0].direcao = 'L';
  valores[0].setor = 1;
  valores[0].valor_sensor = abs(valor_sensor_leste-valor_medio_leste);
  valores[1].direcao = 'N';
  valores[1].setor = 0;
  valores[1].valor_sensor = abs(valor_sensor_norte-valor_medio_norte);
  valores[2].direcao = 'O';
  valores[2].setor = 1;
  valores[2].valor_sensor = abs(valor_sensor_oeste-valor_medio_oeste);
  valores[3].direcao = 'S';
  valores[3].setor = 0;
  valores[3].valor_sensor = abs(valor_sensor_sul-valor_medio_sul);
    
  int n = sizeof(valores) / sizeof(valores[0]);

  quickSort(valores, 0, n - 1);

  if(contador > 3000 and ((valor_sensor_leste > valor_medio_leste+sensibilidade) || (valor_sensor_norte > valor_medio_norte+sensibilidade) || (valor_sensor_oeste > valor_medio_oeste+sensibilidade) || (valor_sensor_sul > valor_medio_sul+sensibilidade))){
    
    contagem_direcao++;
    
    Serial.print("Sensor Leste: ");
    Serial.print(valor_sensor_leste);
    Serial.print(" | Media Leste: ");
    Serial.println(valor_medio_leste);
    Serial.print("Sensor Norte: ");
    Serial.print(valor_sensor_norte);
    Serial.print(" | Media Norte: ");
    Serial.println(valor_medio_norte);
    Serial.print("Sensor Oeste: ");
    Serial.print(valor_sensor_oeste);
    Serial.print(" | Media Oeste: ");
    Serial.println(valor_medio_oeste);
    Serial.print("Sensor Sul: ");
    Serial.print(valor_sensor_sul);
    Serial.print(" | Media Sul: ");
    Serial.println(valor_medio_sul);

    int a = 0;
    int b = 1;
   
    if(valores[0].setor == valores[1].setor){
      b = 2;
    }

    float resultado = float(valores[b].valor_sensor) / (valores[a].valor_sensor);

    //REGIAO NORTE-LESTE | LESTE-NORTE
    if(valores[a].direcao == 'N' && valores[b].direcao == 'L'){ //Mais norte do que para o leste, então temos um calculo para mais do que 45º
      if(resultado < 0.2){
        Serial.println("Norte");
      }else{
        Serial.println("Nordeste"); 
      }
      int angulo = 90-(45*resultado);
      bluetooth.println(angulo);
      Serial.print("Angulo: ");
      Serial.println(angulo);
      return;
    }else if(valores[a].direcao == 'L' && valores[b].direcao == 'N'){
      if(resultado < 0.2){
        Serial.println("Leste");
      }else{
        Serial.println("Nordeste"); 
      }
      int angulo = 45*resultado;
      bluetooth.println(angulo);
      Serial.print("Angulo: ");
      Serial.println(angulo);
      return;
      
    //REGIAO NORTE-OESTE | OESTE-NORTE
    }else if(valores[a].direcao == 'N' && valores[b].direcao == 'O'){
      if(resultado < 0.2){
        Serial.println("Norte");
      }else{
        Serial.println("Noroeste"); 
      }
      int angulo = 90+(45*resultado);
      bluetooth.println(angulo);
      Serial.print("Angulo: ");
      Serial.println(angulo);
      return;
    }else if(valores[a].direcao == 'O' && valores[b].direcao == 'N'){
      if(resultado < 0.2){
        Serial.println("Oeste");
      }else{
        Serial.println("Noroeste"); 
      }
      int angulo = 180-(45*resultado);
      bluetooth.println(angulo);
      Serial.print("Angulo: ");
      Serial.println(angulo);
      return;
      
    //REGIAO SUL-LESTE | LESTE-SUL
    }else if(valores[a].direcao == 'S' && valores[b].direcao == 'L'){ 
      if(resultado < 0.2){
        Serial.println("Sul");
      }else{
        Serial.println("Sudeste"); 
      }
      int angulo = 270+(45*resultado);
      bluetooth.println(angulo);
      Serial.print("Angulo: ");
      Serial.println(angulo);
      return;
    }else if(valores[a].direcao == 'L' && valores[b].direcao == 'S'){
      if(resultado < 0.2){
        Serial.println("Leste");
      }else{
        Serial.println("Sudeste"); 
      }
      int angulo = 360-(45*resultado);
      bluetooth.println(angulo);
      Serial.print("Angulo: ");
      Serial.println(angulo);
      return;
    
    //REGIAO SUL-OESTE | OESTE-SUL
    }else if(valores[a].direcao == 'S' && valores[b].direcao == 'O'){
      if(resultado < 0.2){
        Serial.println("Sul");
      }else{
        Serial.println("Sudoeste"); 
      }
      int angulo = 270-(45*resultado);
      bluetooth.println(angulo);
      Serial.print("Angulo: ");
      Serial.println(angulo);
      return;
    }else if(valores[a].direcao == 'O' && valores[b].direcao == 'S'){
      if(resultado < 0.2){
        Serial.println("Oeste");
      }else{
        Serial.println("Sudoeste"); 
      }
      int angulo = 180+(45*resultado);
      bluetooth.println(angulo);
      Serial.print("Angulo: ");
      Serial.println(angulo);
      return;
    }
    delay(1000); // Aguarda 1 segundo antes de enviar novamente
  } 
}

void loop() {
    valor_sensor_leste = analogRead(sensor_leste);
    valor_sensor_norte = analogRead(sensor_norte);
    valor_sensor_oeste = analogRead(sensor_oeste);
    valor_sensor_sul = analogRead(sensor_sul);

    tempo_atual = millis();

    if(tempo_atual >= tempo_inicial+5000){
      tempo_inicial = tempo_atual;
      contagem_direcao = 0;
    }

    if(contagem_direcao >= 5){
      contador = 0;
      somatorio_leste = 0;
      somatorio_norte = 0;
      somatorio_oeste = 0;
      somatorio_sul = 0;
    }

    if(contador < 4000){
      if(contador >= 1000 and contador < 2000){
        if(valor_sensor_leste < 0){
          valor_sensor_leste = valor_sensor_leste*(-1);
        }
        if(valor_sensor_norte < 0){
          valor_sensor_norte = valor_sensor_norte*(-1);
        }
        if(valor_sensor_oeste < 0){
          valor_sensor_oeste = valor_sensor_oeste*(-1);
        }
        if(valor_sensor_sul < 0){
          valor_sensor_sul = valor_sensor_sul*(-1);
        }
        
        somatorio_leste = somatorio_leste + valor_sensor_leste;
        somatorio_norte = somatorio_norte + valor_sensor_norte;
        somatorio_oeste = somatorio_oeste + valor_sensor_oeste;
        somatorio_sul = somatorio_sul + valor_sensor_sul;
      }
      if(contador == 2000){
        valor_medio_leste = somatorio_leste / 1000;
        valor_medio_norte = somatorio_norte / 1000;
        valor_medio_oeste = somatorio_oeste / 1000;
        valor_medio_sul = somatorio_sul / 1000;
        Serial.println("Medias calculadas: ");
        Serial.print("Media Leste: ");
        Serial.println(valor_medio_leste);
        Serial.print("Media Norte: ");
        Serial.println(valor_medio_norte);
        Serial.print("Media Oeste: ");
        Serial.println(valor_medio_oeste);
        Serial.print("Media Sul: ");
        Serial.println(valor_medio_sul);
        Serial.println("");
      }
      contador++;     
    }
    
    if (bluetooth.available() > 0) { // Verifica se há algum caractere disponível para leitura
    int receber = 0; // Variável para armazenar o número recebido
    char receberCaractere = bluetooth.read(); // Lê o próximo caractere disponível
    
    // Se o caractere recebido for um dígito numérico
    if (isdigit(receberCaractere)) {
      receber = receberCaractere - '0'; // Converte o caractere em um dígito
      
      // Se houver mais caracteres disponíveis e o próximo caractere também for um dígito, continue lendo
      while (bluetooth.available() > 0 && isdigit(bluetooth.peek())) {
        receberCaractere = bluetooth.read(); // Lê o próximo caractere
        receber = receber * 10 + (receberCaractere - '0'); // Atualiza o número inteiro
      }

      if(receber == 0){
        esta_ligado = 0;
        Serial.println("Desligado");
      }else if(receber == 1){
        esta_ligado = 1;
        Serial.println("Ligado");
      }else if(receber == 2){
        esta_ligado = 0;
        mostrar_sensor = 2;
        Serial.println("Mostrar Sensores Leste");
      }else if(receber == 3){
        esta_ligado = 0;
        mostrar_sensor = 1;
        Serial.println("Mostrar Sensores Norte");
      }else if(receber == 4){
        esta_ligado = 0;
        mostrar_sensor = 4;
        Serial.println("Mostrar Sensores Oeste");
      }else if(receber == 5){
        esta_ligado = 0;
        mostrar_sensor = 3;
        Serial.println("Mostrar Sensores Sul");
      }else if(receber == 6){
        esta_ligado = 0;
        mostrar_sensor = 0;
        Serial.println("Desligar Mostrar Sensores");
      }else if(receber == 7){
        contador = 0;
        somatorio_leste = 0;
        somatorio_norte = 0;
        somatorio_oeste = 0;
        somatorio_sul = 0;
        Serial.println("Calculando novamente as médias");
      }else if(receber == 9){
        bluetooth.println(sensibilidade);
        Serial.println("Requisitada a sensibilidade");
      }else if(receber >= 10){
        sensibilidade = receber;
        EEPROM.write(endereco, receber); // Escreve o valor na EEPROM
        Serial.print("Alterada a sensibilidade para: ");
        Serial.println(receber);
      }
      
    }
  }

    if(esta_ligado == 1){
      verificar_posicao();
    }

    if(mostrar_sensor == 1){
      Serial.print("Sensor Norte: ");
      Serial.print(valor_sensor_norte);
      Serial.print(" | Media Norte: ");
      Serial.println(valor_medio_norte);
    }else if(mostrar_sensor == 2){
      Serial.print("Sensor Leste: ");
      Serial.print(valor_sensor_leste);
      Serial.print(" | Media Leste: ");
      Serial.println(valor_medio_leste);
    }else if(mostrar_sensor == 3){
      Serial.print("Sensor Sul: ");
      Serial.print(valor_sensor_sul);
      Serial.print(" | Media Sul: ");
      Serial.println(valor_medio_sul);
    }else if(mostrar_sensor == 4){
      Serial.print("Sensor Oeste: ");
      Serial.print(valor_sensor_oeste);
      Serial.print(" | Media Oeste: ");
      Serial.println(valor_medio_oeste);
    }
    
}
