/*
- Autores. Ing(c) Santiago Ramirez Giraldo, Kenneth Santiago Rodriguez Ramirez & Yigal Fabricio Rojas Acevedo
- date de versión: 17/05/2024
- Versión del código: 
- Nombre del compilador utilizado: gcc
- Versión del compilador: 8.1.0
- Presentado a: Doctor Ricardo Moreno Laverde
- Universidad Tecnológica de Pereira
- Programa de Ingeniería de Sistemas y Computación
- Un descriptivo de que hace el programa: permite acceso con un modulo de usuarios con “username” y “password” para cada usuario, registrando el “log” de acceso a todos los usuarios y luego permite manejar
un modulo que permite graficar una función polinómica del tipo f(x).
- Salvedades: 
*/

#include <SDL.h> //Libreria que nos permite manejar graficos como ventanas
#include <SDL_ttf.h> // Libreria que nos permite manejar texto que queramos mostrar en pantalla
#include <stdio.h>   // Libreria estandar para input y outpunt de c
#include <math.h>   // libreria que nos permite realizar operaciones matematematicas
#include <stdlib.h> // libreria de c , de ella utilizamos funciones como Atoi, size_t y la constante NULL
#include <time.h> // libreria que nos permite acceder al tiempo de la maquina donde se ejecuta el programa
#include <string.h> // Libreria que nos expande el uso y las posibilidades de manipulacion para cadenas de Chars
#include <ctype.h> // Libreria para cadenas en este codigo usada la funcion toLower
#include <stdbool.h>// Libreria para usar expresiones como true y false

#define WINDOW_WIDTH 790 // Definimos el ancho de la ventana
#define WINDOW_HEIGHT 640 // Definimos la altura de la ventana

#pragma pack(push, 1) // Guardar el estado actual de alineación y establecer la alineación en 1 byte

typedef struct 
{
    unsigned short int userKey;// Numero unico de usuario
    char userName[20];// Nombre del usuario
    char password[13];//clave del usuario
} USER; //Fin definicion creacion estructura USER

typedef struct { 
    char userName[ 20 ];// Nombre del usuario 
    char graphicKey[ 5 ];
    char function[ 30 ];
    int lowerBound;
    int upperBound;
    int increment;
    char date[ 11 ];
} GRAPHICS;

#pragma pack(pop) // Restaurar el estado de alineación guardado

//prototipado de las funciones
int CompareUsers( USER *struct0, USER *struct1, char *field );
void ReadUsersFile( USER vector[] );
void WriteUsersFile( USER vector[], int numberRegisters );
void OrganizeUsersFile ();
int binarySearchUserName ( char userNameToSearch[], USER* result );
int GetUserKey ( );
void AddUserToFile( USER user );
void EncryptPasswordWordKey( USER *userToEncryptPassword, char keyWord[] );
void EncryptPasswordDisplacement( USER *userToEncryptPassword, int displacement );
void EncryptPassword ( USER *user );
int ComparePassword ( char passwordEntered[], USER originalUser );
int DeleteUser ( char userName[] );
void ChangePassword ( char userName[], char newPassword[] );

void AddGraphicToFile ( char userName[], char function[], int lowerBound, int upperBound, int increment );
void OrganizeGraphicsFile ( char fieldToSort[] );
void WriteGraphicsFile( GRAPHICS vector[], int numberRegisters );
void ReadGraphicsFile( GRAPHICS vector[] );
void BubbleSortGraphics( GRAPHICS originalStruct[], int numberRegisters, char *field );
void SwapGraphicsStructs( GRAPHICS *struct0, GRAPHICS *struct1 );
int CompareGraphics( GRAPHICS *struct0, GRAPHICS *struct1, char *field );
void GetGraphicKey( GRAPHICS *graphic );
int GetNumberGraphicKey ( );
void CreateDate( GRAPHICS *graphic );

int BinarySearchUserNameInGraphics ( char userNameToSearch[], GRAPHICS* result );
int GetQuantityUserGraphics ( char userName[] );
void GetUserGraphicsVector ( char userName[], GRAPHICS userGraphicsVector[] );

int DeleteAllUserGraphics ( char userName[] );
int DeleteGraphic ( char userName[], char graphicKey[] );
int BinarySearchGraphicKey ( char graphicKey[], GRAPHICS* result );

void createText(SDL_Renderer *renderer, TTF_Font *font, SDL_Color color, char string[], int posX , int posY);


//Funcion para comparar dos mismos elementos de un un campo en un array de estructuras USER
/*struct0: primera estructura para comparar, struct1: ssegunda estructura para comparar, field: campo en el que se comparara*/
int CompareUsers( USER *struct0, USER *struct1, char *field ) {
    /*se comparara el campo userKey como numeros y el campo userName como cadenas. 
    NOTA: EN EL PARAMETRO field SOLO SE PUEDE PONER "userKey" O "userName", DE LO CONTRARIO SE OCASIONARAN ERRORES*/
    if ( strcmp( field, "userKey" ) == 0 )
        return ( struct0->userKey > struct1->userKey ) ? 1 : -1;
    else if ( strcmp( field, "userName" ) == 0 ) 
        return strcmp( struct0->userName, struct1->userName );
    else
        //en caso de error retornara un -1000
        return -1000;
    //end if ( strcmp( field, "userKey" ) == 0 )
}// end function  CompareUsers( USER *struct0, USER *struct1, char *field )


//Funcion para intercambiar estructuras en el metodo de ordenamiento por burbuja
/*struct0: primera estructura a cambiar, struct1: segunda estructura a cambiar*/
void SwapUsersStructs( USER *struct0, USER *struct1 ) {
    //temporarystruct: estructura temporal para realizar el intercambio del algoritmo
    USER temporarystruct = *struct0;
    *struct0 = *struct1;
    *struct1 = temporarystruct;
    return;
}

//Funcion que realiza el ordenamiento de el array de usuario por metodo de burbuja 
/*originalStruct: array de estructura USER original, numberRegisters: numero de registros que existen, field: campo por el que se organizara*/
void BubbleSortUsers( USER originalStruct[], int numberRegisters, char *field ) {
    //counter1: contador de veces que dará la vuelta al vector para ordenarlo
    for ( int counter1 = 0; counter1 < numberRegisters; counter1++ ) 
        //counter2: contador de veces que ha pasado organizando pares en fila, de menor a mayor
        for ( int counter2 = 0; counter2 < numberRegisters; counter2++ ) 
            if ( CompareUsers( &originalStruct[ counter2 ], &originalStruct[ counter2 + 1 ], field ) > 0 ) 
                SwapUsersStructs( &originalStruct[ counter2 ], &originalStruct[ counter2 + 1 ] );
            //end  if ( CompareUsers( &originalStruct[ counter2 ], &originalStruct[ counter2 + 1 ], field ) > 0 ) 
        //end for ( int counter2 = 0; counter2 < numberRegisters; counter2++ ) 
    //end for ( int counter1 = 0; counter1 < numberRegisters; counter1++ ) 
    return;
}



/*Funcion para leer los usuarios de un vecor
vector[]: array/vector donde se guardara los registros */
void ReadUsersFile( USER vector[] ) {
    //archivo donde se encuentra el registro de usuarios
    FILE *file = fopen( "users.txt", "rb" );
    //contador para ir agregando registros al array
    int counter = 0;
    while ( fread( &vector[ counter ], sizeof( USER ), 1, file ) == 1 ) 
        counter++;
    //end while ( fread( &vector[ counter ], sizeof( USER ), 1, file ) == 1 ) 
    fclose( file );
    return;
}//end function ReadUsersFile( USER vector[] )

/*Funcion para escribir los usuarios desde un vector que ya fue ordenado por x campo
vector[]: array/vector de donde salen los registros de usuarios, numberRegisters: numero de registros a escribir */
void WriteUsersFile( USER vector[], int numberRegisters ) {
    //archivo donde se encontraran los registro de usuarios
    FILE *file = fopen( "users.txt", "wb" );
    fwrite( vector, sizeof( USER ), numberRegisters, file );
    fclose( file );
    return;
}//end function void WriteUsersFile( USER vector[], int numberRegisters )

/*Funcion para organizar el archivo de usuarios*/
void OrganizeUsersFile () {
    /*sizeStruct: tamaño de la estructura que se leera, numberRegisters: numero de resgitros en el archivo*/
    int sizeFile = 0, numberRegisters = 0;
    //archivo donde se sacara el tamaño del mismo
    FILE *file = fopen( "users.txt", "rb" );
    fseek ( file, 0, SEEK_END );
    sizeFile = ftell ( file );
    numberRegisters = sizeFile / sizeof ( USER );
    fclose( file );

    //vector/array donde estaran los registros de usuarios para su ordenamiento
    USER usersVector[ numberRegisters ];

    //fieldToSort: campo a ordenar el archivo users.txt
    char fieldToSort[] = "userName";

    ReadUsersFile( usersVector );
    BubbleSortUsers ( usersVector, numberRegisters - 1, fieldToSort );
    WriteUsersFile( usersVector, numberRegisters );

    return;
}// end function OrganizeUsersFile ()

/*funcion para buscar el UserName con el algoritmo  de busqueda binaria
userNameToSearch: nombre a busacr, result: estructura donde se cambiara lo que este por el registro del nombre de usuario que se estaba buscando*/
int binarySearchUserName ( char userNameToSearch[], USER* result ) {
    //Se organizan los usuarios de forma alfabetica por si se salto este proceso previamente
    OrganizeUsersFile();
    //archivo sobre el que se buscara
    FILE *file = fopen( "users.txt", "rb" );

    fseek( file, 0, SEEK_END );
    //fileSize: tamaño del archivo tomado gracias a ftell
    int fileSize = ftell( file );
    //numero de registros en el archivo
    int numberRegisters = fileSize / sizeof( USER );

    //indicador de izquierda para buscar
    int left = 0;
    //indicador de derecha para buscar
    int right = numberRegisters - 1;

    while ( left <= right ) {
        // indicador de mitad 
        int half = left + ( right - left ) / 2;
        // indicador de posicion DENTRO DEL ARCHIVO
        int position = half * sizeof(USER);

        fseek( file, position, SEEK_SET );
        //usuario que se leera ya que es la "mitad" 
        USER user;
        fread( &user, sizeof( USER ), 1, file );

        // comparacion de lo que se busca y de la "mitad"
        int compareResult = strcmp( user.userName, userNameToSearch );

        // revision de si ya fue encontrado
        if ( compareResult == 0 ) {
            fclose( file );
            *result = user;
            return half;
        }//end if ( compareResult == 0 )

        // Si el objetivo es mayor, se ignora la mitad izquierda
        if ( compareResult < 0 )
            left = half + 1;
        // Si el objetivo es menor, se ignora la mitad derecha
        else
            right = half - 1;
        //end if ( compareResult < 0 )
    }// end while ( left <= right )

    fclose( file );

    // Retorna -1 si no se encontro
    return -1;
}//end function binarySearchUserName ( char userNameToSearch[], USER* result )

/*Funcion para obtener la llave unica de usuario*/
int GetUserKey ( ) {
    FILE *file = fopen ( "users.txt", "rb" );
    if ( file == NULL )
        return 0;
    //end if if ( file == NULL )
    fseek( file, 0, SEEK_END );
    //tamaño del archivo
    int fileSize = ftell( file );
    //llave dewl usuario
    int userKey = fileSize / sizeof( USER );
    return userKey;
}// end function  GetUserKey ( );

/*Funcion para añadir usuario a la tabla ( archivo)
user: usuario a añadir*/
void AddUserToFile( USER user ) {
    //archivo donde se agregara el usuario
    FILE *file = fopen ( "users.txt", "ab" );
    fwrite ( &user, sizeof ( user ), 1, file );
    fclose ( file );
    return;
}//end function AddUserToFile( USER user )


/*Funcion para revisar si la contraseña ingresada es igual a la del usuario existente
passwordEntered: contraseña ingresada a la hora de iniciar sesion, originalUser: usuario dueño de userName ingresado*/
int ComparePassword ( char passwordEntered[], USER originalUser ) {
    int passwordSize = sizeof ( originalUser.password );
    for ( int position = 0; position < passwordSize ; position++ )
        if ( passwordEntered[ position ] != originalUser.password[ position ] )
            return -1; //retorna -1 si la contraseña NO ES igual
        //end if ( passwordEntered[ position ] != originalUser.password[ position ] )
    //end for ( int position = 0; position < passwordSize ; position++ )
    return 0;//retorna 0 si la contraseña ES igual
}// end function ComparePassword ( char passwordEntered[], USER originalUser )





/*----------------------------------------------------------------------------------FUNCIONES ENCRIPTAMIENTO DE CONTRASEÑA DE USUARIO---------------------------------------------------------------------------------------------*/

/*Funcion para encriptar la contraseña mediante palabra clave
userToEncryptPassword: estructura tipo USER donde se esta ingresando la contraseña o crreando el usuario, keyWord: palabra clave con la que se encriptara*/
void EncryptPasswordWordKey( USER *userToEncryptPassword, char keyWord[] ) {
    //sizeKeyWord: cantidad caracteres de la palabra clave, sizeStringToEncrypt: cantidad caracteres palabra a encriptar
    int sizeKeyWord = strlen( keyWord ), sizeStringToEncrypt = strlen( userToEncryptPassword->password );
    //positionStringToEncrypt: contador para realizar la encriptacion en la contraseña yendo caracter por caracter
    int positionStringToEncrypt = 0;
    
    while ( positionStringToEncrypt < sizeStringToEncrypt ) {
        //character: caracter tomado de la contraseña para revisar si es igual a algun caracter de la palabra clave
        int character = userToEncryptPassword->password[positionStringToEncrypt];
        //positionKeyWord: contador para ir yendo en la cadena de la palabra clave para ver si cocinicde con la de la cadena a encriptar
        for ( int positionKeyWord = 0; positionKeyWord < sizeKeyWord; positionKeyWord++ ) 
            if ( character == keyWord[positionKeyWord] ) 
                userToEncryptPassword->password[positionStringToEncrypt] = positionKeyWord; // actualizacion de la contraseña en la estructura
            // end if ( character == keyWord[positionKeyWord] )
        //end for ( int positionKeyWord = 0; positionKeyWord < sizeKeyWord; positionKeyWord++ ) 
        positionStringToEncrypt++;
    }// end while ( positionStringToEncrypt < sizeStringToEncrypt )
    
    return;
}//end function EncryptPaaswordWordKey( USER *userToEncryptPassword, char keyWord[])

/*Funcion para encriptar la contraseña mediante desplazamiento
userToEncryptPassword: estructura tipo USER donde se esta ingresando la contraseña o creando el usuario, displacement: desplazamiento que se aplicara*/
void EncryptPasswordDisplacement( USER *userToEncryptPassword, int displacement ) {
    //sizeStringToEncrypt: cantidad caracteres palabra a encriptar
    int sizeStringToEncrypt = sizeof ( userToEncryptPassword->password );

    //positionStringToEncrypt: contador para realizar la encriptacion en la contraseña yendo caracter por caracter
    for ( int positionStringToEncrypt = 0; positionStringToEncrypt < sizeStringToEncrypt; positionStringToEncrypt++ ) {
        //character: caracter que le aumentara el desplazamiento
        char character = userToEncryptPassword->password[positionStringToEncrypt];
        userToEncryptPassword->password[positionStringToEncrypt] = character + displacement;// actualizacion de la contraseña en la estructura
    }//end for ( int positionStringToEncrypt = 0; positionStringToEncrypt < sizeStringToEncrypt; positionStringToEncrypt++ )
    
    return;
}//end function EncryptPaaswordDisplacement( USER *userToEncryptPassword, int displacement )


/*Funcion para encriptar contraseñas de los usuarios
user: usuario al cual se le encriptara la contraseña*/
void EncryptPassword ( USER *user ) {
    //archivo “seguridad.gra” donde se guardara el valor desplazamiento y palabra clave que fueron aplicadas a las contraseñas
    FILE* file;
    //keyWord: palabra clave con la que se encriptara
    char keyWord[22] = "yisankedEgrINpumcJotA";
    //displacement: desplazamiento con el que encriptara
    int displacement = 18;
    EncryptPasswordWordKey ( user, keyWord );
    EncryptPasswordDisplacement ( user, displacement );
    //se abre "seguridad.gra" para despues comparar la ultima 
    file = fopen ( "seguridad.gra", "rb" );
    //si el archivo "seguridad.gra" no existe entonces se crea
    if ( file == NULL ) {
        file = fopen ( "seguridad.gra", "wb" );
        fwrite ( &displacement, sizeof ( displacement ), 1, file );
        fwrite ( &keyWord, sizeof ( keyWord ), 1, file );
        fclose ( file );
        return;
    }
    fclose ( file );
    return;
}//end function EncryptPassword ( USER *user )

