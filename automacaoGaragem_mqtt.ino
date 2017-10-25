#include <Servo.h>
#define SERVO 9 // Porta Digital 9 PWM

#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

const int btnLed = 6;
const int btnPortao = 7;
const int ledGaragem = 5;
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
      abrirPortao();
      acenderLed();
      break;
    //Fechar na Garagem
    case 2:
      fecharPortao();
      apagarLed();
      break;
    case 3:
      acenderLed();
      break;
    case 4:
      apagarLed();
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

  Serial.begin(9600);
  Serial.println("Connecting...");

  while (!Serial) {}

  if (!Ethernet.begin(mac)) {
    Serial.println("DHCP Failed");
  } else {
    Serial.println(Ethernet.localIP());
  }

  // Faz a conexão no cloud com nome do dispositivo, usuário e senha respectivamente
  if (client.connect("arduino23", "codexpiot", "iot")) {
    Serial.println("Connected");
    // Envia uma mensagem para o cloud no topic portao
    client.publish("iot", "Funciona!");
    Serial.println("iot sent");

    // Conecta no topic para receber mensagens
    client.subscribe("iot");
    Serial.println("conectado iot");
  } else {
    Serial.println("Failed to connect to MQTT server");
  }


}

void loop() {

  // A biblioteca PubSubClient precisa que este método seja chamado em cada iteração de `loop()`
  // para manter a conexão MQTT e processar mensagens recebidas (via a função callback)
  client.loop();

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
      abrirPortao();
      delay(500);
      digitalWrite(ledGaragem, HIGH);
    } else {
      fecharPortao();
      delay(500);
      digitalWrite(ledGaragem, LOW);
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

