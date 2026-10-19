# Estruturas (structs) em C

Este documento apresenta a lógica de funcionamento, a organização de memória, a sintaxe em linguagem C e exemplos práticos comentados para o estudo de estruturas (`structs`), organizadas do funcionamento mais simples e elementar até as aplicações mais complexas: declaração básica e operador de acesso ponto, simplificação de tipos com `typedef`, passagem para funções (valor vs. referência e operador seta `->`), vetores de structs, estruturas aninhadas e composição, alocação dinâmica na heap, e alinhamento de memória e preenchimento (*padding*).

## Declaração básica e operador de acesso ponto

Uma `struct` (registro) é um tipo de dado composto e heterogêneo definido pelo programador. Enquanto um vetor tradicional agrupa múltiplos elementos de um mesmo tipo de forma contígua, a `struct` permite agrupar variáveis de tipos distintos (inteiros, pontos flutuantes, ponteiros, etc.) sob um único identificador lógico.

Características técnicas:
- Identificador de tipo: a declaração cria uma *tag* associada à palavra-chave `struct` (por exemplo, `struct Ponto`), exigindo a menção a `struct` em cada nova variável instanciada.
- Operador de acesso: os membros individuais da estrutura são acessados para leitura ou escrita por meio do operador ponto (`.`).
- Disposição na memória: quando declarada localmente dentro de uma função, a estrutura é alocada na pilha (*stack*), reservando espaço contíguo suficiente para abrigar todos os seus campos.
- Inicialização: pode ser realizada posicionalmente na ordem de declaração dos campos, de forma designada (especificando os nomes dos campos com a sintaxe introduzida no padrão C99) ou zerando todos os bytes com `{0}`.

```c
#include <stdio.h>

// Definição do molde da estrutura.
// Não aloca memória neste momento; apenas informa ao compilador
// quais campos e tipos numéricos compõem uma 'struct Ponto'.
struct Ponto {
  float x; // Coordenada no eixo horizontal
  float y; // Coordenada no eixo vertical
};

int main(void) {
  // Forma 1: Declaração simples com atribuição posterior via operador ponto ('.')
  struct Ponto p1;
  p1.x = 10.5f; // Atribui valor diretamente ao campo 'x'
  p1.y = 20.0f; // Atribui valor diretamente ao campo 'y'

  // Forma 2: Inicialização posicional direta
  // Os valores são atribuídos na mesma ordem em que os campos foram declarados na struct
  struct Ponto p2 = {3.0f, 4.0f}; // p2.x = 3.0, p2.y = 4.0

  // Forma 3: Inicialização designada (padrão C99)
  // Permite especificar explicitamente os campos por nome, em qualquer ordem
  struct Ponto p3 = {.y = 5.0f, .x = 1.0f};

  // Forma 4: Inicialização zerada
  // Define todos os bytes dos membros como zero (0.0f para float)
  struct Ponto p4 = {0};

  printf("p1: (%.1f, %.1f)\n", p1.x, p1.y);
  printf("p2: (%.1f, %.1f)\n", p2.x, p2.y);
  printf("p3: (%.1f, %.1f)\n", p3.x, p3.y);
  printf("p4: (%.1f, %.1f)\n", p4.x, p4.y);

  return 0;
}
```

## Simplificação de tipos com typedef

Em C padrão, o nome de uma estrutura requer sempre a palavra-chave `struct` antes de seu identificador (ex.: `struct Aluno a;`). O comando `typedef` cria um alias (sinônimo) para o tipo, permitindo utilizá-lo com a mesma simplicidade com que se utilizam tipos primitivos como `int` ou `float`.

Características técnicas:
- Legibilidade do código: elimina a repetição contínua de `struct` em declarações de variáveis, parâmetros de funções e valores de retorno.
- Preservação da *tag*: a convenção idiomática é declarar `typedef struct Aluno { ... } Aluno;`. Isso define a tag interna `struct Aluno` e o tipo simplificado `Aluno`.
- Sem custo de execução: o `typedef` é resolvido inteiramente durante o tempo de compilação, sem gerar qualquer sobrecusto de processamento ou consumo extra de memória.

