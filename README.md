# 🤖 Wall-e da Peste

O **Wall-e da Peste** é um projeto universitário de um robô autônomo móvel terrestre (AMR). O principal objetivo do robô é navegar por ambientes desconhecidos de forma independente, sendo capaz de identificar obstáculos em sua rota e tomar decisões em tempo real para desviá-los e continuar seu trajeto de forma segura.

---

## 🛠️ Arquitetura do Projeto e Hardware

O sistema foi projetado utilizando a plataforma **Arduino** para o processamento de sinais e controle de atuadores. A arquitetura física do robô divide-se em três pilares principais:

1. **Sensoriamento:** Responsável por coletar informações do ambiente externo. Atualmente, o protótipo conta com **1 Sensor Ultrassônico HC-SR04** posicionado na parte frontal. O projeto prevê a expansão para até **4 sensores de proximidade** para cobrir as laterais e retaguarda, mitigando pontos cegos.
2. **Atuação:** Composta por **2 Motores DC** controlados por uma ponte H (ou driver de motor equivalente) capaz de receber sinais PWM (*Pulse Width Modulation*) para controle de velocidade e direção.
3. **Controle Logístico:** A lógica programada gerencia o comportamento dinâmico através de loops de leitura, tomada de decisão e controle de aceleração suave para preservar os componentes mecânicos.

---

## 🧭 Conceitos de Robótica Aplicados

Para compreender o funcionamento do **Wall-e da Peste**, aplicamos alguns conceitos fundamentais de robótica e eletrônica:

### 1. Sensor Ultrassônico (Sonar de Proximidade)
O sensor funciona emitindo uma onda sonora de alta frequência (acima do espectro audível humano). Essa onda viaja pelo ar, bate em um obstáculo e retorna ao sensor. O código calcula o tempo que a onda levou para ir e voltar para determinar a distância exata até o objeto.

### 2. Controle por PWM (Pulse Width Modulation)
Os motores não são apenas ligados ou desligados. Utilizamos portas PWM do Arduino para controlar a **tensão média** enviada aos motores. Isso nos permite ajustar a velocidade real dos motores em uma escala de `0` (parado) a `255` (velocidade máxima).

### 3. Direção Diferencial
O robô não possui um sistema de direção com eixo móvel (como um carro convencional). As curvas são feitas aplicando velocidades diferentes entre as rodas esquerda e direita. 
* **Para ir reto:** Ambas as rodas giram na mesma velocidade para frente.
* **Para curvar:** Uma roda gira mais rápido que a outra (`VELOCIDADE_DE_CURVA` vs `VELOCIDADE_DE_DIFERENCIAL`), fazendo o chassi rotacionar sobre o próprio eixo estrutural.

### 4. Rampa de Aceleração e Frenagem (Suavização)
Para evitar que o robô derrape, empine ou danifique as engrenagens dos motores devido a mudanças bruscas de torque, o código implementa um controle incremental de velocidade através de loops `for`, subindo ou descendo a velocidade gradativamente.

---

## 💻 Explicação Detalhada das Funções

Abaixo estão descritas todas as funções que estruturam o firmware do robô:

### `long ler_distancia_ultrasonica(int triggerPin, int echoPin)`
Responsável por fazer a interface direta de baixo nível com o hardware do sensor.
* **Como funciona:** Envia um pulso elétrico rápido de 10 microssegundos no pino `TRIG` (Trigger) para disparar a onda sonora. Em seguida, monitora o pino `ECHO` usando a função `pulseIn()` para medir quantos microssegundos o som demorou para retornar.
* **Tratamento de Erros:** Caso o sensor não receba o eco (objeto longe demais ou erro de leitura), a função retorna um valor alto padrão (`999 / 0.01723`) para evitar que o robô interprete uma ausência de leitura como um obstáculo encostado nele.

### `void andar_a_frente()`
Gerencia o deslocamento retilíneo do robô.
* **Como funciona:** Garante que os pinos de marcha ré de ambos os motores estejam desligados (`0`). Em seguida, verifica a `velocidade_atual` do robô e, caso seja menor que a velocidade máxima definida (`VELOCIDADE_MAX`), incrementa a velocidade de 1 em 1 unidade a cada 5 milissegundos até atingir o ápice. Isso gera uma partida suave.

### `void curva_a_esquerda()` e `void curva_a_direita()`
Comandos de manobra direcional utilizando torque diferencial.
* Ambas as funções desativam os pinos de marcha ré.
* Para realizar a curva, uma das rodas recebe a velocidade padrão de manobra (`VELOCIDADE_DE_CURVA`), enquanto a roda oposta recebe uma velocidade reduzida (`VELOCIDADE_DE_DIFERENCIAL`), forçando o desvio físico do robô para o lado desejado.

### `void parar()`
Efetua a frenagem controlada e total do sistema móvel.
* **Como funciona:** Reduz a velocidade atual gradualmente de 5 em 5 unidades a cada 10 milissegundos até atingir zero. Uma vez zerada a velocidade dinâmica, a função força o nível lógico de **todos** os pinos de controle dos motores para `0`, atuando como um freio elétrico estático.

### `void setup()`
Função padrão de inicialização do Arduino.
* Configura a comunicação Serial a `9600 bps` para fins de depuração em laboratório.
* Define os 4 pinos de controle de potência dos motores (`MOTOR_1` e `MOTOR_2` para frente e trás) como pinos de saída (`OUTPUT`).

### `void loop()`
O cérebro operacional do robô que executa infinitamente em ciclo fechado.
1. **Fase de Leitura:** O robô converte o tempo retornado pela função do sensor em centímetros usando a constante matemática `0.01723` baseada na velocidade do som.
2. **Tomada de Decisão (Caminho Livre):** Se a distância lida for maior que o limite de segurança (`DISTANCIA_LIM`), o robô chama a função `andar_a_frente()`.
3. **Tomada de Decisão (Obstáculo Detectado):** Se houver algo no caminho:
   * Ele executa a rotina `parar()`.
   * Aguarda `200ms` para cessar qualquer energia cinética ou oscilação do chassi.
   * Entra em um laço de repetição (`while`) onde gira para a direita em pequenos passos de `600ms`.
   * A cada pequeno passo de giro, ele desliga momentaneamente os motores por `50ms` para realizar uma leitura ultrassônica limpa e precisa (sem interferência magnética dos motores ou vibração estrutural).
   * Assim que o sensor reportar que o caminho à frente está limpa (distância maior que o limite), o robô quebra o laço `while`, zera suas variáveis de controle e retorna a avançar em linha reta.

---

## 📈 Próximos Passos (Expansão do Projeto)
* **Integração de 4 Sensores:** Distribuição estratégica de sensores (Frontal, Esquerdo, Direito e Traseiro) para criar uma máquina com percepção espacial completa de 360 graus.
* **Lógica de Desvio Inteligente:** Alterar o laço `while` estático para tomar decisões baseadas em qual lado possui maior espaço livre (comparando a leitura do sensor esquerdo contra o direito).

---
*Projeto desenvolvido como parte dos requisitos avaliativos acadêmicos.*