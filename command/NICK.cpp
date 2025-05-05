/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NICK.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrocher <mrocher@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 16:28:59 by mrocher           #+#    #+#             */
/*   Updated: 2025/05/05 16:29:07 by mrocher          ###   ########.fr       */
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
	user.setNickname(command[1]);
}