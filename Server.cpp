/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrocher <mrocher@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 09:26:30 by mrocher           #+#    #+#             */
/*   Updated: 2025/05/05 21:40:25 by mrocher          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(std::string port, std::string pass)
    : m_listener(0), m_fd_count(0), m_fd_max(0), m_pfds(NULL), m_password(pass)
{
	m_listener = create(port);
}

Server::~Server()
{
	for (int i = 0; i < m_fd_count; i++)
		close(m_pfds[i].fd);
	if (m_pfds)
		free(m_pfds);
}

static int setNonBlocking(int fd)
{
	return (fcntl(fd, F_SETFL, O_NONBLOCK));
}

/*
Crée un socket.
Configure des options pour faciliter sa réutilisation.
Lie le socket à une adresse et un port donnés, le met en écoute pour des connexions entrantes.
Configure en mode non bloquant, puis renvoie le descripteur du socket.
*/
int Server::create(std::string port)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		std::cerr << "Socket creation error: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}

	int opt = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		std::cerr << "Setsockopt error: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
	
	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(std::atoi(port.c_str()));
	
	if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		std::cerr << "Bind error: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
	
	if (listen(sockfd, 10) < 0)
	{
		std::cerr << "Listen error: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
	
	setNonBlocking(sockfd);
	return (sockfd);
}

/*
Gestion des cas d'erreurs en conformité avec le RFC 1459.
*/
void Server::numericReply(int error, Client &user, std::string *context)
{
	std::string reply;
	const std::string nick = user.getNickname();
	const std::string ctx = (context ? *context : "");
	
	switch (error)
	{
		case 331:
			reply = "331 " + nick + " " + ctx + " :No topic is set\r\n";
			break;

		case 401:
			reply = "401 " + nick + " " + ctx + " :No such nick/channel\r\n";
            break;

		case 403:
			reply = "403 " + nick + " " + ctx + " :No such channel\r\n";
			break;
			
		case 404:
			reply = "404 " + nick + " " + ctx + " :Cannot send to channel\r\n";
			break;

		case 433:
			reply = "433 " + nick + " " + ctx + " :Nickname/Username is already in use\r\n";
			break;
			
		case 441:
			reply = "441 " + nick + " " + ctx + " :They aren't on that channel\r\n";
			break;

		case 442:
			reply = "442 " + nick + " " + ctx + " :You're not on that channel\r\n";
			break;

		case 451:
			reply = "451 " + nick + " :You have not registered\r\n";
			break;

		case 461:
			reply = "461 " + nick + " " + ctx + " :Not enough parameters\r\n";
			break;

		case 464:
			reply = "464 " + nick + " :Password incorrect\r\n";
			break;

		case 471:
			reply = "471 " + nick + " " + ctx + " :Cannot join channel (+l)\r\n";
			break;
			
		case 473:
			reply = "473 " + nick + " " + ctx + " :Cannot join channel (+i)\r\n";
			break;
			
		case 475:
			reply = "475 " + nick + " " + ctx + " :Cannot join channel (+k)\r\n";
			break;

		case 482:
			reply = "482 " + nick + " " + ctx + " :You're not channel operator\r\n";
			break;

		case 483:
			reply = "483 " + nick + " " + ctx + " :You're cannot remove admin operator\r\n";
			break;

		case 484:
			reply = "484 " + nick + " " + ctx + " :You're cannot add admin operator\r\n";
			break;
			
		case 421:
			reply = "421 " + nick + " " + ctx + " :Unknown command\r\n";
			break;

		default:
			reply = "421 " + nick + " " + ctx + " :Unknown error\r\n";
			break;
	}
	sendMessage(user, reply);
}

/*
Accepte une nouvelle connexion sur le socket d'écoute.
Configure le socket en mode non bloquant.
Crée un nouvel objet utilisateur associé à ce socket.
L'ajoute à la structure de suivi.
*/
void Server::newConnection(void)
{
	struct sockaddr_in client_addr;
	socklen_t addr_len = sizeof(client_addr);
	int newfd = accept(m_listener, (struct sockaddr*)&client_addr, &addr_len);
	if (newfd < 0)
	{
		std::cerr << "Accept error: " << strerror(errno) << std::endl;
		return;
    }
	setNonBlocking(newfd);
	Client newUser;
	newUser.setSocket(newfd);
	m_userMap[newfd] = newUser;
	addToPfds(newfd);
	std::cout << "New connection accepted: FD " << newfd << std::endl;
}

/*
Réception des données/messages.
Création d'une chaîne pour convertir le contenu lu.
Découpage en tokens.
Traitement de la commande avec commandDirector (PASS, USER, NICK ...).
*/
void Server::newMessage(int index)
{
	char buffer[BUFFER_SIZE];
	std::memset(buffer, 0, sizeof(buffer));
	int nbytes = recv(m_pfds[index].fd, buffer, BUFFER_SIZE - 1, 0);
	if (nbytes <= 0)
		disconnectUser(m_pfds[index].fd);
	else
	{
		std::string message(buffer, nbytes);
		std::istringstream iss(message);
		std::istream_iterator<std::string> begin(iss);
		std::istream_iterator<std::string> end;
		std::vector<std::string> tokens(begin, end);
		int fd = m_pfds[index].fd;
		if (m_userMap.find(fd) != m_userMap.end())
		{
			Client &user = m_userMap[fd];
			commandDirector(tokens, user);
		}
	}
}

