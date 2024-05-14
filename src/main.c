#include <SDL.h> //Libreria que nos permite manejar graficos como ventanas
#include <SDL_ttf.h> // Libreria que nos permite manejar texto que queramos mostrar en pantalla
#include <stdio.h>   // Libreria estandar para input y outpunt de c
#include <math.h>   // libreria que nos permite realizar operaciones matematematicas
#include <stdlib.h> // libria de c , de ella utilizamos funciones como Atoi, size_t y la constante NULL
#include <time.h> // libreria que nos permite acceder al tiempo de la maquina donde se ejecuta el programa
#include <string.h> // Libreria que nos expande el uso y las posibilidades de manipulacion para cadenas de Chars
#include <ctype.h> // Libreria para cadenas en este codigo usada la funcion toLower
#define WINDOW_WIDTH 790 // Definimos el ancho de la ventana
#define WINDOW_HEIGHT 640 // Definimos la altura de la ventana

int running = 1;

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
void MergeUsers ( USER originalStruct[], int start, int half, int end, char *field );
void MergeSortUsers ( USER originalStruct[], int start, int end, char *field );
void ReadUsersFile( USER vector[] );
void WriteUsersFile( USER vector[], int numberRegisters );
void OrganizeUsersFile ();
int binarySearchUserName ( char userNameToSearch[], USER* result );
int GetUserKey ( );
void AddUserToFile( USER user );
void CreateNewUser ();
void EncryptPasswordWordKey( USER *userToEncryptPassword, char keyWord[] );
void EncryptPasswordDisplacement( USER *userToEncryptPassword, int displacement );
void EncryptPassword ( USER *user );
int ComparePassword ( char passwordEntered[], USER originalUser );
void LoginUser ();
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

int DeleteGraphic ( char userName[], char graphicKey[] );
int BinarySearchGraphicKey ( char graphicKey[], GRAPHICS* result );

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
        //en caso de rror retornara un -1000
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



/*Función que fusiona dos subvectores ordenados en un subvector ordenado
originalStruct[]: array de estructura "original", start: indicador de inicio, half: indicador de mitad, end: indicador de final, field: campo en el que se ordenara*/
void MergeUsers ( USER originalStruct[], int start, int half, int end, char *field ) {
    /*sizeLeft: tamaño para el array de la izquierda, sizeRight: tamaño para el array de la derecha*/
    int sizeLeft = half - start + 1, sizeRight = end - half;
    // arrays temporales para realizar el algoritmo de merge sort 
    USER left[ sizeLeft ], right[ sizeRight ];


    /*Copia de los datos a los arrays temporales left[] y right[]
    counterLeft: contador de la izquierda para copiar datos*/
    for ( int counterLeft = 0; counterLeft < sizeLeft; counterLeft++ )
        left[ counterLeft ] = originalStruct [ start + counterLeft ];
    //end for ( int counter = 0; counter < sizeLeft; counter++ )

    /*counterRight: contador de la derecha para copiar datos*/
    for ( int counterRight = 0; counterRight < sizeRight; counterRight++ )
        right[ counterRight ] = originalStruct [ half + counterRight + 1 ];
    //end for ( int counter = 0; counter < sizeRight; counter++ )

    /* Unimos los arrays temporales en arr[ inicio..fin ]
    counterLeft: Índice inicial del primer subarray, counterRight: Índice inicial del segundo subarray
    counterVector: Índice inicial del subarray unido*/
    int counterLeft = 0, counterRight = 0, counterVector = start;

    while ( counterLeft < sizeLeft && counterRight < sizeRight ) {
        if ( CompareUsers ( &left [ counterLeft ], &right [ counterRight ], field ) <= 0 ) {
            originalStruct [ counterVector ] = left [ counterLeft ];
            counterLeft++;
        } else {
            originalStruct [ counterVector ] = right [ counterRight ];
            counterRight++;
        }//end if ( CompareUsers ( &left [ counterLeft ], &right [ counterRight ], field ) <= 0 )
        counterVector++;
    }//end while ( counterLeft < sizeLeft && counterRight < sizeRight )

    // Copiamos los elementos restantes de left[], si hay algunos
    while ( counterLeft < sizeLeft ) {
        originalStruct[ counterVector ] = left [ counterLeft ];
        counterLeft++;
        counterVector++;
    }//end while ( counterLeft < sizeLeft )

    // Copiamos los elementos restantes de right[], si hay algunos
    while ( counterRight < sizeRight ) {
        originalStruct[ counterVector ] = right [ counterRight ];
        counterRight++;
        counterVector++;
    }//end while ( counterRight < sizeRight )
    return;
}// end function MergeUsers ( USER originalStruct[], int start, int half, int end, char *field )

/*Function que implementa el algoritmo Merge Sort
originalStruct[]: array de estructura USER original, start: indicador de inicio, end: indicador de final, field: campo en el que se ordenara*/
void MergeSortUsers ( USER originalStruct[], int start, int end, char *field ) {
    if ( start < end ) {
        // Lo mismo que (inicio+fin)/2, pero evita overflow para grandes inicio y fin
        int half = start + ( end - start ) / 2;
        // Ordena la primera y la segunda mitad
        MergeSortUsers ( originalStruct, start, half, field );
        MergeSortUsers ( originalStruct, half + 1, end, field );
        MergeUsers ( originalStruct, start, half, end, field );
    }//end if ( start < end )
    return; 
}// end function MergeSortUsers ( USER originalStruct[], int start, int end, char *field )

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
    //MergeSortUsers( usersVector, 0, numberRegisters - 1, fieldToSort );
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

/*Funcion para añadir un nuevo usuario*/
void CreateNewUser () {
    //usuario a añadir
    USER newUser;
    printf ( "Aplicativo - Graficador - UTP\nMenu para usuario XXXX Crear nuevo usuario\nEntre el nuevo usuario:\nUsername: " );
    scanf ( "%s", newUser.userName );
    printf ( "Password: " );
    scanf ( "%s", newUser.password );
    //estructura para buscar si ese usuario ya existe
    USER userSearched;
    //lugar donde se encuentra el usuario SI ES QUE EXISTE
    int index = binarySearchUserName ( newUser.userName, &userSearched );
    if ( index != -1 ) {
        printf ( "USUARIO EXISTENTE\n" );
        printf("Nombre encontrado en el indice %d:\n", index);
        printf("userKey: %d, userName: %s, password: %s\n", userSearched.userKey, userSearched.userName, userSearched.password);
    } else {
        newUser.userKey = GetUserKey ();
        EncryptPassword ( &newUser );
        AddUserToFile ( newUser );
        OrganizeUsersFile();
    }//end if ( index != -1 )
    return;
}//end function CreateNewUser ()



/*Funcion para ingresar usuario*/
void LoginUser () {
    //usuario a ingresar
    USER user;
    printf ( "Aplicativo - Graficador - UTP\nMenu para usuario XXXX Ingresar usuario\nEntre el usuario:\nUsername: " );
    scanf ( "%s", user.userName );
    printf ( "Password: " );
    scanf ( "%s", user.password );
    //estructura para buscar si ese usuario si existe
    USER userSearched;
    //lugar donde se encuentra el usuario SI ES QUE EXISTE
    int index = binarySearchUserName ( user.userName, &userSearched );
    if ( index != -1 ) {
        printf ( "USUARIO EXISTENTE\n" );
        EncryptPassword ( &user );
        printf ( "contraseña igresada: %s\n", user.password );
        if ( ComparePassword ( user.password, userSearched ) == 0 )
            printf("CONTRASEÑA CORRECTA\n" );
        else 
            printf("CONTRASEÑA INCORRECTA\n");
    } else {
        printf ( "USUARIO NO EXISTENTE\n" );
    }//end if ( index != -1 )
    return;
}//end function CreateNewUser ()


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
    //keyWord: palabra clave con la que se encriptara
    char keyWord[22] = "yisankedEgrINpumcJotA";
    //displacement: desplazamiento con el que encriptara
    int displacement = 18;
    EncryptPasswordWordKey ( user, keyWord );
    EncryptPasswordDisplacement ( user, displacement );
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
    //MergeSortUsers( usersVector, 0, numberRegisters - 1, fieldToSort );
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

    ReadUsersFile( usersVector );
    //se cambia el usuario orignal por el nuevo que contiene la nueva contraseña
    usersVector[ userPosition ] = user;
    //se organiza el archivo
    BubbleSortUsers ( usersVector, numberRegisters - 1, fieldToSort );
    //MergeSortUsers( usersVector, 0, numberRegisters - 1, fieldToSort );
    //se escribe le archivo con el cambio hecho
    WriteUsersFile( usersVector, numberRegisters );

    return;
}//end funcion void ChangePassword ( char userName[], char newPassword[] )