/*----------------------------------------------------------------------------------FIN FUNCIONES ENCRIPTAMIENTO DE CONTRASEÑA DE USUARIO---------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------FUNCIONES PARA BORRAR UN USUARIO------------------------------------------------------------------------------------*/

/*Funcion para en un arreglo de estructuras GRAPHICS se actualice la memoria y actualice todas las graficas de un usuario como borradas
userName: usuario al que se le borraran esats graficas, graphicsVector: vector/arreglo de todos los registros, numberRegisters: cantidad de registros existentes */
int MarkAsEliminateUserGraphics ( char userName[], GRAPHICS graphicsVector[], int numberRegisters ) {
    //estructura necesaria para llamar a BinarySearchUserNameInGraphics
    GRAPHICS graphic;
    //posicion enla que se encuentra una de sus graficas
    int indexGraphicsUserVector = BinarySearchUserNameInGraphics ( userName, &graphic );
    //se retorna un -1 en caso de que no tenga graficas y para que pare el proceso
    if ( indexGraphicsUserVector == -1 )
        return -1;

    //se cambia el priemr registro de grafica
    graphicsVector[ indexGraphicsUserVector ].userName[0] = '!';
    

    //se busca hacia atras de forma secuencial si hay mas graficas para eliminar, sirve ya que esta ordenado por nombre de usuario
    for ( int forIndex = indexGraphicsUserVector - 1; forIndex != -1 && strcmp ( graphicsVector[ forIndex ].userName, userName ) == 0 ; forIndex--  )
        graphicsVector[ forIndex ].userName[0] = '!';
    //end for ( int forIndex = indexGraphicsUserVector - 1; forIndex != -1 && strcmp ( graphicsVector[ forIndex ].userName, userName ) == 0 ; forIndex--  )

    //se busca hacia adelante de forma secuencial si hay mas graficas para eliminar
    for ( int forIndex = indexGraphicsUserVector + 1; forIndex != numberRegisters && strcmp ( graphicsVector[ forIndex ].userName, userName ) == 0 ; forIndex++  )
        graphicsVector[ forIndex ].userName[0] = '!';
    //end for ( int forIndex = indexGraphicsUserVector + 1; forIndex != numberRegisters && strcmp ( graphicsVector[ forIndex ].userName, userName ) == 0 ; forIndex++  )
    
    return 0;
}


/*Funcion que eliminara todas las graficas de un usuario dado
userName: usuario al que se le borraran las graficas*/
int DeleteAllUserGraphics ( char userName[] ) {
    //GetNumberGraphicKey realmente retorna el numero de registros creados, por lo que sirve para saber este dato
    int numberRegisters = GetNumberGraphicKey();

    //vector/array donde estaran los registros de graficos para su ordenamiento
    GRAPHICS graphicsVector[ numberRegisters ];

    //fieldToSort: campo a ordenar el archivo graphics.txt
    char fieldToSort[] = "userName";

    ReadGraphicsFile( graphicsVector );
    //se marcan todas las graficas del usuario 
    MarkAsEliminateUserGraphics ( userName, graphicsVector, numberRegisters );
    //se organiza el archivo
    BubbleSortGraphics ( graphicsVector, numberRegisters - 1, fieldToSort );
    //se escribe le archivo con el cambio hecho,
    WriteGraphicsFile( graphicsVector, numberRegisters );
    //retorna 0 ya que el borrado fue exitoso
    return 0;
}//end function DeleteAllUserGraphics ( char userName[] )

/*Funcion para eliminar un usuario
userName: nombre del usuario a borrar*/
int DeleteUser ( char userName[] ) {
    //estructura donde quedara el usuario  a borrar
    USER user;
    //posicion en la que estara el registro a borrar y guardar el registro en user
    int userPosition = binarySearchUserName ( userName, &user );
    //en dado caso que el uuario no exista se retornara un -1
    if ( userPosition == -1 ) 
        return -1;
    
    //se cambia el primer caracter del nombre que originalmente era un ' ' y ahora s eva a volver un '!' ya que '!'>' ' entonces este ira al final del registro cuando se ordene
    user.userName[0] = '!';
    //GetUserKey realmente retorna el numero de registros creados, por lo que sirve para saber este dato
    int numberRegisters = GetUserKey();

    //vector/array donde estaran los registros de usuarios para su ordenamiento
    USER usersVector[ numberRegisters ];

    //fieldToSort: campo a ordenar el archivo users.txt
    char fieldToSort[] = "userName";

    ReadUsersFile( usersVector );
    //se cambia el usuario orignal por el nuevo que contiene un '!'
    usersVector[ userPosition ] = user;
    //se organiza el archivo
    BubbleSortUsers ( usersVector, numberRegisters - 1, fieldToSort );
    //se escribe le archivo con el cambio hecho
    WriteUsersFile( usersVector, numberRegisters );
    //Se borran todas las graficas del usuario
    DeleteAllUserGraphics ( userName );
    //retorna 0 ya que el borrado fue exitoso
    return 0;
}//END funcion DeleteUser ( char userName[] )

/*------------------------------------------------------------FUNCION PARA CAMBIAR CONTRASEÑA DE UN USUARIO----------------------------------------------------------------------------------------*/

/*Funcion para cambiar la contraseña de unusuario
userName: Nombre el usuario a cambiar contarseña, newPassword: nueva contraseña que se pondra
NOTA: Esta funcion ASUME que el usuario SI EXISTE por lo que es importante rectificar esto antes de enviar esta funcion.
Si el cambio de contraseña es para el usuario root debe enviarse tal cual en userName de este, de lo contrario habra errores en la busqueda binaria*/
void ChangePassword ( char userName[], char newPassword[] ){
    //estructura donde quedara el usuario a cambiar contraseña
    USER user;
    //posicion en la que estara el registro a cambiar contraseña y guardar el registro en user
    int userPosition = binarySearchUserName ( userName, &user );
    //se copia la contraseña nueva en el usuario al que le cambiara 
    memcpy ( user.password, newPassword, sizeof ( user.password ) );

    //GetUserKey realmente retorna el numero de registros creados, por lo que sirve para saber este dato
    int numberRegisters = GetUserKey();

    //vector/array donde estaran los registros de usuarios para su ordenamiento
    USER usersVector[ numberRegisters ];

    //fieldToSort: campo a ordenar el archivo users.txt
    char fieldToSort[] = "userName";

    EncryptPassword(&user);
    ReadUsersFile( usersVector );
    //se cambia el usuario orignal por el nuevo que contiene la nueva contraseña
    usersVector[ userPosition ] = user;
    //se organiza el archivo
    BubbleSortUsers ( usersVector, numberRegisters - 1, fieldToSort );
    //se escribe le archivo con el cambio hecho
    WriteUsersFile( usersVector, numberRegisters );

    return;
}//end funcion void ChangePassword ( char userName[], char newPassword[] )


/*------------------------------------------------------------FUNCIONES PARA AGREGAR GRAFICO A EL ARCHIVO----------------------------------------------------------------------------------------*/

/*Funcion para crear la date actual en el sistema que se ejecuta
graphic: estructura GRAPHICS en la cual se pondra la date actual*/
void CreateDate( GRAPHICS *graphic ) {
    // Obteniendo la date y hora actual
    // currentTime: almacena la hora actual del sistema en forma de un valor de tiempo.
    time_t currentTime;
    // timeInformation: Almacena la información desglosada de la date e icnluso la hora en la estructura pereteneciente al lenguaje llamada tm
    struct tm *timeInformation;

    // Obteniendo el tiempo actual del SISTEMA 
    time( &currentTime );
    // Convierte el tiempo a la estructura de date y hora local
    timeInformation = localtime( &currentTime );

    // Formatea la date actual
    strftime( graphic->date, sizeof( graphic->date ), "%Y-%m-%d", timeInformation );

    return;
}// end function CreateDate( GRAPHICS *graphic )



/*Funcion para obtener el numero de la llave unica de un grafico ANTED DE VOLVERLA STRING*/
int GetNumberGraphicKey ( ) {
    FILE *file = fopen ( "graphics.txt", "rb" );
    if ( file == NULL )
        return 0;
    //end if if ( file == NULL )
    fseek( file, 0, SEEK_END );
    //tamaño del archivo
    int fileSize = ftell( file );
    //llave del grafico
    int graphicKey = fileSize / sizeof( GRAPHICS );
    return graphicKey;
}// end function GetGraphicKey ( )

/*Funcion para obtener el verdadero graphicKey como un string
graphic: estructura del grafico al cual se le dara esta llave*/
void GetGraphicKey( GRAPHICS *graphic ) {
    // number: numero el que se le convertira a string con ceros a la izquierda
    int number = GetNumberGraphicKey (); 
    
    // string al cual se le agregara el numero y los 0 a la izquierda
    char graphicKey[5];
    
    // se usa sprintf para convertir el numero a cadena con ceros a la izquierda
    sprintf( graphicKey, "%04d", number );
    //copia el string con 0 a la izquierda a el graphicKey de la estructura
    strcpy ( graphic->graphicKey, graphicKey );
    
    return;
}//end function GetGraphicKey( GRAPHICS *graphic )


//Funcion para comparar dos mismos elementos de un un campo en un array de estructuras GRAPHICS
/*struct0: primera estructura para comparar, struct1: ssegunda estructura para comparar, field: campo en el que se comparara*/
int CompareGraphics( GRAPHICS *struct0, GRAPHICS *struct1, char *field ) {
    /*se comparara los campos "date" y "userName" viendo que cadena es "mayor" o "menor"
    NOTA: EN EL PARAMETRO field SOLO SE PUEDE PONER "date", "graphicKey" O "userName", DE LO CONTRARIO SE OCASIONARAN ERRORES*/
    if ( strcmp( field, "date" ) == 0 )
        return strcmp( struct0->date, struct1->date );
    else if ( strcmp( field, "userName" ) == 0 ) 
        return strcmp( struct0->userName, struct1->userName );
    else if ( strcmp( field, "graphicKey" ) == 0 ) 
        return strcmp( struct0->graphicKey, struct1->graphicKey );
    else
        //en caso de error retornara un -1000
        return -1000;
    //end if ( strcmp( field, "date" ) == 0 )
}// end function CompareGraphics( GRAPHICS *struct0, GRAPHICS *struct1, char *field )


//Funcion para intercambiar estructuras en el metodo de ordenamiento por burbuja
/*struct0: primera estructura a cambiar, struct1: segunda estructura a cambiar*/
void SwapGraphicsStructs( GRAPHICS *struct0, GRAPHICS *struct1 ) {
    //temporarystruct: estructura temporal para realizar el intercambio del algoritmo
    GRAPHICS temporarystruct = *struct0;
    *struct0 = *struct1;
    *struct1 = temporarystruct;
    return;
}//end function SwapGraphicsStructs( GRAPHICS *struct0, GRAPHICS *struct1 )

//Funcion que realiza el ordenamiento de el array de usuario por metodo de burbuja 
/*originalStruct: array de estructura GRAPHICS original, numberRegisters: numero de registros que existen, field: campo por el que se organizara*/
void BubbleSortGraphics( GRAPHICS originalStruct[], int numberRegisters, char *field ) {
    //counter1: contador de veces que dará la vuelta al vector para ordenarlo
    for ( int counter1 = 0; counter1 < numberRegisters; counter1++ ) 
        //counter2: contador de veces que ha pasado organizando pares en fila, de menor a mayor
        for ( int counter2 = 0; counter2 < numberRegisters; counter2++ ) 
            if ( CompareGraphics( &originalStruct[ counter2 ], &originalStruct[ counter2 + 1 ], field ) > 0 ) 
                SwapGraphicsStructs( &originalStruct[ counter2 ], &originalStruct[ counter2 + 1 ] );
            //end  if ( CompareGraphics( &originalStruct[ counter2 ], &originalStruct[ counter2 + 1 ], field ) > 0 )  
        //end for ( int counter2 = 0; counter2 < numberRegisters; counter2++ ) 
    //end for ( int counter1 = 0; counter1 < numberRegisters; counter1++ ) 
    return;
}// end function BubbleSortGraphics( GRAPHICS originalStruct[], int numberRegisters, char *field )


/*Funcion para leer los graficos de un vector
vector[]: array/vector donde se guardara los registros */
void ReadGraphicsFile( GRAPHICS vector[] ) {
    //archivo donde se encuentra el registro de graficos
    FILE *file = fopen( "graphics.txt", "rb" );
    //contador para ir agregando registros al array
    int counter = 0;
    while ( fread( &vector[ counter ], sizeof( GRAPHICS ), 1, file ) == 1 ) 
        counter++;
    //end while ( fread( &vector[ counter ], sizeof( GRAPHICS ), 1, file ) == 1 ) 
    fclose( file );
    return;
}//end function ReadGraphicsFile( GRAPHICS vector[] )

/*Funcion para escribir los graficos desde un vector que ya fue ordenado por x campo
vector[]: array/vector de donde salen los registros de graficos, numberRegisters: numero de registros a escribir */
void WriteGraphicsFile( GRAPHICS vector[], int numberRegisters ) {
    //archivo donde se encontraran los registro de graficos
    FILE *file = fopen( "graphics.txt", "wb" );
    fwrite( vector, sizeof( GRAPHICS ), numberRegisters, file );
    fclose( file );
    return;
}//end function WriteGraphicsFile( GRAPHICS vector[], int numberRegisters )

/*Funcion para organizar el archivo de graficos
fieldToSort: campo a ordenar el archivo graphics.txt*/
void OrganizeGraphicsFile ( char fieldToSort[] ) {
    /*sizeStruct: tamaño de la estructura que se leera, numberRegisters: numero de resgitros en el archivo*/
    int sizeFile = 0, numberRegisters = 0;
    //archivo donde se sacara el tamaño del mismo
    FILE *file = fopen( "graphics.txt", "rb" );
    fseek ( file, 0, SEEK_END );
    sizeFile = ftell ( file );
    numberRegisters = sizeFile / sizeof ( GRAPHICS );
    fclose( file );

    //vector/array donde estaran los registros de graficos para su ordenamiento
    GRAPHICS graphicsVector[ numberRegisters ];

    ReadGraphicsFile( graphicsVector );
    BubbleSortGraphics ( graphicsVector, numberRegisters - 1, fieldToSort );
    WriteGraphicsFile( graphicsVector, numberRegisters );

    return;
}// end function OrganizeUsersFile ()


/*Funcion que agregara al artchivo graphics.txt el grafico que se hara
userName: usuario que hara esta grafica, function: funcion f(x) que graficara, lowerBound: límite inferior de la funcion f(x), upperBound: llímite superior de la funcion f(x)
increment: incremento que se le hara a la funcion f(x)*/
void AddGraphicToFile ( char userName[], char function[], int lowerBound, int upperBound, int increment ) {
    GRAPHICS graphic;
    //Se agregan los datos necesarios a la estructura
    strcpy ( graphic.userName, userName );
    strcpy ( graphic.function, function );
    graphic.lowerBound = lowerBound;
    graphic.upperBound = upperBound;
    graphic.increment = increment;
    CreateDate ( &graphic );
    GetGraphicKey( &graphic );

    //archivo donde se agregara el usuario
    FILE *file = fopen ( "graphics.txt", "ab" );
    //en caso de que no se encuentre el archivo porque no existe lo abrira en escritura creandolo
    if ( file == NULL )
        file = fopen ( "graphics.txt", "wb" ); 
    //end if ( file == NULL )
    fwrite ( &graphic, sizeof ( graphic ), 1, file );
    fclose ( file );
    OrganizeGraphicsFile ( "userName" );
    return;
}//end function AddGraphicToFile ( char userName[], char function[], int lowerBound, int upperBound, int increment )

/*-----------------------------------------------------------------------FUNCIONES PARA OBTENER UN VECTOR DE ESTRUCTURA GRAPHICS ORDENADO DE LAS GRAFICAS HECHAS POR X USUARIO--------------------------------------------------*/


/*funcion para buscar el UserName en graphics.txt con el algoritmo  de busqueda binaria
userNameToSearch: nombre a busacr, result: estructura donde se cambiara lo que este por el registro del nombre de usuario que se estaba buscando*/
int BinarySearchUserNameInGraphics ( char userNameToSearch[], GRAPHICS* result ) {
    //Se organizan los usuarios de forma alfabetica por si se salto este proceso previamente
    OrganizeGraphicsFile ( "userName" );
    //archivo sobre el que se buscara
    FILE *file = fopen( "graphics.txt", "rb" );

    fseek( file, 0, SEEK_END );
    //fileSize: tamaño del archivo tomado gracias a ftell
    int fileSize = ftell( file );
    //numero de registros en el archivo
    int numberRegisters = fileSize / sizeof( GRAPHICS );

    //indicador de izquierda para buscar
    int left = 0;
    //indicador de derecha para buscar
    int right = numberRegisters - 1;

    while ( left <= right ) {
        // indicador de mitad 
        int half = left + ( right - left ) / 2;
        // indicador de posicion DENTRO DEL ARCHIVO
        int position = half * sizeof( GRAPHICS );

        fseek( file, position, SEEK_SET );
        //grafico que se leera ya que es la "mitad" 
        GRAPHICS graphic;
        fread( &graphic, sizeof( GRAPHICS ), 1, file );

        // comparacion de lo que se busca y de la "mitad"
        int compareResult = strcmp( graphic.userName, userNameToSearch );

        // revision de si ya fue encontrado
        if ( compareResult == 0 ) {
            fclose( file );
            *result = graphic;
            return half;
        }//end if ( compareResult == 0 )

        // Si el objetivo es mayor, se ignora la mitad izquierda
        if ( compareResult < 0 )
            left = half + 1;
        // Si el objetivo es menor, se ignora la mitad derecha
        else
            right = half - 1;
        //end if ( compareResult < 0 )
    }// end while ( left <= right )

    fclose( file );

    // Retorna -1 si no se encontro
    return -1;
}//end function BinarySearchUserNameInGraphics ( char userNameToSearch[], GRAPHICS* result )

