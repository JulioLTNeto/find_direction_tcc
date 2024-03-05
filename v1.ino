#include <SoftwareSerial.h>
#include <EEPROM.h>

SoftwareSerial bluetooth(2, 3); // Pinos RX, TX do módulo HC-05
int sensor_leste = A5;
int sensor_norte = A4;
int sensor_oeste = A3;
int sensor_sul = A2;

unsigned int valor_sensor_leste = 0;
unsigned int valor_sensor_norte = 0;
unsigned int valor_sensor_oeste = 0;
unsigned int valor_sensor_sul = 0;

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
}

int angulos(float resultado) {
    if(quadrante == 0){
      float angulo = resultado * 90;
      Serial.println(angulo);
      return angulo;
    }else if(quadrante == 3){
      float angulo = (resultado * 90) + 270;  
      Serial.println(angulo);
      return angulo;
    }
    else if(quadrante == 1){
      float angulo = (resultado * 90) + 90;
      Serial.println(angulo);
      return angulo;
    }else if(quadrante == 2){
      float angulo = (resultado * 90) + 180;
      Serial.println(angulo);
      return angulo;
    }
}

void verificar_posicao(){
  float resultado_leste_oeste = float(abs(valor_sensor_leste-valor_medio_leste)) / (abs(valor_sensor_oeste-valor_medio_oeste));
  float resultado_oeste_leste = float(abs(valor_sensor_oeste-valor_medio_oeste)) / (abs(valor_sensor_leste-valor_medio_leste));
  // Serial.println(abs(valor_sensor_leste-valor_medio_leste));
  // Serial.println(abs(valor_sensor_oeste-valor_medio_oeste));
  // Serial.println(resultado_leste_oeste);
  // Serial.println(resultado_oeste_leste);
  // Serial.println("");
  if(contador > 3000 and ((valor_sensor_leste > valor_medio_leste+sensibilidade) || (valor_sensor_norte > valor_medio_norte+sensibilidade) || (valor_sensor_oeste > valor_medio_oeste+sensibilidade) || (valor_sensor_sul > valor_medio_sul+sensibilidade))){

    Serial.print("Sensor Leste: ");
    Serial.print(valor_sensor_leste);
    Serial.print(" | Media Leste: ");
    Serial.println(valor_medio_leste);
    Serial.print("Sensor Norte: ");
    Serial.print(valor_sensor_norte);
    Serial.print(" | Media norte: ");
    Serial.println(valor_medio_norte);
    Serial.print("Sensor Oeste: ");
    Serial.print(valor_sensor_oeste);
    Serial.print(" | Media Oeste: ");
    Serial.println(valor_medio_oeste);
    Serial.print("Sensor Sul: ");
    Serial.print(valor_sensor_sul);
    Serial.print(" | Media Sul: ");
    Serial.println(valor_medio_sul);
   
    if(abs(valor_sensor_norte-valor_medio_norte) > abs(valor_sensor_sul-valor_medio_sul)){
      if(abs(valor_sensor_leste-valor_medio_leste) > abs(valor_sensor_oeste-valor_medio_oeste)){
        quadrante = 0;
        //direção leste-norte.
        if(resultado_oeste_leste < 0.1){
          // bluetooth.println("Leste"); // Envia a string "Ola Mundo"via Bluetooth
          Serial.println("Leste");
        }else if(resultado_oeste_leste >= 0.1 and resultado_oeste_leste <= 0.675){
          //bluetooth.println("Nordeste"); // Envia a string "Ola Mundo"via Bluetooth
          Serial.println("Nordeste"); 
        }else{
          //bluetooth.println("Norte"); // Envia a string "Ola Mundo"via Bluetooth
          Serial.println("Norte");    
        }
        float retorno = angulos(resultado_oeste_leste);
        bluetooth.println(retorno);
      }else{
        //direção oeste-norte
        quadrante = 1;
        if(resultado_leste_oeste < 0.1){
         // bluetooth.println("Leste"); // Envia a string "Ola Mundo"via Bluetooth
          Serial.println("Oeste");
        }else if(resultado_leste_oeste >= 0.1 and resultado_leste_oeste <= 0.675){
        //  bluetooth.println("Nordeste"); // Envia a string "Ola Mundo"via Bluetooth
          Serial.println("Noroeste"); 
        }else{
        //  bluetooth.println("Norte"); // Envia a string "Ola Mundo"via Bluetooth
          Serial.println("Norte");    
        }
        float retorno = angulos(resultado_leste_oeste);
        bluetooth.println(retorno);
      }
      delay(1000); // Aguarda 1 segundo antes de enviar novamente
      }else{
        if(abs(valor_sensor_leste-valor_medio_leste) > abs(valor_sensor_oeste-valor_medio_leste)){
          //direção leste-sul.
          quadrante = 3;
          if(resultado_oeste_leste < 0.1){
           // bluetooth.println("Oeste"); // Envia a string "Ola Mundo"via Bluetooth
            Serial.println("Leste");
          }else if(resultado_oeste_leste >= 0.1 and resultado_oeste_leste <= 0.675){
           // bluetooth.println("Sudoeste"); // Envia a string "Ola Mundo"via Bluetooth
            Serial.println("Sudeste"); 
          }else{
          //  bluetooth.println("Sul"); // Envia a string "Ola Mundo"via Bluetooth
            Serial.println("Sul");    
          }
          float retorno = angulos(resultado_oeste_leste);
          bluetooth.println(retorno);
        }else{
          //direção oeste-sul
          quadrante = 2;
          if(resultado_leste_oeste < 0.1){
          //  bluetooth.println("Leste"); // Envia a string "Ola Mundo"via Bluetooth
            Serial.println("Oeste");
          }else if(resultado_leste_oeste >= 0.1 and resultado_leste_oeste <= 0.675){
          //  bluetooth.println("Sudoeste"); // Envia a string "Ola Mundo"via Bluetooth
            Serial.println("Sudoeste"); 
          }else{
          //  bluetooth.println("Sul"); // Envia a string "Ola Mundo"via Bluetooth
            Serial.println("Sul");    
          }
          float retorno = angulos(resultado_leste_oeste);
          bluetooth.println(retorno);
        }
      }
      delay(1000); // Aguarda 1 segundo antes de enviar novamente
    } 
}

void loop() {
    valor_sensor_leste = analogRead(sensor_leste);
    valor_sensor_norte = analogRead(sensor_norte);
    valor_sensor_oeste = analogRead(sensor_oeste);
    valor_sensor_sul = analogRead(sensor_sul);

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
        //Serial.println(somatorio_leste);
        //Serial.println(somatorio_norte);
        //Serial.println(somatorio_oeste);
        //Serial.println(somatorio_sul);
        valor_medio_leste = somatorio_leste / 1000;
        valor_medio_norte = somatorio_norte / 1000;
        valor_medio_oeste = somatorio_oeste / 1000;
        valor_medio_sul = somatorio_sul / 1000;
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
      Serial.print(" | Media norte: ");
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
