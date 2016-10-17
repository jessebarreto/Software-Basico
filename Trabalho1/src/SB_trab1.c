/*
 ===========================================================================================
 Name        : Trabalho Pratico 1 - Software Básico											
 Author      : 10/0106510 - Jessé Barreto de Barros
 			 : 10/0114067 - Marco Emílio Rodrigues Miranda
 Description : Trabalho de Software Básico 1
 	 	 	   O trabalho consiste em duas partes
 	 	 	   (i) Implementar em C/C++ um
 	 	 	   método de tradução de uma linguagem de montagem simples para uma
 	 	 	   representação de código objeto.
 	 	 	   (ii) Implementar um programa em C/C++ para simular a execução de
 	 	 	   um código objeto da parte anterior.
 Compilation
 Command	 : gcc -ansi -Wall SB_trab1.c -o SB_trab.o
 Execution
 Command	 : ./SB_trab1 a.asm b.pre c.obj (Montagem - b.pre e c.obj)
			   ./SB_trab1 c.obj (Simulação - c.obj Arq Objeto)				
 ===========================================================================================
 */

/*Libraries*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define TAMANHO_TABELA_SIMB 8001

/*Estrutura de Dados da Tabela Hash*/
typedef struct entrada_tabela
{
	char* key;
	int *valor;
	struct entrada_tabela *prox;
}entrada_tabela;

typedef struct tabela_hash
{
	int tamanho;
	struct entrada_tabela **tabela;
}tabela_hash;


/*Declaracoes das funções*/
void abrir_Arquivos(int , char**, FILE**, FILE**, FILE**);
void fechar_Arquivos(FILE**, FILE**, FILE**);
tabela_hash* criar_TabeladeDiretivas();
tabela_hash* criar_TabeladeIntrucoes();
char* get_Line(int*, FILE**);
int is_Rotulo(char*);
int is_Instrucao(char*, tabela_hash*);
int is_Diretiva(char*, tabela_hash*);
void get_Token(char***, char* , char* , int*);
void zeresima_passagem(FILE**, FILE**, tabela_hash**);
void primeira_passagem(FILE**, tabela_hash*, tabela_hash**);
void segunda_passagem(FILE** , FILE**,tabela_hash* , tabela_hash** );
void simulacao(FILE** arq_obj);

/*Declaracao das funcoes indicadoras de erro*/
void erro_Lexico(char*, int);
void erro_Sintatico(char*, int);
void erro_Semantico(char*, int);

/*Declaracao das funcoes da tabela hash*/
tabela_hash* criar_Tabela(int);
int key_Function(tabela_hash*, char*);
entrada_tabela* criar_Entrada(char*, int*);
void inserir_naTabela(tabela_hash*, char*, int*);
int* receber_daTabela(tabela_hash*, char*);
void destruir_Tabela(tabela_hash*);
void destruir_entradas(entrada_tabela*);

/*
**		MAIN!!!
*/
int main(int argc, char** argv)
{
	/*Ponteiros dos arquivos*/
	printf("--ABRINDO OS ARQUIVOS--\n");
	FILE *arq_asm=NULL, *arq_pre=NULL, *arq_obj=NULL;
	abrir_Arquivos(argc, argv, &arq_asm, &arq_pre, &arq_obj);

	if(argc < 3)
	{
		/*Processo de Simulacao*/
		printf("--INICIO DA SIMULACAO--\n");
		simulacao(&arq_obj);
	}
	else
	{
		/*Cria a tabelas de diretivas e intrucoes*/
		printf("--CRIANDO AS TABELAS DE INTRUCOES E DIRETIVAS--\n");
		tabela_hash** tabela_ins_dir = (tabela_hash**)malloc(2*sizeof(tabela_hash*));
		tabela_ins_dir[0] = criar_TabeladeIntrucoes();
		tabela_ins_dir[1] = criar_TabeladeDiretivas();

		printf("--INICIO DA MONTAGEM--\n");
		/*Tabela de Simbolos*/
		tabela_hash* TS = criar_Tabela(TAMANHO_TABELA_SIMB);
		
		/*Algoritmos da montagem*/
		zeresima_passagem(&arq_asm, &arq_pre, tabela_ins_dir);
		primeira_passagem(&arq_pre, TS, tabela_ins_dir); 
		segunda_passagem(&arq_pre, &arq_obj, TS, tabela_ins_dir);
		
		/*
		*/
	}
	
	printf("--FECHANDO ARQUIVOS--\n");
	fechar_Arquivos(&arq_asm, &arq_pre, &arq_obj);
	return 0;
}