/*------------------------------------------------------------FUNCIONES PARA AGREGAR GRAFICO A EL ARCHIVO----------------------------------------------------------------------------------------*/

/*Funcion para crear la fecha actual en el sistema que se ejecuta
graphic: estructura GRAPHICS en la cual se pondra la fecha actual*/
void CreateDate( GRAPHICS *graphic ) {
    // Obteniendo la fecha y hora actual
    // currentTime: almacena la hora actual del sistema en forma de un valor de tiempo.
    time_t currentTime;
    // timeInformation: Almacena la información desglosada de la fecha e icnluso la hora en la estructura pereteneciente al lenguaje llamada tm
    struct tm *timeInformation;

    // Obteniendo el tiempo actual del SISTEMA 
    time( &currentTime );
    // Convierte el tiempo a la estructura de fecha y hora local
    timeInformation = localtime( &currentTime );

    // Formatea la fecha actual
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
    }
    //se busca hacia adelante de forma secuencial si hay mas graficas para agregar
    for ( int forIndex = indexGraphicsTotalVector + 1; forIndex != numberRegisters && strcmp ( graphicsTotalVector[ forIndex ].userName, userName ) == 0 ; forIndex++  ) {
        userGraphicsVector[ indexUserGraphicsVector ] = graphicsTotalVector[ forIndex ];
        indexUserGraphicsVector++;
    }
    //se ordena medainte metodo de burbuja por le campo de "graphicKey" ya que al darse el graphicKey por orden de entrada de grafica, este puede ayudar a organizar por fecha
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
    //end if ( graphicPosition == -1 )

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



int stringContains(char string[], char containThis) {
    int index = -1;
    for (int i = 0; i < strlen(string) && index == -1; i++) {
        if (string[i] == containThis) {
            index = i + 1;
        }
    }
    return index;
}

double CalculateResult(char str[], int x) {
    const char delim[] = "/";
    char *token = strtok(str, delim);
    double resultado = 0;
    while (token != NULL) {
        char termino[10];
        strcpy(termino, token);
        int esConstante = stringContains(termino, 'x');
        if (esConstante != -1) {
            char exponente[20];
            strcpy(exponente, termino + esConstante);
            char coeficiente[20];
            strncpy(coeficiente, termino, esConstante);
            resultado += atoi(coeficiente) * (pow(x, atoi(exponente)));
        }else{
            resultado += atoi(termino);
        }
        token = strtok(NULL, delim);
    }
    
    return resultado;
}

void insertChar(char *str, char c, int pos) {
    int len = strlen(str);

    for(int i = len; i >= pos; i--){
        str[i + 1] = str[i];
    }

    str[pos] = c;
    str[len + 1] = '\0';
}

void SettearString(char *str , char addThisChar) {
    for (int i = 0; str[i] != '\0'; i++){
        if ( str[i + 1] == '+' || str[i + 1] == '-' ) {
            int pos = i + 1;
            insertChar(str, addThisChar, pos);
            i++;
        }
    }
}

void SettearString2(char *str  ) {
    for (int i = 0; str[i] != '\0'; i++) {
        if ( str[i] == 'x' && str[i + 1] == '/' ) {
            int pos = i + 1;
            insertChar(str, '1', pos);
            i++;
        }

        if ( (str[i] == '+' || str[i] == '-') && str[i + 1] == 'x' ) {
            int pos = i + 1;
            insertChar(str, '1', pos);
            i++;
        }
    }
}


void arreglarInicio(char *vojabes){
    char arreglo[40];
    arreglo[0]='+';

    for (int i=1; i < strlen(vojabes)+1; i++){
        arreglo[i] = vojabes[i-1];
    }

    vojabes[0]='\0';
    strcpy(vojabes,arreglo);
}


void modificarString(char *string){
    char uno[]="1";

    if( string[0]=='x') {
        arreglarInicio(string);
    }
    if( string[strlen(string)-1]=='x') {
        strcat(string,uno);   
    }
    SettearString(string, '/');
    SettearString2(string);
}

void GraphCase1(SDL_Renderer *renderer, char function[], int x1, int y1, int x2, int y2, int increment, int divX, int xmin, int xmax, double maximum){
    char aux[30] = "";
    int height = WINDOW_HEIGHT - 140;
    int height1 = WINDOW_HEIGHT - 20;
    strcpy(aux, function);

    x1 = 20 + (xmin*divX);
    x2 = x1 + divX; 
    y1 = height1 - ((CalculateResult(aux, xmin)/maximum)*height);
    strcpy(aux, function);
    y2 = height1 - ((CalculateResult(aux, xmin+increment)/maximum)*height);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );

    for( int i = xmin+(2*increment); i <= xmax; i = i+increment ){
        strcpy(aux, function);
        x1 = x2; 
        x2 = x1 + divX;
        y1 = y2; 
        y2 = height1 - ((CalculateResult(aux, i)/maximum)*height);
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );
    }
}

void GraphCase2(SDL_Renderer *renderer, char function[], int x1, int y1, int x2, int y2, int increment, int divX, int xmin, int xmax, double maximum){
    char aux[30] = "";
    int height = WINDOW_HEIGHT - 140;
    int height1 = WINDOW_HEIGHT - 20;
    strcpy(aux, function);

    x1 = 20;
    x2 = x1 + divX; 
    y1 = height1 - ((CalculateResult(aux, xmin)/maximum)*height);
    strcpy(aux, function);
    y2 = height1 - ((CalculateResult(aux, xmin+increment)/maximum)*height);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );

    for( int i = xmin+(2*increment); i <= xmax; i = i+increment ){
        strcpy(aux, function);
        x1 = x2; 
        x2 = x1 + divX;
        y1 = y2; 
        y2 = height1 - ((CalculateResult(aux, i)/maximum)*height);
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );
    }
}

void GraphCase3(SDL_Renderer *renderer, char function[], int x1, int y1, int x2, int y2, int increment, int divX, int xmin, int xmax, double maximum){
    char aux[30] = "";
    int height = WINDOW_HEIGHT - 140;
    int height1 = 120;
    strcpy(aux, function);

    x1 = 20;
    x2 = x1 + divX; 
    y1 = height1 - ((CalculateResult(aux, xmin)/maximum)*height);
    strcpy(aux, function);
    y2 = height1 - ((CalculateResult(aux, xmin+increment)/maximum)*height);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );

    for( int i = xmin+(2*increment); i <= xmax; i = i+increment ){
        strcpy(aux, function);
        x1 = x2; 
        x2 = x1 + divX;
        y1 = y2; 
        y2 = height1 - ((CalculateResult(aux, i)/maximum)*height);
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );
    }
}

