/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrocher <mrocher@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 09:26:30 by mrocher           #+#    #+#             */
/*   Updated: 2025/04/24 14:41:19 by mrocher          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/*
Récupère les flags actuels du FD puis ajoute le flag non-bloquant.
*/
static int setNonBlocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		return (-1);
	return (fcntl(fd, F_SETFL, flags | O_NONBLOCK));
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
	switch (error)
	{
		case 401:
			reply = "401 " + user.getNickname() + " :No such nick/channel\r\n";
            break;
		case 403:
			reply = "403 " + (context ? *context : "") + " :No such channel\r\n";
			break;
		case 404:
			reply = "404 " + (context ? *context : "") + " :Cannot send to channel\r\n";
			break;
		case 441:
			reply = "441 " + user.getNickname() + " :They aren't on that channel\r\n";
			break;
		case 442:
			reply = "442 " + user.getNickname() + " :You're not on that channel\r\n";
			break;
		case 451:
			reply = "451 " + user.getNickname() + " :You have not registered\r\n";
			break;
		case 461:
			reply = "461 " + user.getNickname() + " :Not enough parameters\r\n";
			break;
		case 464:
			reply = "464 " + user.getNickname() + " :Password incorrect\r\n";
			break;
		case 471:
			reply = "471 " + (context ? *context : "") + " :Cannot join channel (+l)\r\n";
			break;
		case 473:
			reply = "473 " + (context ? *context : "") + " :Cannot join channel (+i)\r\n";
			break;
		case 475:
			reply = "475 " + (context ? *context : "") + " :Cannot join channel (+k)\r\n";
			break;
		case 482:
			reply = "482 " + (context ? *context : "") + " :You're not channel operator\r\n";
			break;
		case 421:
			reply = "421 " + user.getNickname() + " :Unknown command\r\n";
			break;
		default:
			reply = "421 " + user.getNickname() + " :Unknown error\r\n";
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

/*
Vérifie la présence de 1 argument
Vérifie le mot de passe
*/
void Server::pass(std::vector<std::string> &command, Client &user)
{
	if (command.size() < 2)
	{
		numericReply(461, user);
		return;
	}
	if (command[1] == m_password)
		user.setAuthenticated(true);
	else
		numericReply(464, user);
}

/*
Vérifie la présence de 1 argument
Definie le nickname de l'utilisateur
*/
void Server::nick(std::vector<std::string> &command, Client &user)
{
	if (command.size() < 2)
	{
		numericReply(461, user);
		return;
	}
	user.setNickname(command[1]);
}

/*
Vérifie la présence de 1 argument
Definie le nom de l'utilisateur
*/
void Server::user(std::vector<std::string> &command, Client &user)
{
	if (command.size() < 2)
	{
		numericReply(461, user);
		return;
	}
	user.setUsername(command[1]);
	if (!user.getNickname().empty() && !user.getUsername().empty())
		user.setRegistered(true);
}

/*
Vérifie la présence de 1 argument
Créer un canal avec comme opérateur le créateur
*/
void Server::join(std::vector<std::string> &command, Client &user)
{
	if (command.size() < 2)
	{
		numericReply(461, user);
		return;
	}
	std::string channelName = command[1];

	if (m_channelMap.find(channelName) == m_channelMap.end())
	{
		Channel newChannel(channelName, user);
		m_channelMap[channelName] = newChannel;
	}
	Channel &chan = m_channelMap[channelName];
	if (!chan.getKey().empty())
	{
		if (command.size() < 3 || command[2] != chan.getKey())
		{
			numericReply(475, user, &channelName);
			return;
		}
	}
	if (chan.getLimit() > 0 && chan.getUsers().size() >= (unsigned int)chan.getLimit())
	{
		numericReply(471, user, &channelName);
		return;
	}
	if (chan.isInviteOnly() && !chan.isInvited(user.getSocket()))
	{
		numericReply(473, user, &channelName);
		return;
	}
	chan.addUserSocket(user.getSocket());
	user.addChannel(chan);
	sendNames(channelName, user);
}

/*
Vérifie la présence de 2 arguement.
Cherche la présence du channel demande ou
La personne demande
*/
void Server::privmsg(std::vector<std::string> &command, Client &user)
{
	if (command.size() < 3)
	{
		numericReply(461, user);
		return;
	}
	std::string target = command[1];
	std::string message;
	for (size_t i = 2; i < command.size(); i++)
		message += command[i] + " ";
	if (!message.empty())
		message.erase(message.size() - 1, 1);

	if (!target.empty() && (target[0] == '#' || target[0] == '&'))
	{
		if (m_channelMap.find(target) == m_channelMap.end())
		{
			numericReply(403, user, &target);
			return;
		}
		Channel &chan = m_channelMap[target];
		if (!chan.checkUser(user.getSocket()))
		{
			numericReply(404, user, &target);
			return;
		}
		std::set<int> usersInChannel = chan.getUsers();
		for (std::set<int>::iterator it = usersInChannel.begin(); it != usersInChannel.end(); ++it)
		{
			if (*it != user.getSocket())
				sendMessage(m_userMap[*it], ":" + user.getNickname() + " PRIVMSG " + target + " :" + message + "\r\n");
		}
	}
	else
	{
		for (std::map<int, Client>::iterator it = m_userMap.begin(); it != m_userMap.end(); ++it)
		{
			if (it->second.getNickname() == target)
			{
				sendMessage(it->second, ":" + user.getNickname() + " PRIVMSG " + target + " :" + message + "\r\n");
				return;
			}
		}
		numericReply(401, user, &target);
	}
}

/*
Vérifie la présence de 2 arguement.
Cherche la présence de l'utilisateur dans le channel demande.
Si la personne n'est pas OPS, retourne une erreur.
Retire l'utilisateur kick du channel.
*/
void Server::kick(std::vector<std::string> &command, Client &user)
{
	if (command.size() < 3)
	{
		numericReply(461, user);
		return;
	}
	std::string channelName = command[1];
	std::string target = command[2];
	std::string reason;
	if (command.size() > 3)
	{
		for (size_t i = 3; i < command.size(); i++)
			reason += command[i] + " ";
		if (!reason.empty())
			reason.erase(reason.size() - 1, 1);
	}

	if (m_channelMap.find(channelName) == m_channelMap.end())
	{
		numericReply(403, user, &channelName);
		return;
	}
	Channel &chan = m_channelMap[channelName];
	if (chan.getOperator() == NULL || chan.getOperator()->getSocket() != user.getSocket())
	{
		numericReply(482, user, &channelName);
		return;
	}
	
	int targetSocket = -1;
	for (std::map<int, Client>::iterator it = m_userMap.begin(); it != m_userMap.end(); ++it)
	{
		if (it->second.getNickname() == target)
		{
			targetSocket = it->first;
			break;
		}
	}
	if (targetSocket == -1)
	{
		numericReply(441, user, &target);
		return;
	}

	chan.delUserSocket(targetSocket);
	m_userMap[targetSocket].delChannel(chan);
	std::string kickMsg = ":" + user.getNickname() + " KICK " + channelName + " " + target + " :" + reason + "\r\n";
	sendMessage(user, kickMsg);
	sendMessage(m_userMap[targetSocket], kickMsg);
}

void Server::invite(std::vector<std::string> &command, Client &user)
{
	if (command.size() < 3)
	{
		numericReply(461, user);
		return;
	}
	std::string target = command[1];
	std::string channelName = command[2];
	
	if (m_channelMap.find(channelName) == m_channelMap.end())
	{
		numericReply(403, user, &channelName);
		return;
	}
	
	Channel &chan = m_channelMap[channelName];
	if (chan.getOperator() == NULL || chan.getOperator()->getSocket() != user.getSocket())
	{
		numericReply(482, user, &channelName);
		return;
	}
	
	if (!chan.isInviteOnly())
	{
		numericReply(421, user, &channelName);
		return;
	}
	
	if (!chan.checkUser(user.getSocket()))
	{
		numericReply(442, user, &channelName);
		return;
	}
	int targetSocket = -1;
	for (std::map<int, Client>::iterator it = m_userMap.begin(); it != m_userMap.end(); ++it)
	{
		if (it->second.getNickname() == target)
		{
			targetSocket = it->first;
			break;
		}
	}
	if (targetSocket == -1)
	{
		numericReply(401, user, &target);
		return;
	}

	chan.addInvitedUsers(targetSocket);
	std::string inviteMSG = ":" + user.getNickname() + "!~" + user.getUsername() + 
		" INVITE " + target + " " + channelName + "\r\n";
	sendMessage(m_userMap[targetSocket], inviteMSG);
	sendMessage(user, inviteMSG);
}

/*
Vérification des paramètres.
Vérification des droits.
Traitement de la chaîne de modes (modeStr).
Construction et envoi du message MODE de confirmation.
*/
void Server::mode(std::vector<std::string> &command, Client &user)
{
	if (command.size() < 3)
	{
		numericReply(461, user);
		return;
	}
	std::string channelName = command[1];
	std::string option = command[2];
	
	if (m_channelMap.find(channelName) == m_channelMap.end())
	{
		numericReply(403, user, &channelName);
		return;
	}
	
	Channel &chan = m_channelMap[channelName];
	if (chan.getOperator() == NULL || chan.getOperator()->getSocket() != user.getSocket())
	{
		numericReply(482, user, &channelName);
		return;
	}

	bool adding = true;
	int	params = 3;
	std::string response;
	for (size_t i = 0; i < option.size(); i++)
	{
		char modeChar = option[i];
		if (modeChar == '+')
		{
			adding = true;
			response += "+";
			continue;
		}
		if (modeChar == '-')
		{
			adding 	= false;
			response += "-";
			continue;
		}
		switch (modeChar)
		{
			case 'i':
				chan.setInviteOnly(adding);
				response.push_back('i');
				break;
			case 't':
				chan.setTopicProtected(adding);
				response.push_back('t');
				break;
			case 'k':
				if (adding)
				{
					if ((int)command.size() <= params)
					{
						numericReply(461, user);
						return;
					}
					chan.setKey(command[params]);
					response += "k";
					params++;
				}
				else
				{
					chan.setKey("");
					response += "k";
				}
				break;
			case 'l':
				if (adding)
				{
					if ((int)command.size() <= params)
					{
						numericReply(461, user);
						return;
					}
					int limit = atoi(command[params].c_str());
					chan.setLimit(limit);
					response += "l";
					params++;
				}
				else
				{
					chan.setLimit(0);
					response += "l";
				}
				break;
			case 'o':
				if ((int)command.size() <= params)
				{
					numericReply(461, user);
					return;
				}
				else
				{
					std::string target = command[params];
					int targetSocket = -1;
					for (std::map<int, Client>::iterator it = m_userMap.begin(); it != m_userMap.end(); ++it)
					{
						if (it->second.getNickname() == target)
						{
							targetSocket = it->first;
							break;
						}
					}
					if (targetSocket == -1)
					{
						numericReply(401, user, &target);
						return;
					}
					if (adding)
						chan.addOps(targetSocket);
					else
						chan.removeOps(targetSocket);
					response += "o";
					params++;
				}
				break;
		default:
			std::string unknowMode(1, modeChar);
			numericReply(472, user, &unknowMode);
			return;
		}
	}

	std::string reply = ":" + user.getNickname() + " MODE " + channelName + " " + response + "\r\n";
	sendMessage(user, reply);
}

/*
Permet de quitter proprement.
*/
void Server::quit(std::string reason, Client &user)
{
	sendMessage(user, "QUIT :" + reason + "\r\n");
	disconnectUser(user.getSocket());
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
	else if (command == "QUIT")
		quit("Client quit", client);
	else
		numericReply(421, client);
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