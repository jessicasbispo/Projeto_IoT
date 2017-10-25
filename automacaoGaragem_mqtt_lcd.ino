#include <Servo.h>
#define SERVO 9 // Porta Digital 9 PWM

#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

#include <LiquidCrystal.h> //Inclui a biblioteca do LCD

const int rs = 9, en = 8, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); //Configura os pinos do Arduino para se comunicar com o LCD

int temp; //Inicia uma variável inteira(temp), para escrever no LCD a contagem do tempo

//const int btnLed = 6;
//const int btnPortao = 7;
//const int ledGaragem = 5;

const int btnLed = 7;
const int btnPortao = 8;
const int ledGaragem = 6;

Servo s; // Variável Servo
int pos; // Posição Servo

// Atualizar ultimo valor para ID do seu Kit para evitar duplicatas
byte mac[] = { 0xDE, 0xED, 0xBA, 0xFE, 0xF1, 0x55 };
// Endereço do Cloud MQTT
char* server = "m12.cloudmqtt.com";
// Valor da porta do servidor MQTT
int port = 12397;
EthernetClient ethClient;

// Variável para termos um histórico de se o botão estava pressionado no
// loop() anterior ou não
int prevBtnStateLed = 0;

// Variável para determinar se o led deve ser aceso ou não
int ledOn = 0;

// FUNÇÃO que irá receber o retorno do servidor.
void whenMessageReceived(char* topic, byte* payload, unsigned int length) {
  // Converter pointer do tipo `byte` para typo `char`
  char* payloadAsChar = payload;

  // Converter em tipo String para conveniência
  String msg = String(payloadAsChar);
  Serial.print("Topic received: "); Serial.println(topic);

  // Dentro do whenMessageReceived (callback) da biblioteca MQTT,
  // devemos usar Serial.flush() para garantir que as mensagens serão enviadas
  Serial.flush();

  int msgComoNumero = msg.toInt();

  Serial.print("Numero lido: "); Serial.println(msgComoNumero);
  Serial.flush();

  switch (msgComoNumero) {
    //Abrir na Garagem
    case 1:
      lcd.home();
      lcd.print("Processando...");
      abrirPortao();
      acenderLed();
      lcd.setCursor(0, 0);
      lcd.print("Concluído");
      break;
    //Fechar na Garagem
    case 2:
      lcd.setCursor(0, 0);
      lcd.print("Processando...");
      fecharPortao();
      apagarLed();
      lcd.setCursor(0, 0);
      lcd.print("Concluído");
      break;
    case 3:
      lcd.setCursor(0, 0);
      lcd.print("Processando...");
      acenderLed();
      lcd.setCursor(0, 0);
      lcd.print("Concluído");
      break;
    case 4:
      lcd.setCursor(0, 0);
      lcd.print("Processando...");
      apagarLed();
      lcd.setCursor(0, 0);
      lcd.print("Concluído");
      break;
    default:
      Serial.println("Option not available");
      break;
  }
}


// Dados do MQTT Cloud
PubSubClient client(server, port, whenMessageReceived, ethClient);

void setup() {

  Serial.println("Pronto");
  pinMode(ledGaragem, OUTPUT);
  pinMode(btnLed, INPUT);
  pinMode(btnPortao, INPUT);
  s.attach(SERVO);
  s.write(0); // Inicia motor posição zero
  lcd.begin(16, 2); //Inicia o LCD com dimensões 16x2(Colunas x Linhas)
  lcd.clear();
  lcd.print("Ola!");

  Serial.begin(9600);
  Serial.println("Connecting...");

  while (!Serial) {}

  if (!Ethernet.begin(mac)) {
    Serial.println("DHCP Failed");
    lcd.setCursor(0, 0);
    lcd.print("DHCP failed");
  } else {
    Serial.println(Ethernet.localIP());
    lcd.home();
    lcd.print("Ethernet Connected");
  }

  // Faz a conexão no cloud com nome do dispositivo, usuário e senha respectivamente
  if (client.connect("arduino23", "codexpiot", "iot")) {
    Serial.println("Connected");
    lcd.setCursor(0, 0);
    lcd.print("MQTT Conencted");
    // Envia uma mensagem para o cloud no topic portao
    client.publish("iot", "Funciona!");
    Serial.println("iot sent");

    // Conecta no topic para receber mensagens
    client.subscribe("iot");
    Serial.println("conectado iot");
    lcd.setCursor(0, 0);
    lcd.print("Topic Conencted");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("MQTT failed");
    Serial.println("Failed to connect to MQTT server");
  }


}

void loop() {

  // A biblioteca PubSubClient precisa que este método seja chamado em cada iteração de `loop()`
  // para manter a conexão MQTT e processar mensagens recebidas (via a função callback)
  client.loop();

  //lcd.setCursor(13, 1); //Posiciona o cursor na décima quarta coluna(13) e na segunda linha(1) do LCD
  //lcd.print(temp); //Escreve o valor atual da variável de contagem no LCD
  //temp++; //Incrementa variável de contagem

  //if (temp == 600) //Se a variável temp chegar em 600(10 Minutos),...
  //{
  //  temp = 0; //...zera a variável de contagem
  //}

  mudarBotaoLed (btnLed);
  mudarBotaoPortao (btnPortao);

  //TESTE da posicao do servo motor
  Serial.print(s.read());
  Serial.print("\n");
}

void mudarBotaoLed (int pinLed) {
  int btnStateLed = digitalRead(pinLed);
  // Caso o botão esteja pressionado neste instante, e não estava pressionado anteriormente
  if (btnStateLed && !prevBtnStateLed) {
    // Invertemos o estado do LED
    if (ledOn) {
      ledOn = 0;
    } else {
      ledOn = 1;
    }
    digitalWrite(ledGaragem, ledOn);
  }

  // Armazenando o estado atual do botão para a próxima execução de loop()
  prevBtnStateLed = btnStateLed;
}

void mudarBotaoPortao (int pinPortao) {
  int btnStatePortao = digitalRead(pinPortao);
  // Caso o botão esteja pressionado neste instante, e não estava pressionado anteriormente
  //if (btnStatePortao && !prevBtnStatePortao) {
  if (btnStatePortao == HIGH) {
    if (s.read() == 0) {
      lcd.setCursor(0, 0);
      lcd.print("Processando...");
      abrirPortao();
      delay(500);
      digitalWrite(ledGaragem, HIGH);
      lcd.setCursor(0, 0);
      lcd.print("Concluído");
    } else {
      lcd.setCursor(0, 0);
      lcd.print("Processando...");
      fecharPortao();
      delay(500);
      digitalWrite(ledGaragem, LOW);
      lcd.setCursor(0, 0);
      lcd.print("Concluído");
    }
  }
}

void fecharPortao () {
  if (s.read() == 90) {
    for (pos = 90; pos >= 0; pos--) {
      s.write(pos);
      delay(10);
    }
  }
}

void abrirPortao () {
  if (s.read() == 0) {
    for (pos = 0; pos <= 90; pos++) {
      s.write(pos);
      delay(10);
    }
  }
}

void acenderLed () {
  digitalWrite (ledGaragem, HIGH);
  delay (500);
}

void apagarLed () {
  digitalWrite (ledGaragem, LOW);
  delay (500);
}

