/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TOPIC.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrocher <mrocher@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 16:31:25 by mrocher           #+#    #+#             */
/*   Updated: 2025/05/05 16:31:29 by mrocher          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"

void Server::topic(std::vector<std::string> &command, Client &user)
{
	if (command.size() < 2)
	{
		numericReply(461, user);
		return;
	}
	std::string channelName = command[1];
	
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
	
	if (command.size() > 2)
	{
		std::string topic;
		for (size_t i = 2; i < command.size(); i++)
			topic += command[i] + " ";
		if (!topic.empty())
			topic.erase(topic.size() - 1, 1);
		chan.setTopic(topic);
	}
	std::string reply = ":" + user.getNickname() + " TOPIC " + channelName + " :" + chan.getTopic() + "\r\n";
	sendMessage(user, reply);
}