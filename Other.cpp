/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Other.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrocher <mrocher@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 15:29:39 by mrocher           #+#    #+#             */
/*   Updated: 2025/04/03 16:33:07 by mrocher          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Other.hpp"

bool isNumber(char *s)
{
	if (!s || *s == '\0')
		return (false);
	if (*s == '-' || *s == '+')
		s++;
	while (*s)
	{
		if (!std::isdigit(*s))
			return (false);
		s++;
	}
	return (true);
}

std::vector<std::string> splitMsg(std::string content)
{
	std::vector<std::string> tokens;
	std::istringstream iss(content);
	std::string token;
	while (iss >> token)
		tokens.push_back(token);
	return (tokens);
}

bool isCrlf(std::string str)
{
	return (str == "\r\n");
}

int receiveMsg(int socket, std::string &buff)
{
	char tmp[BUFFER_SIZE + 1];
	int bytes = recv(socket, tmp, BUFFER_SIZE, 0);
	if (bytes > 0)
	{
		tmp[bytes] = '\0';
		buff += tmp;
	}
	return (bytes);
}

std::string toString(int n)
{
	std::stringstream ss;
	ss << n;
	return (ss.str());
}

std::string crop(std::string str)
{
	const std::string whitespace = " \t\n\r";
	size_t start = str.find_first_not_of(whitespace);
	if (start == std::string::npos)
		return ("");
	size_t end = str.find_last_not_of(whitespace);
	return (str.substr(start, end - start + 1));
}
