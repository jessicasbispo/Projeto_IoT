#include <Servo.h>
#define SERVO 9 // Porta Digital 9 PWM

const int btnLed = 2;
const int btnPortao = 8;
const int ledGaragem = 12;
Servo s; // Variável Servo
int pos; // Posição Servo

// Declaramos as variáveis a seguir fora do loop() para que estejam disponíveis para
// todas as execuções de loop(), e seus valores sejam mantidos

// Variável para termos um histórico de se o botão estava pressionado no
// loop() anterior ou não
int prevBtnStateLed = 0;
int prevBtnStatePortao = 0;

// Variável para determinar se o led deve ser aceso ou não
int ledOn = 0;

void setup() {

  Serial.begin(9600);
  while (!Serial) {
  }

  Serial.println("Pronto");
  pinMode(ledGaragem, OUTPUT);
  pinMode(btnLed, INPUT);
  pinMode(btnPortao, INPUT);
  s.attach(SERVO);
  s.write(0); // Inicia motor posição zero

}

void loop() {
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
  }

  //mudarPortao(int estado);
  digitalWrite(ledGaragem, ledOn);

  // Armazenando o estado atual do botão para a próxima execução de loop()
  prevBtnStateLed = btnStateLed;
}

void mudarBotaoPortao (int pinPortao) {
  int btnStatePortao = digitalRead(pinPortao);
  // Caso o botão esteja pressionado neste instante, e não estava pressionado anteriormente
  //if (btnStatePortao && !prevBtnStatePortao) {
  if (btnStatePortao == HIGH) {
    if (s.read() == 0) {
      //mudarPortao (90);
      abrirPortao();
      delay(500);
      digitalWrite(ledGaragem, HIGH);
    } else {
      fecharPortao();
      delay(500);
      digitalWrite(ledGaragem, LOW);
    }

    // Armazenando o estado atual do botão para a próxima execução de loop()
    //prevBtnStatePortao = btnStatePortao;
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
