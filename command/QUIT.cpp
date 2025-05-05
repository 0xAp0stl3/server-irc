/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   QUIT.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrocher <mrocher@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 16:32:08 by mrocher           #+#    #+#             */
/*   Updated: 2025/05/05 16:32:14 by mrocher          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"

/*
Permet de quitter proprement.
*/
void Server::quit(const std::vector<std::string> &tokens, Client &user)
{
	std::string reason;
	if (tokens.size() > 1)
	{
		reason = tokens[1];
		if (!reason.empty() && reason[0] == ':')
			reason.erase(0, 1);
		for (size_t i = 2; i < tokens.size(); ++i)
			reason += " " + tokens[i];
	}
	else
		reason = "Client quit";
	std::string prefix = ":" + user.getNickname() + "!~" + user.getNickname();
	std::string msg = prefix + " QUIT :" + reason + "\r\n";
	for (std::map<int, Client>::iterator it = m_userMap.begin(); it != m_userMap.end(); ++it)
	{
		sendMessage(it->second, msg);
	}
	disconnectUser(user.getSocket());
}