/*FUNCION PARA SABER CUANTOS GRAFICOS TIENE X USER
userName: usuario al que se le averiguaran la cantidad de graficas*/
int GetQuantityUserGraphics ( char userName[] ) {
    //estructura necesaria para llamar a BinarySearchUserNameInGraphics
    GRAPHICS graphic;
    //cantidad de graficas que hay de este usuario
    int quantity = 0;
    //posicion enla que se encuentra una de sus graficas
    int index = BinarySearchUserNameInGraphics ( userName, &graphic );
    //se retrona un -1 si es que no tiene graficas hechas
    if ( index == -1 ) 
        return -1;
    //en dado caso que si haya encontrado se le incrementa +1 a quantity
    quantity++;
    
    /*sizeFile: tamaño de la estructura que se leera, numberRegisters: numero de resgitros en el archivo*/
    int sizeFile = 0, numberRegisters = 0;
    //archivo donde se sacara el tamaño del mismo
    FILE *file = fopen( "graphics.txt", "rb" );
    fseek ( file, 0, SEEK_END );
    sizeFile = ftell ( file );
    numberRegisters = sizeFile / sizeof ( GRAPHICS );
    fclose( file );

    //vector/array donde estaran los registros de graficos para su busqueda
    GRAPHICS graphicsVector[ numberRegisters ];

    //se almacenan los registros en graphicsVector
    ReadGraphicsFile( graphicsVector );

    //se busca hacia atras de forma secuencial si hay mas graficas, sirve ya que esta ordenado por nombre de usuario
    for ( int forIndex = index - 1; forIndex != -1 && strcmp ( graphicsVector[ forIndex ].userName, userName ) == 0 ; forIndex--  )
        quantity++;
    //end for ( int forIndex = index - 1; forIndex != -1 && strcmp ( graphicsVector[ forIndex ].userName, userName ) == 0 ; forIndex--  )

    //se busca hacia adelante de forma secuencial si hay mas graficas
    for ( int forIndex = index + 1; forIndex != numberRegisters && strcmp ( graphicsVector[ forIndex ].userName, userName ) == 0 ; forIndex++  )
        quantity++;
    //end for ( int forIndex = index + 1; forIndex != numberRegisters && strcmp ( graphicsVector[ forIndex ].userName, userName ) == 0 ; forIndex++  )

    //se retorna la cantidad total de graficos que tiene la persona
    return quantity;
}//end function int GetQuantityUserGraphics ( char userName[] )

/*Funcion para en un arreglo de estructuras GRAPHICS se actualice la memoria y guarde solo los registros de graficas de un usurio dado
userName: usuario al que se le creara este vector,userGraphicsVector: vector/arreglo del usuario dado */
void GetUserGraphicsVector ( char userName[], GRAPHICS userGraphicsVector[] ) {
     //Se organizan los usuarios de forma alfabetica por si se salto este proceso previamente
    OrganizeGraphicsFile ( "userName" );
    //estructura necesaria para llamar a BinarySearchUserNameInGraphics y para poner en el nuevo vector
    GRAPHICS graphic;
    //posicion enla que se encuentra una de sus graficas
    int indexGraphicsTotalVector = BinarySearchUserNameInGraphics ( userName, &graphic );
    //contador el cual nos ubicara en la posicion del vector del usuario para agregar el registro correspondiente
    int indexUserGraphicsVector = 0;

    //se guarda el priemr registro de grafica
    userGraphicsVector[ indexUserGraphicsVector ] = graphic;
    //se aumenta la posicion del vector del usuario
    indexUserGraphicsVector++;
    
    /*sizeFile: tamaño de la estructura que se leera, numberRegisters: numero de resgitros en el archivo*/
    int sizeFile = 0, numberRegisters = 0;
    //archivo donde se sacara el tamaño del mismo
    FILE *file = fopen( "graphics.txt", "rb" );
    fseek ( file, 0, SEEK_END );
    sizeFile = ftell ( file );
    numberRegisters = sizeFile / sizeof ( GRAPHICS );
    fclose( file );
    
    //vector/array donde estaran los registros de graficos para su ordenamiento
    GRAPHICS graphicsTotalVector[ numberRegisters ];

    //se almacenan los registros en graphicsTotalVector
    ReadGraphicsFile( graphicsTotalVector );

    //se busca hacia atras de forma secuencial si hay mas graficas para agregar, sirve ya que esta ordenado por nombre de usuario
    for ( int forIndex = indexGraphicsTotalVector - 1; forIndex != -1 && strcmp ( graphicsTotalVector[ forIndex ].userName, userName ) == 0 ; forIndex--  ){
        userGraphicsVector[ indexUserGraphicsVector ] = graphicsTotalVector[ forIndex ];
        indexUserGraphicsVector++;
    }// end for ( int forIndex = indexGraphicsTotalVector - 1; forIndex != -1 && strcmp ( graphicsTotalVector[ forIndex ].userName, userName ) == 0 ; forIndex--  )

    //se busca hacia adelante de forma secuencial si hay mas graficas para agregar
    for ( int forIndex = indexGraphicsTotalVector + 1; forIndex != numberRegisters && strcmp ( graphicsTotalVector[ forIndex ].userName, userName ) == 0 ; forIndex++  ) {
        userGraphicsVector[ indexUserGraphicsVector ] = graphicsTotalVector[ forIndex ];
        indexUserGraphicsVector++;
    }//end for ( int forIndex = indexGraphicsTotalVector + 1; forIndex != numberRegisters && strcmp ( graphicsTotalVector[ forIndex ].userName, userName ) == 0 ; forIndex++  )
    //se ordena medainte metodo de burbuja por le campo de "graphicKey" ya que al darse el graphicKey por orden de entrada de grafica, este puede ayudar a organizar por date
    BubbleSortGraphics ( userGraphicsVector, indexUserGraphicsVector - 1, "graphicKey" );
    return;
}

/*--------------------------------------------------------------------------FUNCIONES PARA BORRAR UN REGISTRO DE UN GRAFICO EN graphics.txt-------------------------------------------------------------------*/


/*funcion para buscar el graphicKey en graphics.txt con el algoritmo  de busqueda binaria
graphicKey: numero de referencia de grafica a busacr, result: estructura donde se cambiara lo que este por el registro del numero de referencia de grafica que se estaba buscando*/
int BinarySearchGraphicKey ( char graphicKey[], GRAPHICS* result ) {
    //Se organizan los registros por el graphicKey por si se salto este proceso previamente
    OrganizeGraphicsFile ( "graphicKey" );
    //archivo sobre el que se buscara
    FILE *file = fopen( "graphics.txt", "rb" );

    fseek( file, 0, SEEK_END );
    //fileSize: tamaño del archivo tomado gracias a ftell
    int fileSize = ftell( file );
    //numero de registros en el archivo
    int numberRegisters = fileSize / sizeof( GRAPHICS );

    //indicador de izquierda para buscar
    int left = 0;
    //indicador de derecha para buscar
    int right = numberRegisters - 1;

    while ( left <= right ) {
        // indicador de mitad 
        int half = left + ( right - left ) / 2;
        // indicador de posicion DENTRO DEL ARCHIVO
        int position = half * sizeof( GRAPHICS );

        fseek( file, position, SEEK_SET );
        //grafico que se leera ya que es la "mitad" 
        GRAPHICS graphic;
        fread( &graphic, sizeof( GRAPHICS ), 1, file );

        // comparacion de lo que se busca y de la "mitad"
        int compareResult = strcmp( graphic.graphicKey, graphicKey );

        // revision de si ya fue encontrado
        if ( compareResult == 0 ) {
            fclose( file );
            *result = graphic;
            return half;
        }//end if ( compareResult == 0 )

        // Si el objetivo es mayor, se ignora la mitad izquierda
        if ( compareResult < 0 )
            left = half + 1;
        // Si el objetivo es menor, se ignora la mitad derecha
        else
            right = half - 1;
        //end if ( compareResult < 0 )
    }// end while ( left <= right )

    fclose( file );

    // Retorna -1 si no se encontro
    return -1;
}//end function BinarySearchGraphicKey ( char graphicKey[], GRAPHICS* result )



/*Funcion que eliminara una grafica de el registro graphics.txt si se cumple que existe el grafico, y este pertenece al usuario que lo desea borrar
userName: usuario que desea borrar el grafico, graphicKey: numero del grafico que desea borrar
Retorna: -2 si el grafico no existe, -1 si el grafico existe pero el usuario que lo desea borrar no es el dueño, 0 si se borro exitosamente*/
int DeleteGraphic ( char userName[], char graphicKey[] ) {
    /*Lugar donde se almacenra el registro del grafico por numero de refertencia si es que existe*/
    GRAPHICS graphic;
    // posicion en la que se encuentra el registro que tiene el graphicKey 
    int graphicPosition = BinarySearchGraphicKey ( graphicKey, &graphic );

    //Si no se encontro el grafico se retorna -2 
    if ( graphicPosition == -1 || graphic.userName[0] == '!' )
        return -2;
    //end if ( graphicPosition == -1 || graphic.userName[0] == '!' )

    //Si el usuario que desea borrar el grafico no es el mismo que el dueño del grafico se retorna un -1
    if ( strcmp ( userName, graphic.userName ) != 0 )
        return -1;
    //end if ( strcmp ( userName, graphic.userName ) != 0 )

    //se cambia el primer caracter del nombre que originalmente era un ' ' y ahora s eva a volver un '!' ya que '!'>' ' entonces este ira al final del registro cuando se ordene
    graphic.userName[0] = '!';

    //GetNumberGraphicKey realmente retorna el numero de registros creados, por lo que sirve para saber este dato
    int numberRegisters = GetNumberGraphicKey();

    //vector/array donde estaran los registros de graficos para su ordenamiento
    GRAPHICS graphicsVector[ numberRegisters ];

    //fieldToSort: campo a ordenar el archivo graphics.txt
    char fieldToSort[] = "userName";

    ReadGraphicsFile( graphicsVector );
    //se cambia el grafico orignal por el nuevo que contiene un '!'
    graphicsVector[ graphicPosition ] = graphic;
    //se organiza el archivo
    BubbleSortGraphics ( graphicsVector, numberRegisters - 1, fieldToSort );
    //se escribe le archivo con el cambio hecho,
    WriteGraphicsFile( graphicsVector, numberRegisters );
    //retorna 0 ya que el borrado fue exitoso
    return 0;
}//end function DeleteGraphic ( char userName[], char graphicKey[] )

// ----------------------------------------- modulo grafica





// La función "StringContains" verificará si un string contiene un caracter en específico, para ello sus 2 parámetros (string, containThis).
int StringContains(char string[], char containThis) {
    int index = -1; // se define la variable "index" y se inicializa en -1 ya que si la función no contiene el caracter que se le indica, 
                    // este es el valor que retornará como indicador, de lo contrario, retornará la posición en la cual está contenida la "x".
    for (int i = 0; i < strlen(string) && index == -1; i++) {
        if (string[i] == containThis) {
            index = i + 1;
        }
    }

    return index;
}
// fin de la función "StringContains".


// La función "CalculateResult" calcula y retorna el resultado de un valor indicado después de evaluarlo en una función exponencial f(x) específica.
double CalculateResult(char str[], int x) {
    char delim[] = "/"; // se define la variable "delim" y se inicializa con un '/', ya que este será el indicador para identificar cada término de la función f(x).
    char *token = strtok(str, delim); // se define la variable "token" con la funcionalidad de almacenar cada término a evaluar de le ecuación. 
    double result = 0; // se define la variable "result", en esta se irá almacenando el valor del resultado total de la función (se evaluará término a término).

    while (token != NULL) {
        char term[10]; // se define la variable "term", en esta se almacenará cada término que se extrae de la función f(x).
        strcpy(term, token);
        int constant = StringContains(term, 'x'); // se define la variable "constant", esta indicará si la función posee términos constantes o no.
        if (constant != -1) {
            char exp[20]; // se define la variable "exp", en esta se almacenará el exponente que le corresponde a la varible X en un término específico de la función f(x).
            strcpy(exp, term + constant);
            char coefficient[20]; // se define la variable "coefficient", en esta se almacenará el coeficiente que le coresponde a la variable X en un término específico de la función f(x).
            strncpy(coefficient, term, constant);
            result += atoi(coefficient) * (pow(x, atoi(exp)));
        }else{
            result += atoi(term);
        }
        token = strtok(NULL, delim);
    }
    
    return result;
}
// fin de la función "CalculateResult". 


// La función "InsertChar" se utiliza para insertar un caracter en una cadena, en una posición específica de la cadena.
void InsertChar(char *str, char c, int pos) {
    int length = strlen(str);  // se define la variable "length", esta almacenará el largo de la cadena original.

    for(int i = length; i >= pos; i--){
        str[i + 1] = str[i];
    }

    str[pos] = c; // se inserta el caracter deseado en la posición específica de la cadena.
    str[length + 1] = '\0'; // se agrega el cierre de cadena a la cadena original después de haber insertado el caracter.
}
// fin de la función "InsertChar".


// La función "SettearString" se utiliza para separar los términos de una función y los acomoda como el programador los necesita para evaluarlos
// de una manera sencilla, la idea es dejar la función ingresada de la siguiente manera: "/+2x3/-4x2/+x/+10/".
void SettearString(char *str , char addThisChar) {
    for (int i = 0; str[i] != '\0'; i++){
        if ( str[i + 1] == '+' || str[i + 1] == '-' ) {
            int pos = i + 1;
            InsertChar(str, addThisChar, pos);
            i++;
        }
    }
}
// fin de la función "SettearString".


// La función "SettearString2" se utiliza para evaluar si cada término de la función ingresada por el usuario tiene los respectivos atributos,
// es decir, exponente, variable, exponente, en caso de no tener exponente, se asume que es 1 y se le agrega, igualmente con el exponente.
void SettearString2(char *str){
    for (int i = 0; str[i] != '\0'; i++) {
        if ( str[i] == 'x' && str[i + 1] == '/' ) {
            int pos = i + 1;
            InsertChar(str, '1', pos);
            i++;
        }

        if ( (str[i] == '+' || str[i] == '-') && str[i + 1] == 'x' ) {
            int pos = i + 1;
            InsertChar(str, '1', pos);
            i++;
        }
    }
}
// fin de la función "SettearString2".


// La función "ArrangeStart" se utiliza para "arreglar" el inicio de la función que se ingrese, esto añadiendo un "+" 
// al inicio de la cadena para luego trabajarla fácilmente. Esta recibe el string a modificar como parámetro.
void ArrangeStart(char *str){
    char array[40]; // se define la varible "array", esta almacenará inicialmente el "+" que se modificará en la función
                    // original y se le irán añadiendo los demás caracteres de la misma cadena original.
    array[0]='+';

    for (int i=1; i < strlen(str)+1; i++){
        array[i] = str[i-1];
    }

    str[0]='\0'; // se "vacía" la cadena original para poder almacenar el valor modificado en ella.
    strcpy(str,array); 
}
// fin de la función "ArrangeStart".


// La función "ModifyString" se encarga de evaluar los cambios que necesita la función después de que el usuario la ingrese para poder ser evaluada
// correctamente, en caso de necesitarlos, los realiza.
void ModifyString(char *string){
    char one[]="1"; // se define la variable "one", esta se inicializa en 1 ya que este será el exponente que le corresponde a un término X.

    if( string[0]=='x') {
        ArrangeStart(string);
    }
    if( string[strlen(string)-1]=='x') {
        strcat(string,one);   
    }
    SettearString(string, '/');
    SettearString2(string);
}
// fin de la función "ModifyString".


// ************************************************************ CASOS DE GRAFICACIÓN (DIFERENTES CUADRANTES) ************************************************************************************

