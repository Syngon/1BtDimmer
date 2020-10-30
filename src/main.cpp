#include "mbed.h"

InterrCIMAtIn btt(p10);
DigitalOut led_vermelho(p11);
DigitalOut led_azivis(p12);
PwmOut intensidade(p13);

Timeout timeout;
Ticker ticker;

enum STATE{ BAIXO, CIMA, QUEDA, MIN, SUBIDA, MAX };

void Iniciar();
void Pressionarbtt();
void Soltarbtt();
void MudarEstado();
void PiscarLed();

int Estado_atual;
bool Estado_btt;
float Frequencia;
float x;
float y;

void MudarEstado(){
  timeout.attach(&PiscarLed, 0.1);
  if (Estado_btt){
    switch (Estado_atual){
      case CIMA:
        Estado_atual = SUBIDA;
        printf("Subida\n");
        break;
      case BAIXO:
        Estado_atual = QUEDA;
        printf("Queda\n");
        break;
      case QUEDA:
        if (intensidade == 0.0){
          Estado_atual = MIN;
          printf("Min\n");
        }
        else{
          intensidade -= 0.05;
          printf("Intensidade: %.2f\n", intensidade.read());
        }
        break;
      case SUBIDA:
        if (intensidade == 1.0){
          Estado_atual = MAX;
          printf("Max\n");
        }
        else{
          intensidade += 0.05;
          printf("Intensidade: %.2f\n", intensidade.read());
        }
        break;
      default:
        if (intensidade >= 1.0){
          Estado_atual = MAX;
          printf("Max\n");
        }
        else if (intensidade <= 0.0){
          Estado_atual = MIN;
          printf("Min\n");
        }
        break;
      }
  }
}
void Iniciar(){
  MudarLeds(0.0, 1.0);
  Estado_atual = CIMA;
  intensidade = 1.0;
  printf("Cima\n");
}
void MudarLeds(float x, float y){
  led_vermelho = x;
  led_azivis = y;
}

void Pressionarbtt(){
  printf("Apertar btt\n");
  Estado_btt = true;
  ticker.attach(&MudarEstado, 1.0);
}
void Soltarbtt(){
  printf("Soltar btt\n");
  Estado_btt = false;
  ticker.detach();
  if (Estado_atual == CIMA || Estado_atual == QUEDA || Estado_atual == MAX){
    if (intensidade > 0.0){
      MudarLeds(1.0, 0.0);
      Estado_atual = BAIXO;
      printf("Baixo\n");
    }
  }
  else if (Estado_atual == SUBIDA || Estado_atual == BAIXO || Estado_atual == MIN){
    if (intensidade < 1.0){
      MudarLeds(0.0, 1.0);
      Estado_atual = CIMA;
      printf("Cima\n");
    }
  }
}

void PiscarLed(){
  switch (Estado_atual){
    case QUEDA:
      Frequencia = 1;
      led_vermelho = !led_vermelho;
      break;
    case SUBIDA:
      Frequencia = 1;
      led_azivis = !led_azivis;
      break;
    case MIN:
      Frequencia = 0.1;
      led_vermelho = !led_vermelho;
      break;
    case MAX:
      Frequencia = 0.1;
      led_azivis = !led_azivis;
      break;
    default:
      break;
  }
  timeout.attach(&PiscarLed, Frequencia);
}

int main(){
  Iniciar();
  btt.Subida(&Pressionarbtt);
  btt.Queda(&Soltarbtt);
}