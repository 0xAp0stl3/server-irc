/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrocher <mrocher@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/05 08:56:18 by mrocher           #+#    #+#             */
/*   Updated: 2025/04/03 15:56:29 by mrocher          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Channel.hpp"
#include <set>
#include <algorithm>

Client::Client()
    : m_socket(-1),
      m_nickname(""),
      m_username(""),
      m_realname(""),
      m_hostname(""),
      m_command(""),
      m_correctPassword(false),
      m_nicknameSet(false),
      m_usernameSet(false),
	  p_auth(false),
	  p_registered(false),
      m_channelSet()
{
}

bool Client::operator<(const Client &other) const
{
	return (m_socket < other.m_socket);
}

void Client::setAuthenticated(bool auth)
{
	p_auth = auth;
}

bool Client::isAuthentificated(void) const
{
	return (p_auth);
}

void Client::setRegistered(bool reg)
{
	p_registered = reg;
}

bool Client::isRegistered(void) const
{
	return (p_registered);
}

void Client::setSocket(int socket)
{
	m_socket = socket;
}

int Client::getSocket(void) const
{
	return (m_socket);
}

void Client::setPasswd(bool type)
{
	m_correctPassword = type;
}

bool Client::getPasswd(void) const
{
	return (m_correctPassword);
}

bool Client::getLogin(void) const
{
	return (m_correctPassword && m_nicknameSet && m_usernameSet);
}

void Client::setNickname(std::string nickname)
{
	m_nickname = nickname;
	m_nicknameSet = true;
}

std::string Client::getNickname(void) const
{
	return (m_nickname);
}

void Client::setUsername(std::string username)
{
	m_username = username;
	m_usernameSet = true;
}

std::string Client::getUsername(void) const
{
	return (m_username);
}

void Client::setHostname(std::string hostname)
{
	m_hostname = hostname;
}

std::string Client::getHostname(void) const
{
    return (m_hostname);
}

void Client::setRealname(std::string realname)
{
	m_realname = realname;
}

std::string Client::getRealname(void) const
{
	return (m_realname);
}

void Client::addChannel(const Channel &channel)
{
	m_channelSet.insert(channel);
}

bool Client::findChannel(std::string str)
{
	for (std::set<Channel>::iterator it = m_channelSet.begin(); it != m_channelSet.end(); ++it)
	{
		if (it->getName() == str)
			return true;
	}
	return (false);
}

void Client::delChannel(const Channel &channel)
{
	m_channelSet.erase(channel);
}

std::string Client::getSource(void) const
{
	return (m_nickname + "!" + m_username + "@" + m_hostname);
}

bool Client::hasNickname(void) const
{
	return (m_nicknameSet);
}

bool Client::hasUsername(void) const
{
	return (m_usernameSet);
}

std::string Client::getCommand(void) const
{
	return (m_command);
}

void Client::addCommand(std::string command)
{
	if (m_command.empty())
		m_command = command;
	else
		m_command += " " + command;
}

std::string Client::drawCommand(void)
{
	std::string tmp = m_command;
	m_command.clear();
	return (tmp);
}

std::set<Channel>::iterator Client::getChannelsBegin(void) const
{
	return const_cast<std::set<Channel>&>(m_channelSet).begin();
}

std::set<Channel>::iterator Client::getChannelsEnd(void) const
{
	return const_cast<std::set<Channel>&>(m_channelSet).end();
}
