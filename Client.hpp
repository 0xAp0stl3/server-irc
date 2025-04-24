/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrocher <mrocher@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/05 08:56:04 by mrocher           #+#    #+#             */
/*   Updated: 2025/04/03 16:31:13 by mrocher          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Channel.hpp"
# include <iostream>
# include <set>
# include <algorithm>

class Channel;

class Client
{
	private:
		int	m_socket;
		std::string	m_nickname;
		std::string	m_username;
		std::string	m_realname;
		std::string	m_hostname;
		std::string	m_command;
		bool	m_correctPassword;
		bool	m_nicknameSet;
		bool	m_usernameSet;
		bool	p_auth;
		bool	p_registered;

		std::set<Channel>	m_channelSet;
		
	public:
		Client();
		bool		operator<(const Client &other) const;
		void		setAuthenticated(bool auth);
		bool		isAuthentificated(void) const;
		void		setRegistered(bool reg);
		bool		isRegistered(void) const;
		void		setSocket(int socket);
		int			getSocket(void) const;
		void		setPasswd(bool type);
		bool		getPasswd(void) const;
		bool		getLogin(void) const;
		void		setNickname(std::string nickname);
		std::string	getNickname(void) const;
		void		setUsername(std::string username);
		std::string	getUsername(void) const;
		void		setHostname(std::string hostname);
		std::string	getHostname(void) const;
		void		setRealname(std::string realname);
		std::string	getRealname(void) const;
		void		addChannel(const Channel &channel);
		bool		findChannel(std::string str);
		void		delChannel(const Channel &channel);
		std::string	getSource(void) const;
		bool		hasNickname(void) const;
		bool		hasUsername(void) const;
		std::string	getCommand(void) const;
		void		addCommand(std::string command);
		std::string	drawCommand(void);
		std::set<Channel>::iterator	getChannelsBegin(void) const;
		std::set<Channel>::iterator	getChannelsEnd(void) const;
};

#endif