void GraphCase4(SDL_Renderer *renderer, char function[], int x1, int y1, int x2, int y2, int increment, int divX, int xmin, int xmax, double maximum){
    char aux[30] = "";
    int height = WINDOW_HEIGHT - 140;
    int height1 = 120;
    strcpy(aux, function);

    x1 = 20 + (xmin*divX);
    x2 = x1 + divX; 
    y1 = height1 - ((CalculateResult(aux, xmin)/maximum)*height);
    strcpy(aux, function);
    y2 = height1 - ((CalculateResult(aux, xmin+increment)/maximum)*height);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );

    for( int i = xmin+(2*increment); i <= xmax; i = i+increment ){
        strcpy(aux, function);
        x1 = x2; 
        x2 = x1 + divX;
        y1 = y2; 
        y2 = height1 - ((CalculateResult(aux, i)/maximum)*height);
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );
    }
}

void GraphCase5(SDL_Renderer *renderer, char function[], int x1, int y1, int x2, int y2, int increment, int divX, int xmin, int xmax, double maximum, double ymin){
    char aux[30] = "";
    int height = WINDOW_HEIGHT - 140;
    int height1 = (height+120) - abs((ymin/maximum)*height);
    strcpy(aux, function);

    x1 = 20 + (xmin*divX);
    x2 = x1 + divX; 
    y1 = height1 - ((CalculateResult(aux, xmin)/maximum)*height);
    strcpy(aux, function);
    y2 = height1 - ((CalculateResult(aux, xmin+increment)/maximum)*height);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );

    for( int i = xmin+(2*increment); i <= xmax; i = i+increment ){
        strcpy(aux, function);
        x1 = x2; 
        x2 = x1 + divX;
        y1 = y2; 
        y2 = height1 - ((CalculateResult(aux, i)/maximum)*height);
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );
    }
}

void GraphCase6(SDL_Renderer *renderer, char function[], int x1, int y1, int x2, int y2, int increment, int divX, int xmin, int xmax, double maximum, double ymin){
    char aux[30] = "";
    int height = WINDOW_HEIGHT - 140;
    int height1 = (height+120) - abs((ymin/maximum)*height);
    strcpy(aux, function);

    x1 = 20;
    x2 = x1 + divX; 
    y1 = height1 - ((CalculateResult(aux, xmin)/maximum)*height);
    strcpy(aux, function);
    y2 = height1 - ((CalculateResult(aux, xmin+increment)/maximum)*height);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );

    for( int i = xmin+(2*increment); i <= xmax; i = i+increment ){
        strcpy(aux, function);
        x1 = x2; 
        x2 = x1 + divX;
        y1 = y2; 
        y2 = height1 - ((CalculateResult(aux, i)/maximum)*height);
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );
    }
}

void GraphCase7(SDL_Renderer *renderer, char function[], int x1, int y1, int x2, int y2, int increment, int divX, int xmin, int xmax, double maximum){
    char aux[30] = "";
    int height = WINDOW_HEIGHT - 140;
    int height1 = WINDOW_HEIGHT - 20;
    strcpy(aux, function);

    x1 = 20;
    x2 = x1 + divX; 
    y1 = height1 - ((CalculateResult(aux, xmin)/maximum)*height);
    strcpy(aux, function);
    y2 = height1 - ((CalculateResult(aux, xmin+increment)/maximum)*height);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );

    for( int i = xmin+(2*increment); i <= xmax; i = i+increment ){
        strcpy(aux, function);
        x1 = x2; 
        x2 = x1 + divX;
        y1 = y2; 
        y2 = height1 - ((CalculateResult(aux, i)/maximum)*height);
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );
    }
}

void GraphCase8(SDL_Renderer *renderer, char function[], int x1, int y1, int x2, int y2, int increment, int divX, int xmin, int xmax, double maximum){
    char aux[30] = "";
    int height = WINDOW_HEIGHT - 140;
    int height1 = 120;
    strcpy(aux, function);

    x1 = 20;
    x2 = x1 + divX; 
    y1 = height1 - ((CalculateResult(aux, xmin)/maximum)*height);
    strcpy(aux, function);
    y2 = height1 - ((CalculateResult(aux, xmin+increment)/maximum)*height);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );

    for( int i = xmin+(2*increment); i <= xmax; i = i+increment ){
        strcpy(aux, function);
        x1 = x2; 
        x2 = x1 + divX;
        y1 = y2; 
        y2 = height1 - ((CalculateResult(aux, i)/maximum)*height);
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );
    }
}

void GraphCase9(SDL_Renderer *renderer, char function[], int x1, int y1, int x2, int y2, int increment, int divX, int xmin, int xmax, double maximum, double ymin, double ymax){
    char aux[30] = "";
    int height = WINDOW_HEIGHT - 140;
    int height1 = 120 + (height*maximum/(abs(ymin)+abs(ymax)));
    strcpy(aux, function);

    x1 = 20;
    x2 = x1 + divX; 
    y1 = height1 - ((CalculateResult(aux, xmin)/maximum)*(height1-120));
    strcpy(aux, function);
    y2 = height1 - ((CalculateResult(aux, xmin+increment)/maximum)*(height1-120));
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );

    for( int i = xmin+(2*increment); i <= xmax; i = i+increment ){
        strcpy(aux, function);
        x1 = x2; 
        x2 = x1 + divX;
        y1 = y2; 
        y2 = height1 - ((CalculateResult(aux, i)/maximum)*(height1-120));
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2 );
    }
}

void CreateAxis(SDL_Renderer *renderer, int xmin, int xmax, double ymax, double ymin, int divX, double maximum, int increment){
    int startX = 20; 
    int startY = 120;
    int endX = WINDOW_WIDTH - 20, endY = WINDOW_HEIGHT - 20;
    int ubiX = 0, ubiY = 0;

    // UBICACIÓN DEL EJE Y
    if( xmin >= 0 )  
        SDL_RenderDrawLine(renderer, startX, startY, startX, endY);
    else if( xmax <= 0 )
        SDL_RenderDrawLine(renderer, endX, startY, endX, endY);
    else{
        ubiX = 20 + (abs(xmin)*divX)/increment; 
        SDL_RenderDrawLine(renderer, ubiX, startY, ubiX, endY);
    } 

    // UBICACIÓN DEL EJE X
    if( ymin >= 0 )
        SDL_RenderDrawLine(renderer, startX, endY, endX, endY);
    else if( ymax <= 0 )
        SDL_RenderDrawLine(renderer, startX, startY, endX, startY);
    else{
        ubiY = 120 + (500*maximum/(abs(ymin)+abs(ymax)));
        SDL_RenderDrawLine(renderer, startX, ubiY, endX, ubiY);
    }

}

int CalculateDiv(int superior, int inferior){
    if(inferior >= 0)
        return superior; 
    else if( superior <= 0)
        return abs(inferior);  
    else
        return abs(superior)+abs(inferior);
}

void GraphFunction(SDL_Renderer *renderer, char function[], int lowerBound, int upperBound, int increment) {
    int width = WINDOW_WIDTH - 40;
    int height = WINDOW_HEIGHT - 140;
    double ymax = 0;
    double ymin = 0; 
    double valueMax = 0;
    char auxFunction[30]="";
    int divX = (width*increment)/(CalculateDiv(upperBound, lowerBound));
    modificarString(function);
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


    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Negro
    CreateAxis(renderer, lowerBound, upperBound, ymax, ymin, divX, valueMax, increment);
    
    int x1=0, x2=0, y1=0, y2=0; 

    if( ymin>=0 && lowerBound>=0 ) 
        GraphCase1(renderer, function, x1, y1, x2, y2, increment, divX, lowerBound, upperBound, valueMax);
    else if( ymin>=0 && upperBound<=0 )
        GraphCase2(renderer, function, x1, y1, x2, y2, increment, divX, lowerBound, upperBound, valueMax);
    else if( ymax<=0 && upperBound<=0 )
        GraphCase3(renderer, function, x1, y1, x2, y2, increment, divX, lowerBound, upperBound, valueMax);
    else if( ymax<=0 && lowerBound>=0)
        GraphCase4(renderer, function, x1, y1, x2, y2, increment, divX, lowerBound, upperBound, valueMax);
    else if( lowerBound>=0 && ymax>0 && ymin<0 )
        GraphCase5(renderer, function, x1, y1, x2, y2, increment, divX, lowerBound, upperBound, valueMax, ymin);
    else if( upperBound<=0 && ymax>0 && ymin<0 )
        GraphCase6(renderer, function, x1, y1, x2, y2, increment, divX, lowerBound, upperBound, valueMax, ymin);
    else if( ymin>=0 && lowerBound<0 && upperBound>0 )
        GraphCase7(renderer, function, x1, y1, x2, y2, increment, divX, lowerBound, upperBound, valueMax);
    else if( ymax<=0 && lowerBound<0 && upperBound>0 )
        GraphCase8(renderer, function, x1, y1, x2, y2, increment, divX, lowerBound, upperBound, valueMax);
    else if( ymax>0 && ymin<0 && lowerBound<0 && upperBound>0)
        GraphCase9(renderer, function, x1, y1, x2, y2, increment, divX, lowerBound, upperBound, valueMax, ymin, ymax);

    SDL_RenderPresent(renderer);
}




