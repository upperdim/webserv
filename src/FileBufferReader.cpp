/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileBufferReader.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmihaile <nmihaile@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 18:07:45 by nmihaile          #+#    #+#             */
/*   Updated: 2025/03/15 19:09:14 by nmihaile         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FileBufferReader.hpp"

FileBufferReader::FileBufferReader(const std::string& _path, const size_t& _buff_size)
	:	m_state(FileBuffer::State::INIT),
		m_path(_path),
		m_buff_size(_buff_size)
{
	m_fd = std::ifstream(m_path, std::ios::binary);

	m_fd.open(path);
	if (!m_fd.is_open())
	{
		m_state = FileBuffer::State::ERROR;
		return ;
	}

	// get the file size
    m_fd.seekg (0, m_fd.end);
    m_file_size = m_fd.tellg();
    m_fd.seekg (0, m_fd.beg);

	m_state = FileBuffer::State::READING;
}

FileBufferReader::~FileBufferReader()
{
	if (m_fd.is_open())
	{
		LOG_WARNING_LM("CLOSING", std::string("FileBufferReader ") + m_path + " fd: " + std::to_string(m_body_fd));
		close(m_fd);
	}
}


/* ************************************************************************** */
/* ************************************************************************** */


std::string	FileBufferReader::getNextChunk(void)
{
	std::string	content(m_buff_size, '\0');
	m_fd.read(&content[0], m_buff_size);
	
	content.resize(m_fd.gcount());

	if (m_fd.eof())
		m_state = FileBuffer::State::COMPLETEl;

	return (content);
}

State	FileBufferReader::getState(void)
{
	return (m_state);
}

size_t	FileBufferReader::getSize(void)
{
	return (m_file_size);
}