```c
#include <stdio.h>

// Definição da estrutura associada diretamente a um sinônimo via 'typedef'.
// 'struct Aluno' é a tag interna e 'Aluno' torna-se o novo tipo disponível no programa.
typedef struct Aluno {
  int matricula; // Identificador numérico do estudante
  int turma;     // Código numérico da turma
  float cra;     // Coeficiente de Rendimento Acadêmico
} Aluno;

int main(void) {
  // Agora é possível declarar a variável sem a palavra-chave 'struct'
  Aluno a1;

  a1.matricula = 202401;
  a1.turma = 102;
  a1.cra = 8.75f;

  // Inicialização direta utilizando o novo tipo
  Aluno a2 = {202402, 103, 9.20f};

  printf("Aluno 1: Matrícula %d | Turma %d | CRA: %.2f\n",
         a1.matricula, a1.turma, a1.cra);
  printf("Aluno 2: Matrícula %d | Turma %d | CRA: %.2f\n",
         a2.matricula, a2.turma, a2.cra);

  return 0;
}
```

## Passagem para funções: Valor vs. Referência e operador seta (->)

Ao transferir uma estrutura para uma função, o comportamento padrão da linguagem C é a **passagem por valor**: o compilador aloca espaço na pilha de chamadas e duplica integralmente todos os bytes da estrutura. Para evitar o custo computacional dessa cópia e para permitir que a função modifique a estrutura original, utiliza-se a **passagem por referência** através de ponteiros.

Características técnicas:
- Custo de memória e cópia:
  - Por valor: duplicação de `sizeof(struct)` bytes a cada chamada de função.
  - Por referência: transferência apenas do endereço de memória (4 bytes em arquiteturas de 32 bits, 8 bytes em 64 bits).
- Operador seta (`->`): açúcar sintático para desreferenciar o ponteiro e acessar o campo em uma única operação. A expressão `ponteiro->campo` é estritamente equivalente a `(*ponteiro).campo`. Os parênteses são obrigatórios em `(*ponteiro).campo` porque o operador ponto (`.`) tem precedência maior do que o operador de desreferenciação (`*`).
- Qualificador `const`: quando uma função precisa apenas consultar os dados de uma struct sem alterá-los, a boa prática exige a passagem por ponteiro constante (`const Tipo *ptr`). Isso une a eficiência de não copiar a struct à garantia de que o compilador impedirá modificações acidentais.

```c
#include <stdio.h>

typedef struct {
  int matricula;
  int turma;
  float cra;
} Aluno;

// Passagem por referência constante (somente leitura):
// O ponteiro 'const Aluno *a' evita a cópia completa da struct (eficiência O(1)),
// enquanto o qualificador 'const' impede alterações acidentais nos dados originais.
void exibir_aluno(const Aluno *a) {
  // Validação defensiva de ponteiro nulo para prevenir falhas de segmentação (segfault)
  if (a == NULL) {
    return;
  }

  // O operador seta ('->') desreferencia o ponteiro 'a' e acessa o campo correspondente.
  // Equivale exatamente a escrever: (*a).matricula, (*a).turma e (*a).cra
  printf("Matrícula: %d | Turma: %d | CRA: %.2f\n",
         a->matricula, a->turma, a->cra);
}

// Passagem por referência mutável (leitura e escrita):
// O ponteiro 'Aluno *a' permite modificar o conteúdo da variável original alocada no chamador.
void atualizar_cra(Aluno *a, float novo_cra) {
  if (a == NULL) {
    return;
  }

  // Atualiza o valor do campo 'cra' diretamente na instância original
  a->cra = novo_cra;
}

int main(void) {
  Aluno a = {101, 1, 7.50f};

  printf("Estado inicial:\n");
  exibir_aluno(&a); // Passa o endereço de memória de 'a' usando o operador '&'

  // Modifica o campo 'cra' através da função receptora do ponteiro
  atualizar_cra(&a, 8.40f);

  printf("\nApós atualização do CRA:\n");
  exibir_aluno(&a);

  return 0;
}
```

## Vetores de structs (Arrays de registros)

Um vetor de structs organiza múltiplos registros de forma contígua em memória. Cada elemento do vetor é uma instância completa da estrutura, permitindo modelar tabelas numéricas, leituras de instrumentos, coleções de medidas e registros tabulares.

Características técnicas:
- Disposição na memória: se uma estrutura ocupa $S$ bytes, um array de tamanho $N$ ocupará rigorosamente $N \times S$ bytes consecutivos na memória.
- Sintaxe de acesso: combina o operador de indexação de array (`[]`) com o operador de acesso a membro (`.`): `vetor[i].campo`.
- Aritmética de ponteiros: a expressão `(vetor + i)->campo` é equivalente a `vetor[i].campo`, avançando exatamente $i \times \text{sizeof(struct)}$ bytes a partir do endereço base.

