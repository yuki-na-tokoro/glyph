#ifndef CLIENT_H
#define CLIENT_H
#include "data.h"

/**
 * @brief sendet nachricht an client
 *
 * @param client  target client 
 * @param msg     nachricht
 *
 * @return #send() result
 */
int msg_client(struct client* client, char* msg);

/**
 * @brief fügt einen neuen client zum array hinzu
 *
 * @param clients  clinet* array mit verbundenen clients
 * @param ccount   anzahl der verbundenen clients
 * @param c        neuer client
 *
 * @return ob der client hinzugefügt werden konnte
 */
bool push_client(struct client* clients[], int* ccount, struct client* c); 

/**
 * @brief entfernt alle clients die in der mask geflagt sind
 *
 * @param mask     array mit flags für geschlossene Verbindungen
 * @param clients  client* array mit verbundenen clients
 * @param ccount   anzahl der verbundenen clients
 */
void close_clients(bool *mask, struct client* clients[], int* ccount);

#endif // CLIENT_H
