/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileBufferReader.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 18:07:47 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/15 19:09:27 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILEBUFFERREADER_HPP
#define FILEBUFFERREADER_HPP

#include <string>
#include <fstream>

namespace FileBuffer
{
	enum class State
	{
		INIT,
		READING,
		COMPLETE,
		ERROR
	};
}

class FileBufferReader
{
public:
	FileBufferReader(const std::string& _path, const size_t& _buff_size);
	~FileBufferReader();

	std::string			getNextChunk(void);
	FileBuffer::State	getState(void);
	size_t				getSize(void);

private:
	FileBufferReader(const FileBufferReader& other);
	FileBufferReader&	operator=(const FileBufferReader& rhs);

	FileBuffer::State	m_state;
	const std::string	m_path;
	const size_t		m_buff_size;
	std::ifstream		m_fd;
	size_t				m_file_size;
};

#endif