/*Abrir os arquivos que serao utilizados e checar erros iniciais*/
void abrir_Arquivos(int argc, char** argv, FILE** arq_asm, FILE** arq_pre, FILE** arq_obj)
{
	/*Checa se ha pelo menos o paramentro .asm*/
	if(argc < 2)
	{
		printf("ERRO - Programa sem os parâmetros .asm .pre .obj\n");
		exit(1);
	}
	/*Se não existirem os parametros .pre e .obj usa se apenas o simulador - abrindo o .obj em +r*/
	if(argc == 2)
	{
		/*Se nao for possivel abrir o arquivo .obj sai do programa*/
		*arq_obj = fopen(argv[1], "r+");
		if(*arq_obj == NULL)
		{
			printf("ERRO - Impossivel abrir o arquivo %s\n", argv[1]);
			exit(1);
		}
		return;
	}
	/*Se não existir o parametro .obj usa se um nome generico*/
	else if(argc == 3)
	{
		char** aux = (char**)malloc(4*sizeof(char*));
		aux[0] = argv[0]; aux[1] = argv[1]; aux[2] = argv[2];
		aux[3] = (char*)malloc(5*sizeof(char));
		aux[3] = "a.obj";
		argv = aux;
	}
	/*Se nao for possivel abrir o arquivo .asm sai do programa*/
	*arq_asm = fopen(argv[1], "r+");
	if(*arq_asm == NULL)
	{
		printf("ERRO - Impossivel abrir o arquivo %s\n", argv[1]);
		exit(1);
	}
	/*Cria/Abre os arq do .pre e .obj*/
	*arq_pre = fopen(argv[2], "w+");
	*arq_obj = fopen(argv[3], "r+w");
}

/*Fecha os arquivos utilizados*/
void fechar_Arquivos(FILE** arq_asm, FILE** arq_pre, FILE** arq_obj)
{
	if(*arq_asm!=NULL)
		fclose(*arq_asm);
	if(*arq_pre!=NULL)
		fclose(*arq_pre);
	if(*arq_obj!=NULL)
		fclose(*arq_obj);
}

/*Pega toda a linha de um arquivo*/
char* get_Line(int* cont_linha, FILE** arq)
{
	if(*arq==NULL)
		return NULL;

	long int cont_pos, pos_inicial = ftell(*arq);
	char aux, *linha, *sem_coment;

	/*Vai testando o fim da linha ou do arquivo*/
	for(aux=fgetc(*arq),cont_pos=1;aux!='\n'&&aux!=EOF;cont_pos++,aux=fgetc(*arq));

	/*Caso a linha tenha tamanho 1 e não comece com \n isso indica um erro*/
	if(cont_pos==1)
	{
		if(aux!='\n')
		{
			return NULL;
		}
		else
		{
			linha = get_Line(cont_linha, arq);
			if(linha==NULL)
				return NULL;
		}
	}
	else
	{
		/*Aloca o tamanho da linha*/
		linha = (char*)malloc(cont_pos*sizeof(char));

		/*Volta a posicao incial da contagem*/
		fseek(*arq, pos_inicial, SEEK_SET);
		
		/*Le a linha*/
		linha = fgets(linha, cont_pos, *arq);
		/*Pula o \n*/
		fseek(*arq, 1, SEEK_CUR);
	}
	
	/*Converte os caracteres minusculos em maiusculos*/
	int i;
	for(i=0;linha[i]!='\0';i++)
		linha[i]=toupper(linha[i]);
			
	/*Ignora todos os comentarios lidos*/
	if(linha[0]==';')
		linha = get_Line(cont_linha, arq);
	sem_coment = strtok(linha, ";");
	
	(*cont_linha)++;

	return sem_coment;
}

/*Verifica se o token eh um rotulo*/
int is_Rotulo(char* token)
{
	int tam = strlen(token);
	if(token[tam-1]==':')
		return 1;
	return 0;
}

/*Verifica se o token eh uma instrucao*/
int is_Instrucao(char* token, tabela_hash* tabela_inst)
{
	if(receber_daTabela(tabela_inst, token)!=NULL)
		return 1;
	return 0;
}

/*Verifica se o token eh uma diretiva*/
int is_Diretiva(char* token, tabela_hash* tabela_dir)
{
	if(receber_daTabela(tabela_dir, token)!=NULL)
		return 1;
	return 0;
}

/*Checa erros lexicos*/
void erro_Lexico(char* token, int linha)
{ 

	/*const char nmrs[] = "0123456789";*/
	const char caract_invalidos[] = "ç~&";
	int tam = strlen(token);

	int logico = (tam>100) || (strpbrk(token, caract_invalidos));


	if(logico)
	{
		printf("%s\n",token);
		printf("ERRO - LEXICO - LIN%d\n", linha);
		printf("TOKEN - %s\n", token);
		exit(1);
	}
}