//---------------------------------------- separar modulo
void eliminarUltimaLetra(char *cadena) {
    if (cadena == NULL || *cadena == '\0') {
        // Manejo del caso donde la cadena es nula o vacía
        return;
    }
    
    size_t longitud = strlen(cadena);
    if (longitud > 0) {
        cadena[longitud - 1] = '\0'; // Establecer el último carácter como nulo
    }
}


void toLowerCase(char *str) {
    int i;
    for (i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}

void RellenarCadena(char *cadenaALlenar, int size){
    for ( int i= strlen(cadenaALlenar); i<size-1;i++ ){
        cadenaALlenar[i]=' ';
    }
    cadenaALlenar[size-1]='\0';
}

void crearTexto(SDL_Renderer *renderer , TTF_Font *font ,SDL_Color white, char string[], int posX , int posY){
        char otro_texto[50] = "2"; strcpy(otro_texto,string);// Define otra variable de cadena para el nuevo texto
        SDL_Surface* otro_texto_surface = TTF_RenderText_Solid(font, otro_texto, white);
        SDL_Texture* otro_texto_texture = SDL_CreateTextureFromSurface(renderer, otro_texto_surface);
        int otro_texto_width = otro_texto_surface->w;
        int otro_texto_height = otro_texto_surface->h;
        SDL_FreeSurface(otro_texto_surface);
        SDL_Rect otro_texto_rect = {posX, posY, otro_texto_width, otro_texto_height};
        SDL_RenderCopy(renderer, otro_texto_texture, NULL, &otro_texto_rect);
        SDL_DestroyTexture(otro_texto_texture);
}


void moduloCrearUsuario(SDL_Renderer *renderer , TTF_Font *font ,SDL_Color white){
    // Para este programa se ha decidido que los string se inicializaran con " "
    char userName[20] = " ";  // Es el el string que guarda el nombre de usuario que se entre
    char passHidded[40]=" "; // Es la contraseña enmascarada  (ejemplo: ****)
    char passWord[13]=" ";  // Es la contraseña sin enmascarar (ejemplo: hola)
    int  writingUser=0;    // Esta varible permite identificar que campo se esta escribiendo siendo { 0 = userName , 1 = password }
    char warnings[70]="AQUI PONDRE LA CONRASEÑA QUE DEBE PONER:"; // Variable para poner advertencias o indicativos que se le deban dar al usuario
    char titleOfModule[50] = "Aplicativo - Graficadora - UTP"; // titulo que se le pone al modulo
    char userNameLabel[20]="Username: "; // indicativo de lo que se debe escribir es el nombre de usuario
    int exitApp=0; //necesita habilitar para que cuando se presione cualquier tecla se finalice el programa
    int exitAppAuxiliar=0; // Es la variable encargada de habilitar exitApp a 1 (podria ser cualquier numero mayor a 0)
    char warnings2[70]="AQUI PONDRE LA CONTRASEÑA QUE EL USARIO INGRESA: ";
    //while (running) mantiene actualizando el modulo actual , esto para que el usuario pueda ver lo que va escribiendo
    int flag=1;
    while (flag) {

        SDL_Event event; // event es una variable tipo evento , estas variables guardan lo que ha sucedido en el aplicativo
        // while (SDL_PollEvent(&event)) , ira mirando cada uno de los eventos que esten guardados en event
        while (SDL_PollEvent(&event)) { 
            if (event.type == SDL_QUIT || exitApp) { // si se debe finalizar el programa
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
                        USER bellavista; 
                        RellenarCadena(userName,20);            
                        int index = binarySearchUserName(userName,&bellavista);
                        if ( index !=-1){
                            strcpy(warnings, "USUARIO YA EXISTE");
                        }else{
                            strcpy(passHidded, "Password: ");
                            strcpy(bellavista.userName,  userName);
                            strcpy(warnings, bellavista.userName);
                            writingUser= 1;    
                        }                   
                }


            }else if (writingUser==1 ){ // contraseña
                    if (event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z){
                        strcat(passHidded,"*");
                        if (SDL_GetModState() & KMOD_CAPS || SDL_GetModState() & KMOD_SHIFT){
                            strcat(passWord,SDL_GetKeyName(event.key.keysym.sym));
                        }else {
                            char ara[12];
                            strcpy(ara,SDL_GetKeyName(event.key.keysym.sym));
                            toLowerCase(ara);
                            strcat(passWord,ara);
                        }
                    }else if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9  ){
                        strcat(passHidded,"*");
                        strcat(passWord,SDL_GetKeyName(event.key.keysym.sym));
                    }else if (event.key.keysym.sym == SDLK_RETURN ){
                            if (strlen(passWord) >=5){
                                USER newUser;
                                RellenarCadena(userName,20);
                                RellenarCadena(passWord,13);
                                strcpy ( newUser.userName, userName );
                                strcpy ( newUser.password, passWord );
                                newUser.userKey= GetUserKey();
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
        
        crearTexto(renderer , font, white, userName, 80,50); //renderiza el texto que contiene lo que el usuario vaya ingresando
        crearTexto(renderer , font, white, titleOfModule, 5,10);
        crearTexto(renderer , font, white, userNameLabel, 10,50); //renderiza la el texto "Usermame: " como indicativo
        crearTexto(renderer,font,white, passHidded, 10,75);
        crearTexto(renderer,font,white,warnings,10,120); //renderiza las advertencias que se tengan que hacer
        crearTexto(renderer,font,white,warnings2,10,140); //renderiza las advertencias que se tengan que hacer

        // Actualizar pantalla
        SDL_RenderPresent(renderer);
    }
}

void crearGrafico(SDL_Renderer *renderer , TTF_Font *font ,SDL_Color white, char nombreUsuario[]){
    char title[40]= "APLICATIVO - GRAFICADOR - UTP";
    char subTitle[60]= "Crear Grafico";
    char indicativo[80]= "C = Coeficiente , E = exponente , entre termino en formato CXE de tenerlos";
    char indicativo2[80]= "Rango en formato Inicio,final ejem: -10,15 ";
    char indicativoEcuacion[40]= "f(x)=";
    char ecuacion[30]=" ";
    char indicativoRango[40]= "Rango=";
    char rango[20]=" ";
    char indicativoIncremento[40]= "Incremento=";
    char incremento[40]= " ";
    int userWriting=0;
    char ecuacionAuxilair[30];
    int rangoInferior= 0;
    int rangoSuperior= 0;
    int incrementador=0;
    int running1=1;


    while (running1) {
        SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT ) {
                    running = 0;
                }else if(event.type==SDL_KEYDOWN){

                if ( userWriting==0){
                    if (event.key.keysym.sym == SDLK_x || event.key.keysym.sym == SDLK_PLUS || event.key.keysym.sym == SDLK_MINUS){
                        if (SDL_GetModState() & KMOD_CAPS || SDL_GetModState() & KMOD_SHIFT){
                            strcat(ecuacion,SDL_GetKeyName(event.key.keysym.sym));
                        }else{
                            char ara[20];
                            strcpy(ara,SDL_GetKeyName(event.key.keysym.sym));
                            toLowerCase(ara);
                            strcat(ecuacion,ara);
                        }
                    }else if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9  ){
                            strcat(ecuacion,SDL_GetKeyName(event.key.keysym.sym));
                    }else if (event.key.keysym.sym == SDLK_RETURN){
                        userWriting=1;
                    }
                    }else if (userWriting==1){
                        if (event.key.keysym.sym == SDLK_COMMA || event.key.keysym.sym == SDLK_MINUS){
                                strcat(rango,SDL_GetKeyName(event.key.keysym.sym));
                        }else if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9  ){
                                strcat(rango,SDL_GetKeyName(event.key.keysym.sym));
                        }else if (event.key.keysym.sym == SDLK_RETURN){
                            userWriting=2;
                        }
                    }else{
                        if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9  ){
                                strcat(incremento,SDL_GetKeyName(event.key.keysym.sym));
                        }else if (event.key.keysym.sym == SDLK_RETURN){
                            strcpy(ecuacionAuxilair,ecuacion+1);
                            rangoInferior= atoi(strtok(rango,","));
                            rangoSuperior= atoi(strtok(NULL, " "));
                            incrementador= atoi(incremento);
                            AddGraphicToFile ( nombreUsuario, ecuacionAuxilair, rangoInferior, rangoSuperior, incrementador );
                            running1=0;
                        }

                    }
                }
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        crearTexto(renderer , font, white, title, 10,10);
        crearTexto(renderer , font, white, subTitle, 15,30);
        crearTexto(renderer , font, white, indicativo, 10,50);
        crearTexto(renderer , font, white, indicativo2, 10,70);
        crearTexto(renderer , font, white, indicativoEcuacion, 10,90);
        crearTexto(renderer , font, white, ecuacion, 40,90);
        crearTexto(renderer , font, white, indicativoRango, 160,90);
        crearTexto(renderer , font, white, rango, 210,90);
        crearTexto(renderer , font, white, indicativoIncremento, 310,90);
        crearTexto(renderer , font, white, incremento, 400,90);

        // Actualizar pantalla
        SDL_RenderPresent(renderer);
        
    }
}

    GraphFunction(renderer, ecuacionAuxilair,rangoInferior,rangoSuperior,incrementador);
