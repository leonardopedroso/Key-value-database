#include "KVSLocalServer-auth.h" // Include header

int authGetSecret(char * group, char ** secret){
    // Comunicar com o auth server a pedir segredo do grupo char * group
    // Recebe o segredo em packs de string e allca o tamahnaho necessário com malloc
    // o free é feito fora

    // Retorna 
    // AUTH_OK on succsess
    // AUTH_COM_ERROR if there was an error on the cmmunication
    // AUTH_GOUP_DSN_EXIST if the group doesnt exist
    // Free should be done inside on error

    // Código para teste da minha parte abaixo
    char secretDefault[MAX_STR_LENGTH];
    strcpy(secretDefault,"abcdef123");
    * secret = (char  *) malloc(sizeof(char)*(strlen("secretDefault")+1));
    strcpy(*secret,secretDefault);

    return AUTH_OK;
}

int authCreateGroup(char * group, char ** secret){

    // Gerar secredo de comprimento aleatorio e allocar com malloc
    // free é feito fora
    // Retorna
    // AUTH_OK on susscess -> does not do free
    // AUTH_AUTH_ALLOC_ERROR -> does not do free
    // Returns below do free inside
    // AUTH_COM_ERROR if there was an error on the cmmunication
    // AUTH_GROUP_ALREADY_EXISTS if the group already exists on the server
    

    // WARNING: DO FREE in this function if returns ERROR

    // Código para teste da minha parte abaixo
    char secretDefault[MAX_STR_LENGTH];
    strcpy(secretDefault,"abcdef123");
    * secret = (char  *) malloc(sizeof(char)*(strlen("secretDefault")+1));
    if(*secret == NULL){
        return AUTH_ALLOC_ERROR;
    }
    strcpy(*secret,secretDefault);
    return AUTH_OK;
}

int authDeleteGroup(char * group){
    // Returns 
    // AUTH_OK on succsess
    // AUTH_COM_ERROR on communication error
    // AUTH_GOUP_DSN_EXIST 

    return AUTH_OK;

}


//int authent´icate(char * group, char * secret);
//returns 