/*Checa erros sintaticos*/
void erro_Sintatico(char* erro, int linha)
{
	printf("ERRO - SINTATICO - LIN %d\n", linha);
	printf("TOKEN - %s\n", erro);
	exit(1);
}

/*Checa erros semanticos*/
void erro_Semantico(char* erro, int linha)
{
	printf("ERRO - SEMANTICO - LIN %d\n", linha);
	printf("TOKEN - %s\n", erro);
	exit(1);
}

/*Cria Tabelas de Instrucoes*/
tabela_hash* criar_TabeladeIntrucoes()
{
	tabela_hash* TI = criar_Tabela(20);
	int *aux = NULL;

	/*Instrucoes - Vetor(Operando, Código, Tamanho)*/
	/*ADD*/
	aux=(int*)malloc(3*sizeof(int));
	aux[0]=1;aux[1]=1;aux[2]=2;
	inserir_naTabela(TI, "ADD", aux);
	/*SUB*/
	aux=(int*)malloc(3*sizeof(int));
	aux[0]=1;aux[1]=2;aux[2]=2;
	inserir_naTabela(TI, "SUB", aux);
	/*MULT*/
	aux=(int*)malloc(3*sizeof(int));
	aux[0]=1;aux[1]=3;aux[2]=2;
	inserir_naTabela(TI, "MULT", aux);
	/*DIV*/
	aux=(int*)malloc(3*sizeof(int));
	aux[0]=1;aux[1]=4;aux[2]=2;
	inserir_naTabela(TI, "DIV", aux);
	/*JMP*/
	aux=(int*)malloc(3*sizeof(int));
	aux[0]=1;aux[1]=5;aux[2]=2;
	inserir_naTabela(TI, "JMP", aux);
	/*JMPN*/
	aux=(int*)malloc(3*sizeof(int));
	aux[0]=1;aux[1]=6;aux[2]=2;
	inserir_naTabela(TI, "JMPN", aux);
	/*JMPP*/
	aux=(int*)malloc(3*sizeof(int));
	aux[0]=1;aux[1]=7;aux[2]=2;
	inserir_naTabela(TI, "JMPP", aux);
	/*JMPZ*/
	aux=(int*)malloc(3*sizeof(int));
	aux[0]=1;aux[1]=8;aux[2]=2;
	inserir_naTabela(TI, "JMPZ", aux);
	/*COPY*/
	aux=(int*)malloc(3*sizeof(int));
	aux[0]=2;aux[1]=9;aux[2]=3;
	inserir_naTabela(TI, "COPY", aux);
	/*LOAD*/
	aux=(int*)malloc(3*sizeof(int));
	aux[0]=1;aux[1]=10;aux[2]=2;
	inserir_naTabela(TI, "LOAD", aux);
	/*STORE*/
	aux=(int*)malloc(3*sizeof(int));
	aux[0]=1;aux[1]=11;aux[2]=2;
	inserir_naTabela(TI, "STORE", aux);
	/*INPUT*/
	aux=(int*)malloc(3*sizeof(int));
	aux[0]=1;aux[1]=12;aux[2]=2;
	inserir_naTabela(TI, "INPUT", aux);
	/*OUTPUT*/
	aux=(int*)malloc(3*sizeof(int));
	aux[0]=1;aux[1]=13;aux[2]=2;
	inserir_naTabela(TI, "OUTPUT", aux);
	/*STOP*/
	aux=(int*)malloc(3*sizeof(int));
	aux[0]=0;aux[1]=14;aux[2]=1;
	inserir_naTabela(TI, "STOP", aux);

	return TI;
}

/*Cria Tabelas de Diretivas*/
tabela_hash* criar_TabeladeDiretivas()
{
	tabela_hash* TD = criar_Tabela(20);
	int *aux = NULL;

	/*Diretivas - Vetor(Operando, Tamanho)*/
	/*SECTION*/
	aux=(int*)malloc(2*sizeof(int));
	aux[0]=1;aux[1]=0;
	inserir_naTabela(TD, "SECTION", aux);
	/*SPACE*/
	aux=(int*)malloc(2*sizeof(int));
	aux[0]=-1;aux[1]=-1;
	inserir_naTabela(TD, "SPACE", aux);
	/*CONST*/
	aux=(int*)malloc(2*sizeof(int));
	aux[0]=1;aux[1]=1;
	inserir_naTabela(TD, "CONST", aux);
	/*EQU*/
	aux=(int*)malloc(2*sizeof(int));
	aux[0]=1;aux[1]=0;
	inserir_naTabela(TD, "EQU", aux);
	/*IF*/
	aux=(int*)malloc(2*sizeof(int));
	aux[0]=1;aux[1]=0;
	inserir_naTabela(TD, "IF", aux);
	/*TESTE - TEXT*/
	aux=(int*)malloc(2*sizeof(int));
	aux[0]=0;aux[1]=0;
	inserir_naTabela(TD, "TEXT", aux);
	/*TESTE - DATA*/
	aux=(int*)malloc(2*sizeof(int));
	aux[0]=0;aux[1]=0;
	inserir_naTabela(TD, "DATA", aux);

	return TD;
}

