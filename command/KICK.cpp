/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   KICK.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrocher <mrocher@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 16:30:42 by mrocher           #+#    #+#             */
/*   Updated: 2025/05/05 16:30:50 by mrocher          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"

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
	if (!chan.isOps(user.getSocket()))
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

	if (chan.isAdministrator(targetSocket) && !chan.isAdministrator(user.getSocket()))
	{
		numericReply(483, user, &target);
		return;
	}
	
	chan.delUserSocket(targetSocket);
	m_userMap[targetSocket].delChannel(chan);
	std::string kickMsg = ":" + user.getNickname() + " KICK " + channelName + " " + target + " :" + reason + "\r\n";
	sendMessage(user, kickMsg);
	sendMessage(m_userMap[targetSocket], kickMsg);
}