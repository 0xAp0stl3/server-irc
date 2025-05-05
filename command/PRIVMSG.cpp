/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PRIVMSG.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrocher <mrocher@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 16:30:21 by mrocher           #+#    #+#             */
/*   Updated: 2025/05/05 16:30:26 by mrocher          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"

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