/*
** ALGORITMOS DAS PASSAGENS DO MONTADOR
*/
/*Algoritmo da zeresima passagem*/
void zeresima_passagem(FILE** arq_asm, FILE** arq_pre, tabela_hash** TID)
{
	/*Delimitadores utilizados no codigo*/
	char delim[]=" 	,";
	/*Linha atual, vetor de tokens, nmr de tokens e contador de linha, */
	char *linha, *linha_aux, **tokens;
	int nmr_tokens, contador_linha, i, *valor;
	char *aux;
	
	/*Tabela que contem os valores que a diretiva EQU guarda*/
	tabela_hash* tabela_valores_equ = criar_Tabela(TAMANHO_TABELA_SIMB);
	
	/*Passagem 1 - Lidar apenas com os valores de EQU*/
	for(linha = get_Line(&contador_linha, arq_asm), contador_linha=1;linha!=NULL;linha = get_Line(&contador_linha, arq_asm))
	{
		get_Token(&tokens, linha, delim, &nmr_tokens);
		for(i=0;i<nmr_tokens;i++)
		{
			if(is_Rotulo(tokens[i]))
				/*Checa se há os dois proxímos parâmetros*/
				if(i+2<nmr_tokens)
					/*Checa se o rotulo eh EQU*/
					if(!strcmp(tokens[i+1],"EQU"))
					{
						/*Remove o : no fim do rótulo*/
						aux = (char*)malloc(sizeof(char)*strlen(tokens[i]));
						sscanf(tokens[i], "%[^:]s", aux);
						/*Adiciona na tabela o valor da diretiva EQU*/
						valor = (int*)malloc(sizeof(int));
						(*valor) = atoi(tokens[i+2]);
						inserir_naTabela(tabela_valores_equ,aux ,valor);
						/*Pula para os proximos tokens*/	
						i+=3;
					}				
		}	
	}	
	
	/*Volta a posicao inicial do arquivo asm*/
	fseek(*arq_asm, 0, SEEK_SET);
	
	/*Passagem 2 - EQU, IF e escrever as linhas*/
	for(contador_linha=0, linha = get_Line(&contador_linha, arq_asm);linha!=NULL;linha = get_Line(&contador_linha, arq_asm))
	{
		/*Utiliza a linha auxiliar para obter os tolkens*/
		linha_aux = (char*)malloc(sizeof(char)*(strlen(linha)+1));
		memcpy(linha_aux, linha, strlen(linha));
		get_Token(&tokens, linha_aux, delim, &nmr_tokens);
		
		for(i=0;i<nmr_tokens;i++)
		{
			/*Checa se eh rotulo e se eh EQU e pula a linha*/
			if(is_Rotulo(tokens[i]) && i+2<nmr_tokens)
			{
				if(!strcmp(tokens[i+1],"EQU"))
					break;
			}
			/*Checa se eh diretiva e se a diretiva eh IF*/
			if(is_Diretiva(tokens[i], TID[1]))
			{
				if(!(strcmp(tokens[i],"IF")) && (i+1 < nmr_tokens))
				{
					/*Checa o valor armazenado na tabela para o rotulo usado*/
					valor = receber_daTabela(tabela_valores_equ, tokens[i+1]); 
					/*Como eh um if jah recebe a proxima linha*/
					linha = get_Line(&contador_linha, arq_asm);
					/*Checa se existe mesmo esse valor definido por EQU, se nao nao escreve a linha*/
					if(valor!=NULL)
						if(*valor) /*Checa se o valor eh diferente de zero para escrever a proxima linha*/
							fprintf(*arq_pre, "%s\n", linha);
					/*Pula para a proxima linha*/
					break;
				}
			}
			/*Checa se o token eh um dos simbolos do EQU*/
			
			
			/*Caso nao seja nem IF, nem EQU escreve no .pre*/
			fprintf(*arq_pre, "%s\n", linha);
			break;
		}
	}
	
	/*Destroi a tabela de valores utilizados para o EQU*/
	destruir_Tabela(tabela_valores_equ);
}

