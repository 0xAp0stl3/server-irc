/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Other.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrocher <mrocher@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 15:28:37 by mrocher           #+#    #+#             */
/*   Updated: 2025/05/05 21:31:55 by mrocher          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef OTHER_HPP
# define OTHER_HPP

# include "Server.hpp"
# include <iostream>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <unistd.h>
# include <poll.h>
# include <map>
# include <vector>
# include <cstring>
# include <sstream>
# include <cctype>
# include <cstdlib>

bool						isNumber(char *s);
std::vector<std::string>	splitMsg(const std::string &content);
bool						isCrlf(std::string str);
int							receiveMsg(int socket, std::string &buff);
std::string					toString(int n);
std::string					crop(std::string str);

#endif