/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrocher <mrocher@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 14:59:24 by mrocher           #+#    #+#             */
/*   Updated: 2025/05/05 15:41:21 by mrocher          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Client.hpp"

Channel::Channel() : m_name(""), m_topic(""), m_userSockets(), 
	m_ops(), m_operator(NULL), inviteOnly(false), topicProtected(false),
	m_key(""), m_limit(0), invitedUsers()
{
}

Channel::Channel(std::string &name, Client &user) : m_name(name), m_topic(""), m_userSockets(),
	m_ops(), m_operator(&user), inviteOnly(false), topicProtected(false), m_key(""),
	m_limit(0), invitedUsers()
{
	m_userSockets.insert(user.getSocket());
	m_ops.push_back(user.getSocket());
}

Channel::~Channel() {}

bool Channel::operator<(const Channel &other) const
{
	return (m_name < other.m_name);
}

Channel &Channel::operator=(const Channel &other)
{
	if (this != &other)
	{
		m_name = other.m_name;
		m_topic = other.m_topic;	
		m_userSockets = other.m_userSockets;
		inviteOnly = other.inviteOnly;
		topicProtected = other.topicProtected;
		m_key = other.m_key;
		m_limit = other.m_limit;
		invitedUsers = other.invitedUsers;
		m_operator = other.m_operator;
	}
	return (*this);
}

std::set<int> Channel::getUsers(void) const
{
	return (m_userSockets);
}

std::set<int>::iterator Channel::getUsersBegin(void) const
{
	return const_cast<std::set<int>&>(m_userSockets).begin();
}

std::set<int>::iterator Channel::getUsersEnd(void) const
{
	return const_cast<std::set<int>&>(m_userSockets).end();
}

std::string Channel::getName(void) const
{
	return (m_name);
}

Client *Channel::getOperator(void) const
{
	return (m_operator);
}

void Channel::setOperator(Client *user)
{
	m_operator = user;
}

void Channel::addUserSocket(int socket)
{
	m_userSockets.insert(socket);
}

bool Channel::checkUser(int socket)
{
	return (m_userSockets.find(socket) != m_userSockets.end());
}

void Channel::delUserSocket(int socket)
{
	m_userSockets.erase(socket);
}

bool Channel::isEmpty(void)
{
	return (m_userSockets.empty());
}

bool Channel::isInviteOnly() const
{
	return (inviteOnly);
}

void Channel::setInviteOnly(bool flag)
{
	inviteOnly = flag;
}

void Channel::addInvitedUsers(int socket)
{
	invitedUsers.insert(socket);
}

bool Channel::isInvited(int socket) const
{
	return (invitedUsers.find(socket) != invitedUsers.end());
}

bool Channel::isTopicProtected(void) const
{
	return (topicProtected);
}

void Channel::setTopicProtected(bool flag)
{
	topicProtected = flag;
}

void Channel::setTopic(std::string topic)
{
	m_topic = topic;
}

std::string Channel::getTopic(void) const
{
	return (m_topic);
}

const std::string& Channel::getKey(void) const
{
	return (m_key);
}

void Channel::setKey(const std::string &key)
{
	m_key = key;
}

int Channel::getLimit(void) const
{
	return (m_limit);
}

void Channel::setLimit(int limit)
{
	m_limit = limit;
}

void Channel::addOps(int socket)
{
	if (std::find(m_ops.begin(), m_ops.end(), socket) == m_ops.end())
		m_ops.push_back(socket);
}

void Channel::removeOps(int socket)
{
	std::vector<int>::iterator it = std::find(m_ops.begin(), m_ops.end(), socket);
	if (it != m_ops.end())
		m_ops.erase(it);
}

bool Channel::isOps(int socket) const
{
	return (m_operator && m_operator->getSocket() == socket) || 
		(std::find(m_ops.begin(), m_ops.end(), socket) != m_ops.end());
}

bool Channel::isAdministrator(int socket) const
{
	return (m_operator && m_operator->getSocket() == socket);
}