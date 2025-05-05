/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PASS.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrocher <mrocher@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 16:27:38 by mrocher           #+#    #+#             */
/*   Updated: 2025/05/05 16:28:46 by mrocher          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"

/*
Vérifie la présence de 1 argument
Vérifie le mot de passe
*/
void Server::pass(std::vector<std::string> &command, Client &user)
{
	if (command.size() < 2)
	{
		numericReply(461, user);
		return;
	}
	if (command[1] == m_password)
		user.setAuthenticated(true);
	else
		numericReply(464, user);
}