#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <signal.h>

// Configurações padrão do Pomodoro
#define TRABALHO_PADRAO 25  // minutos
#define PAUSA_CURTA_PADRAO 5  // minutos
#define PAUSA_LONGA_PADRAO 15  // minutos
#define CICLOS_PADRAO 4  // número de ciclos antes de uma pausa longa

// Códigos ANSI para cores
#define COR_RESET "\033[0m"
#define COR_VERDE "\033[1;32m"
#define COR_AMARELO "\033[1;33m"
#define COR_AZUL "\033[1;34m"
#define COR_VERMELHO "\033[1;31m"

// Flag para controlar o encerramento com Ctrl+C
volatile sig_atomic_t flag_encerrar = 0;

// Handler para sinal de interrupção (Ctrl+C)
void handle_signal(int sig) {
    flag_encerrar = 1;
}

// Função para limpar a tela
void limparTela() {
    printf("\033[H\033[J");  // Códigos ANSI para limpar tela
}

// Função para verificar se uma tecla foi pressionada
int kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;
    
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    
    ch = getchar();
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    
    if(ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    
    return 0;
}

// Função para ler um caractere sem esperar Enter
int getch() {
    int ch;
    struct termios oldt, newt;
    
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    
    ch = getchar();
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    
    return ch;
}

// Função para tocar um som de alerta (usando o comando 'play' do pacote 'sox')
void tocarAlerta() {
    // Verificar se o comando 'play' existe
    if (system("which play > /dev/null 2>&1") == 0) {
        // Gerar um beep usando o comando play
        system("play -q -n synth 0.3 sine 750 && sleep 0.2 && play -q -n synth 0.3 sine 750 &");
    } else {
        // Fallback: Usar o método de beep ASCII (menos eficaz)
        printf("\a");
        usleep(200000);
        printf("\a");
    }
}

// Função para exibir notificação do sistema (requer notify-send)
void exibirNotificacao(const char *titulo, const char *mensagem) {
    char comando[256];
    snprintf(comando, sizeof(comando), "notify-send \"%s\" \"%s\"", titulo, mensagem);
    system(comando);
}

// Função para formatar o tempo em minutos e segundos
void formatarTempo(int segundos, char *resultado) {
    int minutos = segundos / 60;
    int segs = segundos % 60;
    sprintf(resultado, "%02d:%02d", minutos, segs);
}

// Função para contar regressivamente
void contagemRegressiva(int segundos, const char *fase) {
    char tempoFormatado[6];  // Para armazenar o tempo no formato MM:SS
    time_t inicio = time(NULL);
    time_t atual;
    int segundosRestantes;
    int percentual;
    const char *cor;
    
    // Definir cor baseada na fase
    if (strstr(fase, "Trabalho") != NULL) {
        cor = COR_VERDE;
    } else if (strstr(fase, "Pausa Curta") != NULL) {
        cor = COR_AMARELO;
    } else {
        cor = COR_AZUL;
    }
    
    while (!flag_encerrar) {
        atual = time(NULL);
        segundosRestantes = segundos - (int)(atual - inicio);
        
        if (segundosRestantes <= 0) {
            break;
        }
        
        // Calcula a porcentagem concluída
        percentual = 100 - (segundosRestantes * 100 / segundos);
        
        // Formata o tempo restante
        formatarTempo(segundosRestantes, tempoFormatado);
        
        // Limpa a tela e exibe as informações
        limparTela();
        printf("=== %sPOMODORO TIMER%s ===\n\n", cor, COR_RESET);
        printf("Fase atual: %s%s%s\n", cor, fase, COR_RESET);
        printf("Tempo restante: %s\n\n", tempoFormatado);
        
        // Exibe uma barra de progresso com cor
        printf("[");
        for (int i = 0; i < 20; i++) {
            if (i < percentual / 5) {
                printf("%s#%s", cor, COR_RESET);
            } else {
                printf(" ");
            }
        }
        printf("] %d%%\n\n", percentual);
        
        printf("Pressione '%sq%s' para sair ou '%sp%s' para pausar/continuar\n", 
               COR_VERMELHO, COR_RESET, COR_AMARELO, COR_RESET);
        
        // Verifica se alguma tecla foi pressionada
        if (kbhit()) {
            char tecla = getch();
            if (tecla == 'q' || tecla == 'Q') {
                printf("\nPomodoroTimer encerrado pelo usuário.\n");
                return;
            } else if (tecla == 'p' || tecla == 'P') {
                printf("\nTimer pausado. Pressione qualquer tecla para continuar...\n");
                getch();
                inicio = time(NULL) - (segundos - segundosRestantes);  // Ajusta o tempo de início
            }
        }
        
        usleep(500000);  // Atualiza a cada 0.5 segundo (500000 microssegundos)
    }
    
    if (flag_encerrar) {
        printf("\nPrograma interrompido pelo usuário.\n");
        return;
    }
    
    // Quando o tempo terminar
    limparTela();
    printf("=== %sPOMODORO TIMER%s ===\n\n", cor, COR_RESET);
    printf("Fase concluída: %s%s%s\n\n", cor, fase, COR_RESET);
    tocarAlerta();
    
    // Envia notificação do sistema
    exibirNotificacao("Pomodoro Timer", fase);
}