/*Algoritmo da primeira passagem*/
void primeira_passagem(FILE** arq_pre, tabela_hash* TS, tabela_hash** TID)
{
	/*Delimitadores utilizados no codigo*/
	char delim[]=" 	,";

	int contador_linha, contador_pos;
	char *linha, *linha_aux, **tokens, *aux;
	int i, j, cont_section=0;

	/*Volta a posicao inicial do arquivo pre*/
	fseek(*arq_pre, 0, SEEK_SET);
	fflush(*arq_pre);

	linha = get_Line(&contador_linha, arq_pre);
	for(contador_linha=1, contador_pos=0;linha!=NULL;linha = get_Line(&contador_linha, arq_pre))
	{
		/*Utiliza a linha auxiliar para obter os tolkens*/
		linha_aux = (char*)malloc(sizeof(char)*(strlen(linha)+1));
		memcpy(linha_aux, linha, strlen(linha));
		get_Token(&tokens, linha_aux, delim, &i);

		for(j=0;j<i;j++)
		{
			/*Checa a presenca de erro lexico no token*/
			erro_Lexico(tokens[j], contador_linha);

			/*Verifica se eh rotulo*/
			if(	is_Rotulo(tokens[j]))
			{
				/*Remove o : do rotulo*/
				aux = (char*)malloc(sizeof(char)*strlen(tokens[j]));
				sscanf(tokens[j], "%[^:]s", aux);
				/*Verifica se o rotulo ja esta na TS*/
				if(receber_daTabela(TS, aux)!=NULL)
					erro_Semantico(aux, contador_linha);
				else
				{
					int* pos_mem = (int*)malloc(sizeof(int));
					*pos_mem = contador_pos;
					inserir_naTabela(TS, aux, pos_mem);
				}

				continue;
			}
			/*Verifica se eh instrucao*/
			if(is_Instrucao(tokens[j], TID[0]))
			{
				contador_pos += (*(receber_daTabela(TID[0], tokens[j])+2));
				if(j+(*(receber_daTabela(TID[0], tokens[j])+2)-1)<i)
				{
					j+=(*(receber_daTabela(TID[0], tokens[j])+2)-1);
				}
				else
					erro_Sintatico(tokens[j], contador_linha);
			}
			/*Verifica se eh diretiva*/
			else if(is_Diretiva(tokens[j],TID[1]))
			{
				if(!strcmp(tokens[j], "SECTION"))
				{
					cont_section++;
					if(cont_section>2)
						erro_Sintatico(tokens[j], contador_linha);

					if(!strcmp(tokens[j+1], "DATA")|| !strcmp(tokens[j+1], "TEXT"))
						break;
					else
						erro_Sintatico(tokens[j], contador_linha);
				}
				/*TESTE SPACE e CONST*/
				else if(!strcmp(tokens[j], "SPACE"))
				{
					if(i>j+1)
					{
						int tam_mem = atoi(tokens[j+1]);
						contador_pos += tam_mem;
						break;
					}
					contador_pos++;
					
				}
				else if(!strcmp(tokens[j], "CONST"))
				{
					if(j+1<i)
					{
						j++;
						contador_pos++;
					}
					else
						erro_Sintatico(tokens[j], contador_linha);
				}
			}
			else
				erro_Semantico(tokens[j], contador_linha);
		}
	}
}

