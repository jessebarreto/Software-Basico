/*
 * Calculadora em notação pos-fixa
 * (Polones Reverso)
 * 
*/

/*DEBUG Functions of Zoeira*/
#define DEBUG_int(x) printf("DEBUG %d\n", x)
#define DEBUG_float(x) printf("DEBUG %f\n", x)
#define DEBUG_string(x) printf("DEBUG %s\n", x)
#define DEBUG 0

/*Constantes*/
#define euler 2.71828182
#define pi 3.141591653

/*Libraries and stuff!*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*Struct da Pilha*/
typedef struct StackEle
{
	void* data;
	int tamanho;
	struct StackEle* prox;
} StackEle;

typedef struct Stack
{
	StackEle* top;
} Stack;


/*Declaracoes das funções de pilha*/
StackEle* criar_elem_pilha(void*, StackEle*);
Stack* criar_pilha(void*);
void push(Stack*, void*);
void* pop(Stack*);
void limpar_pilha(Stack*);
int checar_pilha(Stack*);
void mostrar_pilha_float(Stack*);

/*Declaracoes das funções da calc*/
void erro_operador(int, char*);
void erro_num(char*);
void sair_calc();

/*Main da Galera!*/
int main(int argc, char** argv)
{	
	if(argc == 1)
	{
		printf("Nada a ser feito!\n");
		exit(0);
	}
	
	Stack* pilha_nmrs = criar_pilha(NULL);
	float aux, *nmr;
	int i = 0;
	for(i=1; i<argc; ++i)
	{
		#if DEBUG
			DEBUG_string(argv[i]);
		#endif
		
		switch (argv[i][0])
		{
			case '\0':
				break;
				
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				nmr = (float*)malloc(sizeof(float));
				*nmr = atof(argv[i]);
				push(pilha_nmrs, (void*)nmr);
				break;
			case 'e':
				if(argv[i][1]=='\0')
				{
					nmr = (float*)malloc(sizeof(float));
					*nmr = euler;
					push(pilha_nmrs, (void*)nmr);
				}
				if(argv[i][1] =='x')
				{
					if(!checar_pilha(pilha_nmrs))
						erro_operador(1, argv[i]);
					nmr = (float*)pop(pilha_nmrs);
					*nmr = (float)exp((double)*nmr);
					push(pilha_nmrs, (void*)nmr);
				}
				break;
			case 'p':
				nmr = (float*)malloc(sizeof(float));
				*nmr = pi;
				push(pilha_nmrs, (void*)nmr);
				break;
			case '+':
				if(!checar_pilha(pilha_nmrs))
					erro_operador(2, argv[i]);
				if(!pilha_nmrs->top->prox)
					erro_operador(2, argv[i]);
				
				nmr = (float*)pop(pilha_nmrs);
				*nmr = *nmr + *((float*)pop(pilha_nmrs));
				push(pilha_nmrs, (void*)nmr);
				
				break;
			case '-':
				if(!checar_pilha(pilha_nmrs))
					erro_operador(2, argv[i]);
				if(!pilha_nmrs->top->prox)
					erro_operador(2, argv[i]);
				
				nmr = (float*)pop(pilha_nmrs);
				push(pilha_nmrs, (void*)nmr);
				break;
			case '/':
			
				if(!checar_pilha(pilha_nmrs))
					erro_operador(2, argv[i]);
				if(!pilha_nmrs->top->prox)
					erro_operador(2, argv[i]);
				nmr = (float*)pop(pilha_nmrs);
				aux = *((float*)pop(pilha_nmrs));
				if(!(int)aux)
					erro_num("IMPOSSIVEL DIVIDIR POR ZERO!");
				*nmr = *nmr / aux;
				push(pilha_nmrs, (void*)nmr);
				
				break;
			case '%':
				if(!checar_pilha(pilha_nmrs))
					erro_operador(2, argv[i]);
				if(!pilha_nmrs->top->prox)
					erro_operador(2, argv[i]);
				nmr = (float*)pop(pilha_nmrs);
				aux = *((float*)pop(pilha_nmrs));
					if(!(int)aux)
					erro_num("IMPOSSIVEL DIVIDIR POR ZERO!");
				*nmr = (float)((int)*nmr % (int)aux);
				push(pilha_nmrs, (void*)nmr);
				
				break;	
				
			case 'x':
				if(!checar_pilha(pilha_nmrs))
					erro_operador(2, argv[i]);
				if(!pilha_nmrs->top->prox)
					erro_operador(2, argv[i]);
				
				nmr = (float*)pop(pilha_nmrs);
				*nmr = (*nmr) * (*((float*)pop(pilha_nmrs)));
				push(pilha_nmrs, (void*)nmr);
				
				break;
			case '^':
				if(!checar_pilha(pilha_nmrs))
					erro_operador(2, argv[i]);
				if(!pilha_nmrs->top->prox)
					erro_operador(2, argv[i]);
				nmr = (float*)pop(pilha_nmrs);
				aux = *((float*)pop(pilha_nmrs));
				*nmr=pow((double)*nmr, (double)aux);
				
				push(pilha_nmrs, (void*)nmr);
				break;
			default:
				erro_operador(0, argv[i]);
				break;
		}
	}
	
	#if DEBUG
	mostrar_pilha_float(pilha_nmrs);
	#endif
	printf("RESULTADO: %f\n", *((float*)pop(pilha_nmrs)) );
	
	/*Limpa as Sujeiras e Sai da Calculadora*/
	limpar_pilha(pilha_nmrs);
	free((void*)nmr);
	sair_calc();
	
	return 0;
}


/*Implementacao Funcoes de Pilha*/
StackEle* criar_elem_pilha(void* data, StackEle* prox)
{
	StackEle* elem = (StackEle*)malloc(sizeof(StackEle));
	
	if(elem)
	{
		elem->data = data;
		elem->tamanho = sizeof(data);
		elem->prox = prox;
	}
	return elem;
}
void push(Stack* pilha, void* data)
{
	if(pilha)
		pilha->top = criar_elem_pilha(data, pilha->top);
}

void* pop(Stack* pilha)
{
	if(!pilha)
		return NULL;
	if(!pilha->top)
		return NULL;
		
	StackEle* aux = pilha->top;
	pilha->top = aux->prox;

	return aux->data;
}

Stack* criar_pilha(void* data)
{
	Stack* pilha = (Stack*)malloc(sizeof(Stack));
	if(pilha)
	{
		if(data)
			push(pilha, data);
		else
			pilha->top = NULL;
	}
	
	return pilha;
}
	
void limpar_pilha(Stack* pilha)
{
	for(;pilha->top;)
		free(pop(pilha));
		
	pilha->top = NULL;
}

int checar_pilha(Stack* pilha)
{
	return pilha->top != NULL;
}


void mostrar_pilha_float(Stack* pilha)
{
	StackEle* aux = pilha->top;
	int i = 0;
	for(i=1;aux;i++)
	{
		printf("lvl:%d - %f\n", i, *((float*)aux->data));
		aux = aux->prox;
	}	
}

/*Implementacoes Funcoes da Calc*/
void erro_operador(int tipo, char* simboperador)
{
	if(!tipo)
		printf("Operador %s não reconhecido.\n", simboperador);
	if(tipo==1)
		printf("Operador Unário %s Sem Operandos\n", simboperador);
	if(tipo==2)
		printf("Operador Binário %s Com Poucos Operandos\n", simboperador);
	sair_calc();
}

void erro_num(char* tipo)
{
	printf("ERRO: %s\n", tipo);
	sair_calc();
}


void sair_calc()
{
	printf("Programa Finalizado.\n");
	exit(0);
}
