/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JOIN.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrocher <mrocher@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 16:29:54 by mrocher           #+#    #+#             */
/*   Updated: 2025/05/05 16:29:59 by mrocher          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"

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
	
	if (channelName[0] != '#' && channelName[0] != '&')
	{
		numericReply(403, user, &channelName);
		return;
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