while (running) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT ) {
                    running = 0;
                }
            }
}


}


void cambiarMiContrasena(SDL_Renderer *renderer , TTF_Font *font ,SDL_Color white, char nombreUsario[]){
        char title[40]= "APLICATIVO - GRAFICADORA - UTP";
        char subTitle[60]= "Cambiar contraseña del usuario: "; strcat(subTitle, nombreUsario);
        char indicativo[40]= "Entre contraseña actual:";
        char contrasenaActual[13]=" ";char contrasenaActualOculta[40]=" ";
        char indicativo2[40]=" ";
        char contrasenaNueva[20]=" ";char contrasenaNuevaOculta[20]=" ";
        char indicativo3[40]= " ";
        char contrasenaNuevaConfirmacion[13]=" "; char contrasenaNuevaConfirmacionOculta[13]=" ";
        char warnings[40]=" ";
        int userWriting=0;
        USER tin;
        while (running) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT ) {
                    running = 0;
                }else if(event.type==SDL_KEYDOWN){

                    if ( userWriting==0){
                        if (event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z){
                            if (SDL_GetModState() & KMOD_CAPS || SDL_GetModState() & KMOD_SHIFT){
                                strcat(contrasenaActual,SDL_GetKeyName(event.key.keysym.sym));
                                strcat(contrasenaActualOculta,"*");
                            }else{
                                char ara[20];
                                strcpy(ara,SDL_GetKeyName(event.key.keysym.sym));
                                toLowerCase(ara);
                                strcat(contrasenaActual,ara);
                                strcat(contrasenaActualOculta,"*");
                            }
                        }else if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9  ){
                                strcat(contrasenaActual,SDL_GetKeyName(event.key.keysym.sym));
                                strcat(contrasenaActualOculta,"*");
                        }else if (event.key.keysym.sym == SDLK_RETURN){
                            RellenarCadena(contrasenaActual,13);
                            int index=binarySearchUserName(nombreUsario, &tin);
                            if ( strcmp(contrasenaActual, tin.password)==0){
                                strcpy(indicativo2,"Entre contraseñan nueva: ");
                                userWriting=1;
                            }else{
                                // devuelve al modulo anterior
                            }
                        }
                    }else if ( userWriting==1){
                        if (event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z){
                            if (SDL_GetModState() & KMOD_CAPS || SDL_GetModState() & KMOD_SHIFT){
                                strcat(contrasenaNueva,SDL_GetKeyName(event.key.keysym.sym));
                                strcat(contrasenaNuevaOculta,"*");
                            }else{
                                char ara[20];
                                strcpy(ara,SDL_GetKeyName(event.key.keysym.sym));
                                toLowerCase(ara);
                                strcat(contrasenaNueva,ara);
                                strcat(contrasenaNuevaOculta,"*");
                            }
                        }else if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9  ){
                                strcat(contrasenaNueva,SDL_GetKeyName(event.key.keysym.sym));
                                strcat(contrasenaNuevaOculta,"*");
                        }else if (event.key.keysym.sym == SDLK_RETURN){
                            strcpy(indicativo3,"Confirme contraseña nueva:");
                            userWriting=2;
                        }
                    }else if ( userWriting==2){
                        if (event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z){
                            if (SDL_GetModState() & KMOD_CAPS || SDL_GetModState() & KMOD_SHIFT){
                                strcat(contrasenaNuevaConfirmacion,SDL_GetKeyName(event.key.keysym.sym));
                                strcat(contrasenaNuevaConfirmacionOculta,"*");
                            }else{
                                char ara[20];
                                strcpy(ara,SDL_GetKeyName(event.key.keysym.sym));
                                toLowerCase(ara);
                                strcat(contrasenaNuevaConfirmacion,ara);
                                strcat(contrasenaNuevaConfirmacionOculta,"*");
                            }
                        }else if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9  ){
                                strcat(contrasenaNuevaConfirmacion,SDL_GetKeyName(event.key.keysym.sym));
                                strcat(contrasenaNuevaConfirmacionOculta,"*");
                        }else if (event.key.keysym.sym == SDLK_RETURN){
                            if (strcmp(contrasenaNueva, contrasenaNuevaConfirmacion)==0){
                                RellenarCadena(contrasenaNueva,13);
                                ChangePassword(nombreUsario,contrasenaNueva);
                            }else{
                                strcpy(warnings,"las contraseñas No coinciden");
                            }
                        }
                    }
                }

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderClear(renderer);

            crearTexto(renderer , font, white, title, 10,10);
            crearTexto(renderer , font, white, subTitle, 15,30);
            crearTexto(renderer , font, white, indicativo, 10,50);crearTexto(renderer , font, white, contrasenaActualOculta, 180,50);
            crearTexto(renderer , font, white, indicativo2, 10,70);crearTexto(renderer , font, white, contrasenaNuevaOculta, 180,70);
            crearTexto(renderer , font, white, indicativo3, 10,90);;crearTexto(renderer , font, white, contrasenaNuevaConfirmacionOculta, 210,90);
            crearTexto(renderer , font, white, warnings, 10,110);



            // Actualizar pantalla
            SDL_RenderPresent(renderer);
        }   
    }
}