/*Algoritmo da segunda passagem*/
void segunda_passagem(FILE** arq_pre, FILE** arq_obj, tabela_hash* TS, tabela_hash** TID)
{
	/*Delimitadores utilizados no codigo*/
	char delim[]="	 ,";
	/*Linha atual, vetor de tokens, contador de linha e posicao e valor da tabela */
	char *linha, *linha_aux, **tokens;
	int contador_linha, contador_pos=0, i, j, k, *valor, aux=0;
	/*Buffer de escrita do codigo objeto*/
	short *cod_obj;
	
	/*Volta a posicao inicial do arquivo pre*/
	fseek(*arq_pre, 0, SEEK_SET);
	fflush(*arq_pre);
	
	for(linha = get_Line(&contador_linha, arq_pre), contador_linha=1;linha!=NULL;linha = get_Line(&contador_linha, arq_pre))
	{
		printf("End %d: ", contador_pos);
		
		/*Utiliza a linha auxiliar para obter os tolkens*/
		linha_aux = (char*)malloc(sizeof(char)*(strlen(linha)+1));
		memcpy(linha_aux, linha, strlen(linha));
		get_Token(&tokens, linha_aux, delim, &i);
		
		/*Percorre por todos os tokens dessa linha*/
		for(j=0;j<i;j++)
		{			
			/*Verifica se eh rotulo*/
			if(	is_Rotulo(tokens[j]))
				continue;
				
			/*Verifica se eh instrucao*/
			if(is_Instrucao(tokens[j], TID[0]))
			{
				/*Recebe as informacoes da instrucao da tabela de intrucoes*/
				valor = receber_daTabela(TID[0], tokens[j]);
				
				/*Aloca o buffer de saida do cod. obj*/
				cod_obj = (short*)malloc(sizeof(short)*valor[2]);
				
				/*Coloca o nmr da instrucao*/
				cod_obj[0] = (short)valor[1];
				
				/*Checa se o nmr de operandos esta certo - Passagem 1*/
				/*Comeca a varrer os operandos da instrucao*/
				for(k=1;k<=*valor;k++)
				{
					/*Busca o operando na tabela de simbolos*/
					if(receber_daTabela(TS,tokens[j+k])==NULL) /*Se nao encontrar erro semantico*/
						erro_Semantico(tokens[j+k], contador_linha);
					/*Recebe o valor do operando*/
					cod_obj[k] = *(receber_daTabela(TS, tokens[j+k]));
				}
				
				/*Pula os operandos dessa instrucao*/
				if(j+valor[0]<i)
				{
					j+=valor[0];
				}
				else
				{
					break;
				}
				/***********ESCREVE O OBJ***********/
				for(k=0;k<=*valor;k++)
				{
					printf("%d ", (int)cod_obj[k]);
					fwrite(&cod_obj[k], sizeof(short), 1, *arq_obj);
				}
				printf("\n");
				
				/*Soma o contador de posicao*/
				contador_pos += *(valor+2);
			}
			/*Verifica se eh diretiva*/
			else if(is_Diretiva(tokens[j], TID[1]))
			{	
				/*Recebe as informacoes da instrucao da tabela de intrucoes*/
				valor = receber_daTabela(TID[1], tokens[j]);
				
				if(!strcmp(tokens[j],"SECTION"))
				{
					/*Pula os tokens*/
					j+=2;
					continue;
				}
				else if(!strcmp(tokens[j],"CONST"))
				{
					/*Aloca o buffer de saida do cod. obj*/
					cod_obj = (short*)malloc(sizeof(short));
					
					/*Coloca o valor da cte no cod_obj*/
					*cod_obj = atoi(tokens[j+1]);
					
					contador_pos++;
					j++;
					
					
					/***********ESCREVE O OBJ***********/
					for(k=0;k<*valor;k++)
					{
						printf("%d ", (int)cod_obj[k]);
						fwrite(&cod_obj[k], sizeof(short), 1, *arq_obj);
					}
					printf("\n");
					continue;
				}
				else if(!strcmp(tokens[j],"SPACE"))
				{
					if(!(j+1<i))
					{
						/*Aloca o buffer de saida do cod. obj*/
						cod_obj = (short*)malloc(sizeof(short));
						
						/*Coloca o valor da cte no cod_obj*/
						*cod_obj = 0;
						
						aux=1;
						contador_pos++;
						j++;
					}
					else
					{
						/*Aloca o buffer de saida do cod. obj*/
						cod_obj = (short*)malloc(sizeof(short)*atoi(tokens[j+1]));
						
						/*Preenche com 0*/
						for(k=0;k<atoi(tokens[j+1]);k++)
							cod_obj[k] = 0;
						
						aux= atoi(tokens[j+1]);
						contador_pos += atoi(tokens[j+1]);
						j++;
					}
					/***********ESCREVE O OBJ***********/
					for(k=0;k<aux;k++)
					{
						printf("%d ", (int)cod_obj[k]);
						fwrite(&cod_obj[k], sizeof(short), 1, *arq_obj);
					}

					printf("\n");
					continue;
				}
				
			}
			else
				erro_Semantico(tokens[j], contador_linha);
		}
	}
}

/*Separa uma linha em tokens*/
void get_Token(char*** tokens, char* linha, char* delim, int* nmr_tokens)
{
	char *aux, **aux_vec;
	int i, j;

	aux=strtok(linha,delim);
	
	/*Armazenar os tokens por linha*/
	for(i=0;aux!=NULL;aux=strtok(NULL,delim),i++)
	{
		(*tokens) = (char**)calloc((i + 1),sizeof(char*));
		(*tokens)[i] = aux;
		for(j=0;j<i;j++)
		{
			(*tokens)[j]=aux_vec[j];
		}
		aux_vec=(*tokens);
	}
	(*nmr_tokens) = i;
}

