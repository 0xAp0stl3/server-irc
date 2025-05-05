/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MODE.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrocher <mrocher@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 16:31:46 by mrocher           #+#    #+#             */
/*   Updated: 2025/05/05 16:31:51 by mrocher          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"

/*
Vérification des paramètres.
Vérification des droits.
Traitement de la chaîne de modes (modeStr).
Construction et envoi du message MODE de confirmation.
*/
void Server::mode(std::vector<std::string> &command, Client &user)
{
	if (command.size() < 3)
	{
		numericReply(461, user);
		return;
	}
	std::string channelName = command[1];
	std::string option = command[2];
	
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
	
	bool adding = true;
	int	params = 3;
	std::string response;
	for (size_t i = 0; i < option.size(); i++)
	{
		char modeChar = option[i];
		if (modeChar == '+')
		{
			adding = true;
			response += "+";
			continue;
		}
		if (modeChar == '-')
		{
			adding 	= false;
			response += "-";
			continue;
		}
		switch (modeChar)
		{
			case 'i':
				chan.setInviteOnly(adding);
				response.push_back('i');
				break;
			case 't':
				chan.setTopicProtected(adding);
				response.push_back('t');
				break;
			case 'k':
				if (adding)
				{
					if ((int)command.size() <= params)
					{
						numericReply(461, user);
						return;
					}
					chan.setKey(command[params]);
					response += "k";
					params++;
				}
				else
				{
					chan.setKey("");
					response += "k";
				}
				break;
			case 'l':
				if (adding)
				{
					if ((int)command.size() <= params)
					{
						numericReply(461, user);
						return;
					}
					int limit = atoi(command[params].c_str());
					chan.setLimit(limit);
					response += "l";
					params++;
				}
				else
				{
					chan.setLimit(0);
					response += "l";
				}
				break;
			case 'o':
				if ((int)command.size() <= params)
				{
					numericReply(461, user);
					return;
				}
				else
				{
					std::string target = command[params];
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
					if (!adding)
					{
						if (chan.isAdministrator(targetSocket))
						{
							numericReply(483, user, &target);
							return;
						}
						chan.removeOps(targetSocket);
					}
					else
					{
						if (chan.isAdministrator(targetSocket))
						{
							numericReply(484, user, &target);
							return;
						}
						chan.addOps(targetSocket);
					}
					response += "o";
					params++;
				}
				break;
		default:
			std::string unknowMode(1, modeChar);
			numericReply(472, user, &unknowMode);
			return;
		}
	}

	std::string reply = ":" + user.getNickname() + " MODE " + channelName + " " + response + "\r\n";
	sendMessage(user, reply);
}
