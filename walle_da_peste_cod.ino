// MOTORES DO ARDUINO
#define MOTOR_1_PWM_FRENTE 6
#define MOTOR_1_PWM_ATRAS 5

#define MOTOR_2_PWM_FRENTE 11
#define MOTOR_2_PWM_ATRAS 10

// SENSOR ULTRASONICO FRENTE
#define PINO_ECHO 8
#define PINO_TRIG  9

// VARIÁVEIS DE CONFIGURAÇÃO
#define VELOCIDADE_MAX 150
#define DISTANCIA_LIM 40
#define VELOCIDADE_DE_CURVA 150
#define VELOCIDADE_DE_DIFERENCIAL 80

int velocidade_atual = 0;

// FUNÇÃO BÁSICA DE SENSOR ULTRASONICO (AQUELE SENSORZINHO QUE PARECE UMA CAIXA DE SOM)
long ler_distancia_ultrasonica(int triggerPin, int echoPin){
  pinMode(triggerPin, OUTPUT);  
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  
  long duracao = pulseIn(echoPin, HIGH, 20000);
  if (duracao == 0) return 999 / 0.01723; // Evita leituras falsas de zero (fora de alcance)
  return duracao;
}

// COLOCANDO O MOTOR PARA ANDAR A FRENTE
void andar_a_frente()
{
  // Garante que os pinos de marcha ré estão totalmente desligados
  analogWrite(MOTOR_1_PWM_ATRAS, 0);
  analogWrite(MOTOR_2_PWM_ATRAS, 0);

  // Acelera gradualmente a partir da velocidade atual real
  if (velocidade_atual < VELOCIDADE_MAX) {
    for(int i = velocidade_atual; i <= VELOCIDADE_MAX; i++)
    {
      velocidade_atual = i;
      analogWrite(MOTOR_1_PWM_FRENTE, velocidade_atual);
      analogWrite(MOTOR_2_PWM_FRENTE, velocidade_atual);
      delay(5); 
    }
  }
}

void curva_a_esquerda()
{
  analogWrite(MOTOR_1_PWM_ATRAS, 0);
  analogWrite(MOTOR_2_PWM_ATRAS, 0);

  // Define os motores para a curva
  analogWrite(MOTOR_1_PWM_FRENTE, VELOCIDADE_DE_CURVA);
  analogWrite(MOTOR_2_PWM_FRENTE, VELOCIDADE_DE_DIFERENCIAL);
  
  // Sincroniza a variável com a maior velocidade aplicada nos motores
  velocidade_atual = VELOCIDADE_DE_CURVA; 
}

void curva_a_direita()
{
  analogWrite(MOTOR_1_PWM_ATRAS, 0);
  analogWrite(MOTOR_2_PWM_ATRAS, 0);

  // Define os motores para a curva
  analogWrite(MOTOR_1_PWM_FRENTE, VELOCIDADE_DE_DIFERENCIAL);
  analogWrite(MOTOR_2_PWM_FRENTE, VELOCIDADE_DE_CURVA);
  
  // Sincroniza a variável com a maior velocidade aplicada nos motores
  velocidade_atual = VELOCIDADE_DE_CURVA; 
}

void parar()
{
  // Vai diminuindo a velocidade até zero gradualmente
  for(int i = velocidade_atual; i >= 0; i -= 5)
  {
    velocidade_atual = i;
    analogWrite(MOTOR_1_PWM_FRENTE, velocidade_atual);
    analogWrite(MOTOR_2_PWM_FRENTE, velocidade_atual);
    delay(10); 
  }
  
  // Força ambos os pinos de ambos os motores a ficarem em ZERO (Freio)
  analogWrite(MOTOR_1_PWM_FRENTE, 0);
  analogWrite(MOTOR_1_PWM_ATRAS, 0);
  analogWrite(MOTOR_2_PWM_FRENTE, 0);
  analogWrite(MOTOR_2_PWM_ATRAS, 0);
  velocidade_atual = 0;
}

void setup() {
  Serial.begin(9600);
  
  // Configura os pinos dos motores como saídas
  pinMode(MOTOR_1_PWM_FRENTE, OUTPUT);
  pinMode(MOTOR_1_PWM_ATRAS, OUTPUT);
  pinMode(MOTOR_2_PWM_FRENTE, OUTPUT);
  pinMode(MOTOR_2_PWM_ATRAS, OUTPUT);
}

void loop() {
  // AQUI ESTÁ CALCULANDO A DISTANCIA DO OBJETO DETECTADO
  int distancia_frente = 0.01723 * ler_distancia_ultrasonica(PINO_TRIG, PINO_ECHO); 

  Serial.print("Distancia: ");
  Serial.println(distancia_frente);

  // SE A DISTANCIA DO OBJETO FOR MAIOR QUE A DISTANCIA LIMITE, VÁ A FRENTE.
  if(distancia_frente > DISTANCIA_LIM)
  {
    Serial.println("----ANDANDO----");
    andar_a_frente();
  }
  else // SE NÃO CAIR NO OUTRO IF, CAI NESSE AQUI
  {
    Serial.println("----PARANDO----");
    parar();
    delay(200); // Pequena pausa para estabilizar o robô físico antes de girar

    // Gira até encontrar o caminho livre
    int calc_distancia = 0.01723 * ler_distancia_ultrasonica(PINO_TRIG, PINO_ECHO);
    
    while (calc_distancia <= DISTANCIA_LIM)
    {
      Serial.println("----GIRANDO ESQUERDA----");
      curva_a_direita();
      delay(600); // Gira em pequenos passos para não passar do ponto ideal
      
      // Para o motor brevemente para fazer uma leitura limpa do sensor (sem ruído elétrico/vibração)
      analogWrite(MOTOR_1_PWM_FRENTE, 0);
      analogWrite(MOTOR_2_PWM_FRENTE, 0);
      delay(50); 
      
      calc_distancia = 0.01723 * ler_distancia_ultrasonica(PINO_TRIG, PINO_ECHO);
    }
    
    // Antes de voltar ao loop principal, zera a velocidade atual para que a aceleração
    // da função andar_a_frente() comece do zero corretamente
    velocidade_atual = 0;
  }

  delay(50);
}