/*  En esta sección encontrará 9 funciones con el nombre GraphCaseX, cada una adecuada para cada cuadrante donde el usuario quiera visualizar 
    la función que ingresó, esto se define con el rango inferior y rango superior que el mismo usuario ingresa por teclado.

    Cada una de las funciones contendrá los siguientes parámetros en común (las que no sean comunes en general, serán documentadas donde se utilicen):
    "renderer": contiene una estructura que se verá reflejada en pantalla, básicamente la creación de la ventana.
    "font": contiene la fuente que será utilizada a la hora de crear texto.
    "function": contiene la función f(x) que se evaluará y se graficará.
    "x1": contine la coordenada inicial en el eje X.
    "y1": contine la coordenada inicial en el eje Y.
    "x2": contine la coordenada final en el eje X.
    "y2": contine la coordenada final en el eje Y.
    "increment": contiene el incremento de la función en el eje X.
    "divX": contiene la cantidad de pixeles en la que fue dividida el eje X para lograr almacenar los valores en pantalla.
    "xmin": contiene el rango inferior que ingresó el usuario para el eje X.
    "xmax": contiene el rango superior que ingresó el usuario para el eje X.
    "maximum": contiene el valor absoluto del resultado máximo que se evaluó en la función en el dominio definido por el usuario.
*/

// La función "GraphCase1" se utiliza para graficar las funciones que se encuentren en el cuadrante I.
void GraphCase1(SDL_Renderer *renderer, TTF_Font *font, char function[], int x1, int y1, int x2, int y2, int increment, int divX, int xmin, int xmax, double maximum){
    char aux[30] = ""; // se define la variable "aux", esta almacenará la función original. 
    int height = WINDOW_HEIGHT - 140; // se define la variable "height", esta almacenará la medida del eje Y en pixeles.
    int height1 = WINDOW_HEIGHT - 20; // se define la variable "height1", esta almacenará la referencia para mapear los puntos que se vayan evaluando en la función.
    strcpy(aux, function);

    x1 = 20 + (xmin*divX);
    x2 = x1 + divX; 
    y1 = height1 - ((CalculateResult(aux, xmin)/maximum)*height);
    strcpy(aux, function);
    y2 = height1 - ((CalculateResult(aux, xmin+increment)/maximum)*height);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );
    
    //Estructura para aplicar el color azul
    SDL_Color color; color.r = 0; color.g = 0; color.b = 255; color.a = 255; 
    for( int i = xmin+(2*increment); i <= xmax; i = i+increment ){
        createText(renderer, font, color,"X", x1-5, y1-10);
        strcpy(aux, function);
        x1 = x2; 
        x2 = x1 + divX;
        y1 = y2; 
        y2 = height1 - ((CalculateResult(aux, i)/maximum)*height);
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );
        createText(renderer, font, color,"X", x2-5, y2-10);
    }
}
// fin de la función "GraphicCase1".


// La función "GraphCase2" se utiliza para graficar las funciones que se encuentren en el cuadrante II.
void GraphCase2(SDL_Renderer *renderer, TTF_Font *font, char function[], int x1, int y1, int x2, int y2, int increment, int divX, int xmin, int xmax, double maximum){
    char aux[30] = ""; // se define la variable "aux", esta almacenará la función original. 
    int height = WINDOW_HEIGHT - 140; // se define la variable "height", esta almacenará la medida del eje Y en pixeles.
    int height1 = WINDOW_HEIGHT - 20; // se define la variable "height1", esta almacenará la referencia para mapear los puntos que se vayan evaluando en la función.
    strcpy(aux, function);

    x1 = 20;
    x2 = x1 + divX; 
    y1 = height1 - ((CalculateResult(aux, xmin)/maximum)*height);
    strcpy(aux, function);
    y2 = height1 - ((CalculateResult(aux, xmin+increment)/maximum)*height);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );

    //Estructura para aplicar el color azul
    SDL_Color color; color.r = 0; color.g = 0; color.b = 255; color.a = 255; 

    for( int i = xmin+(2*increment); i <= xmax; i = i+increment ){
        createText(renderer, font, color,"X", x1-5, y1-10);
        strcpy(aux, function);
        x1 = x2; 
        x2 = x1 + divX;
        y1 = y2; 
        y2 = height1 - ((CalculateResult(aux, i)/maximum)*height);
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );
        createText(renderer, font, color,"X", x2-5, y2-10);
    }
}
// fin de la función "GraphicCase2".


// La función "GraphCase3" se utiliza para graficar las funciones que se encuentren en el cuadrante III.
void GraphCase3(SDL_Renderer *renderer, TTF_Font *font, char function[], int x1, int y1, int x2, int y2, int increment, int divX, int xmin, int xmax, double maximum){
    char aux[30] = ""; // se define la variable "aux", esta almacenará la función original. 
    int height = WINDOW_HEIGHT - 140; // se define la variable "height", esta almacenará la medida del eje Y en pixeles.
    int height1 = 120; // se define la variable "height1", esta almacenará la referencia para mapear los puntos que se vayan evaluando en la función.
    strcpy(aux, function);

    x1 = 20;
    x2 = x1 + divX; 
    y1 = height1 - ((CalculateResult(aux, xmin)/maximum)*height);
    strcpy(aux, function);
    y2 = height1 - ((CalculateResult(aux, xmin+increment)/maximum)*height);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );

    //Estructura para aplicar el color azul
    SDL_Color color; color.r = 0; color.g = 0; color.b = 255; color.a = 255; 

    for( int i = xmin+(2*increment); i <= xmax; i = i+increment ){
        createText(renderer, font, color,"X", x1-5, y1-10);
        strcpy(aux, function);
        x1 = x2; 
        x2 = x1 + divX;
        y1 = y2; 
        y2 = height1 - ((CalculateResult(aux, i)/maximum)*height);
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );
        createText(renderer, font, color,"X", x2-5, y2-10);
    }
}
// fin de la función "GraphicCase3".


// La función "GraphCase4" se utiliza para graficar las funciones que se encuentren en el cuadrante IV.
void GraphCase4(SDL_Renderer *renderer, TTF_Font *font, char function[], int x1, int y1, int x2, int y2, int increment, int divX, int xmin, int xmax, double maximum){
    char aux[30] = ""; // se define la variable "aux", esta almacenará la función original. 
    int height = WINDOW_HEIGHT - 140; // se define la variable "height", esta almacenará la medida del eje Y en pixeles.
    int height1 = 120; // se define la variable "height1", esta almacenará la referencia para mapear los puntos que se vayan evaluando en la función.
    strcpy(aux, function);

    x1 = 20 + (xmin*divX);
    x2 = x1 + divX; 
    y1 = height1 - ((CalculateResult(aux, xmin)/maximum)*height);
    strcpy(aux, function);
    y2 = height1 - ((CalculateResult(aux, xmin+increment)/maximum)*height);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );

    //Estructura para aplicar el color azul
    SDL_Color color; color.r = 0; color.g = 0; color.b = 255; color.a = 255; 

    for( int i = xmin+(2*increment); i <= xmax; i = i+increment ){
        createText(renderer, font, color,"X", x1-5, y1-10);
        strcpy(aux, function);
        x1 = x2; 
        x2 = x1 + divX;
        y1 = y2; 
        y2 = height1 - ((CalculateResult(aux, i)/maximum)*height);
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );
        createText(renderer, font, color,"X", x2-5, y2-10);
    }
}
// fin de la función "GraphicCase4".


// La función "GraphCase5" se utiliza para graficar las funciones que se encuentren en los cuadrantes I y IV simultáneamente.
// El parámetro "ymin" almacena el valor mínimo de Y que se obtiene al evaluar la función en el rango definido por el usuario.
void GraphCase5(SDL_Renderer *renderer, TTF_Font *font, char function[], int x1, int y1, int x2, int y2, int increment, int divX, int xmin, int xmax, double maximum, double ymin){
    char aux[30] = ""; // se define la variable "aux", esta almacenará la función original. 
    int height = WINDOW_HEIGHT - 140; // se define la variable "height", esta almacenará la medida del eje Y en pixeles.
    int height1 = (height+120) - abs((ymin/maximum)*height); // se define la variable "height1", esta almacenará la referencia para mapear los puntos que se vayan evaluando en la función.
    strcpy(aux, function);

    x1 = 20 + (xmin*divX);
    x2 = x1 + divX; 
    y1 = height1 - ((CalculateResult(aux, xmin)/maximum)*height);
    strcpy(aux, function);
    y2 = height1 - ((CalculateResult(aux, xmin+increment)/maximum)*height);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );

    //Estructura para aplicar el color azul
    SDL_Color color; color.r = 0; color.g = 0; color.b = 255; color.a = 255; 

    for( int i = xmin+(2*increment); i <= xmax; i = i+increment ){
        createText(renderer, font, color,"X", x1-5, y1-10);
        strcpy(aux, function);
        x1 = x2; 
        x2 = x1 + divX;
        y1 = y2; 
        y2 = height1 - ((CalculateResult(aux, i)/maximum)*height);
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );
        createText(renderer, font, color,"X", x2-5, y2-10);
    }
}
// fin de la función "GraphicCase5".


// La función "GraphCase6" se utiliza para graficar las funciones que se encuentren en los cuadrantes II y III simultáneamente.
// El parámetro "ymin" almacena el valor mínimo de Y que se obtiene al evaluar la función en el rango definido por el usuario.
void GraphCase6(SDL_Renderer *renderer, TTF_Font *font, char function[], int x1, int y1, int x2, int y2, int increment, int divX, int xmin, int xmax, double maximum, double ymin){
    char aux[30] = ""; // se define la variable "aux", esta almacenará la función original. 
    int height = WINDOW_HEIGHT - 140; // se define la variable "height", esta almacenará la medida del eje Y en pixeles.
    int height1 = (height+120) - abs((ymin/maximum)*height); // se define la variable "height1", esta almacenará la referencia para mapear los puntos que se vayan evaluando en la función.
    strcpy(aux, function);

    x1 = 20;
    x2 = x1 + divX; 
    y1 = height1 - ((CalculateResult(aux, xmin)/maximum)*height);
    strcpy(aux, function);
    y2 = height1 - ((CalculateResult(aux, xmin+increment)/maximum)*height);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );

    //Estructura para aplicar el color azul
    SDL_Color color; color.r = 0; color.g = 0; color.b = 255; color.a = 255; 

    for( int i = xmin+(2*increment); i <= xmax; i = i+increment ){
        createText(renderer, font, color,"X", x1-5, y1-10);
        strcpy(aux, function);
        x1 = x2; 
        x2 = x1 + divX;
        y1 = y2; 
        y2 = height1 - ((CalculateResult(aux, i)/maximum)*height);
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );
        createText(renderer, font, color,"X", x2-5, y2-10);
    }
}
// fin de la función "GraphicCase6".


// La función "GraphCase7" se utiliza para graficar las funciones que se encuentren en los cuadrantes III y IV simultáneamente.
void GraphCase7(SDL_Renderer *renderer, TTF_Font *font, char function[], int x1, int y1, int x2, int y2, int increment, int divX, int xmin, int xmax, double maximum){
    char aux[30] = ""; // se define la variable "aux", esta almacenará la función original. 
    int height = WINDOW_HEIGHT - 140; // se define la variable "height", esta almacenará la medida del eje Y en pixeles.
    int height1 = WINDOW_HEIGHT - 20; // se define la variable "height1", esta almacenará la referencia para mapear los puntos que se vayan evaluando en la función.
    strcpy(aux, function);

    x1 = 20;
    x2 = x1 + divX; 
    y1 = height1 - ((CalculateResult(aux, xmin)/maximum)*height);
    strcpy(aux, function);
    y2 = height1 - ((CalculateResult(aux, xmin+increment)/maximum)*height);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );

    //Estructura para aplicar el color azul
    SDL_Color color; color.r = 0; color.g = 0; color.b = 255; color.a = 255; 

    for( int i = xmin+(2*increment); i <= xmax; i = i+increment ){
        createText(renderer, font, color,"X", x1-5, y1-10);
        strcpy(aux, function);
        x1 = x2; 
        x2 = x1 + divX;
        y1 = y2; 
        y2 = height1 - ((CalculateResult(aux, i)/maximum)*height);
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );
        createText(renderer, font, color,"X", x2-5, y2-10);
    }
}
// fin de la función "GraphicCase7".


// La función "GraphCase8" se utiliza para graficar las funciones que se encuentren en los cuadrantes I y II simultáneamente.
void GraphCase8(SDL_Renderer *renderer, TTF_Font *font, char function[], int x1, int y1, int x2, int y2, int increment, int divX, int xmin, int xmax, double maximum){
    char aux[30] = ""; // se define la variable "aux", esta almacenará la función original. 
    int height = WINDOW_HEIGHT - 140; // se define la variable "height", esta almacenará la medida del eje Y en pixeles.
    int height1 = 120; // se define la variable "height1", esta almacenará la referencia para mapear los puntos que se vayan evaluando en la función.
    strcpy(aux, function);

    x1 = 20;
    x2 = x1 + divX; 
    y1 = height1 - ((CalculateResult(aux, xmin)/maximum)*height);
    strcpy(aux, function);
    y2 = height1 - ((CalculateResult(aux, xmin+increment)/maximum)*height);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );

    //Estructura para aplicar el color azul
    SDL_Color color; color.r = 0; color.g = 0; color.b = 255; color.a = 255; 


    for( int i = xmin+(2*increment); i <= xmax; i = i+increment ){
        createText(renderer, font, color,"X", x1-5, y1-10);
        strcpy(aux, function);
        x1 = x2; 
        x2 = x1 + divX;
        y1 = y2; 
        y2 = height1 - ((CalculateResult(aux, i)/maximum)*height);
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );
        createText(renderer, font, color,"X", x2-5, y2-10);
    }
}
// fin de la función "GraphicCase8".


// La función "GraphCase9" se utiliza para graficar las funciones que ocupen coordenadas que se distribuyan a lo largo del plano XY.
// El parámetro "ymin" almacena el valor mínimo de Y que se obtiene al evaluar la función en el rango definido por el usuario.
// El parámetro "ymax" almacena el valor máximo de Y que se obtiene al evaluar la función en el rango definido por el usuario.
void GraphCase9(SDL_Renderer *renderer, TTF_Font *font, char function[], int x1, int y1, int x2, int y2, int increment, int divX, int xmin, int xmax, double maximum, double ymin, double ymax){
    char aux[30] = ""; // se define la variable "aux", esta almacenará la función original. 
    int height = WINDOW_HEIGHT - 140; // se define la variable "height", esta almacenará la medida del eje Y en pixeles.
    int height1 = 0; // se define la variable "height1", esta almacenará la referencia para mapear los puntos que se vayan evaluando en la función.

    if(maximum>ymax){
        height1 = height + 120 - (height*maximum/(abs(ymin)+abs(ymax)));

        strcpy(aux, function);

        x1 = 20;
        x2 = x1 + divX; 
        y1 = height1 - ((CalculateResult(aux, xmin)/maximum)*(height1-height1+120));
        strcpy(aux, function);
        y2 = height1 - ((CalculateResult(aux, xmin+increment)/maximum)*(height1-height1+120));
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );

        //Estructura para aplicar el color azul
        SDL_Color color; color.r = 0; color.g = 0; color.b = 255; color.a = 255; 

        for( int i = xmin+(2*increment); i <= xmax; i = i+increment ){
            createText(renderer, font, color,"X", x1-5, y1-10);
            strcpy(aux, function);
            x1 = x2; 
            x2 = x1 + divX;
            y1 = y2; 
            y2 = height1 - ((CalculateResult(aux, i)/maximum)*(height1-height1+120));
            SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );
            createText(renderer, font, color,"X", x2-5, y2-10);
        } 

    } else{
        height1 = 120 + (height*maximum/(abs(ymin)+abs(ymax)));

        strcpy(aux, function);

        x1 = 20;
        x2 = x1 + divX; 
        y1 = height1 - ((CalculateResult(aux, xmin)/maximum)*(height1-120));
        strcpy(aux, function);
        y2 = height1 - ((CalculateResult(aux, xmin+increment)/maximum)*(height1-120));
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );
    
        //Estructura para aplicar el color azul
        SDL_Color color; color.r = 0; color.g = 0; color.b = 255; color.a = 255; 

        for( int i = xmin+(2*increment); i <= xmax; i = i+increment ){
            createText(renderer, font, color,"X", x1-5, y1-10);
            strcpy(aux, function);
            x1 = x2; 
            x2 = x1 + divX;
            y1 = y2; 
            y2 = height1 - ((CalculateResult(aux, i)/maximum)*(height1-120));
            SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );
            createText(renderer, font, color,"X", x2-5, y2-10);
        }
    }
}
// fin de la función "GraphicCase9".


