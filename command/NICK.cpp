/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NICK.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrocher <mrocher@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 16:28:59 by mrocher           #+#    #+#             */
/*   Updated: 2025/05/05 21:42:07 by mrocher          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"

/*
Vérifie la présence de 1 argument
Definie le nickname de l'utilisateur
*/
void Server::nick(std::vector<std::string> &command, Client &user)
{
	if (command.size() < 2)
	{
		numericReply(461, user);
		return;
	}
	std::string nickname = command[1];

	for (std::map<int, Client>::iterator it = m_userMap.begin(); it != m_userMap.end(); ++it)
	{
		if (it->second.getNickname() == nickname && it->first != user.getSocket())
		{
			numericReply(433, user, &nickname);
			return;
		}
	}
	user.setNickname(command[1]);
	if (!user.getNickname().empty() && !user.getUsername().empty())
	{
		user.setRegistered(true);
		sendWelcome(user);
	}
}