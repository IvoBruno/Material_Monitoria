/* Esse código implementa uma array dinâmica de ponteiros para
estruturas (Person).
O incremento / decremento da array é de tamanho fixo (SLICE 3).
O código implementa um menu de opções e as seguintes operações:
cadastrar, alterar, deletar, exibir e exibir todos. */

#include <stdio.h>
#include <stdlib.h>

#define SLICE 3     //SLICE define o tamanho das fatias da array de estruturas

//Declarando a estrutura, membros e o nome alternativo
typedef struct Person {
    char name[51];
    char cpf[12];
    int age;
    char gender;
    char phone[11];
} Person;

/*>>> Variáveis globais que são necessárias ao funcionamento do programa  <<<*/
//Como essas variáveis são indispensáveis, não há motivo para não serem globais.

//POR UMA LIMITAÇÃO DA LINGUAGEM C, NÃO É POSSÍVEL INICIALIZAR UMA ARRAY GLOBAL
//USANDO ALOCAÇÃO DINÂMICA. PORTANTO, INICIALIZAMOS SOMENTE O PONTEIRO DA ARRAY.
//COMO NESTE CASO TEMOS UMA ARRAY DE PONTEIROS, ENTAO PRECISAMOS DECLARAR UM PONTEIRO
//PONTEIRO (COMO SABEMOS, O NOME DE UMA ARRAY APONTA PARA UM ENDERECO DE MEMORIA)
Person** crowd = NULL;     //Ponteiro  global para uma array dinâmica de ponteiros do tipo Person

//NÃO É POSSÍVEL DETERMINAR COM EXATIDÃO O TAMANHO DE UMA ARRAY ALOCADA DINAMICAMENTE
//A PARTIR DE UM PONTEIRO NO ESCOPO DE UMA FUNCÃO. PORTANTO, USAREI AS SEGUINTES VARIÁVEIS.
int crowd_size = 0;      //Contagem da quantidade de pessoas inseridas.
int crowd_max = 0;       //Controle da quantidade máxima de pessoas que podem ser armazenadas

/*>>> Protótipos das funções implementadas <<<*/
//Coleta os dados do novo cadastro e insere na array
void insert (void);

//Modifica um cadastro já existente
void modify (void);

//Apaga um cadastro existente
void erase (void);

//Exibe os dados de uma pessoa
void show (void);

//Exibe os dados de todas as pessoas
void show_all (void);

//Interno: Alocação dinâmica inicial da array de estruturas e das estruturas
int inicialize();

//Interno: Redimensiona a array se necessário
int redimension (int opt);

//Interno: recebe os dados do usuário e retorna um ponteiro para o local de memória
Person* receive_data ();

//Interno: Exibe os dados de uma pessoa específica na tela
void show_data (int p);

//Interno: Rearranja o vetor "removendo" um dos elementos
int rearrange (int p);

//Interno: liberando a memória do programa que por ventura estiver alocada antes de finalizar.
void finalize();

int main() {
    int option = -1; //Variável de controle do menu

    /* Inicilização da array de structs com alocação dinâmica.
     * A verificação de NULL é para garantir que é a primeira alocação.
     * A função inicialize() faz a alocação. Caso haja erro, o programa encerra */
    if (crowd == NULL)
        if (inicialize() == 1) {
            printf(">>>Problema na alocação de memória!<<<\nPrograma Finalizado!");
            return 0;
        }

    do {
        printf(">>> Menu de Opcoes: <<<");
        printf("\n1 - Inserir\n2 - Modificar\n3 - Apagar\n4 - Exibir\n5 - Exibir todos");
        printf("\n0 - Sair\n");

        scanf("%d", &option);

        switch (option) {
            case 1:
                insert();
                break;
            case 2:
                modify();
                break;
            case 3:
                erase();
                break;
            case 4:
                show();
                break;
            case 5:
                show_all();
                break;
            case 0:
                printf("Programa Finalizado!");
                break;
            default:
                printf("Opcao Invalida!");
        }
    }while (option!=0);
    finalize();
    return 0;
}

//Método para cadastrar uma nova pessoa. Utiliza o receive_data().
void insert (){
    //Checa se a array está inserindo o último elemento possível.
    //e tenta redimensionar aumentando a array, caso necessário.

    //9 posicoes 8 posicoes
    if(crowd_max-crowd_size > 1){
        printf("Insira as informacoes do novo cadastro:\n");
        crowd[crowd_size] = receive_data();
        crowd_size++;                           //Altera a quantidade pessoas
    } else {
        if (redimension(1) == 1) {
            printf(">>>Problema na alocação de memória!<<<");
            printf("Nao eh possivel cadastrar mais pessoas!");
        } else {
            printf("Insira as informacoes do novo cadastro:\n");
            crowd[crowd_size] = receive_data();
            crowd_size++;                       //Altera a quantidade pessoas
        }
    }
}

//Modifica um cadastro existente. Exibe o cadastro e exige confirmacao.
void modify (void){
    int p, opt;
    //Voce pode implementar a busca por CPF ou fazer o codigo reconhecer automaticamente
    printf("Qual cadastro voce deseja modificar?");
    scanf("%d", &p);

    printf("O cadastro a seguir sera modificado:\n");
    show_data(p-1);

    printf("Confirme a operacao com \"1\" ou cancele com \"0\": ");
    scanf("%d", &opt);

    if(opt == 1){
        crowd[p-1] = receive_data();
        printf("Operacao Concluida!\n");
    } else
        printf("Operacao cancelada!\n");
}

