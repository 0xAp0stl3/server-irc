/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrocher <mrocher@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 14:59:35 by mrocher           #+#    #+#             */
/*   Updated: 2025/05/05 15:40:50 by mrocher          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "Client.hpp"
# include <iostream>
# include <set>
# include <vector>

class Client;

class Channel
{
	private:
		std::string			m_name;
		std::string			m_topic;
		std::set<int>		m_userSockets;
		std::vector<int>	m_ops;
		Client				*m_operator;
		
		bool				inviteOnly;
		bool				topicProtected;
		std::string			m_key;
		int					m_limit;
		std::set<int>		invitedUsers;
		

	public:
		Channel();
		Channel(std::string &name, Client &user);
		~Channel();
		
		bool operator<(const Channel &other) const;
		Channel &operator=(const Channel &other);

		// Accessors
		std::set<int>			getUsers(void) const;
		std::set<int>::iterator	getUsersBegin(void) const;
		std::set<int>::iterator	getUsersEnd(void) const;
		std::string				getName(void) const;
		Client					*getOperator(void) const;

		// Basic Operations
		void					setOperator(Client *user);
		void					addUserSocket(int socket);
		bool					checkUser(int socket);
		void					delUserSocket(int socket);
		bool					isEmpty(void);

		// Mode : invite-only
		bool					isInviteOnly() const;
		void					setInviteOnly(bool flag);
		void					addInvitedUsers(int socket);
		bool					isInvited(int socket) const;

		// Mode : topic protection
		bool					isTopicProtected(void) const;
		void					setTopicProtected(bool flag);
		void					setTopic(std::string topic);
		std::string				getTopic(void) const;
		
		// Mode : key
		const					std::string& getKey(void) const;
		void					setKey(const std::string &key);

		// Mode : user limit
		int						getLimit(void) const;
		void					setLimit(int limit);

		// Add / Remove Ops
		void					addOps(int socket);
		void					removeOps(int socket);
		bool					isOps(int socket) const;
		bool					isAdministrator(int socket) const;
};

#endif