```c
#include <stdio.h>

#define TOTAL_ALUNOS 3

typedef struct {
  int matricula;
  int turma;
  float cra;
} Aluno;

int main(void) {
  // Declaração e inicialização de um array estático com 3 estruturas Aluno
  Aluno turma[TOTAL_ALUNOS] = {
    {101, 1, 9.10f},
    {102, 1, 7.80f},
    {103, 2, 8.45f}
  };

  float soma_cra = 0.0f;
  int idx_maior_cra = 0;

  // Itera pelo vetor para calcular a média e localizar o aluno com maior CRA
  for (int i = 0; i < TOTAL_ALUNOS; i++) {
    // Acumula o valor do campo 'cra' de cada aluno da turma
    soma_cra += turma[i].cra;

    // Compara o CRA atual com o maior registrado até o momento
    if (turma[i].cra > turma[idx_maior_cra].cra) {
      idx_maior_cra = i;
    }
  }

  float media_turma = soma_cra / TOTAL_ALUNOS;

  printf("--- Relatório da Turma ---\n");
  for (int i = 0; i < TOTAL_ALUNOS; i++) {
    printf("Matrícula: %d | Turma: %d | CRA: %.2f\n",
           turma[i].matricula,
           turma[i].turma,
           turma[i].cra);
  }

  printf("\nMédia geral da turma: %.2f\n", media_turma);
  printf("Destaque acadêmico: Matrícula %d (CRA: %.2f)\n",
         turma[idx_maior_cra].matricula,
         turma[idx_maior_cra].cra);

  return 0;
}
```

## Estruturas aninhadas e composição

A composição é o princípio de construir tipos de dados complexos através da combinação de tipos mais simples. Uma estrutura pode conter outra estrutura como campo interno, refletindo relações hierárquicas e modulares (como um evento que possui uma data de ocorrência e um horário específico).

Características técnicas:
- Armazenamento embutido: a estrutura interna não é um ponteiro; seus bytes residem fisicamente dentro do bloco de memória alocado para a estrutura externa.
- Acesso encadeado: os membros mais internos são acessados encadeando operadores ponto sucessivos: `externa.interna.campo`. Se a estrutura externa for manipulada por ponteiro, a sintaxe torna-se `ptr->interna.campo`.
- Modularidade: permite reutilizar pequenos tipos bem definidos (ex.: `Data`, `Horario`, `Ponto`) em múltiplos registros diferentes.

```c
#include <stdio.h>

// Estrutura independente para representar datas numéricas
typedef struct {
  int dia;
  int mes;
  int ano;
} Data;

// Estrutura independente para representar horários numéricos
typedef struct {
  int hora;
  int minuto;
  int segundo;
} Horario;

// Estrutura composta que aninha 'Data' e 'Horario'
typedef struct {
  int id_evento;      // Identificador numérico do evento
  Data data;          // Membro do tipo 'Data' embutido diretamente na struct
  Horario horario;    // Membro do tipo 'Horario' embutido diretamente na struct
} Evento;

// Função para exibir os dados aninhados recebendo a struct por ponteiro constante
void exibir_evento(const Evento *e) {
  if (e == NULL) {
    return;
  }

  // Acesso encadeado: desreferencia o ponteiro de Evento (->) e acessa os campos
  // das estruturas aninhadas internas através do ponto (.)
  printf("Evento ID %d registrado em %02d/%02d/%04d às %02d:%02d:%02d\n",
         e->id_evento,
         e->data.dia,
         e->data.mes,
         e->data.ano,
         e->horario.hora,
         e->horario.minuto,
         e->horario.segundo);
}

int main(void) {
  // Inicialização designada de estrutura com membros aninhados
  Evento ev1 = {
    .id_evento = 1001,
    .data = {.dia = 19, .mes = 10, .ano = 2026},
    .horario = {.hora = 14, .minuto = 30, .segundo = 0}
  };

  exibir_evento(&ev1);

  return 0;
}
```

## Alocação dinâmica de structs (Heap)

Quando o ciclo de vida de uma estrutura precisa ultrapassar o retorno da função que a criou, ou quando a quantidade de elementos só é conhecida em tempo de execução, a alocação na pilha (*stack*) torna-se insuficiente. Nesses casos, recorre-se à alocação dinâmica na memória *Heap* via `malloc` e à sua posterior liberação com `free`.

Características técnicas:
- Ciclo de vida explícito: os dados na Heap persistem até que `free` seja explicitamente invocado para aquele bloco de memória.
- Gerenciamento de dois níveis: se a struct contiver campos que são ponteiros para vetores alocados dinamicamente (como um conjunto dinâmico de medições numéricas), a liberação da memória deve obrigatoriamente ocorrer em **ordem inversa**: primeiro desalocam-se os vetores internos e somente depois a estrutura contêiner. Caso contrário, perde-se o endereço dos dados internos, causando vazamento de memória (*memory leak*).
- Checagem mandatória de `NULL`: chamadas a `malloc` podem falhar por falta de memória e retornar `NULL`, devendo ser sempre verificadas antes do uso.