//Apaga um cadastro existente. Exibe o cadastro e exige confirmacao.
void erase (void){
    int p, opt;
    //Voce pode implementar a busca por CPF ou fazer o codigo reconhecer automaticamente
    printf("Qual cadastro voce deseja excluir?");
    scanf("%d", &p);

    printf("O cadastro a seguir sera excluido:\n");
    show_data(p-1);

    printf("Confirme a operacao com \"1\" ou cancele com \"0\": ");
    scanf("%d", &opt);

    if(opt == 1){
        if(rearrange(p) == 1)
            printf("Operacao nao realizada!\n");
        else {
            printf("Operacao Concluida!\n");
            if (crowd_max - crowd_size > 4)
                if (redimension(0) == 1)
                    printf(">>>Erro no controle de memória!<<<");
        }
    } else
        printf("Operacao cancelada!\n");
}

//Exibe os dados de uma pessoa. Utiliza o show_data().
void show (void){
    int p;
    //Voce pode implementar a busca por CPF ou fazer o codigo reconhecer automaticamente
    printf("Informe o numero do cadastro que voce deseja exibir: ");
    scanf("%d", &p);
    show_data(p-1);
}

//Exibe os dados de todas as pessoas. Utiliza o show_data().
void show_all (void){
    printf(">>> Exibindo todos os cadastros em ordem: <<<\n");
    for(int i=0;i<crowd_size;i++){
        show_data(i);
    }
}

//Interno: Alocação dinâmica inicial da array de estruturas
//Retorno determina o sucesso da operação
//IMPORTANTE LEMBRAR QUE ESSE MÉTODO PRECISA ALOCAR O ESPAÇO DO VETOR DE PONTEIROS
//TAMBEM PRECISA ALOCAR O ESPACO DE MEMORIA PARA QUAL OS PONTEIROS APONTAM (DAS ESTRUTURAS)
int inicialize(){
    //Alocação do ponteiro de ponteiros
    crowd = (Person**) malloc(sizeof(Person*)*SLICE);
    if(crowd == NULL)
        return 1;
    else{
        //Alocação das estruturas que os ponteiros apontam
        for(int i = 0; i<SLICE; i++) {
            crowd[i] = (Person*) malloc(sizeof(Person));
            if (crowd[i] == NULL)
                return 1;
        }
        crowd_max = SLICE;
        return 0;
    }
}

//Interno: Redimensiona a array dependendo do valor de opt
//Caso opt == 1: aumenta a array; caso opt == 0: diminui a array
//Retorno determina o sucesso da operação
//IMPORTANTE LEMBRAR QUE ESSE MÉTODO PRECISA REALOCAR O ESPAÇO DO VETOR DE PONTEIROS
//TAMBEM PRECISA REALOCAR O ESPACO DE MEMORIA PARA QUAL OS PONTEIROS APONTAM (DAS ESTRUTURAS)
//OU LIBERAR O ESPAÇO QUE NÃO SERÁ MAIS UTILIZADO
int redimension (int opt){

    if(opt == 1){
        //Realoca a array de ponteiros (aumentando o tamanho)
        crowd = (Person**) realloc(crowd, (crowd_max+SLICE)*sizeof(Person*));
        if(crowd == NULL)
            return 1;
        else {
            //aloca o espaço das estruturas para as quais os novos ponteiros apontam
            for(int i = (crowd_size+1); i<(crowd_max-1)+SLICE; i++) {
                crowd[i] = (Person*) malloc(sizeof(Person));
                if (crowd[i] == NULL) {
                    printf(">>>Erro no controle de memória!<<<");
                    return 1;
                }
            }
            crowd_max += SLICE;
            return 0;
        }
    }else {
        //desaloca o espaço das estruturas que serão removidas
        for(int i = (crowd_max-1); i>crowd_max-4; i--) {
            free(crowd[i]);
            crowd[i]=NULL;
            if (crowd[i] != NULL)
                return 1;
        }
        //Realoca a array de ponteiros (dimnuindo o tamanho)
        crowd = (Person**) realloc(crowd, sizeof(crowd_max-SLICE));
        if (crowd == NULL)
            return 1;
        crowd_max -= SLICE;
        return 0;
    }
}

//Interno: recebe os dados do usuário e retorna um ponteiro para o local de memória
Person* receive_data (){
    Person* new = (Person*) malloc(sizeof (Person));

    printf("Nome:");
    scanf("%s", new->name);

    printf("CPF:");
    scanf("%s", new->cpf);

    printf("Idade:");
    scanf("%d", &new->age);

    printf("Sexo:");
    scanf(" %c", &new->gender);

    printf("Telefone:");
    scanf("%s", new->phone);
    return(new);
}

//Interno: Exibe os dados de uma pessoa específica na tela dada a posição na array
void show_data (int p) {

    printf("Cadastro %d:\n", p + 1);
    printf("Nome: %s\n", crowd[p]->name);
    printf("CPF: %s\n", crowd[p]->cpf);
    printf("Idade: %d\n", crowd[p]->age);
    printf("Sexo: %c\n", crowd[p]->gender);
    printf("Tel: %s\n", crowd[p]->phone);
    printf("===========================\n");
}

//Interno: Reorganiza o vetor quando um registro é removido
int rearrange (int p){
    if (p == crowd_size){
        crowd_size--;
        return 0;
    }

    if (p < crowd_size){
        for(int i=p-1;i<crowd_size;i++) {
            crowd[i] = crowd[i+1];
        }
        crowd_size--;
        return 0;
    }else
        return 1;
}

//Interno: liberando a memória do programa que por ventura estiver alocada antes de finalizar.
void finalize(){
    for(int i=0; i<crowd_size; i++){
        free(crowd[i]);
        crowd[i]=NULL;
    }
    free(crowd);
    crowd = NULL;
}