// La función "CreateAxis" se utiliza para graficar los ejes "Y" y "X" en la ventana.
void CreateAxis(SDL_Renderer *renderer, TTF_Font *font, int xmin, int xmax, double ymax, double ymin, int divX, double maximum, int increment){
    int startX = 20; // se define la variable "startX", en esta se almacena la coordenada inicial para el eje X.
    int startY = 120; // se define la variable "startY", en esta se almacena la coordenada inicial para el eje Y.
    int endX =  WINDOW_WIDTH - 20; // se define la variable "endX", en esta se almacena la coordenada final para el eje X.
    int endY = WINDOW_HEIGHT - 20; // se define la variable "endY", en esta se almacena la coordenada final para el eje Y.
    int ubiX = 0, ubiY = 0; // se definen variables "ubiX" y "ubiY", en estas se almacenan las coordenadas para los ejes X e Y respectivamente 
                            // en caso de que estos se deban desplazar acorde a la gráfica de la función.
    char maxText[sizeof(xmax)] = "";
    char minText[sizeof(xmin)] = "";

    sprintf(maxText, "%i", xmax);
    sprintf(minText, "%i", xmin);
    

    // UBICACIÓN DEL EJE Y
    if( xmin >= 0 )
        SDL_RenderDrawLine(renderer, startX, startY, startX, endY);
    else if( xmax <= 0 )
        SDL_RenderDrawLine(renderer, endX, startY, endX, endY);
    else{
        ubiX = 20 + (abs(xmin)*divX)/increment; 
        SDL_RenderDrawLine(renderer, ubiX, startY, ubiX, endY); 
    } 

    //estructura de color rojo
    SDL_Color red = {255, 0, 0, 255};
    //estructura de color negro
    SDL_Color black = {0, 0, 0, 255};

    // UBICACIÓN DEL EJE X
    if( ymin >= 0 ){
        SDL_RenderDrawLine(renderer, startX, endY, endX, endY);
        createText(renderer, font, red, "|", startX-1, endY-10);
        createText(renderer, font, red, "|", endX, endY-10);
        createText(renderer, font, black, minText, startX, endY);
        createText(renderer, font, black, maxText, endX, endY);
    }
    else if( ymax <= 0 ){
        SDL_RenderDrawLine(renderer, startX, startY, endX, startY);
        createText(renderer, font, red, "|", startX-1, startY-10);
        createText(renderer, font, red, "|", endX, startY-10);
        createText(renderer, font, black, minText, startX, startY);
        createText(renderer, font, black, maxText, endX, startY);
    }
    else{
        if(maximum>ymax)
            ubiY = endY - (500*maximum/(abs(ymin)+abs(ymax)));
        else
            ubiY = 120 + (500*maximum/(abs(ymin)+abs(ymax)));
    
        SDL_RenderDrawLine(renderer, startX, ubiY, endX, ubiY);
        createText(renderer, font, red, "|", startX-1, ubiY-10);
        createText(renderer, font, red, "|", endX, ubiY-10);
        createText(renderer, font, black, minText, startX, ubiY);
        createText(renderer, font, black, maxText, endX, ubiY);
    }
}
// fin de la función "CreateAxis".


// La función "CalculateDiv" calcula y retorna el divisor de la división necesaria para saber en cuántas secciones se debe dividir el eje X.
int CalculateDiv(int superior, int inferior){
    if(inferior >= 0)
        return superior; 
    else if( superior <= 0)
        return abs(inferior);  
    else
        return abs(superior)+abs(inferior);
}
// fin de la función "CalculateDiv".


// La función "GraphFunction" se encarga de recibir los valores que ingresa el usuario y distribuirlos a través de las funciones necesarias para
// graficar la función deseada.
void GraphFunction(SDL_Renderer *renderer, TTF_Font *font, char function[], int lowerBound, int upperBound, int increment) {
    int width = WINDOW_WIDTH - 40; // se define la variable "width", esta almacena el largo del eje X en pixeles.
    int height = WINDOW_HEIGHT - 140; // se define la variable "height", esta almacena el largo del eje Y en pixeles.
    double ymax = 0; // se define la variable "ymax", esta almacena el mayor valor positivo evaluado en la función con los respectivos rangos.
    double ymin = 0; // se define la variable "ymin", esta almacena el menor valor positivo evaluado en la función con los respectivos rangos.
    double valueMax = 0; // se define la variable "valueMax", esta almacena el valor absoluto mayor evaluado en la función con los respectivos rangos.
    char auxFunction[30]=""; // se define la variable "auxFunction", esta almacenará la función original ingresada.
    int divX = (width*increment)/(CalculateDiv(upperBound, lowerBound)); // se define la variable "divX", esta almacena el número de pixeles de cada 
                                                                        // sección en la que se dividirá el eje X.
    function = strcat(function, "+0");
    ModifyString(function);
    strcpy(auxFunction, function);

    for (int i = lowerBound; i<= upperBound; i=i+increment ){
        strcpy(auxFunction, function);
        double result =  CalculateResult(auxFunction, i);

        if(result >= ymax)
            ymax = result;
        else if(result<= ymin)
            ymin = result;
    }

    if(abs(ymin) > ymax )
        valueMax = abs(ymin);
    else
        valueMax = ymax; 

    SDL_SetRenderDrawColor(renderer, 255,0,0,255);
    CreateAxis(renderer, font, lowerBound, upperBound, ymax, ymin, divX, valueMax, increment);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Negro
    
    int x1=0, x2=0, y1=0, y2=0; // se definen las variables "x1, x2, y1, y2", estas almacenarán las coordenadas iniciales (x1, y1) y las coordenadas
                                // finales (x2, y2) en las que se graficará la linea.

    if( ymin>=0 && lowerBound>=0 ) 
        GraphCase1(renderer, font, function, x1, y1, x2, y2, increment, divX, lowerBound, upperBound, valueMax);
    else if( ymin>=0 && upperBound<=0 )
        GraphCase2(renderer, font, function, x1, y1, x2, y2, increment, divX, lowerBound, upperBound, valueMax);
    else if( ymax<=0 && upperBound<=0 )
        GraphCase3(renderer, font, function, x1, y1, x2, y2, increment, divX, lowerBound, upperBound, valueMax);
    else if( ymax<=0 && lowerBound>=0)
        GraphCase4(renderer, font, function, x1, y1, x2, y2, increment, divX, lowerBound, upperBound, valueMax);
    else if( lowerBound>=0 && ymax>0 && ymin<0 )
        GraphCase5(renderer, font, function, x1, y1, x2, y2, increment, divX, lowerBound, upperBound, valueMax, ymin);
    else if( upperBound<=0 && ymax>0 && ymin<0 )
        GraphCase6(renderer, font, function, x1, y1, x2, y2, increment, divX, lowerBound, upperBound, valueMax, ymin);
    else if( ymin>=0 && lowerBound<0 && upperBound>0 )
        GraphCase7(renderer, font, function, x1, y1, x2, y2, increment, divX, lowerBound, upperBound, valueMax);
    else if( ymax<=0 && lowerBound<0 && upperBound>0 )
        GraphCase8(renderer, font, function, x1, y1, x2, y2, increment, divX, lowerBound, upperBound, valueMax);
    else if( ymax>0 && ymin<0 && lowerBound<0 && upperBound>0)
        GraphCase9(renderer, font, function, x1, y1, x2, y2, increment, divX, lowerBound, upperBound, valueMax, ymin, ymax);

    SDL_RenderPresent(renderer);
}
// fin de la función "GraphFunction".



//---------------------------------------- separar modulo
void DelateLastChar(char *cadena) {
    if (cadena == NULL || *cadena == '\0') {
        // Manejo del caso donde la cadena es nula o vacía
        return;
    }
    
    size_t longitud = strlen(cadena);
    if (longitud > 0) {
        cadena[longitud - 1] = '\0'; // Establecer el último carácter como nulo
    }
}


// funcion que pasa una cadena que contenga mayusculas a minusculas
void toLowerCase(char *str) { 
    int i;
    for (i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}

// Esta funcion rellena un string con espacios en los espacios faltantes para llenarla
void FillString(char *cadenaALlenar, int size){
    for ( int i= strlen(cadenaALlenar); i<size-1;i++ ){
        cadenaALlenar[i]=' ';
    }
    cadenaALlenar[size-1]='\0';
}


// Esta funcion Es usada para crear un texto en la pantalla 
void createText(SDL_Renderer *renderer , TTF_Font *font ,SDL_Color color, char string[], int posX , int posY){
        char otro_texto[50] = "2"; strcpy(otro_texto,string);// Define otra variable de cadena para el nuevo texto
        SDL_Surface* otro_texto_surface = TTF_RenderText_Solid(font, otro_texto, color);
        SDL_Texture* otro_texto_texture = SDL_CreateTextureFromSurface(renderer, otro_texto_surface);
        int otro_texto_width = otro_texto_surface->w;
        int otro_texto_height = otro_texto_surface->h;
        SDL_FreeSurface(otro_texto_surface);
        SDL_Rect otro_texto_rect = {posX, posY, otro_texto_width, otro_texto_height};
        SDL_RenderCopy(renderer, otro_texto_texture, NULL, &otro_texto_rect);
        SDL_DestroyTexture(otro_texto_texture);
}


void CreateUser(SDL_Renderer *renderer , TTF_Font *font ,SDL_Color color){
    // Para este programa se ha decidido que los string se inicializaran con " "
    char userName[20] = " ";  // Es el el string que guarda el nombre de usuario que se entre
    char passHidded[40]=" "; // Es la contraseña enmascarada  (ejemplo: ****)
    char passWord[13]=" ";  // Es la contraseña sin enmascarar (ejemplo: hola)
    int  writingUser=0;    // Esta varible permite identificar que campo se esta escribiendo siendo { 0 = userName , 1 = password }
    char warnings[70]="AQUI PONDRE LA CONRASEÑA QUE DEBE PONER:"; // Variable para poner advertencias o Indicatives que se le deban dar al usuario
    char titleOfModule[50] = "Aplicativo - Graficadora - UTP"; // titulo que se le pone al modulo
    char userNameLabel[20]="Username: "; // Indicative de lo que se debe escribir es el nombre de usuario
    int exitApp=0; //necesita habilitar para que cuando se presione cualquier tecla se finalice el programa
    int exitAppAuxiliar=0; // Es la variable encargada de habilitar exitApp a 1 (podria ser cualquier numero mayor a 0)
    char warnings2[70]="AQUI PONDRE LA CONTRASEÑA QUE EL USARIO INGRESA: ";
    //while (flag) mantiene actualizando el modulo actual , esto para que el usuario pueda ver lo que va escribiendo
    int flag=1;
    while (flag) {

        SDL_Event event; // event es una variable tipo evento , estas variables guardan lo que ha sucedido en el aplicativo
        // while (SDL_PollEvent(&event)) , ira mirando cada uno de los eventos que esten guardados en event
        while (SDL_PollEvent(&event)) { 
            if (exitApp) { // si se debe finalizar el programa
                flag = 0;
            }


//--------------------------------- Code Optimizado ----------------------------------------------------------
            if(event.type==SDL_KEYDOWN){

           
            if (writingUser==0){ // userName

                if (event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z){
                    if (SDL_GetModState() & KMOD_CAPS || SDL_GetModState() & KMOD_SHIFT){
                        strcat(userName,SDL_GetKeyName(event.key.keysym.sym));
                    }else{
                        char ara[20];
                        strcpy(ara,SDL_GetKeyName(event.key.keysym.sym));
                        toLowerCase(ara);
                        strcat(userName,ara);
                    }
                }else if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9  ){
                        strcat(userName,SDL_GetKeyName(event.key.keysym.sym));
                }else if (event.key.keysym.sym == SDLK_RETURN){
                        USER userToCreate; 
                        FillString(userName,20);            
                        int index = binarySearchUserName(userName,&userToCreate);
                        if ( index !=-1){
                            strcpy(warnings, "USUARIO YA EXISTE");
                        }else{
                            strcpy(passHidded, "Password: ");
                            strcpy(userToCreate.userName,  userName);
                            strcpy(warnings, userToCreate.userName);
                            writingUser= 1;    
                        }                   
                }


            }else if (writingUser==1 ){ // contraseña
                    if (event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z){
                        strcat(passHidded,"*");
                        if (SDL_GetModState() & KMOD_CAPS || SDL_GetModState() & KMOD_SHIFT){
                            strcat(passWord,SDL_GetKeyName(event.key.keysym.sym));
                        }else {
                            char stringLikeChar[12];
                            strcpy(stringLikeChar,SDL_GetKeyName(event.key.keysym.sym));
                            toLowerCase(stringLikeChar);
                            strcat(passWord,stringLikeChar);
                        }
                    }else if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9  ){
                        strcat(passHidded,"*");
                        strcat(passWord,SDL_GetKeyName(event.key.keysym.sym));
                    }else if (event.key.keysym.sym == SDLK_RETURN ){
                            if (strlen(passWord) >=5){
                                USER newUser;
                                FillString(userName,20);
                                FillString(passWord,13);
                                strcpy ( newUser.userName, userName );
                                strcpy ( newUser.password, passWord );
                                newUser.userKey= GetUserKey();
                                EncryptPassword ( &newUser );
                                AddUserToFile ( newUser );
                                flag=0;
                            }else{
                                strcpy(passWord," ");
                                strcpy(passHidded," ");
                                strcpy(userName," ");
                                writingUser=0;
                            }         
                    }
            }
        }
    }

//------------------------------------------------------------------------------------------------------------

        // Limpiar pantalla
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        
        createText(renderer , font, color, userName, 80,50); //renderiza el texto que contiene lo que el usuario vaya ingresando
        createText(renderer , font, color, titleOfModule, 5,10);
        createText(renderer , font, color, userNameLabel, 10,50); //renderiza la el texto "Usermame: " como Indicative
        createText(renderer,font,color, passHidded, 10,75);
        createText(renderer,font,color,warnings,10,120); //renderiza las advertencias que se tengan que hacer
        createText(renderer,font,color,warnings2,10,140); //renderiza las advertencias que se tengan que hacer

        // Actualizar pantalla
        SDL_RenderPresent(renderer);
    }
}

void CreateGraphic(SDL_Renderer *renderer , TTF_Font *font ,SDL_Color color, char userNameParemeter[]){
    char title[40]= "APLICATIVO - GRAFICADOR - UTP";
    char subTitle[60]= "Crear Grafico";
    char Indicative[80]= "C = Coeficiente , E = exponente , entre termino en formato CXE de tenerlos";
    char Indicative2[80]= "Rango en formato Inicio,final ejem: -10,15 ";
    char IndicativeEcuacion[40]= "f(x)=";
    char ecuacion[30]=" ";
    char IndicativeRange[40]= "Rango=";
    char range[20]=" ";
    char IndicativeIncremento[40]= "Incremento=";
    char incremento[40]= " ";
    int userWriting=0;
    char ecuacionAuxilair[30];
    int LowerBound= 0;
    int UpperBound= 0;
    int incrementador=0;
    int flag=1;
    char rangoCopy[20] = " ";


    while (flag) {
        SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT ) {
                    flag = 0;
                }else if(event.type==SDL_KEYDOWN){

                if ( userWriting==0){
                    if (event.key.keysym.sym == SDLK_x || event.key.keysym.sym == SDLK_PLUS || event.key.keysym.sym == SDLK_MINUS){
                        if (SDL_GetModState() & KMOD_CAPS || SDL_GetModState() & KMOD_SHIFT){
                            strcat(ecuacion,SDL_GetKeyName(event.key.keysym.sym));
                        }else{
                            char stringLikeChar[20];
                            strcpy(stringLikeChar,SDL_GetKeyName(event.key.keysym.sym));
                            toLowerCase(stringLikeChar);
                            strcat(ecuacion,stringLikeChar);
                        }
                    }else if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9  ){
                            strcat(ecuacion,SDL_GetKeyName(event.key.keysym.sym));
                    }else if (event.key.keysym.sym == SDLK_RETURN){
                        userWriting=1;
                    }
                    }else if (userWriting==1){
                        if (event.key.keysym.sym == SDLK_COMMA || event.key.keysym.sym == SDLK_MINUS){
                                strcat(range,SDL_GetKeyName(event.key.keysym.sym));
                        }else if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9  ){
                                strcat(range,SDL_GetKeyName(event.key.keysym.sym));
                        }else if (event.key.keysym.sym == SDLK_RETURN){
                            userWriting=2;
                        }
                    }else{
                        if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9  ){
                                strcat(incremento,SDL_GetKeyName(event.key.keysym.sym));
                        }else if (event.key.keysym.sym == SDLK_RETURN){
                            strcpy(ecuacionAuxilair,ecuacion+1);
                            strcpy ( rangoCopy, range );
                            LowerBound= atoi(strtok(rangoCopy,","));
                            UpperBound= atoi(strtok(NULL, " "));
                            incrementador= atoi(incremento);
                            AddGraphicToFile ( userNameParemeter, ecuacionAuxilair, LowerBound, UpperBound, incrementador );
                            flag=0;
                        }

                    }
                }
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        createText(renderer , font, color, title, 10,10);
        createText(renderer , font, color, subTitle, 15,30);
        createText(renderer , font, color, Indicative, 10,50);
        createText(renderer , font, color, Indicative2, 10,70);
        createText(renderer , font, color, IndicativeEcuacion, 10,90);
        createText(renderer , font, color, ecuacion, 40,90);
        createText(renderer , font, color, IndicativeRange, 160,90);
        createText(renderer , font, color, range, 210,90);
        createText(renderer , font, color, IndicativeIncremento, 310,90);
        createText(renderer , font, color, incremento, 400,90);

        // Actualizar pantalla
        SDL_RenderPresent(renderer);
        
    }
}
    int flag2=1;
    GraphFunction(renderer, font, ecuacionAuxilair,LowerBound,UpperBound,incrementador);
    FILE *file = fopen ( "graphicCounter.txt", "wb" );
    int quantityGraphics = GetNumberGraphicKey ();
    fprintf ( file, "%d", quantityGraphics );
    fclose ( file );
    while (flag2==2) {
                SDL_Event event;
                while (SDL_PollEvent(&event)) {
                    if (event.type == SDL_QUIT ) {
                        flag2 = 0;
                    }
                }
    }
}


