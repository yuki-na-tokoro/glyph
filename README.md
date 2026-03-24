# glyph

- IRC läuft Standardmäßig auf Port 6667 
- irc.libera.chat -> öffentliches IRC-Netzwerk
- IRC-Nachrichten haben eine maximale Größe von 512 Bytes

## Resources
- Networking in C: [Beej's guide to Network Programming](https://beej.us/guide/bgnet/)
- IRC Protocol -> for Communication: [Better IRC Guide](https://modern.ircdocs.horse/) [IRC](https://www.rfc-editor.org/rfc/rfc1459)
- git commit conventions: [Conventional Commit Messages](https://gist.github.com/qoomon/5dfcdf8eec66a051ecd85625518cfd13)

## Client-Commands

  ### Verbindung

  | Command | Syntax | Beschreibung |
  |---------|--------|--------------|
  | CONNECT | `CONNECT <host> <port>\r\n` | Mit einem Server verbinden |
  | NICK | `NICK <nickname>\r\n` | Nickname setzen/ändern |
  | USER | `USER <user> 0 * :<realname>\r\n` | User registrieren |
  | QUIT | `QUIT [:<reason>]\r\n` | Verbindung trennen |
  | PASS | `PASS <password>\r\n` | Server-Passwort |

  ### Channel

  | Command | Syntax | Beschreibung |
  |---------|--------|--------------|
  | JOIN | `JOIN <#channel>\r\n` | Channel betreten |
  | PART | `PART <#channel> [:<reason>]\r\n` | Channel verlassen |
  | TOPIC | `TOPIC <#channel> [:<topic>]\r\n` | Topic lesen/setzen |
  | NAMES | `NAMES <#channel>\r\n` | User im Channel auflisten |
  | LIST | `LIST [<#channel>]\r\n` | Channels auflisten |
  | KICK | `KICK <#channel> <user> [:<reason>]\r\n` | User kicken |
  | INVITE | `INVITE <user> <#channel>\r\n` | User einladen |
  | MODE | `MODE <#channel> <flags>\r\n` | Channel-Modus setzen |
  | KNOCK | `KNOCK <#channel>\r\n` | An invite-only Channel anklopfen |

  ### Nachrichten

  | Command | Syntax | Beschreibung |
  |---------|--------|--------------|
  | PRIVMSG | `PRIVMSG <target> :<message>\r\n` | Nachricht an Channel oder User |
  | NOTICE | `NOTICE <target> :<message>\r\n` | Wie PRIVMSG, kein Auto-Reply |

  ### Informationen

  | Command | Syntax | Beschreibung |
  |---------|--------|--------------|
  | WHOIS | `WHOIS <user>\r\n` | Info über einen User |
  | WHOWAS | `WHOWAS <user>\r\n` | Info über früheren User |
  | INFO | `INFO\r\n` | Info über den Server |
  | TIME | `TIME\r\n` | Lokalzeit des Servers |
  | VERSION | `VERSION\r\n` | Server-Version abfragen |
  | ADMIN | `ADMIN\r\n` | Admin-Info |
  | LUSERS | `LUSERS\r\n` | User-Statistiken |
  | LINKS | `LINKS\r\n` | Verlinkte Server |
  | TRACE | `TRACE\r\n` | Route zum Server |
  | HELP | `HELP\r\n` | Server-Hilfe |
  | MAP | `MAP\r\n` | Server-Netzwerk-Karte |
  | USERS | `USERS\r\n` | User auf dem Server |
  | STATS | `STATS <query>\r\n` | Server-Statistiken |
  | WHO | `WHO <#channel>\r\n` | User in einem Channel |
  | MOTD | `MOTD\r\n` | Message of the Day |
  | PING | `PING <token>\r\n` | Verbindung prüfen |
  | PONG | `PONG <token>\r\n` | Antwort auf PING |
  | AWAY | `AWAY [:<message>]\r\n` | Abwesenheit setzen |
  | ISON | `ISON <nick> [<nick>...]\r\n` | Prüfen ob User online |
  | USERHOST | `USERHOST <nick>\r\n` | Host eines Users |
  | MONITOR | `MONITOR +/-/l <nick>\r\n` | User online-Status überwachen |
  | WALLOPS | `WALLOPS :<message>\r\n` | Broadcast an Operatoren |

  ### Operator

  | Command | Syntax | Beschreibung |
  |---------|--------|--------------|
  | OPER | `OPER <name> <password>\r\n` | Operator-Rechte erlangen |
  | KILL | `KILL <user> :<reason>\r\n` | User trennen |
  | SQUIT | `SQUIT <server> <reason>\r\n` | Server-Link trennen |
  | REHASH | `REHASH\r\n` | Server-Config neu laden |
  | RESTART | `RESTART\r\n` | Server neu starten |