/* ALGORITMO PARA A SIMULACAO */
void simulacao(FILE** arq_obj)
{
	int flag;
	short PC = 0, ACC = 0;
	long tamanho;
	short MEM[216];

	printf("--ABRINDO ARQUIVOS--\n");

	fseek(*arq_obj , 0 , SEEK_END); /* Percorre todo o arquivo */
	tamanho = ftell(*arq_obj);	/* O ponteiro passa a posiçao final em bytes para tamanho */
	rewind(*arq_obj); /* Recua o ponteiro para o começo */
	tamanho = tamanho / sizeof(short); /* O tamanho do aquivo em palavras ´e o tamanho em bytes divido pela tamanho da palavra */

	if(tamanho < 216)	/* Como visto em aula a maquina hipotetica tem apenas 216 palavras de 16 bits de memoria */
	{	
		printf("--CARREGANDO NA MEMORIA--\n");
		fread(MEM, sizeof(short), tamanho, *arq_obj); /* Carrega o arquivo pra memoria */

		flag = 1;
		while(flag)					/*Flag de encerramento da simulaçao*/
		{	
			switch(MEM[PC])
			{
				case 1: /*ADD*/
					printf("ADD:    ACC = %d + %d\n", ACC, MEM[MEM[PC + 1]]);
					ACC = ACC + MEM[MEM[PC + 1]];	/*ACC = ACC + MEM[OP]*/
					PC = PC + 2;					/*Incrementa PC*/
				break;

				case 2: /*SUB*/
					printf("SUB:    ACC = %d - %d\n", ACC, MEM[MEM[PC + 1]]);
					ACC = ACC - MEM[MEM[PC + 1]];	/*ACC = ACC - MEM[OP]*/
					PC = PC + 2;					/*Incrementa PC*/
				break;
				
				case 3: /*MULT*/
					printf("MULT:   ACC = %d * %d\n", ACC, MEM[MEM[PC + 1]]);
					ACC = ACC * MEM[MEM[PC + 1]];	/*ACC = ACC * MEM[OP]*/
					PC = PC + 2;					/*Incrementa PC*/
				break;
				
				case 4: /*DIV*/
					if(MEM[MEM[PC]] == 0)
					{
						printf("ERRO: Tentatica de divisão por 0 \n");
					}
					else
					{
						printf("DIV:    ACC = %d / %d\n", ACC, MEM[MEM[PC + 1]]);
						ACC = ACC / MEM[MEM[PC + 1]];/*ACC = ACC / MEM[OP]*/
						PC = PC + 2;				 /*Incrementa PC*/
					}
				break;
				
				case 5: /*JMP*/
					printf("JMP:    PC = %d \n", MEM[PC + 1]);
					PC = MEM[PC + 1];				/*PC = OP*/
				break;
				
				case 6: /*JMPN*/
					if(ACC < 0)						/*Se ACC eh negativo*/
					{	
						printf("JMPN:   PC = %d \n", MEM[PC + 1]);
						PC = MEM[PC + 1];			/*PC = OP*/
					}
					else
					{
						PC = PC + 2;				/*Incrementa PC*/
					}
				break;
				
				case 7: /*JMPP*/
					if(ACC > 0)						/*Se ACC eh positivo*/
					{
						printf("JMPP:   PC = %d \n", MEM[PC + 1]);
						PC = MEM[PC + 1];			/*PC = OP*/
					}
					else
					{
						PC = PC + 2; 				/*Incrementa PC*/
					}
				break;
				
				case 8: /*JMPZ*/
					if(ACC == 0)					/*Se ACC eh ZERO*/
					{
						printf("JMPZ:   PC = %d \n", MEM[PC + 1]);
						PC = MEM[PC + 1];			/*PC = OP*/
					}
					else
					{
						PC = PC + 2; 				/*Incrementa PC*/
					}
				break;
				
				case 9: /*COPY*/
					printf("COPY:   MEM[%d] =  MEM[%d] \n", MEM[PC + 2], MEM[PC + 1]);
					MEM[MEM[PC + 2]] = MEM[MEM[PC + 1]]; /*Copia do endereço 1 pra endereço 2*/
					PC = PC + 3;					/*Incrementa PC*/
				break;
				
				case 10: /*LOAD*/
					printf("LOAD:   ACC = %d \n", MEM[MEM[PC + 1]]);
					ACC = MEM[MEM[PC + 1]];			/*Carrega MEM[OP] em ACC*/
					PC = PC + 2; 					/*Incrementa PC*/
				break;
				
				case 11: /*STORE*/
					printf("STORE:  MEM[%d] = %d \n", MEM[PC + 1], ACC);
					MEM[MEM[PC + 1]] = ACC;			/*Grava ACC em MEM[OP]*/
					PC = PC + 2;					/*Incrementa PC*/
				break;
				
				case 12: /*INPUT*/
					printf("INPUT:  MEM[%d] = ", MEM[PC + 1]);
					scanf("%hd", &MEM[MEM[PC + 1]]);/*Carrega do dispositivo de entrada na memoria*/
					PC = PC + 2;					/*Incrementa PC*/
				break;
				
				case 13: /*OUTPUT*/
					printf("OUTPUT: %hd\n", MEM[MEM[PC + 1]]);/*Descarrega da memoria no dispositivo de saida*/
					PC = PC + 2;					/*Incrementa PC*/
				break;
				
				case 14: /*STOP*/
					printf("\t--STOP--\n");	/*Para o programa*/
					flag = 0;
				break;

				default:
					flag = 0;
					printf("\tERRO: Instruçao nao encontrada!!!\n");
				break;
			}
		}
	}
}