void cambiarContraRoot(SDL_Renderer *renderer , TTF_Font *font ,SDL_Color white){
        char title[40]= "APLICATIVO - GRAFICADOR - UTP";
        char subTitle[60]= "Menu para usuario: root Actualizar contraseña ";
        char indicativo[40]= "Entre nueva contraseña:";
        char userNameLabel[40]= "Nueva contraseña:";
        char userName[13]=" ";
        char warnings[32]=" ";


        while (running) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT ) {
                    running = 0;
                }else if(event.type==SDL_KEYDOWN){

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
                            USER taca;
                            RellenarCadena(userName, 13);
                            char tin[20]=" root              ";
                            ChangePassword(tin, userName);
                    }
            
                }
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderClear(renderer);

            crearTexto(renderer , font, white, title, 10,10);
            crearTexto(renderer , font, white, subTitle, 15,30);
            crearTexto(renderer , font, white, userNameLabel, 10,50);
            crearTexto(renderer , font, white, userName, 80,50);
            crearTexto(renderer , font, white, warnings, 10,70);


            // Actualizar pantalla
            SDL_RenderPresent(renderer);
        }   
    }
}

void cambiarContraUser(SDL_Renderer *renderer , TTF_Font *font ,SDL_Color white){
        char title[40]= "APLICATIVO - GRAFICADORA - UTP";
        char subTitle[60]= "Menu para usuario: root Actualizar contraseña ";
        char indicativo[40]= "Entre nueva contraseña:";
        char userNameLabel[40]= "Username:";
        char userName[20]=" ";
        char passWord[13]=" "; 
        char passWordHidded[60]=" ";
        char warnings[32]=" ";
        int userWriting=0;

        while (running) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT ) {
                    running = 0;
                }else if(event.type==SDL_KEYDOWN){

                if ( userWriting==0){
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
                        USER tin;
                        RellenarCadena(userName,20);
                        if ( binarySearchUserName(userName, &tin)!=-1){
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
                            RellenarCadena(passWord,13);
                            strcpy(warnings,passWord);
                            ChangePassword(userName,passWord);
                        }
                    }
                }
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderClear(renderer);

            crearTexto(renderer , font, white, title, 10,10);
            crearTexto(renderer , font, white, subTitle, 15,30);
            crearTexto(renderer , font, white, userNameLabel, 10,50);
            crearTexto(renderer , font, white, userName, 80,50);
            crearTexto(renderer , font, white, passWordHidded, 10,70);
            crearTexto(renderer , font, white, warnings, 10,90);
            crearTexto(renderer , font, white, passWord, 10,110);


            // Actualizar pantalla
            SDL_RenderPresent(renderer);
        }   
    }
}

void borrarUsuario(SDL_Renderer *renderer , TTF_Font *font ,SDL_Color white){
        char title[40]= "APLICATIVO - GRAFICADOR - UTP";
        char subTitle[60]= "Menu para usuario: root Borrar usuario";
        char indicativo[40]= "Entre usuario:";
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
                            char ara[20];
                            strcpy(ara,SDL_GetKeyName(event.key.keysym.sym));
                            toLowerCase(ara);
                            strcat(userName,ara);
                        }
                    }else if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9  ){
                            strcat(userName,SDL_GetKeyName(event.key.keysym.sym));
                    }else if (event.key.keysym.sym == SDLK_RETURN){
                            USER taca;
                            RellenarCadena(userName, 20);
                            int index = binarySearchUserName(userName, &taca);
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

            crearTexto(renderer , font, white, title, 10,10);
            crearTexto(renderer , font, white, subTitle, 15,30);
            crearTexto(renderer , font, white, userNameLabel, 10,50);
            crearTexto(renderer , font, white, userName, 80,50);
            crearTexto(renderer , font, white, warnings, 10,70);


            // Actualizar pantalla
            SDL_RenderPresent(renderer);
        }   
    }
}

void ListarAccesosDeUsarios(SDL_Renderer *renderer , TTF_Font *font ,SDL_Color white){
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
                            char ara[20];
                            strcpy(ara,SDL_GetKeyName(event.key.keysym.sym));
                            toLowerCase(ara);
                            strcat(userName,ara);
                        }
                    }else if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9  ){
                            strcat(userName,SDL_GetKeyName(event.key.keysym.sym));
                    }else if (event.key.keysym.sym == SDLK_RETURN){
                        RellenarCadena(userName,20);
                        tin=false;
                    }
            
                }
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderClear(renderer);

            crearTexto(renderer , font, white, title, 10,10);
            crearTexto(renderer , font, white, subtitle, 15,30);
            crearTexto(renderer , font, white, userNameLabel, 10,50);
            crearTexto(renderer , font, white, userName, 120,50);


            // Actualizar pantalla
            SDL_RenderPresent(renderer);
        }   
    }

    int k = GetQuantityUserGraphics ( userName );
    GRAPHICS vector[ k ];
    GetUserGraphicsVector ( userName, vector );


    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.key.keysym.sym == SDLK_RETURN) {
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
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        int y = 20 - scrollOffset; // Aplicar el desplazamiento vertical
        if (y > 0) y = 0; // Asegurar que no se desplace más allá del inicio
        
        crearTexto(renderer ,font,white,title, 20, y);y+=20;
        crearTexto(renderer ,font,white,subtitle, 20, y);y+=20;
        crearTexto(renderer ,font,white,userNameLabel, 20, y);y+=20;
        crearTexto(renderer ,font,white,titleData, 20, y);y+=20;

        // Imprimir la tabla
        for (int i = 0; i < k; i++) {
            char texto[80];
            sprintf(texto, "%s    |     %s     |   %s", vector[i].date,vector[i].graphicKey, vector[i].function);
            crearTexto(renderer ,font,white,texto, 20, y);y+=20;
            crearTexto(renderer ,font,white,"-------------------------------------", 20, y);
            y += 20; // Ajusta la posición horizontal para la próxima celda
        }

        SDL_RenderPresent(renderer);
    }


}

void ListarUsarios(SDL_Renderer *renderer , TTF_Font *font ,SDL_Color white){
    bool quit = false;
    int scrollOffset = 20;
    char title[]= "Aplicativo -- Graficador -- UTP";
    char subtitle[]= "Listadodo de accesos todos los usuarios";
    char titleData[]="Fecha             Nro grafico    Polinomio f(x)";

    int cantidadDeUsuarios= GetUserKey();
    USER todoslosUsuarios[cantidadDeUsuarios];
    ReadUsersFile(todoslosUsuarios);

    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.key.keysym.sym== SDLK_RETURN) {
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
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        int y = 20 - scrollOffset; // Aplicar el desplazamiento vertical
        if (y > 0) y = 0; // Asegurar que no se desplace más allá del inicio
        
        crearTexto(renderer ,font,white,title, 20, y);y+=20;
        crearTexto(renderer ,font,white,subtitle, 20, y);y+=20;

        // Imprimir la tabla
        for (int i = 0; i < cantidadDeUsuarios; i++) {
            char texto[80]; strcpy(texto,todoslosUsuarios[i].userName);
            if ( texto[0]==' ' && strcmp(texto," root              ")!=0  ){
                int k = GetQuantityUserGraphics ( texto );
                GRAPHICS vector[ k ];
                char userNameLabel[40]="Username:";strcat(userNameLabel,texto);
                GetUserGraphicsVector ( texto, vector );
                crearTexto(renderer ,font,white, userNameLabel, 20, y);y+=20;
                crearTexto(renderer ,font,white,titleData, 20, y);y+=20;
                for (int i = 0; i < k; i++) {
                    char movistar[80];
                    sprintf(movistar, "%s    |     %s     |   %s", vector[i].date,vector[i].graphicKey, vector[i].function);
                    crearTexto(renderer ,font,white,movistar, 20, y);
                    y += 20; // Ajusta la posición horizontal para la próxima celda
                }
                crearTexto(renderer ,font,white,"-------------------------------------------------", 20, y);
                y += 20; // Ajusta la posición horizontal para la próxima celda
            }
        }

        SDL_RenderPresent(renderer);
    }


}