/*
Vérifie d'abord si le tableau de pollfd a suffisamment de place.
Si besoin, elle augmente dynamiquement la capacité en doublant le nombre d'éléments.
Ajoute ensuite le nouveau descripteur dans le tableau en configurant les événements à surveiller.
Incrémente le compteur de descripteurs.
*/
void Server::addToPfds(int newfd)
{
	if (m_fd_count >= m_fd_max)
	{
		m_fd_max = m_fd_max ? m_fd_max * 2 : 10;
		m_pfds = (struct pollfd*)realloc(m_pfds, m_fd_max * sizeof(struct pollfd));
		if (!m_pfds)
		{
			std::cerr << "Memory allocation error\n";
			exit(EXIT_FAILURE);
		}
	}
	m_pfds[m_fd_count].fd = newfd;
	m_pfds[m_fd_count].events = POLLIN;
	m_pfds[m_fd_count].revents = 0;
	m_fd_count++;
}

/*
Retire un élément du tableau
*/
void Server::delFromPfds(int index)
{
	for (int i = index; i < m_fd_count - 1; i++)
		m_pfds[i] = m_pfds[i+1];
	m_fd_count--;
}

/*
Déconnecte proprement un utilisateur du serveur.
*/
void Server::disconnectUser(int user_fd)
{
	close(user_fd);
	m_userMap.erase(user_fd);
	for (int i = 0; i < m_fd_count; i++)
	{
		if (m_pfds[i].fd == user_fd)
		{
			delFromPfds(i);
			break;
		}
	}
	std::cout << "Disconnected user: FD " << user_fd << std::endl;
}

void Server::sendWelcome(Client &user)
{
	const std::string srv = "irc.server";
	std::string msg = ":" + srv + 
		" 001 " + user.getNickname() + " :Welcome to " + srv + ", " + user.getNickname() + "\r\n";
	sendMessage(user, msg);
}

/*
Gere tous les commandes. 
*/
void Server::commandDirector(std::vector<std::string> &msg, Client &client)
{
	if (msg.empty())
		return;
	std::string command = msg[0];
	std::transform(command.begin(), command.end(), command.begin(), ::toupper);
	
	if (command == "CAP" && msg.size() >= 2 )
	{
		if (msg[1] == "LS")
			sendMessage(client, "CAP * LS :\r\n");
		if (msg[1] == "END")
			sendMessage(client, "CAP END\r\n");
		return;
	}

	if (command == "PING" && msg.size() >= 2 )
	{
		sendMessage(client, "PONG " + msg[1] + "\r\n");
		return;
	}

	if (!client.isAuthentificated() || !client.isRegistered())
	{
		if (command != "PASS" && command != "NICK" && command != "USER")
		{
			numericReply(462, client);
			return;
		}
	}
	
	if (command == "PASS")
		pass(msg, client);
	else if (command == "NICK")
		nick(msg, client);
	else if (command == "USER")
		user(msg, client);
	else if (command == "JOIN")
		join(msg, client);
	else if (command == "PRIVMSG")
		privmsg(msg, client);
	else if (command == "KICK")
		kick(msg, client);
	else if (command == "INVITE")
		invite(msg, client);
	else if (command == "MODE")
		mode(msg, client);
	else if (command == "TOPIC")
		topic(msg, client);
	else if (command == "QUIT")
		quit(msg, client);
	else
		numericReply(421, client, &msg[0]);
}

/*
Envoie un message a l'utilisateur
*/
void Server::sendMessage(Client &user, std::string msg)
{
	send(user.getSocket(), msg.c_str(), msg.size(), 0);
}

/*
Envoie un message a tous les utilisateurs dans le channel
*/
void Server::sendMessage(Client &user, std::string channel_name, std::string msg, bool send_to_sender)
{
	if (m_channelMap.find(channel_name) != m_channelMap.end())
	{
		Channel &chan = m_channelMap[channel_name];
		std::set<int> users = chan.getUsers();
		for (std::set<int>::iterator it = users.begin(); it != users.end(); ++it)
		{
			if (!send_to_sender && *it == user.getSocket())
				continue;
			send(*it, msg.c_str(), msg.size(), 0);
		}
	}
}

/*
Retourne le NICK et USER
*/
void Server::sendNames(std::string name, Client &user)
{
	if (m_channelMap.find(name) == m_channelMap.end())
		return;
	Channel &chan = m_channelMap[name];
	std::string names;
	std::set<int> users = chan.getUsers();
	for (std::set<int>::iterator it = users.begin(); it != users.end(); ++it)
		names += m_userMap[*it].getNickname() + " ";
	if (!names.empty())
		names.erase(names.size() - 1, 1);
	std::string reply = "353 " + user.getNickname() + " = " + name + " :" + names + "\r\n";
	sendMessage(user, reply);
}

int Server::run(void)
{
	m_fd_max = 10;
	m_fd_count = 0;
	m_pfds = (struct pollfd*)malloc(m_fd_max * sizeof(struct pollfd));
	if (!m_pfds)
	{
		std::cerr << "Memory allocation error\n";
		return (1);
	}

	addToPfds(m_listener);
	while (true)
	{
		int poll_count = poll(m_pfds, m_fd_count, -1);
		if (poll_count < 0)
		{
			std::cerr << "Poll error: " << strerror(errno) << "\n";
			break;
		}
		for (int i = 0; i < m_fd_count; i++)
		{
			if (m_pfds[i].revents & POLLIN)
			{
				if (m_pfds[i].fd == m_listener)
					newConnection();
				else
					newMessage(i);
			}
		}
	}
	return (0);
}