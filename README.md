# IRCserv

Un serveur IRC minimaliste en C++98, conforme au RFC 1459, développé dans le cadre du cursus 42.

## Fonctionnalités

- Gestion multi-clients via sockets (poll)
- Authentification par PASS, NICK, USER
- Commandes IRC de base : JOIN, PART, PRIVMSG, QUIT, KICK, INVITE, MODE, etc.
- Gestion des channels avec modes (+i, +t, +k, +l, +o)
- Gestion des opérateurs de channel
- Vérification des droits et des accès (clé, invitation, limite, etc.)
- Messages d’erreur et de confirmation selon le protocole IRC

## Compilation

```bash
make
```

Le binaire sera généré sous le nom `ircserv`.

## Lancement

```bash
./ircserv <port> <password>
```

- `<port>` : Port d’écoute du serveur (ex : 6667)
- `<password>` : Mot de passe à fournir par les clients via la commande PASS

## Utilisation

Connectez-vous avec un client IRC standard :

```bash
irc <adresse_ip> <port>
```

Ou via netcat/telnet pour tester manuellement.

### Commandes supportées

- `PASS <password>`
- `NICK <nickname>`
- `USER <username> <hostname> <servername> <realname>`
- `JOIN <#channel> [key]`
- `PART <#channel>`
- `PRIVMSG <cible> :<message>`
- `MODE <#channel> <modes> [params]`
- `KICK <#channel> <user> [reason]`
- `INVITE <user> <#channel>`
- `QUIT [message]`

## Structure du projet

```
.
├── Channel.cpp / .hpp
├── Client.cpp / .hpp
├── Server.cpp / .hpp
├── Other.cpp / .hpp
├── main.cpp
└── Makefile
```

## Notes

- Un client doit obligatoirement s’authentifier (PASS, NICK, USER) avant d’utiliser d’autres commandes.
- Les modes de channel sont strictement respectés :  
  - +i : invitation requise  
  - +k : mot de passe requis  
  - +l : limite d’utilisateurs  
  - +o : opérateur  
  - +t : seul un opérateur peut changer le topic
- Les messages d’erreur sont conformes au protocole IRC.