void ChangeMyPassword(SDL_Renderer *renderer , TTF_Font *font ,SDL_Color color, char nombreUsario[]){
        char title[40]= "APLICATIVO - GRAFICADORA - UTP";
        char subTitle[60]= "Cambiar contraseña del usuario: "; strcat(subTitle, nombreUsario);
        char Indicative[40]= "Entre contraseña actual:";
        char currentPassword[13]=" ";char currentPasswordhidden[40]=" ";
        char Indicative2[40]=" ";
        char newPassword[13]=" ";char newPasswordhidden[20]=" ";
        char Indicative3[40]= " ";
        char newPasswordConfirmacion[13]=" "; char newPasswordConfirmacionhidden[13]=" ";
        char warnings[40]=" ";
        int userWriting=0;
        int flag =1;
        while (flag==1) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT ) {
                    flag = 0;
                }else if(event.type==SDL_KEYDOWN){

                    if ( userWriting==0){
                        if (event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z){
                            if (SDL_GetModState() & KMOD_CAPS || SDL_GetModState() & KMOD_SHIFT){
                                strcat(currentPassword,SDL_GetKeyName(event.key.keysym.sym));
                                strcat(currentPasswordhidden,"*");
                            }else{
                                char stringLikeChar[20];
                                strcpy(stringLikeChar,SDL_GetKeyName(event.key.keysym.sym));
                                toLowerCase(stringLikeChar);
                                strcat(currentPassword,stringLikeChar);
                                strcat(currentPasswordhidden,"*");
                            }
                        }else if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9  ){
                                strcat(currentPassword,SDL_GetKeyName(event.key.keysym.sym));
                                strcat(currentPasswordhidden,"*");
                        }else if (event.key.keysym.sym == SDLK_RETURN){
                            FillString(currentPassword,13);
                            USER UserInFound ,UserInBase ;
                            strcpy(UserInFound.password, currentPassword);
                            EncryptPassword(&UserInFound);
                            binarySearchUserName(nombreUsario ,&UserInBase);
                            if ( ComparePassword(UserInBase.password, UserInFound)==0){
                                strcpy(Indicative2,"Entre contraseñan nueva: ");
                                userWriting=1;
                            }else{
                                strcpy(warnings,"contrasena incorrecta");
                                // devuelve al modulo anterior
                            }
                        }
                    }else if ( userWriting==1){
                        if (event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z){
                            if (SDL_GetModState() & KMOD_CAPS || SDL_GetModState() & KMOD_SHIFT){
                                strcat(newPassword,SDL_GetKeyName(event.key.keysym.sym));
                                strcat(newPasswordhidden,"*");
                            }else{
                                char stringLikeChar[20];
                                strcpy(stringLikeChar,SDL_GetKeyName(event.key.keysym.sym));
                                toLowerCase(stringLikeChar);
                                strcat(newPassword,stringLikeChar);
                                strcat(newPasswordhidden,"*");
                            }
                        }else if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9  ){
                                strcat(newPassword,SDL_GetKeyName(event.key.keysym.sym));
                                strcat(newPasswordhidden,"*");
                        }else if (event.key.keysym.sym == SDLK_RETURN){
                            strcpy(Indicative3,"Confirme contraseña nueva:");
                            userWriting=2;
                        }
                    }else if ( userWriting==2){
                        if (event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z){
                            if (SDL_GetModState() & KMOD_CAPS || SDL_GetModState() & KMOD_SHIFT){
                                strcat(newPasswordConfirmacion,SDL_GetKeyName(event.key.keysym.sym));
                                strcat(newPasswordConfirmacionhidden,"*");
                            }else{
                                char stringLikeChar[20];
                                strcpy(stringLikeChar,SDL_GetKeyName(event.key.keysym.sym));
                                toLowerCase(stringLikeChar);
                                strcat(newPasswordConfirmacion,stringLikeChar);
                                strcat(newPasswordConfirmacionhidden,"*");
                            }
                        }else if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9  ){
                                strcat(newPasswordConfirmacion,SDL_GetKeyName(event.key.keysym.sym));
                                strcat(newPasswordConfirmacionhidden,"*");
                        }else if (event.key.keysym.sym == SDLK_RETURN){
                            //se rellenan las cadenas para evitar que se compare basura
                            FillString( newPassword, sizeof ( newPassword) );
                            FillString( newPasswordConfirmacion, sizeof ( newPasswordConfirmacion) );
                            if (strcmp(newPassword, newPasswordConfirmacion)==0){
                                FillString(newPassword,13);
                                ChangePassword(nombreUsario,newPassword);
                            }else{
                                strcpy(warnings,"las contraseñas No coinciden");
                            }
                        }
                    }
                }

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderClear(renderer);

            createText(renderer , font, color, title, 10,10);
            createText(renderer , font, color, subTitle, 15,30);
            createText(renderer , font, color, Indicative, 10,50);createText(renderer , font, color, currentPasswordhidden, 180,50);
            createText(renderer , font, color, Indicative2, 10,70);createText(renderer , font, color, newPasswordhidden, 180,70);
            createText(renderer , font, color, Indicative3, 10,90);;createText(renderer , font, color, newPasswordConfirmacionhidden, 210,90);
            createText(renderer , font, color, warnings, 10,110);



            // Actualizar pantalla
            SDL_RenderPresent(renderer);
        }   
    }
}





void cambiarContraRoot(SDL_Renderer *renderer , TTF_Font *font ,SDL_Color color){
        char title[40]= "APLICATIVO - GRAFICADOR - UTP";
        char subTitle[60]= "Menu para usuario: root Actualizar contraseña ";
        char Indicative1[40]= "Entre contraseña antigua:";
        char legacyPassword[13]=" ";
        char newPasswordLabel[40]=" ";
        char newPassword[13]=" ";
        char newPasswordHidden[13]=" ";
        char contrasenaConfirmedLabel[40]=" ";
        char contrasenaConfirmed[13]=" ";
        char contrasenaConfirmedHidden[13]=" ";
        char warnings[60]=" ";
        int writingUser=0;
        int flag=1;
        USER root1; //se guardara el usuario root


        while (flag) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT ) {
                    flag = 0;
                }else if(event.type==SDL_KEYDOWN){

                    if (writingUser==0){
                        if (event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z){
                            if (SDL_GetModState() & KMOD_CAPS || SDL_GetModState() & KMOD_SHIFT){
                                strcat(legacyPassword,SDL_GetKeyName(event.key.keysym.sym));
                            }else{
                                char ara[20];
                                strcpy(ara,SDL_GetKeyName(event.key.keysym.sym));
                                toLowerCase(ara);
                                strcat(legacyPassword,ara);
                            }
                        }else if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9  ){
                                strcat(legacyPassword,SDL_GetKeyName(event.key.keysym.sym));
                        }else if (event.key.keysym.sym == SDLK_RETURN){
                                USER root;
                                FillString(legacyPassword, 13);
                                strcpy(root1.password, legacyPassword);
                                EncryptPassword(&root1);
                                binarySearchUserName(" root              ",&root);
                                if ( ComparePassword(root1.password, root)==0){
                                    strcpy(newPasswordLabel,"Entre su nueva contraseña: ");
                                    writingUser=1;
                                }else{
                                    strcpy(warnings, "Contrasena incorrecta" );
                                    createText(renderer , font, color, warnings, 10,80);
                                    SDL_RenderPresent(renderer);   
                                    SDL_Delay(3000);
                                    flag=0;
                                }

                        }
                    }else if (writingUser==1){
                        if (event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z){
                            if (SDL_GetModState() & KMOD_CAPS || SDL_GetModState() & KMOD_SHIFT){
                                strcat(newPassword,SDL_GetKeyName(event.key.keysym.sym));
                                strcat(newPasswordHidden,"*");
                            }else{
                                char stringLikeChar[20];
                                strcpy(stringLikeChar,SDL_GetKeyName(event.key.keysym.sym));
                                toLowerCase(stringLikeChar);
                                strcat(newPassword,stringLikeChar);
                                strcat(newPasswordHidden,"*");
                            }
                        }else if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9  ){
                                strcat(newPassword,SDL_GetKeyName(event.key.keysym.sym));
                                strcat(newPasswordHidden,"*");
                        }else if (event.key.keysym.sym == SDLK_RETURN){
                            writingUser=2;
                            strcpy(contrasenaConfirmedLabel,"Confirme su contrasena:");
                            
                        }
                    }else if (writingUser==2){
                        if (event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z){
                            if (SDL_GetModState() & KMOD_CAPS || SDL_GetModState() & KMOD_SHIFT){
                                strcat(contrasenaConfirmed,SDL_GetKeyName(event.key.keysym.sym));
                                strcat(contrasenaConfirmedHidden,"*");
                            }else{
                                char stringLikeChar[20];
                                strcpy(stringLikeChar,SDL_GetKeyName(event.key.keysym.sym));
                                toLowerCase(stringLikeChar);
                                strcat(contrasenaConfirmed,stringLikeChar);
                                strcat(contrasenaConfirmedHidden,"*");
                            }
                        }else if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9  ){
                                strcat(contrasenaConfirmed,SDL_GetKeyName(event.key.keysym.sym));
                                strcat(contrasenaConfirmedHidden,"*");
                        }else if (event.key.keysym.sym == SDLK_RETURN){
                            if ( strcmp(newPassword,contrasenaConfirmed)==0){
                                FillString(newPassword,13);
                                ChangePassword(" root              ", newPassword);
                                strcpy(warnings, legacyPassword );
                                createText(renderer , font, color, warnings, 10,80);
                                SDL_RenderPresent(renderer);   
                                SDL_Delay(3000);
                                flag=0;
                            }else{
                                    strcpy(warnings, "Contrasenas diferentes" );
                                    createText(renderer , font, color, warnings, 10,80);
                                    SDL_RenderPresent(renderer);   
                                    SDL_Delay(3000);
                                    flag=0;
                            }

                        }
                    }
            
                }
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderClear(renderer);

            createText(renderer , font, color, title, 10,10);
            createText(renderer , font, color, subTitle, 15,30);
            createText(renderer , font, color, Indicative1, 10,50);
            createText(renderer , font, color, legacyPassword, 180,50);
            createText(renderer , font, color, newPasswordLabel, 10,70);
            createText(renderer , font, color, newPasswordHidden, 200,70);
            createText(renderer , font, color, contrasenaConfirmedLabel, 10,90);
            createText(renderer , font, color, contrasenaConfirmedHidden, 200,90);
            createText(renderer , font, color, warnings, 10,80);


            // Actualizar pantalla
            SDL_RenderPresent(renderer);
        }   
    }
}

void ChangePasswordUser(SDL_Renderer *renderer , TTF_Font *font ,SDL_Color color){
        char title[40]= "APLICATIVO - GRAFICADORA - UTP";
        char subTitle[60]= "Menu para usuario: root Actualizar contraseña ";
        char Indicative[40]= "Entre nueva contraseña:";
        char userNameLabel[40]= "Username:";
        char userName[20]=" ";
        char passWord[13]=" "; 
        char passWordHidded[60]=" ";
        char warnings[32]=" ";
        int userWriting=0;
        int flag =1;

        while (flag ==1) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT ) {
                    flag = 0;
                }else if(event.type==SDL_KEYDOWN){

                if ( userWriting==0){
                    if (event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z){
                        if (SDL_GetModState() & KMOD_CAPS || SDL_GetModState() & KMOD_SHIFT){
                            strcat(userName,SDL_GetKeyName(event.key.keysym.sym));
                        }else{
                            char stringLikeChar[20];
                            strcpy(stringLikeChar,SDL_GetKeyName(event.key.keysym.sym));
                            toLowerCase(stringLikeChar);
                            strcat(userName,stringLikeChar);
                        }
                    }else if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9  ){
                            strcat(userName,SDL_GetKeyName(event.key.keysym.sym));
                    }else if (event.key.keysym.sym == SDLK_RETURN){
                        USER userFound;
                        FillString(userName,20);
                        if ( binarySearchUserName(userName, &userFound)!=-1){
                            strcpy(passWordHidded,"Ingresa nueva contraseña:");
                            userWriting=1;
                        }else{
                            strcpy(warnings,"Este usario no existe");
                        }
                    }
                    }else{
                        if (event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z){
                            if (SDL_GetModState() & KMOD_CAPS || SDL_GetModState() & KMOD_SHIFT){
                                strcat(passWord,SDL_GetKeyName(event.key.keysym.sym));
                                strcat(passWordHidded,"*");
                            }else{
                                char ara[20];
                                strcpy(ara,SDL_GetKeyName(event.key.keysym.sym));
                                toLowerCase(ara);
                                strcat(passWord,ara);
                                strcat(passWordHidded,"*");
                            }
                        }else if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9  ){
                                strcat(passWord,SDL_GetKeyName(event.key.keysym.sym));
                                strcat(passWordHidded,"*");
                        }else if (event.key.keysym.sym == SDLK_RETURN){
                            USER userToChangePassword;
                            FillString(passWord,13);
                            strcpy(userToChangePassword.password,passWord);
                            EncryptPassword(&userToChangePassword);
                            strcpy(warnings,passWord);
                            ChangePassword(userName,passWord);
                        }
                    }
                }
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderClear(renderer);

            createText(renderer , font, color, title, 10,10);
            createText(renderer , font, color, subTitle, 15,30);
            createText(renderer , font, color, userNameLabel, 10,50);
            createText(renderer , font, color, userName, 80,50);
            createText(renderer , font, color, passWordHidded, 10,70);
            createText(renderer , font, color, warnings, 10,90);
            createText(renderer , font, color, passWord, 10,110);


            // Actualizar pantalla
            SDL_RenderPresent(renderer);
        }   
    }
}

void removeUser(SDL_Renderer *renderer , TTF_Font *font ,SDL_Color color){
        char title[40]= "APLICATIVO - GRAFICADOR - UTP";
        char subTitle[60]= "Menu para usuario: root Borrar usuario";
        char Indicative[40]= "Entre usuario:";
        char userNameLabel[40]= "Username:";
        char userName[20]=" ";
        char warnings[32]=" ";
        int flag=1;

        while (flag) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT ) {
                    flag = 0;
                }else if(event.type==SDL_KEYDOWN){

                    if (event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z){
                        if (SDL_GetModState() & KMOD_CAPS || SDL_GetModState() & KMOD_SHIFT){
                            strcat(userName,SDL_GetKeyName(event.key.keysym.sym));
                        }else{
                            char stringLikeChar[20];
                            strcpy(stringLikeChar,SDL_GetKeyName(event.key.keysym.sym));
                            toLowerCase(stringLikeChar);
                            strcat(userName,stringLikeChar);
                        }
                    }else if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9  ){
                            strcat(userName,SDL_GetKeyName(event.key.keysym.sym));
                    }else if (event.key.keysym.sym == SDLK_RETURN){
                            USER userToDelete;
                            FillString(userName, 20);
                            int index = binarySearchUserName(userName, &userToDelete);
                            if ( index == -1){
                                strcpy(warnings, "no existe");
                            }else{
                                if ( strcmp(" root              ",userName)==0){
                                    strcpy(warnings, "Este usuario no se puede borrar");
                                }else{
                                    DeleteUser(userName);
                                    flag=0;
                                }
                            }
                    }
            
                }
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderClear(renderer);

            createText(renderer , font, color, title, 10,10);
            createText(renderer , font, color, subTitle, 15,30);
            createText(renderer , font, color, userNameLabel, 10,50);
            createText(renderer , font, color, userName, 80,50);
            createText(renderer , font, color, warnings, 10,70);


            // Actualizar pantalla
            SDL_RenderPresent(renderer);
        }   
    }
}