void graficoPorReferencia(SDL_Renderer *renderer , TTF_Font *font ,SDL_Color white,char name[]){
    char title[70]= "APLICATIVO - GRAFICADOR - UTP -impresion de grafica por referencia";
    char subTitle[60]= "Crear Grafico";
    char indicativo[80]= "C = Coeficiente , E = exponente , entre termino en formato CXE de tenerlos";
    char indicativo2[80]= "Rango en formato Inicio,final ejem: -10,15 ";
    char indicativoReferecia[40]= "Entre Nro(referencia):";
    char referencia[30]=  " ";
    char indicativoNombre[40]= "Username: ";
    char nombre[20]=" ";
    char indicativoFecha[40]= "Date=";
    char fecha[40]= " ";
    int userWriting=0;

    int running1=1;

    GRAPHICS funcionAGraficar;


    while (running1) {
        SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT ) {
                    running = 0;
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
                        running1=0;
                    }
                    }
                }
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        crearTexto(renderer , font, white, title, 10,10);
        crearTexto(renderer , font, white, subTitle, 15,30);
        crearTexto(renderer , font, white, indicativo, 10,50);
        crearTexto(renderer , font, white, indicativo2, 10,70);
        crearTexto(renderer , font, white, indicativoReferecia, 10,90);
        crearTexto(renderer , font, white, referencia, 150,90);

        // Actualizar pantalla
        SDL_RenderPresent(renderer);
        
    }
}

if ( strcmp(funcionAGraficar.userName, name)==0 || strcmp(" root              ", name)==0 ){
    char rango[20];sprintf(rango, "%i,%i", funcionAGraficar.lowerBound,funcionAGraficar.upperBound);
    crearTexto(renderer , font, white, indicativoFecha, 350,90);
    crearTexto(renderer , font, white, indicativoNombre, 210,90);
    crearTexto(renderer , font, white, funcionAGraficar.userName, 280,90);
    crearTexto(renderer , font, white, funcionAGraficar.date, 390,90);
    crearTexto(renderer , font, white, "Funcion:", 10,105);
    crearTexto(renderer , font, white, funcionAGraficar.function, 70,105);
    crearTexto(renderer , font, white, "Rango", 210,105);
    crearTexto(renderer , font, white, rango, 280,105);
    crearTexto(renderer , font, white, "Incremento:", 350,105);
    char incremento[5];sprintf(incremento,"%i",funcionAGraficar.increment);
    crearTexto(renderer , font, white, incremento, 430,105);
    GraphFunction(renderer, funcionAGraficar.function,funcionAGraficar.lowerBound,funcionAGraficar.upperBound,funcionAGraficar.increment);
while (running) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT ) {
                    running = 0;
                }
            }
}
}


}

void MisGraficas(SDL_Renderer *renderer , TTF_Font *font ,SDL_Color white, char name[]){
    bool quit = false;
    int scrollOffset = 20;
    char title[]= "Aplicativo -- Graficador -- UTP";
    char subtitle[70]= "Listados de graficos creados por el usuario (tu):";strcat(subtitle,name);
    char titleData[]="Fecha             Nro grafico    Polinomio f(x)";

    int k = GetQuantityUserGraphics ( name );
    GRAPHICS vector[ k ];
    GetUserGraphicsVector ( name, vector );


    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event) ) {
            if (event.type == SDL_QUIT || event.key.keysym.sym == SDLK_RETURN) {
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
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        int y = 20 - scrollOffset; // Aplicar el desplazamiento vertical
        if (y > 0) y = 0; // Asegurar que no se desplace más allá del inicio
        
        crearTexto(renderer ,font,white,title, 20, y);y+=20;
        crearTexto(renderer ,font,white,subtitle, 20, y);y+=20;
        crearTexto(renderer ,font,white,titleData, 20, y);y+=20;

        // Imprimir la tabla
        for (int i = 0; i < k; i++) {
            char texto[80];
            sprintf(texto, "%s    |     %s     |   %s", vector[i].date,vector[i].graphicKey, vector[i].function);
            crearTexto(renderer ,font,white,texto, 20, y);y+=20;
            crearTexto(renderer ,font,white,"-------------------------------------", 20, y);
            y += 20; // Ajusta la posición horizontal para la próxima celda
        }

        SDL_RenderPresent(renderer);
    }


}

void modulo2User(SDL_Renderer *renderer , TTF_Font *font ,SDL_Color white, char nombreUsario[]) {
    char title[40]= "APLICATIVO - GRAFICADOR - UTP";
    char subTitle[60]= "Menu principal para usuario: ";strcat(subTitle, nombreUsario);
    char option0[40]= "0. Salir del aplicativo";
    char option1[40]= "1. Crear nuevo grafico";
    char option2[40]= "2. Borrar un grafico existente";
    char option3[40]= "3. Listar accesos";
    char option4[40]= "4. Imprimir grafico por referencia";
    char option5[60]= "5. Cambiar password del usuario:";strcat(option5, nombreUsario);

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT ) {
                running = 0;
            }else if(event.type==SDL_KEYDOWN){
                switch(event.key.keysym.sym ){
                    case SDLK_0:
                                running=0;break;
                    case SDLK_1:
                        crearGrafico(renderer, font, white,nombreUsario);
                        break;
                    case SDLK_2:
                        crearGrafico(renderer, font, white,nombreUsario); //borrar grafica
                        break;
                    case SDLK_3:
                        MisGraficas(renderer, font, white,nombreUsario);
                        break;
                    case SDLK_4:
                        graficoPorReferencia(renderer, font, white,nombreUsario);
                        break;
                    case SDLK_5:
                        cambiarMiContrasena(renderer, font, white, nombreUsario);
                        break;
                }
            }
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        crearTexto(renderer , font, white, title, 10,10);
        crearTexto(renderer , font, white, subTitle, 15,30);
        crearTexto(renderer , font, white, option0, 10,50);
        crearTexto(renderer , font, white, option1, 10,70);
        crearTexto(renderer , font, white, option2, 10,90);
        crearTexto(renderer , font, white, option3, 10,110);
        crearTexto(renderer , font, white, option4, 10,130);
        crearTexto(renderer , font, white, option5, 10,150);

        // Actualizar pantalla
        SDL_RenderPresent(renderer);
    }   
}
}

void modulo2Root(SDL_Renderer *renderer , TTF_Font *font ,SDL_Color white) {
    char title[40]= "APLICATIVO - GRAFICADOR - UTP";
    char subTitle[40]= "Menu principal para usuario: root";
    char option0[40]= "0. Salir del aplicativo";
    char option1[40]= "1. Crear usuario";
    char option2[40]= "2. Borrar usuario";
    char option3[40]= "3. Listar accesos por usuario";
    char option4[40]= "4. Listar accesos todos los usuarios";
    char option5[40]= "5. Imprimir grafico por referencia";
    char option6[40]= "6. Cambiar password del usuario: root";
    char option7[40]= "7. cambiar password de un usuario";

    int flag=1;
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
                        moduloCrearUsuario(renderer, font, white);
                        break;
                    case SDLK_2:
                        borrarUsuario(renderer, font, white);
                        break;
                    case SDLK_3:
                        ListarAccesosDeUsarios(renderer, font, white);
                        break;
                    case SDLK_4:
                        ListarUsarios(renderer, font, white);
                        break;
                    case SDLK_5:
                        graficoPorReferencia(renderer, font, white," root              ");
                        break;
                    case SDLK_6:
                        cambiarContraRoot(renderer, font, white);
                        break;
                    case SDLK_7:
                        cambiarContraUser(renderer, font, white);
                        break;

                }
            }
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        crearTexto(renderer , font, white, title, 10,10);
        crearTexto(renderer , font, white, subTitle, 15,30);
        crearTexto(renderer , font, white, option0, 10,50);
        crearTexto(renderer , font, white, option1, 10,70);
        crearTexto(renderer , font, white, option2, 10,90);
        crearTexto(renderer , font, white, option3, 10,110);
        crearTexto(renderer , font, white, option4, 10,130);
        crearTexto(renderer , font, white, option5, 10,150);
        crearTexto(renderer , font, white, option6, 10,170);
        crearTexto(renderer , font, white, option7, 10,190);

        // Actualizar pantalla
        SDL_RenderPresent(renderer);
    }   
}
}





