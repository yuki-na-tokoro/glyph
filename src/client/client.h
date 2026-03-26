#ifndef CLIENT_H
#define CLIENT_H

#define VERSION "v0.1.0"
#define AUTHORS "Phil P.S. & Jona S."
#define COMPILE_DATE "25.03.2026"


/**
 * @brief   Formatiert die Nachrichten vom Server 
 *
 * @param answer    Nachricht vom Server
 * @param nickname  Nickname vom Nutzer
 */
void print_answer(char *answer, char *nickname);

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
 * @return  Fehlercode
 */
int recv_msg(int sockfd, char *msg, int buffersize);

/**
 * @brief   Stellt Verbindung mit dem Server her
 * 
 * @param host  Domain/IP-Adresse vom Server
 * @param port  Port
 * @return sockfd    File Descriptor vom Socket
 */
int connect_to_server(char *host, char *port);

/**
 * @brief   Wartet auf Eingaben vom User und Nachrichten vom Server
 */
void poll_loop();

#endif // CLIENT_H