void ListUsers(SDL_Renderer *renderer , TTF_Font *font ,SDL_Color color){
    bool quit = false;
    int scrollOffset = 20;
    char title[]= "Aplicativo -- Graficador -- UTP";
    char subtitle[]= "Listado de accesos por usuario";
    char userNameLabel[]="Entre el usuario:";
    char userName[20]=" ";
    char titleData[]="Fecha             Nro grafico    Polinomio f(x)";

    bool tin =true;
    while (tin) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT ) {
                    tin = false;
                }else if(event.type==SDL_KEYDOWN){

                    if (event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z){
                        if (SDL_GetModState() & KMOD_CAPS || SDL_GetModState() & KMOD_SHIFT){
                            strcat(userName,SDL_GetKeyName(event.key.keysym.sym));
                        }else{
                            char stringLikeChar[20];
                            strcpy(stringLikeChar,SDL_GetKeyName(event.key.keysym.sym));
                            toLowerCase(stringLikeChar);
                            strcat(userName,stringLikeChar);
                        }
                    }else if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9  ){
                            strcat(userName,SDL_GetKeyName(event.key.keysym.sym));
                    }else if (event.key.keysym.sym == SDLK_RETURN){
                        USER userFound;
                        FillString(userName,20);
                        int index =binarySearchUserName(userName, &userFound);
                        if (index !=-1){
                            tin=false;
                        }else{
                            quit=true;
                            tin=false;   
                                    createText(renderer , font, color, "Este usuario no existe", 10,80);
                                    SDL_RenderPresent(renderer);   
                                    SDL_Delay(3000);       
                        }
                    }
            
                }

            SDL_RenderClear(renderer);

            createText(renderer , font, color, title, 10,10);
            createText(renderer , font, color, subtitle, 15,30);
            createText(renderer , font, color, userNameLabel, 10,50);
            createText(renderer , font, color, userName, 120,50);


            // Actualizar pantalla
            SDL_RenderPresent(renderer);
        }   
    }

    int k = GetQuantityUserGraphics ( userName );

    if (k>0){
        GRAPHICS vector[ k ];
        GetUserGraphicsVector ( userName, vector );
        while (!quit ) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    quit = true;
                }else if (event.type==SDL_KEYDOWN){
                    if( event.key.keysym.sym == SDLK_RETURN){
                        quit= true;
                    }
                } else if (event.type == SDL_MOUSEWHEEL) {
                    if (event.wheel.y > 0) {
                        // Scroll up
                        scrollOffset -= 20;
                        if (scrollOffset < 0) {
                            scrollOffset = 0;
                        }
                    } else if (event.wheel.y < 0) {
                        // Scroll down
                        scrollOffset += 20;
                    }
                }
            }

            SDL_RenderClear(renderer);

            int y = 20 - scrollOffset; // Aplicar el desplazamiento vertical
            if (y > 0) y = 0; // Asegurar que no se desplace más allá del inicio
            
            createText(renderer ,font,color,title, 20, y);y+=20;
            createText(renderer ,font,color,subtitle, 20, y);y+=20;
            createText(renderer ,font,color,userNameLabel, 20, y);y+=20;
            createText(renderer ,font,color,titleData, 20, y);y+=20;

            // Imprimir la tabla
            for (int i = 0; i < k; i++) {
                char texto[80];
                sprintf(texto, "%s    |     %s     |   %s", vector[i].date,vector[i].graphicKey, vector[i].function);
                createText(renderer ,font,color,texto, 20, y);y+=20;
                createText(renderer ,font,color,"-------------------------------------", 20, y);
                y += 20; // Ajusta la posición horizontal para la próxima celda
            }

            SDL_RenderPresent(renderer);
        }
    }else if (quit!=true){
        createText(renderer , font, color, "Este usuario no tiene graficas", 10,80);
        SDL_RenderPresent(renderer);   
        SDL_Delay(3000);
    }
}

void ListAllUsers(SDL_Renderer *renderer , TTF_Font *font ,SDL_Color color){
    bool quit = false; // variable que mantiene el bucle de esta funcion
    int scrollOffset = 20;
    char title[]= "Aplicativo -- Graficador -- UTP";
    char subtitle[]= "Listadodo de accesos todos los usuarios";
    char titleData[]="Fecha             Nro grafico    Polinomio f(x)";
    int cantidadDeUsuarios= GetUserKey(); // mira cuantos usuarios activos hay en la base

    if (cantidadDeUsuarios>1){
        USER todoslosUsuarios[cantidadDeUsuarios];
        ReadUsersFile(todoslosUsuarios);
        while (!quit) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT ) {
                    quit = true;
                }else if (event.type==SDL_KEYDOWN){
                    if( event.key.keysym.sym == SDLK_RETURN){
                        quit= true;
                    }
                } else if (event.type == SDL_MOUSEWHEEL) {
                    if (event.wheel.y > 0) {
                        // Scroll up
                        scrollOffset -= 20;
                        if (scrollOffset < 0) {
                            scrollOffset = 0;
                        }
                    } else if (event.wheel.y < 0) {
                        // Scroll down
                        scrollOffset += 20;
                    }
                }
            }

            SDL_RenderClear(renderer);

            int y = 20 - scrollOffset; // Aplicar el desplazamiento vertical
            if (y > 0) y = 0; // Asegurar que no se desplace más allá del inicio
            
            createText(renderer ,font,color,title, 20, y);y+=20;
            createText(renderer ,font,color,subtitle, 20, y);y+=20;

            // Imprimir la tabla
            for (int i = 0; i < cantidadDeUsuarios; i++) {
                char texto[80]; strcpy(texto,todoslosUsuarios[i].userName);
                if ( texto[0]==' ' && strcmp(texto," root              ")!=0  ){
                    int k = GetQuantityUserGraphics ( texto );
                    if ( k>0){
                        GRAPHICS vector[ k ];
                        char userNameLabel[40]="Username:";strcat(userNameLabel,texto);
                        GetUserGraphicsVector ( texto, vector );
                        createText(renderer ,font,color, userNameLabel, 20, y);y+=20;
                        createText(renderer ,font,color,titleData, 20, y);y+=20;
                        for (int i = 0; i < k; i++) {
                            char dataToPrint[80];
                            sprintf(dataToPrint, "%s    |     %s     |   %s", vector[i].date,vector[i].graphicKey, vector[i].function);
                            createText(renderer ,font,color,dataToPrint, 20, y);
                            y += 20; // Ajusta la posición horizontal para la próxima celda
                        }
                        createText(renderer ,font,color,"-------------------------------------------------", 20, y);
                        y += 20; // Ajusta la posición horizontal para la próxima celda
                    }
                }
            }
            createText(renderer , font, color, "ESTA ES TODA LA LISTA, SI NO VE NADA ES PORQUE NO SE HAN CREADO GRAFICAS", 10,y);

            SDL_RenderPresent(renderer);
        }

    }else{
        SDL_RenderClear(renderer);
        createText(renderer , font, color, "Aun no hay usarios creados", 10,80);
        SDL_RenderPresent(renderer);   
        SDL_Delay(3000);
    }   
}


void GraphicByReference(SDL_Renderer *renderer , TTF_Font *font ,SDL_Color color,char name[]){
    char title[70]= "APLICATIVO - GRAFICADOR - UTP -impresion de grafica por referencia";
    char subTitle[60]= "Crear Grafico";
    char Indicative[80]= "C = Coeficiente , E = exponente , entre termino en formato CXE de tenerlos";
    char Indicative2[80]= "Rango en formato Inicio,final ejem: -10,15 ";
    char IndicativeReferecia[40]= "Entre Nro(referencia):";
    char referencia[30]=  " ";
    char IndicativeNombre[40]= "Username: ";
    char nombre[20]=" ";
    char IndicativeFecha[40]= "Date=";
    char date[40]= " ";
    int userWriting=0;

    int flag=1;

    GRAPHICS funcionAGraficar;


    while (flag) {
        SDL_Event event;
            while (SDL_PollEvent(&event)) {
                    if (event.type == SDL_QUIT ) {
                        flag = 0;
                    }else if(event.type==SDL_KEYDOWN){

                    if ( userWriting==0){
                        if (event.key.keysym.sym == SDLK_x || event.key.keysym.sym == SDLK_PLUS || event.key.keysym.sym == SDLK_MINUS){
                            if (SDL_GetModState() & KMOD_CAPS || SDL_GetModState() & KMOD_SHIFT){
                                strcat(referencia,SDL_GetKeyName(event.key.keysym.sym));
                            }else{
                                char ara[20];
                                strcpy(ara,SDL_GetKeyName(event.key.keysym.sym));
                                toLowerCase(ara);
                                strcat(referencia,ara);
                            }
                        }else if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9  ){
                                strcat(referencia,SDL_GetKeyName(event.key.keysym.sym));
                        }else if (event.key.keysym.sym == SDLK_RETURN){
                            char nroRefencia[5]; sprintf(nroRefencia, "%04d", atoi(referencia));
                            BinarySearchGraphicKey(nroRefencia,&funcionAGraficar);
                            flag=0;
                        }
                        }
                    }
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderClear(renderer);

            createText(renderer , font, color, title, 10,10);
            createText(renderer , font, color, subTitle, 15,30);
            createText(renderer , font, color, Indicative, 10,50);
            createText(renderer , font, color, Indicative2, 10,70);
            createText(renderer , font, color, IndicativeReferecia, 10,90);
            createText(renderer , font, color, referencia, 150,90);

            // Actualizar pantalla
            SDL_RenderPresent(renderer);
            
        }
    }

    if ( ( strcmp(funcionAGraficar.userName, name)==0 || strcmp(" root              ", name)==0 ) && funcionAGraficar.userName[0] != '!' ){
        char rango[20];sprintf(rango, "%i,%i", funcionAGraficar.lowerBound,funcionAGraficar.upperBound);
        createText(renderer , font, color, IndicativeFecha, 350,90);
        createText(renderer , font, color, IndicativeNombre, 210,90);
        createText(renderer , font, color, funcionAGraficar.userName, 280,90);
        createText(renderer , font, color, funcionAGraficar.date, 390,90);
        createText(renderer , font, color, "Funcion:", 10,105);
        createText(renderer , font, color, funcionAGraficar.function, 70,105);
        createText(renderer , font, color, "Rango", 210,105);
        createText(renderer , font, color, rango, 280,105);
        createText(renderer , font, color, "Incremento:", 350,105);
        char incrimente[5];sprintf(incrimente,"%i",funcionAGraficar.increment);
        createText(renderer , font, color, incrimente, 430,105);
        GraphFunction(renderer, font, funcionAGraficar.function,funcionAGraficar.lowerBound,funcionAGraficar.upperBound,funcionAGraficar.increment);

        int flag2=1;
        while (flag2==1) {
                    SDL_Event event;
                    while (SDL_PollEvent(&event)) {
                        if (event.type == SDL_QUIT || event.key.keysym.sym== SDLK_RETURN) {
                            flag2 = 0;
                        }
                    }
        }
    }


}

void myGraphics(SDL_Renderer *renderer , TTF_Font *font ,SDL_Color color, char name[]){
    bool quit = false;
    int scrollOffset = 20;
    char title[]= "Aplicativo -- Graficadora -- UTP";
    char subtitle[70]= "Modulo listar accesos - Usuario:";strcat(subtitle,name);
    char sort[] = "Ordenar por date. Presione(1) Orden ascendente (2) Orden Descendente: ";//texto que muestra la eleccion de ordenamiento
    char option = '\0';//almacenara la opcion que ingreso el usuario para ordenar el listado y como estado de si ya ingreso la opcion o no
    char titleData[]="Fecha             Nro grafico    Ecuacion-F(x)";
    char warnings[40]=" ";

    int k = GetQuantityUserGraphics ( name );
    if ( k !=-1){
        GRAPHICS vector[ k ];
        GetUserGraphicsVector ( name, vector );


        while (!quit) {
            SDL_Event event;
            while (SDL_PollEvent(&event) ) {
                if (event.key.keysym.sym == SDLK_RETURN) {
                    quit = true;
                } else if (event.type == SDL_MOUSEWHEEL) {
                    if (event.wheel.y > 0) {
                        // Scroll up
                        scrollOffset -= 20;
                        if (scrollOffset < 0) {
                            scrollOffset = 0;
                        }
                    } else if (event.wheel.y < 0) {
                        // Scroll down
                        scrollOffset += 20;
                    }
                } else if (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_2 && option == '\0'  ){ // si he presioando un numero entre 1 y 2
                    option = event.key.keysym.sym;
                    strcat(sort,SDL_GetKeyName(event.key.keysym.sym)); // lo agramaos a sort
                }
            }

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderClear(renderer);

            int y = 20 - scrollOffset; // Aplicar el desplazamiento vertical
            if (y > 0) y = 0; // Asegurar que no se desplace más allá del inicio
            
            createText(renderer ,font,color,title, 20, y);y+=20;
            createText(renderer ,font,color,subtitle, 20, y);y+=20;
            createText(renderer ,font,color,sort, 20, y);y+=20;
            createText(renderer ,font,color,warnings, 20, y);y+=20;
            
            if ( k ==-1 ){
                strcpy(warnings, "Este usuario aun no ha creado graficas");
            }else{
                if ( option == '1' ) {
                    createText(renderer ,font,color,titleData, 20, y);y+=20;
                    for (int i = 0; i < k; i++) {
                        char texto[80];
                        sprintf(texto, "%s    |     %s     |   %s", vector[i].date,vector[i].graphicKey, vector[i].function);
                        createText(renderer ,font,color,texto, 20, y);y+=20;
                        createText(renderer ,font,color,"-------------------------------------", 20, y);
                        y += 20; // Ajusta la posición horizontal para la próxima celda
                    }
                } else if ( option == '2' ) {
                    createText(renderer ,font,color,titleData, 20, y);y+=20;
                    for (int i = k - 1; i >= 0; i--) {
                        char texto[80];
                        sprintf(texto, "%s    |     %s     |   %s", vector[i].date,vector[i].graphicKey, vector[i].function);
                        createText(renderer ,font,color,texto, 20, y);y+=20;
                        createText(renderer ,font,color,"-------------------------------------", 20, y);
                        y += 20; // Ajusta la posición horizontal para la próxima celda
                    }
                }
            }

            // Imprimir la tabla
            

            SDL_RenderPresent(renderer);
        }

    }else{
        while (!quit) {
            SDL_Event event;
            while (SDL_PollEvent(&event) ) {
                if (event.type == SDL_QUIT || event.key.keysym.sym == SDLK_RETURN) {
                    quit = true;
                }
            }  
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderClear(renderer);

            // Imprimir la tabla
            createText(renderer ,font,color,"AUN NO TIENES GRAFICAS", 20, 20);

            SDL_RenderPresent(renderer);
        }
    }
}


void removeGraphic(SDL_Renderer *renderer , TTF_Font *font ,SDL_Color color, char name[]){
        char title[40]= "APLICATIVO - GRAFICADOR - UTP"; //diseño de pantalla
        char subTitle[60]= "Menu borrar grafico para usuario:";strcat(subTitle,name); // desiseño de pantalla le agregamos el nombre del usuario actual
        char Indicative[40]= "Entre nroGrafica:";
        char nroGrafica[20]=" "; // variable que guarda el numero de de la grafica que se quiere imprimir
        char warnings[40]=" "; // variable que contendrá la adventencias que se deban hacer
        int flag=1; // variable encargada del bucle principal de este modulo

        while (flag) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT ) {
                    flag = 0;
                }else if(event.type==SDL_KEYDOWN){

                    if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9  ){
                            strcat(nroGrafica,SDL_GetKeyName(event.key.keysym.sym));
                    }else if (event.key.keysym.sym == SDLK_BACKSPACE){
                        DelateLastChar(nroGrafica);
                    }else if (event.key.keysym.sym == SDLK_RETURN){
                        char nroRefencia[5]; sprintf(nroRefencia, "%04d", atoi(nroGrafica)); //nroReferencia guarda el numero que se ha ingresado en formato xxxx
                        int status = DeleteGraphic(name,nroRefencia); // status busca la grafica y guarda el estado para la variable que se busca
                        if (status == -2){ // si el estatus es -2 entonces la grafica no existe
                            strcpy(warnings,"La grafica que deseas borrar no existe");
                        }else if ( status == -1){ // si el estatus es -1 , la grafica existe pero no es del usuario que la quiere imprimir
                            strcpy(warnings , "Este grafico no te pertenece");
                        }else{ 
                            
                            strcpy(warnings, "Se ha borrado el grafico con exito");
                        }
                    }
            
                }
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderClear(renderer);

            createText(renderer , font, color, title, 10,10);
            createText(renderer , font, color, Indicative, 15,50);
            createText(renderer , font, color, nroGrafica, 130,50);
            createText(renderer , font, color, subTitle, 15,30);
            createText(renderer , font, color, warnings, 10,70);


            // Actualizar pantalla
            SDL_RenderPresent(renderer);
        }   
    }
}

