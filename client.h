#ifndef CLIENT_H
#define CLIENT_H

#define HOST "irc.libera.chat"

/**
 * @brief   Formatiert die Nachrichten vom Server 
 *
 * @param answer    Nachricht vom Server
 * @param nickname  Nickname vom Nutzer
 */
void print_answer(char *answer, char *nickname)

/**
 * @brief   Sendet Nachricht über Socket
 *
 * @param sockfd    File Descriptor vom Socket
 * @param msg   Nachricht die versendet werden soll
 */
void send_msg(int sockfd, char *msg);

/**
 * @brief   Empfängt Nachricht über Socket
 *
 * @param sockfd    File Descriptor vom Socket
 * @param msg   Buffer in der die empfangene Nachricht gespeichert wird
 * @param buffersize    Größe des msg Buffers
 */
void recv_msg(int sockfd, char *msg, int buffersize);

/**
 * @brief   Stellt Verbindung mit dem Server her
 *
 * @return sockfd    File Descriptor vom Socket
 */
int connect_to_server();

/**
 * @brief   Sendet NICK und USER an Server basierend auf Nutzereingabe
 *
 * @param sockfd    File Descriptor vom Socket
 */
char *introduce(int sockfd);

/**
 * @brief   Wartet auf Eingaben vom User und Nachrichten vom Server
 *
 * @param sockfd    File Descriptor vom Socket
 */
void poll_loop(int sockfd);

#endif // CLIENT_H