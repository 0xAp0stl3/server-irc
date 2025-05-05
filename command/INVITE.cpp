/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   INVITE.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrocher <mrocher@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 16:31:06 by mrocher           #+#    #+#             */
/*   Updated: 2025/05/05 16:31:10 by mrocher          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"

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
	if (!chan.isOps(user.getSocket()))
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