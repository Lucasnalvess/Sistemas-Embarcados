Atividade 3: Controle de Saída Digital com o ESP32 no Simulador Wokwi
Objetivo:
Desenvolver uma atividade prática para controlar 4 LEDs utilizando as GPIOs do ESP32
configuradas como saídas digitais.
Esta atividade será realizada no simulador Wokwi e deve utilizar a ESP-IDF como
framework de desenvolvimento.
Material Necessário:
● Conta no Wokwi (https://wokwi.com/)
● Conhecimento básico de ESP-IDF
● O ESP32 como microcontrolador
● 4 LEDs
● 4 resistores limitadores de corrente
Passos para a Atividade:
1. Diagrama em Blocos
Elabore o diagrama em blocos representando:
● EESP32-S3: microcontrolador responsável pelo acionamento das saídas digitais.
● LED1, LED2, LED3 e LED4: atuadores controlados pelo ESP32.
2. Esquemático:
Desenhe o circuito esquemático, incluindo:
● 4 LEDs conectados a GPIOs configuradas como saída digital, cada um em série
com seu resistor.
● Alimentação de 3,3 V fornecida pelo ESP32.
3. Configuração do Ambiente de Simulação:
Crie o circuito no Wokwi:
● Acesse o simulador Wokwi e crie um novo projeto.
● Adicione o ESP32-S3 ao projeto.
● Adicione 4 LEDs e seus resistores.
● Realize todas as conexões elétricas entre os LEDs e as GPIOs de saída..
4. Desenvolvimento do Código:
Implemente em C com ESP-IDF:
● Configure as GPIOs dos LEDs como saída digital.
● Crie funções para controlar o estado dos LEDs (ON e OFF).

● Implemente a seguinte lógica:
○ Fase 1: Contador Binário → Os LEDs devem exibir valores binários de 0 a 15
(0000 até 1111), mudando a cada 500 ms.
○ Fase 2: Sequência de Varredura → Os LEDs devem acender em sequência
(LED1 → LED2 → LED3 → LED4) e depois retornar (LED4 → LED3 → LED2
→ LED1).
Observações Importantes
● Todos os LEDs devem iniciar apagados.
● O tempo de atualização deve ser configurável via #define (por exemplo: #define
DELAY_MS 500).
● A lógica deve estar organizada em funções para facilitar manutenção do código..