// recibe pantalla donde se renderizara , fuente con la que se hara y color del texto
void modulo1(SDL_Renderer *renderer , TTF_Font *font ,SDL_Color white){
    // Para este programa se ha decidido que los string se inicializaran con " "
    char userName[20] = " ";  // Es el el string que guarda el nombre de usuario que se entre
    char passHidded[40]=" "; // Es la contraseña enmascarada  (ejemplo: ****)
    char passWord[13]=" ";  // Es la contraseña sin enmascarar (ejemplo: hola)
    int  writingUser=0;    // Esta varible permite identificar que campo se esta escribiendo siendo { 0 = userName , 1 = password }
    char warnings[70]="AQUI PONDRE LA CONRASEÑA QUE DEBE PONER:"; // Variable para poner advertencias o indicativos que se le deban dar al usuario
    char titleOfModule[50] = "Aplicativo - Graficadora - UTP"; // titulo que se le pone al modulo
    char userNameLabel[20]="Username: "; // indicativo de lo que se debe escribir es el nombre de usuario
    int exitApp=0; //necesita habilitar para que cuando se presione cualquier tecla se finalice el programa
    int exitAppAuxiliar=0; // Es la variable encargada de habilitar exitApp a 1 (podria ser cualquier numero mayor a 0)
    char warnings2[70]="AQUI PONDRE LA CONTRASEÑA QUE EL USARIO INGRESA: ";
    char usuarioparamodulo2[20]=" ";
    int vecesIntentadas=0;

    //while (running) mantiene actualizando el modulo actual , esto para que el usuario pueda ver lo que va escribiendo 
    while (running && vecesIntentadas!=3) {

        SDL_Event event; // event es una variable tipo evento , estas variables guardan lo que ha sucedido en el aplicativo
        // while (SDL_PollEvent(&event)) , ira mirando cada uno de los eventos que esten guardados en event
        while (SDL_PollEvent(&event)) { 
            if (event.type == SDL_QUIT || exitApp) { // si se debe finalizar el programa
                running = 0;
            }


//--------------------------------- Code Optimizado ----------------------------------------------------------
            if(event.type==SDL_KEYDOWN){

           if(exitAppAuxiliar==1){
                exitApp=1;
           }else if (writingUser==0){ // userName

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
                    if (GetUserKey() == 0){
                        if ( strcmp(userName , " root")==0){
                            strcpy(passHidded, "Password: ");
                            strcpy(warnings, " ");
                            writingUser= 1;                
                        }else{
                            strcpy(warnings,"USUARIO INCORRECTO");
                            strcpy(userName, " ");
                            exitAppAuxiliar=1;
                        }
                    }else{
                        USER bellavista;
                        RellenarCadena(userName,20);            
                        int index = binarySearchUserName(userName,&bellavista);
                        if ( index ==-1){
                            strcpy(warnings, "USUARIO NO EXISTENTE");
                            sprintf(warnings2, "%i", index);
                        }else{
                            strcpy(passHidded, "Password: ");
                            strcpy(warnings,  bellavista.password);
                            writingUser= 1;    
                        }
                    }
                }


            }else if (writingUser==1 ){ // contraseña
                    if (event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z){
                        strcat(passHidded,"*");
                        if (SDL_GetModState() & KMOD_CAPS || SDL_GetModState() & KMOD_SHIFT){
                            strcat(passWord,SDL_GetKeyName(event.key.keysym.sym));
                        }else {
                            char ara[12];
                            strcpy(ara,SDL_GetKeyName(event.key.keysym.sym));
                            toLowerCase(ara);
                            strcat(passWord,ara);
                        }
                    }else if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9  ){
                        strcat(passHidded,"*");
                        strcat(passWord,SDL_GetKeyName(event.key.keysym.sym));
                    }else if (event.key.keysym.sym == SDLK_RETURN ){
                        if ( GetUserKey()==0){
                            if (strlen(passWord) >=5){
                                USER root;
                                RellenarCadena(userName,20);
                                RellenarCadena(passWord,13);
                                strcpy ( root.userName, userName );
                                strcpy ( root.password, passWord );
                                root.userKey = 0; AddUserToFile ( root );
                                running=0;
                            }else{
                                strcpy(passWord," ");
                                strcpy(passHidded," ");
                                strcpy(userName," ");
                                vecesIntentadas++;
                                writingUser=0;
                            }
                        }else {
                            USER bellavista;
                            RellenarCadena(userName,20);
                            RellenarCadena(passWord,13);
                            int index = binarySearchUserName(userName,&bellavista);
                            if ( index ==-1){
                                strcpy(warnings, "USUARIO NO EXISTENTE");
                            }else{
                                strcpy(passHidded, "Password: ");
                                strcpy(warnings2,  passWord);
                                if ( strcmp(passWord, bellavista.password)==0){
                                    strcpy(usuarioparamodulo2,userName);                                
                                    if ( strcmp (" root              ", userName)==0){
                                        strcpy(userName," ");
                                        strcpy(passWord," ");
                                        modulo2Root(renderer, font, white);
                                        writingUser=0;
                                        
                                    }else{
                                        strcpy(userName," ");
                                        modulo2User(renderer, font, white, usuarioparamodulo2);
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
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        
        crearTexto(renderer , font, white, userName, 80,50); //renderiza el texto que contiene lo que el usuario vaya ingresando
        crearTexto(renderer , font, white, titleOfModule, 5,10);
        crearTexto(renderer , font, white, userNameLabel, 10,50); //renderiza la el texto "Usermame: " como indicativo
        crearTexto(renderer,font,white, passHidded, 10,75);
        crearTexto(renderer,font,white,warnings,10,120); //renderiza las advertencias que se tengan que hacer
        crearTexto(renderer,font,white,warnings2,10,140); //renderiza las advertencias que se tengan que hacer

        // Actualizar pantalla
        SDL_RenderPresent(renderer);
    }
}

void InitGraphics(SDL_Window **window, SDL_Renderer **renderer, TTF_Font **font) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL no pudo inicializarse! SDL_Error: %s\n", SDL_GetError());
    }

    if (TTF_Init() == -1) {
        SDL_Log("SDL_ttf no pudo inicializarse! SDL_ttf Error: %s\n", TTF_GetError());
    }

    *window = SDL_CreateWindow("GRAFICADORA", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (*window == NULL) {
        SDL_Log("La ventana no pudo ser creada! SDL_Error: %s\n", SDL_GetError());
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (*renderer == NULL) {
        SDL_Log("El renderer no pudo ser creado! SDL_Error: %s\n", SDL_GetError());
    }

    *font = TTF_OpenFont("c:\\Windows\\Fonts\\ARIAL.TTF", 14);
    if (*font == NULL) {
        SDL_Log("No se pudo cargar la fuente! SDL_ttf Error: %s\n", TTF_GetError());
    }
}

void closeGraphics(SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font) {
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    TTF_Font* font = NULL; // TTF_Font es una variable para guarda la fuente que se utilizara y su tamaño
    SDL_Color white = {0, 0, 0, 255}; 
    InitGraphics(&window, &renderer, &font); //llamamos la funcion InitGraphics para que esta le de valores a nuestras variables
    modulo1(renderer, font, white); // Se inicia el programa como tal con el modulo 1;
    closeGraphics(window, renderer, font); // cerramos las librerias visuales y todo lo relacionado con los graficos
    return 0;
}