/*
    Proyecto 2 - Automata Finito (FSA)
    Materia: Compiladores
    Descripcion: automata que verifica si un numero es valido
    segun la expresion regular del slide 13

    numeros validos: 42, 3.14, 1E10, 3.14E+2, 0.5E-3
    numeros invalidos: .5, 3., 3E, E10, etc
*/

#include <iostream>
#include <string>
#include <map>

using namespace std;

/*
    estados del automata:
    0 = inicio
    1 = leyendo digitos enteros (acepta)
    2 = leyo el punto
    3 = leyendo digitos decimales (acepta)
    5 = leyo la E del exponente
    6 = leyo el signo + o -
    7 = leyendo digitos del exponente (acepta)
   -1 = error
*/

// funcion que dice que tipo de caracter es
string getCategoria(char c)
{
    if (c >= '0' && c <= '9')
        return "digit";
    if (c == '.')
        return "dot";
    // acepta E mayuscula y minuscula
    if (c == 'E' || c == 'e')
        return "E";
    if (c == '+' || c == '-')
        return "sign";
    // cualquier otro caracter no es valido
    return "other";
}

// aqui construyo la tabla de transiciones
// uso map<int, map<string, int>> porque es facil de manejar
// tabla[estadoActual][categoria] = estadoSiguiente
map<int, map<string, int>> crearTabla()
{
    map<int, map<string, int>> tabla;

    // estado 0: solo acepta digitos para empezar
    tabla[0]["digit"] = 1;

    // estado 1: puede seguir con digitos, punto o E
    tabla[1]["digit"] = 1;
    tabla[1]["dot"] = 2;
    tabla[1]["E"] = 5;

    // estado 2: despues del punto debe haber un digito
    tabla[2]["digit"] = 3;

    // estado 3: puede seguir con mas digitos o la E
    tabla[3]["digit"] = 3;
    tabla[3]["E"] = 5;

    // estado 5: despues de E puede ir signo o digito
    tabla[5]["sign"] = 6;
    tabla[5]["digit"] = 7;

    // estado 6: despues del signo debe haber digito
    tabla[6]["digit"] = 7;

    // estado 7: puede seguir con mas digitos del exponente
    tabla[7]["digit"] = 7;

    return tabla;
}

// revisa si el estado es de aceptacion
bool esEstadoFinal(int estado)
{
    // solo estos 3 estados son validos al terminar
    return estado == 1 || estado == 3 || estado == 7;
}

// funcion principal que corre el automata letra por letra
bool validarNumero(string entrada, map<int, map<string, int>> tabla)
{
    int estado = 0; // siempre empezamos en 0

    for (int i = 0; i < (int)entrada.size(); i++)
    {
        string cat = getCategoria(entrada[i]);

        // busco si el estado actual existe en la tabla
        map<int, map<string, int>>::const_iterator it1 = tabla.find(estado);

        // si el estado no existe en la tabla es error
        if (it1 == tabla.end())
        {
            estado = -1;
            break;
        }

        // busco si hay transicion con esa categoria
        map<string, int>::const_iterator it2 = it1->second.find(cat);

        // si no hay transicion para ese caracter es error
        if (it2 == it1->second.end())
        {
            estado = -1;
            break;
        }

        // si encontre la transicion, avanzo al siguiente estado
        estado = it2->second;
    }

    return esEstadoFinal(estado);
}

// imprime la tabla bonito en consola aqui nos ayudamos de claude
void imprimirTabla(map<int, map<string, int>> tabla)
{
    string cats[] = {"digit", "dot", "E", "sign", "other"};
    int estados[] = {0, 1, 2, 3, 5, 6, 7};

    cout << "\n--- Tabla de Transiciones ---\n";
    cout << "Estado\tdigit\tdot\tE\tsign\tother\tAcepta?\n";
    cout << "----------------------------------------------------\n";

    for (int i = 0; i < 7; i++)
    {
        int e = estados[i];
        cout << "  " << e << "\t";

        for (int j = 0; j < 5; j++)
        {
            map<int, map<string, int>>::const_iterator it1 = tabla.find(e);
            if (it1 != tabla.end())
            {
                map<string, int>::const_iterator it2 = it1->second.find(cats[j]);
                if (it2 != it1->second.end())
                    cout << it2->second << "\t";
                else
                    cout << "-\t";
            }
            else
            {
                cout << "-\t";
            }
        }
        cout << (esEstadoFinal(e) ? "SI" : "no") << "\n";
    }
    cout << "\n";
}

// prueba un numero y muestra si es valido o no
void probar(string token, map<int, map<string, int>> tabla)
{
    bool resultado = validarNumero(token, tabla);

    cout << "  " << token;
    for (int i = token.size(); i < 15; i++)
        cout << " ";

    if (resultado)
        cout << "-> VALIDO\n";
    else
        cout << "-> INVALIDO\n";
}

int main()
{
    map<int, map<string, int>> tabla = crearTabla();

    imprimirTabla(tabla);

    cout << "=== Casos de prueba ===\n\n";

    cout << "// validos\n";
    probar("0", tabla);
    probar("42", tabla);
    probar("3.14", tabla);
    probar("100", tabla);
    probar("1E10", tabla);
    probar("3E+2", tabla);
    probar("3E-2", tabla);
    probar("3.14E10", tabla);
    probar("3.14E+10", tabla);
    probar("0.5E-3", tabla);

    cout << "\n// invalidos\n";
    probar("", tabla);
    probar(".", tabla);
    probar(".5", tabla);
    probar("3.", tabla);
    probar("3E", tabla);
    probar("3E+", tabla);
    probar("3.1.4", tabla);
    probar("12abc", tabla);
    probar("E10", tabla);
    probar("3.14E10.5", tabla);

    // modo interactivo para probar manualmente
    cout << "\nEscribe un numero para validar (Ctrl+Z para salir):\n";
    string input;
    while (cout << "> " && getline(cin, input))
    {
        probar(input, tabla);
    }

    return 0;
}