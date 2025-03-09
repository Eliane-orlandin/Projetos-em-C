#include <stdio.h>

int main()
{

    float num1, num2, resultado;
    int operador;

    printf("Digite o primeiro número: \n");
    scanf("%f", &num1);

    printf("Digite o segundo número: \n");
    scanf("%f", &num2);

    printf("Selecione o operador:\n");
    printf("1 - Adição\n"
           "2 - Subtração\n"
           "3 - Multiplicação\n"
           "4 - Divisão\n");
    scanf("%d", &operador);

    switch (operador)
    {
    case 1:
        resultado = num1 + num2;
        printf("Resultado: %.2f + %.2f = %.2f\n", num1, num2, resultado);
        break;

    case 2:
        resultado = num1 - num2;
        printf("Resultado: %.2f - %.2f = %.2f\n", num1, num2, resultado);
        break;
    case 3:
        resultado = num1 * num2;
        printf("Resultado: %.2f * %.2f = %.2f\n", num1, num2, resultado);
        break;
    case 4:
        resultado = num1 / num2;
        printf("Resultado: %.2f / %.2f = %.2f\n", num1, num2, resultado);
        break;

    default:
        printf("Operador inválido");
        break;
    }

    return 0;
}