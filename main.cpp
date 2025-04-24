/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrocher <mrocher@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/06 17:04:21 by mrocher           #+#    #+#             */
/*   Updated: 2025/04/19 18:05:39 by mrocher          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Other.hpp"

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cout << "Error: ./ircserv <port> <password>" << std::endl;
		return (1);
	}
	Server server(argv[1], argv[2]);
	std::cout << "=> Server has been created\nPort: " << argv[1] << " Password: " << argv[2] << std::endl;
	server.run();
	
	return (0);
}