void modulo2User(SDL_Renderer *renderer , TTF_Font *font ,SDL_Color color, char nombreUsario[]) { //menu que sera presentado a un usario deferente a root
    //diseño de pantalla {
    char title[40]= "APLICATIVO - GRAFICADOR - UTP";
    char subTitle[60]= "Menu principal para usuario: ";strcat(subTitle, nombreUsario); // le agregamos a este texto el nombre del usuario actual
    char option0[40]= "0. Salir del aplicativo";
    char option1[40]= "1. Crear nuevo grafico";
    char option2[40]= "2. Borrar un grafico existente";
    char option3[40]= "3. Listar accesos";
    char option4[40]= "4. Imprimir grafico por referencia";
    char option5[60]= "5. Cambiar password del usuario:";strcat(option5, nombreUsario);//}  le agregamos a este texto el nombre del usuario actual
    int flag =1;
    while (flag==1) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if(event.type==SDL_KEYDOWN){
                switch(event.key.keysym.sym ){
                    case SDLK_0:
                                flag=0;break;
                    case SDLK_1:
                        CreateGraphic(renderer, font, color,nombreUsario);
                        break;
                    case SDLK_2:
                        removeGraphic(renderer, font, color,nombreUsario);
                        break;
                    case SDLK_3:
                        myGraphics(renderer, font, color,nombreUsario);
                        break;
                    case SDLK_4:
                        GraphicByReference(renderer, font, color,nombreUsario);
                        break;
                    case SDLK_5:
                        ChangeMyPassword(renderer, font, color, nombreUsario);
                        break;
                }
            }
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        createText(renderer , font, color, title, 10,10);
        createText(renderer , font, color, subTitle, 15,30);
        createText(renderer , font, color, option0, 10,50);
        createText(renderer , font, color, option1, 10,70);
        createText(renderer , font, color, option2, 10,90);
        createText(renderer , font, color, option3, 10,110);
        createText(renderer , font, color, option4, 10,130);
        createText(renderer , font, color, option5, 10,150);

        // Actualizar pantalla
        SDL_RenderPresent(renderer);
    }   
}
}

//
void RootModule(SDL_Renderer *renderer , TTF_Font *font ,SDL_Color color) {
    char title[40]= "APLICATIVO - GRAFICADOR - UTP"; // Estas son las opciones que se le presentaran
    char subTitle[40]= "Menu principal para usuario: root"; // al usuario root
    char option0[40]= "0. Salir del aplicativo";
    char option1[40]= "1. Crear usuario";
    char option2[40]= "2. Borrar usuario";
    char option3[40]= "3. Listar accesos por usuario";
    char option4[40]= "4. Listar accesos todos los usuarios";
    char option5[40]= "5. Imprimir grafico por referencia";
    char option6[40]= "6. Cambiar password del usuario: root";
    char option7[40]= "7. cambiar password de un usuario";

    int flag=1; // varible que mantiene la aplicacion es este modulo
    while (flag) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT ) {
                flag = 0;
            }else if(event.type==SDL_KEYDOWN){
                switch(event.key.keysym.sym ){
                    case SDLK_0:
                                flag=0;break;
                    case SDLK_1:
                        CreateUser(renderer, font, color);
                        break;
                    case SDLK_2:
                        removeUser(renderer, font, color);
                        break;
                    case SDLK_3:
                        ListUsers(renderer, font, color);
                        break;
                    case SDLK_4:
                        ListAllUsers(renderer, font, color);
                        break;
                    case SDLK_5:
                        GraphicByReference(renderer, font, color," root              ");
                        break;
                    case SDLK_6:
                        cambiarContraRoot(renderer, font, color);
                        break;
                    case SDLK_7:
                        ChangePasswordUser(renderer, font, color);
                        break;

                }
            }
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        createText(renderer , font, color, title, 10,10);
        createText(renderer , font, color, subTitle, 15,30);
        createText(renderer , font, color, option0, 10,50);
        createText(renderer , font, color, option1, 10,70);
        createText(renderer , font, color, option2, 10,90);
        createText(renderer , font, color, option3, 10,110);
        createText(renderer , font, color, option4, 10,130);
        createText(renderer , font, color, option5, 10,150);
        createText(renderer , font, color, option6, 10,170);
        createText(renderer , font, color, option7, 10,190);

        // Actualizar pantalla
        SDL_RenderPresent(renderer);
    }   
}
}





// recibe pantalla donde se renderizara , fuente con la que se hara y color del texto
void LogIn(SDL_Renderer *renderer , TTF_Font *font ,SDL_Color color){ // Este mudulo es el prin
    // Para este programa se ha decidido que los string se inicializaran con " "
    char userName[20] = " ";  // Es el el string que guarda el nombre de usuario que se entre
    char passHidded[30]=" "; // Es la contraseña enmascarada  (ejemplo: ****)
    char passWord[13]=" ";  // Es la contraseña sin enmascarar (ejemplo: hola)
    int  writingUser=0;    // Esta varible permite identificar que campo se esta escribiendo siendo { 0 = userName , 1 = password }
    char warnings[70]=" "; // Variable para poner advertencias o Indicatives que se le deban dar al usuario
    char titleOfModule[50] = "Aplicativo - Graficadora - UTP"; // titulo que se le pone al modulo
    char userNameLabel[20]="Username: "; // Indicative de lo que se debe escribir es el nombre de usuario
    int exitApp=0; //necesita habilitar para que cuando se presione cualquier tecla se finalice el programa
    int exitAppAuxiliar=0; // Es la variable encargada de habilitar exitApp a 1 (podria ser cualquier numero mayor a 0)
    char warnings2[70]=" ";
    char usuarioparamodulo2[20]=" "; // Esta variable sera enviada solo para UserModule , sirve de auxiliar para enviar el nombre del usuario que se ha loggueado
    int timesTried=0; // lleva el contador de las veces que se introduce una contraseña incorrectamente
    int flag =1 ;
    
     
    //while (flag) mantiene actualizando el modulo actual , esto para que el usuario pueda ver lo que va escribiendo 
    while (flag ==1 && timesTried!=3) {

        SDL_Event event; // event es una variable tipo evento , estas variables guardan lo que ha sucedido en el aplicativo
        // while (SDL_PollEvent(&event)) , ira mirando cada uno de los eventos que esten guardados en event
        while (SDL_PollEvent(&event)) {  //lee todos los eventos que han ocurrido
            if (event.type == SDL_QUIT || exitApp) { // si se debe finalizar el programa
                flag = 0;
            }
//--------------------------------- Code Optimizado ----------------------------------------------------------
            if(event.type==SDL_KEYDOWN){ // lee solo cuando la tecla fue presionada , no si se esta presionando

                if(exitAppAuxiliar==1){
                    exitApp=1;
                }else if (writingUser==0){ // userName

                    if (event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z){  // si se ha presionado  una tecla del abecedadario
                        if (SDL_GetModState() & KMOD_CAPS || SDL_GetModState() & KMOD_SHIFT){ // si hay un modificador que la haga mayuscula (esten las mayusculas activadas)
                            strcat(userName,SDL_GetKeyName(event.key.keysym.sym)); // Concatenamos la tecla que se quiere agregar a userName
                        }else{
                            char charLikeString[2]; // cadena auxiliar a la ..................
                            strcpy(charLikeString,SDL_GetKeyName(event.key.keysym.sym));// que añadimos el caracter q hemos presionado (para poder trabajarlos como string)
                            toLowerCase(charLikeString); //Esta funcion nos convierte a minusculas un string q contenga mayusculas
                            strcat(userName,charLikeString); // concatenamos la tecla que se quiere agragar a userName
                        }
                    }else if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9  ){ // si he presioando un numero
                            strcat(userName,SDL_GetKeyName(event.key.keysym.sym)); // lo agramaos a userName
                    }else if (event.key.keysym.sym == SDLK_BACKSPACE){ // si se ha presionado la tecla de borrar
                            DelateLastChar(userName);
                    }else if (event.key.keysym.sym == SDLK_RETURN){ // cuando el usuario presione enter
                        if (GetUserKey() == 0){ // esto indica qu es la primera vez que se ejecuta el programa, por ende se tratará de manera distinta
                            if ( strcmp(userName , " root")==0){ // la primera vez si o si se debe creear el usuario root , verificamos eso
                                strcpy(passHidded, "Password: "); // habilitamos el campo para Password
                                strcpy(warnings, " "); // y limpiamos advertencias de haberlas
                                writingUser= 1; // ajustamos writingUSer a uno (lo que signigica que ahora se escribira la contraseña)
                            }else{ // si la primera vez no ha añadido el usuario root
                                strcpy(warnings,"USUARIO INCORRECTO"); // notificamos que el usuario que se queire ingresar no es el adecuado
                                strcpy(userName, " "); // limpiamos el username que el usuario ha ingresado para que no se vea en pantalla ya
                                exitAppAuxiliar=1; // y habilitamos que para la proxima vez que se presione una tecla se cierre el aplicativo
                            }
                        }else{ // de lo contrario el usuario root ya existe , es decir no es la primera vez iniciado el programa
                            USER userFound; // guardará el usuario al que el usuario actual quiere acceder
                            FillString(userName,20);        
                            int index = binarySearchUserName(userName,&userFound); 
                            if ( index ==-1){ // si el usuario no existe
                                strcpy(warnings, "USUARIO NO EXISTENTE");
                                sprintf(warnings2, "%i", index);
                            }else{ // si el usuario existe entonces
                                strcpy(passHidded, "Password: "); // habiliamos el campo para la contraseña
                                writingUser= 1;  // habilitamos al usario para que escriba ahora la contraseña
                            }
                        }
                    }


                }else if (writingUser==1 ){ // contraseña
                    if (event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z){
                        strcat(passHidded,"*");
                        if (SDL_GetModState() & KMOD_CAPS || SDL_GetModState() & KMOD_SHIFT){
                            strcat(passWord,SDL_GetKeyName(event.key.keysym.sym));
                        }else {
                            char charLikeString[12];
                            strcpy(charLikeString,SDL_GetKeyName(event.key.keysym.sym));
                            toLowerCase(charLikeString);
                            strcat(passWord,charLikeString);
                        }
                    }else if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9  ){
                        strcat(passHidded,"*");
                        strcat(passWord,SDL_GetKeyName(event.key.keysym.sym));
                    }else if (event.key.keysym.sym == SDLK_BACKSPACE){
                        DelateLastChar(passWord);
                        DelateLastChar(passHidded);
                    }else if (event.key.keysym.sym == SDLK_RETURN ){
                        if ( GetUserKey()==0){ // si es la primera vez que se ejecuta el programa debemos verificar que la contraseña sea mayos de 4 caracteres
                            if (strlen(passWord) >=5){ // aqui hacemos esa verificacion
                                USER root; // creamos el usuario que agregaremos a la base 
                                FillString(userName,20);
                                FillString(passWord,13);
                                strcpy ( root.userName, userName );
                                strcpy ( root.password, passWord );
                                // le asignos a root el userkey 0 (es el primero) | encriptamos su contraseña | lo añadimos a la base
                                root.userKey = 0; EncryptPassword( &root ); AddUserToFile ( root );
                                RootModule(renderer,font,color); // se ha creado correctamente el usario, por ende lo mandamos a la pantalla de rood
                            }else{// si el usuario ha entrado correctamente el userName (la primera vez debe ser root) pero no ingresa una contraseña adecuada
                                strcpy(passWord," "); // borramos todo lo ya hecho
                                strcpy(passHidded," ");
                                strcpy(userName," ");//--------
                                timesTried++; // aumentamos las veces intentadas a 1 
                                writingUser=0; // y le decimos que vuelva a ingresar datos desde userName
                            }
                        }else { // si no es la primera vez iniciando el aplicativo
                            USER userFound; // creamos el USER que guarda el usuario al que se quiere acceder 
                            FillString(userName,20);
                            FillString(passWord,13);
                            int index = binarySearchUserName(userName,&userFound);
                            if ( index ==-1){ // si el no usuario existe
                                strcpy(warnings, "USUARIO NO EXISTENTE");
                            }else{ 
                                strcpy(passHidded, "Password: ");
                                strcpy(warnings2,  passWord);
                                //BORRAAAAR----------------NO SE SI DEBA CAMBIAR OTRO LUGAR DE LA CONTRASEÑA, YO SOLO HICE ESTE CAMBIO EN LO DE LOS MODULOS--------------------
                                //Ya que la funcion EncryptPassword solicita es una estructura, creo esta para enmascarar la contra 
                                USER structToEncryptPassword;
                                //almaceno la contraseña en la nueva estructura
                                strcpy ( structToEncryptPassword.password, passWord );
                                //encripto la contraseña
                                EncryptPassword ( &structToEncryptPassword );
                                if ( ComparePassword( structToEncryptPassword.password, userFound) ==0){
                                    strcpy(usuarioparamodulo2,userName);                                
                                    if ( strcmp (" root              ", userName)==0){
                                        strcpy(userName," ");
                                        strcpy(passWord," ");
                                        flag=0;
                                        RootModule(renderer, font, color);
                                        writingUser=0;
                                        
                                    }else{
                                        strcpy(userName," ");
                                        flag=0;
                                        modulo2User(renderer, font, color, usuarioparamodulo2);
                                    }
                                }else{
                                    strcpy(userName, " " );
                                    strcpy(passWord, " " );
                                    writingUser=0;
                                    
                                }
                            }
                    

                        }
                    }
            }
        }
    }
        // Limpiar pantalla
        SDL_RenderClear(renderer);
        
        createText(renderer , font, color, userName, 80,50); //renderiza el texto que contiene lo que el usuario vaya ingresando
        createText(renderer , font, color, titleOfModule, 5,10);
        createText(renderer , font, color, userNameLabel, 10,50); //renderiza la el texto "Usermame: " como Indicative
        createText(renderer,font,color, passHidded, 10,75);
        createText(renderer,font,color,warnings,10,120); //renderiza las advertencias que se tengan que hacer
        createText(renderer,font,color,warnings2,10,140); //renderiza las advertencias que se tengan que hacer

        // Actualizar pantalla
        SDL_RenderPresent(renderer);
    }
}

void InitGraphics(SDL_Window **window, SDL_Renderer **renderer, TTF_Font **font) { // funcion para inicializar las graficas 
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {  // inicia todo lo relacionado con la visualización de gráficos en la pantalla,creación de ventanas,eventos de ventana,color y la renderización de gráficos.
        SDL_Log("SDL no pudo inicializarse! SDL_Error: %s\n", SDL_GetError());
    }

    if (TTF_Init() == -1) { // inicializa TTF para manejar texto
        SDL_Log("SDL_ttf no pudo inicializarse! SDL_ttf Error: %s\n", TTF_GetError());
    }
    //                                        |Donde se creara la ventana en la pantalla      | El tamaño de la ventana    | le decimos que se muestre cuando se cree|
    *window = SDL_CreateWindow("GRAFICADORA", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (*window == NULL) {
        SDL_Log("La ventana no pudo ser creada! SDL_Error: %s\n", SDL_GetError());
    }

    // crea el renderizador , window es la ventana que hemos creado y siguiente parametro es el controlador que se le debe asignar, al ser -1 le decimos a SDL que elija
    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);  //el ultimo parametro le decimos que si hay un GPU dispobible la utilice para optimizacion
    if (*renderer == NULL) {
        SDL_Log("El renderer no pudo ser creado! SDL_Error: %s\n", SDL_GetError());
    }

    // Abre y deja disponible el uso de una fuente IMPORTANTE: tener la fuente instalada en el pc y con la misma ruta
    *font = TTF_OpenFont("c:\\Windows\\Fonts\\ARIAL.TTF", 14); // ubicacion de la fuente , tamaño de esta
    if (*font == NULL) {
        SDL_Log("No se pudo cargar la fuente! SDL_ttf Error: %s\n", TTF_GetError());
    }

    // Establecer el color de fondo (RGB: rojo, verde, azul, alfa)
    SDL_SetRenderDrawColor(*renderer, 255, 255, 255, 255); // Color blanco
}

void closeGraphics(SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font) { // funcion para cerrar las librias graficas y terminar el aplicativo
    TTF_CloseFont(font); // Terminamos el uso de la fuente
    TTF_Quit(); //Terminamos el uso de la libreria TTF que nos permite el manejo de texto en la pantalla
    SDL_DestroyRenderer(renderer); // destruimos el renderes de la ventana 
    SDL_DestroyWindow(window); // destruimos la ventana
    SDL_Quit(); // Terminamos el uso de SDL
}

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL; // se crea una ventana (lienzo) donde vamos a poner nuestro contenido
    SDL_Renderer* renderer = NULL; // se crea el espacio que se quiere renderizar , render es lo que nos permite dibujar en el lienzo (window)
    TTF_Font* font = NULL; // TTF_Font es una variable para guarda la fuente que se utilizara y su tamaño
    SDL_Color color = {0, 0, 0, 255}; 
    InitGraphics(&window, &renderer, &font); //llamamos la funcion InitGraphics para que esta le de valores a nuestras variables
    LogIn(renderer, font, color); // Se inicia el programa como tal con el modulo 1;
    closeGraphics(window, renderer, font); // cerramos las librerias visuales y todo lo relacionado con los graficos
    return 0;
}