```c
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int id_sensor;       // Identificador numérico do sensor
  int total_leituras;  // Quantidade de leituras armazenadas
  float *leituras;     // Ponteiro para vetor numérico alocado dinamicamente
} Sensor;

// Função construtora/fábrica: aloca a struct e o vetor interno na Heap
Sensor* criar_sensor(int id, int total_leituras) {
  if (total_leituras <= 0) {
    return NULL;
  }

  // 1º Passo: Aloca a estrutura contêiner na Heap
  Sensor *s = (Sensor *)malloc(sizeof(Sensor));
  if (s == NULL) {
    return NULL; // Falha na alocação da struct principal
  }

  // 2º Passo: Aloca memória para o vetor de medições numéricas
  s->leituras = (float *)malloc(total_leituras * sizeof(float));
  if (s->leituras == NULL) {
    // Se a alocação do vetor interno falhar, libera a struct contêiner antes de sair
    free(s);
    return NULL;
  }

  s->id_sensor = id;
  s->total_leituras = total_leituras;

  // Inicializa as medições com zero
  for (int i = 0; i < total_leituras; i++) {
    s->leituras[i] = 0.0f;
  }

  return s; // Retorna o ponteiro para a estrutura persistente na Heap
}

// Função destruidora: libera os recursos na ordem inversa de alocação
void destruir_sensor(Sensor *s) {
  if (s == NULL) {
    return;
  }

  // 1º: Libera a memória do vetor interno
  free(s->leituras);

  // 2º: Libera a memória da struct contêiner
  free(s);
}

int main(void) {
  // Cria dinamicamente um sensor com espaço para 3 leituras
  Sensor *s = criar_sensor(101, 3);
  if (s == NULL) {
    fprintf(stderr, "Erro ao alocar memória para o sensor.\n");
    return 1;
  }

  // Atribui leituras numéricas diretamente via ponteiro
  s->leituras[0] = 23.5f;
  s->leituras[1] = 24.1f;
  s->leituras[2] = 23.8f;

  printf("Sensor ID: %d (Total de medições: %d)\n",
         s->id_sensor, s->total_leituras);
  for (int i = 0; i < s->total_leituras; i++) {
    printf("  Leitura [%d]: %.1f°C\n", i, s->leituras[i]);
  }

  // Liberação completa da memória alocada
  destruir_sensor(s);
  s = NULL; // Boa prática: anula o ponteiro para evitar ponteiro dangling (solto)

  return 0;
}
```

## Alinhamento de memória e preenchimento (Padding)

Um aspecto crítico no estudo de estruturas em linguagem C é que **o tamanho de uma struct nem sempre é a soma aritmética dos tamanhos de seus membros**. A arquitetura do processador e o barramento do sistema exigem que variáveis primitivas sejam alinhadas em endereços que são múltiplos do seu tamanho natural (ex.: um `int` de 4 bytes deve residir em um endereço divisível por 4; um ponteiro de 8 bytes, em endereço divisível por 8). Para satisfazer essa exigência física do hardware com o máximo desempenho, o compilador insere bytes extras vazios denominados **padding** (preenchimento).

Características técnicas:
- Alinhamento natural: cada membro de tamanho $K$ começa em um *offset* interno múltiplo de $K$.
- Alinhamento da estrutura e *tail padding*: a estrutura inteira precisa ser múltiplo do seu maior membro primitivo para que, quando organizada em um array, cada elemento subsequente continue alinhado.
- Custo de layout desordenado: alternar variáveis menores com variáveis maiores força o compilador a injetar bytes de padding entre elas.
- Otimização prática: organizar os campos em ordem decrescente de tamanho (ou agrupar tipos do mesmo tamanho) reduz o padding a zero ou ao mínimo possível, economizando memória sem alterar a lógica do programa.

