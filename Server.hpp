/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrocher <mrocher@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 15:05:41 by mrocher           #+#    #+#             */
/*   Updated: 2025/05/05 15:05:45 by mrocher          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# define BUFFER_SIZE 1024

# include "Other.hpp"
# include "Client.hpp"
# include "Channel.hpp"
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <poll.h>
# include <cstdlib>
# include <cstring>
# include <unistd.h>
# include <iostream>
# include <sstream>
# include <vector>
# include <iterator>
# include <algorithm>
# include <cerrno>
# include <fcntl.h>
# include <map>

class Server
{
	private:
		int				m_listener;
		int				m_fd_count;
		int				m_fd_max;
		struct pollfd	*m_pfds;

		std::string		m_password;

		std::map<int, Client>			m_userMap;
		std::map<std::string, Channel>	m_channelMap;

	private:
		int		create(std::string port);

		void	numericReply(int error, Client &user, std::string *context = NULL);

		void	newConnection(void);
		void	newMessage(int index);
		void	addToPfds(int newfd);
		void	delFromPfds(int index);
		void	disconnectUser(int user_fd);
		void	pass(std::vector<std::string> &command, Client &user);
		void	nick(std::vector<std::string> &command, Client &user);
		void	user(std::vector<std::string> &command, Client &user);
		void	join(std::vector<std::string> &command, Client &user);
		void	privmsg(std::vector<std::string> &command, Client &user);
		void	kick(std::vector<std::string> &command, Client &user);
		void	invite(std::vector<std::string> &command, Client &user);
		void	topic(std::vector<std::string> &command, Client &user);
		void	mode(std::vector<std::string> &command, Client &user);
		void	quit(const std::vector<std::string> &tokens, Client &user);
		
		void	commandDirector(std::vector<std::string> &msg, Client &user);
		void	sendMessage(Client &user, std::string msg);
		void	sendMessage(Client &user, std::string channel_name, std::string msg, bool send_to_sender);
		void	sendNames(std::string name, Client &user);
		void	sendWelcome(Client &user);

	public:
		Server(std::string port, std::string pass);
		~Server();
		
		int run(void);
};

#endif