int main() {
    int tempoTrabalho = TRABALHO_PADRAO;
    int tempoPausaCurta = PAUSA_CURTA_PADRAO;
    int tempoPausaLonga = PAUSA_LONGA_PADRAO;
    int ciclos = CICLOS_PADRAO;
    int cicloAtual = 1;
    char opcao;
    
    // Configura o handler de sinal para Ctrl+C
    signal(SIGINT, handle_signal);
    
    // Verifica dependências
    printf("Verificando dependências...\n");
    if (system("which notify-send > /dev/null 2>&1") != 0) {
        printf("Aviso: 'notify-send' não encontrado. As notificações de desktop não funcionarão.\n");
        printf("Instale com: sudo apt-get install libnotify-bin\n\n");
    }
    
    if (system("which play > /dev/null 2>&1") != 0) {
        printf("Aviso: 'play' não encontrado. Os alertas sonoros serão limitados.\n");
        printf("Instale com: sudo apt-get install sox\n\n");
    }
    
    sleep(2);  // Pausa para mostrar as mensagens acima
    
    // Configuração inicial
    limparTela();
    printf("=== %sCONFIGURAÇÃO DO POMODORO%s ===\n\n", COR_AZUL, COR_RESET);
    printf("Valores padrão:\n");
    printf("- Tempo de trabalho: %d minutos\n", tempoTrabalho);
    printf("- Tempo de pausa curta: %d minutos\n", tempoPausaCurta);
    printf("- Tempo de pausa longa: %d minutos\n", tempoPausaLonga);
    printf("- Ciclos antes da pausa longa: %d\n\n", ciclos);
    
    printf("Deseja personalizar as configurações? (s/n): ");
    opcao = getchar();
    
    if (opcao == 's' || opcao == 'S') {
        getchar();  // Limpa o buffer do Enter
        limparTela();
        printf("=== %sPERSONALIZAÇÃO DO POMODORO%s ===\n\n", COR_AZUL, COR_RESET);
        
        printf("Tempo de trabalho (minutos): ");
        scanf("%d", &tempoTrabalho);
        
        printf("Tempo de pausa curta (minutos): ");
        scanf("%d", &tempoPausaCurta);
        
        printf("Tempo de pausa longa (minutos): ");
        scanf("%d", &tempoPausaLonga);
        
        printf("Ciclos antes da pausa longa: ");
        scanf("%d", &ciclos);
        
        getchar();  // Limpa o buffer do Enter
    } else {
        getchar();  // Limpa o buffer do Enter
    }
    
    // Converte minutos para segundos
    tempoTrabalho *= 60;
    tempoPausaCurta *= 60;
    tempoPausaLonga *= 60;
    
    // Loop principal do Pomodoro
    while (!flag_encerrar) {
        // Fase de trabalho
        char faseTrabalho[50];
        snprintf(faseTrabalho, sizeof(faseTrabalho), "Trabalho (Ciclo %d/%d)", cicloAtual, ciclos);
        contagemRegressiva(tempoTrabalho, faseTrabalho);
        
        if (flag_encerrar) break;
        
        // Verifica se é hora de uma pausa longa ou curta
        if (cicloAtual % ciclos == 0) {
            contagemRegressiva(tempoPausaLonga, "Pausa Longa");
        } else {
            contagemRegressiva(tempoPausaCurta, "Pausa Curta");
        }
        
        if (flag_encerrar) break;
        
        // Incrementa o ciclo
        cicloAtual++;
        if (cicloAtual > ciclos) {
            cicloAtual = 1;
        }
        
        // Pergunta se quer continuar após cada ciclo completo
        limparTela();
        printf("Ciclo concluído! Deseja continuar? (s/n): ");
        opcao = getch();
        
        if (opcao != 's' && opcao != 'S') {
            break;
        }
    }
    
    printf("\nObrigado por usar o Pomodoro Timer!\n");
    printf("Pressione qualquer tecla para sair...");
    getch();
    
    return 0;
}