```c
#include <stdio.h>
#include <stddef.h> // Fornece a macro 'offsetof' para inspecionar os bytes

// Estrutura com disposição ineficiente dos campos:
// O alternamento entre tipos menores e maiores força a inserção de bytes de padding.
// Nota: 'char' é utilizado aqui exclusivamente como tipo escalar numérico de 1 byte (flag de controle).
struct Ineficiente {
  char flag1;   // 1 byte  -> Offset 0 (+ 3 bytes de padding para alinhar o int)
  int valor;    // 4 bytes -> Offset 4
  char flag2;   // 1 byte  -> Offset 8 (+ 3 bytes de tail padding para fechar múltiplo de 4)
}; // Tamanho total: 12 bytes (desperdício de 6 bytes em 12!)

// Estrutura com disposição otimizada dos campos:
// Agrupar tipos maiores no início ou agrupar os menores minimiza os preenchimentos.
struct Otimizada {
  int valor;    // 4 bytes -> Offset 0
  char flag1;   // 1 byte  -> Offset 4
  char flag2;   // 1 byte  -> Offset 5 (+ 2 bytes de tail padding para fechar múltiplo de 4)
}; // Tamanho total: 8 bytes (redução de 33% no consumo de memória)

int main(void) {
  printf("--- Análise da struct Ineficiente ---\n");
  printf("Tamanho total (sizeof): %zu bytes\n", sizeof(struct Ineficiente));
  printf("Offset de flag1: %zu\n", offsetof(struct Ineficiente, flag1));
  printf("Offset de valor: %zu\n", offsetof(struct Ineficiente, valor));
  printf("Offset de flag2: %zu\n", offsetof(struct Ineficiente, flag2));

  printf("\n--- Análise da struct Otimizada ---\n");
  printf("Tamanho total (sizeof): %zu bytes\n", sizeof(struct Otimizada));
  printf("Offset de valor: %zu\n", offsetof(struct Otimizada, valor));
  printf("Offset de flag1: %zu\n", offsetof(struct Otimizada, flag1));
  printf("Offset de flag2: %zu\n", offsetof(struct Otimizada, flag2));

  return 0;
}
```

## Comparação entre as abordagens e boas práticas

| Abordagem / Padrão | Sintaxe de Acesso | Localização na Memória | Custo de Cópia / Passagem | Cenário Ideal |
| :--- | :--- | :--- | :--- | :--- |
| **Struct simples local** | `var.campo` | Pilha (*Stack*) | $O(S)$ na cópia | Dados temporários de pequeno porte com ciclo de vida restrito ao escopo da função. |
| **Struct com `typedef`** | `var.campo` | Pilha ou Heap | Igual à struct pura (sem sobrecusto) | Praticamente todas as declarações de registros em C para garantir código limpo e idiomático. |
| **Passagem por `const Tipo *`** | `ptr->campo` | Endereço de memória | $O(1)$ (apenas 4 ou 8 bytes de ponteiro) | Leitura de estruturas médias ou grandes em funções, garantindo imunidade a modificações acidentais. |
| **Passagem por `Tipo *` (mutável)** | `ptr->campo` | Endereço de memória | $O(1)$ (apenas 4 ou 8 bytes de ponteiro) | Funções que precisam alterar o estado original da estrutura no chamador ou preencher dados de saída. |
| **Vetor estático de structs** | `vetor[i].campo` | Pilha (ou estática/global) | Alocação contígua fixa de $N \times S$ bytes | Coleções de tamanho fixo conhecido previamente durante o tempo de compilação. |
| **Estruturas aninhadas** | `p.data.ano` ou `ptr->data.ano` | Embutida dentro da struct pai | Depende de como a struct pai é passada | Modelagem hierárquica e modular de entidades compostas (composição orientada a dados). |
| **Struct dinâmica na Heap** | `ptr->campo` | Memória dinâmica (*Heap*) | $O(1)$ para transferência de ponteiro | Entidades com tempo de vida além da função atual ou registros com vetores numéricos de tamanho dinâmico. |

### Armadilhas comuns a serem evitadas

1. **Uso inadvertido de cópia por valor**: passar uma struct de tamanho moderado por valor para uma função que apenas lê dados gera uma duplicação desnecessária de bytes a cada invocação. Priorize `const Tipo *`.
2. **Confundir `.` com `->`**: o ponto (`.`) é usado em **instâncias diretas** (variáveis locais da struct); a seta (`->`) é usada exclusivamente em **ponteiros** para struct.
3. **Esquecer de liberar membros dinâmicos internos**: ao liberar uma struct que possui ponteiros alocados (ex.: vetores dinâmicos), chame `free(struct->vetor)` **antes** de chamar `free(struct)`.
4. **Assumir que `sizeof(struct)` é a soma exata dos campos**: utilize sempre o operador `sizeof(struct)` em vez de somar manualmente os tamanhos dos membros, pois o *padding* adicionado pelo compilador para alinhamento altera o total de bytes.
5. **Não checar o retorno de `malloc`**: qualquer alocação dinâmica pode falhar se a memória estiver esgotada; sempre verifique se o ponteiro retornado é diferente de `NULL` antes de utilizá-lo.
