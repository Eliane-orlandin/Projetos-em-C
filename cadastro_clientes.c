#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Cliente
{
    char nome[100];
    char email[100];
};

int main()
{
    struct Cliente clientes[100];
    int opcao, numClientes = 0;

    do
    {
        printf("\n========= MENU ========\n");
        printf("1 - Adicionar cliente\n");
        printf("2 - Visualizar clientes\n");
        printf("3 - Editar cliente\n");
        printf("4 - Excluir cliente\n");
        printf("5 - Sair\n");
        printf("=======================\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        getchar(); // Limpa o buffer do scanf para evitar problemas com fgets.

        switch (opcao)
        {
        case 1:
        {
            if (numClientes >= 100)
            {
                printf("Limite de clientes atingido!\n");
                break;
            }

            printf("Digite o nome do cliente: ");
            fgets(clientes[numClientes].nome, sizeof(clientes[numClientes].nome), stdin);
            clientes[numClientes].nome[strcspn(clientes[numClientes].nome, "\n")] = '\0'; // Remove o \n do final

            printf("Digite o e-mail do cliente: ");
            fgets(clientes[numClientes].email, sizeof(clientes[numClientes].email), stdin);
            clientes[numClientes].email[strcspn(clientes[numClientes].email, "\n")] = '\0'; // Remove o \n do final

            numClientes++;
            printf("Cliente adicionado com sucesso!\n");
            break;
        }

        case 2:
        {
            if (numClientes == 0)
            {
                printf("Nenhum cliente cadastrado.\n");
                break;
            }

            printf("\n=== Clientes Cadastrados ===\n");
            printf("+-----+-------------------------------------+-----------------------------------+\n");
            printf("| #   | Nome                                | E-mail                            |\n");
            printf("+-----+-------------------------------------+-----------------------------------+\n");

            for (int i = 0; i < numClientes; i++)
            {
                printf("| %-3d | %-35s | %-33s |\n", i + 1, clientes[i].nome, clientes[i].email);
            }

            printf("+-----+-------------------------------------+-----------------------------------+\n");
            break;
        }

        case 3:
        {
            if (numClientes == 0)
            {
                printf("Nenhum cliente para editar.\n");
                break;
            }

            char nome[100];
            printf("Digite o nome do cliente que deseja editar: ");
            fgets(nome, sizeof(nome), stdin);
            nome[strcspn(nome, "\n")] = '\0'; // Remove o \n do final

            int encontrado = 0;
            for (int i = 0; i < numClientes; i++)
            {
                if (strcmp(clientes[i].nome, nome) == 0)
                {
                    printf("Digite o novo nome do cliente: ");
                    fgets(clientes[i].nome, sizeof(clientes[i].nome), stdin);
                    clientes[i].nome[strcspn(clientes[i].nome, "\n")] = '\0';

                    printf("Digite o novo e-mail do cliente: ");
                    fgets(clientes[i].email, sizeof(clientes[i].email), stdin);
                    clientes[i].email[strcspn(clientes[i].email, "\n")] = '\0';

                    printf("Cliente editado com sucesso!\n");
                    encontrado = 1;
                    break;
                }
            }

            if (!encontrado)
            {
                printf("Cliente não encontrado!\n");
            }
            break;
        }

        case 4:
        {
            if (numClientes == 0)
            {
                printf("Nenhum cliente para excluir.\n");
                break;
            }

            char nome[100];
            printf("Digite o nome do cliente que deseja excluir: ");
            fgets(nome, sizeof(nome), stdin);
            nome[strcspn(nome, "\n")] = '\0';

            int encontrado = 0;
            for (int i = 0; i < numClientes; i++)
            {
                if (strcmp(clientes[i].nome, nome) == 0)
                {
                    for (int j = i; j < numClientes - 1; j++)
                    {
                        clientes[j] = clientes[j + 1];
                    }

                    numClientes--;
                    printf("Cliente excluído com sucesso!\n");
                    encontrado = 1;
                    break;
                }
            }

            if (!encontrado)
            {
                printf("Cliente não encontrado!\n");
            }
            break;
        }

        case 5:
            printf("Encerrando o programa...\n");
            break;

        default:
            printf("Opção inválida! Tente novamente.\n");
            break;
        }
    } while (opcao != 5);

    return 0;
}