/*
** ALGORITMOS PARA A TABELA HASH
*/
/*Funcoes da Tabela Hash*/
tabela_hash* criar_Tabela(int tamanho)
{
	tabela_hash* tabela = NULL;
	if(tamanho < 1)
		return NULL;

	tabela = (tabela_hash*)malloc(sizeof(tabela_hash));
	tabela->tamanho = tamanho;
	tabela->tabela = (entrada_tabela**)malloc(tamanho*sizeof(entrada_tabela));
	int i;
	for(i=0;i<tamanho;i++)
		tabela->tabela[i] = NULL;

	return tabela;
}

int key_Function(tabela_hash* tabela_hash, char* key)
{
	unsigned int valor=0;
	int i;

	for(i=0;i<strlen(key) && valor < LONG_MAX; i++)
	{
		valor = valor << 2;
		valor += key[i]-')';
	}

	return valor % tabela_hash->tamanho;
}

entrada_tabela* criar_Entrada(char* key, int* valor)
{
	entrada_tabela* nova_entrada;

	nova_entrada = (entrada_tabela*)malloc(sizeof(nova_entrada));
	nova_entrada->key = (char*)malloc(sizeof(char)*strlen(key));
	memcpy(nova_entrada->key, key, strlen(key));
	nova_entrada->valor = (int*)malloc(sizeof(int));
	nova_entrada->valor = valor;
	nova_entrada->prox = NULL;

	return nova_entrada;
}

/*Insere um registro na tabela*/
void inserir_naTabela(tabela_hash* tabela_hash, char* key, int* valor)
{
	int valor_key = 0;
	entrada_tabela *prox=NULL, *ant=NULL;
	entrada_tabela *nova_entrada=NULL;

	valor_key = key_Function(tabela_hash, key);
	prox = tabela_hash->tabela[valor_key];

	for(;prox != NULL && prox->key != NULL && strcmp(key, prox->key) > 0;)
	{
		ant = prox;
		prox = prox->prox;
	}

	/*Se já existe essa entrada. Renovar!*/
	if(prox != NULL && prox->key != NULL && strcmp(key, prox->key) > 0)
		prox->valor = valor;
	/*Se a entrada não existe basta cria-la*/
	else
	{
		nova_entrada = criar_Entrada(key, valor);

		/*Essa entrada está no inicio da lista encadeada para essa chave*/
		if(prox == tabela_hash->tabela[valor_key])
		{
			nova_entrada->prox = prox;
			tabela_hash->tabela[valor_key] = nova_entrada;
		}
		/*A entrada está no fim*/
		else if(prox == NULL)
			ant->prox = nova_entrada;
		/*A entrada está no meio*/
		else
		{
			nova_entrada->prox = prox;
			ant->prox = nova_entrada;
		}
	}
}

/*Recebe o vetor int caso exita o registro na tabela*/
int* receber_daTabela(tabela_hash* tabela_hash, char* key)
{
	int valor_key = 0;
	entrada_tabela *entrada;

	valor_key = key_Function(tabela_hash, key);

	/*Inicia a busca*/
	entrada = tabela_hash->tabela[valor_key];

	for(;entrada != NULL && entrada->key != NULL && strcmp(key, entrada->key) != 0;)
		entrada = entrada->prox;

	/*Se não encontrar nada*/
	if(entrada == NULL || entrada->key == NULL || strcmp(key, entrada->key) != 0)
		return NULL;
	else
		return entrada->valor;
}

/*Destroi uma tabela hash*/
void destruir_Tabela(tabela_hash *tabela)
{
    int i;
    for (i = 0; i < tabela->tamanho; ++i)
		destruir_entradas(tabela->tabela[i]);
		
	free(tabela->tabela);
	free(tabela);
	tabela = NULL;
}

/*Destroi as entradas de uma tabela hash*/
void destruir_entradas(entrada_tabela* entrada)
{	
	if(entrada == NULL)
		return;
	if(entrada->prox != NULL)
		destruir_entradas(entrada->prox);
		
	free(entrada->valor);
	free(entrada->key);
	free(entrada);
	entrada = NULL;
}
